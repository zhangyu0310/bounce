/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of TimerQueue.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/event_loop.h>
#include <bounce/timer_queue.h>

#include <utility>
#include <vector>

bounce::TimerQueue::TimerPtr bounce::TimerQueue::addTimer(
        const TimePoint& expiration,
        const NanoSeconds& duration,
        TimeOutCallback&& cb) {
    TimerPtr timer(new Timer(expiration, duration, std::move(cb)));
    loop_->doTaskInThread(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return timer;
}

void bounce::TimerQueue::addTimerInLoop(
        const bounce::TimerQueue::TimerPtr& timer) {
    if (!timer->repeat()) {
        timer_map_once_.insert(
                std::make_pair(timer->expiration(), timer));
    } else {
        timer_map_repeat_.insert(
                std::make_pair(timer->expiration(), timer));
    }
    if (timer->expiration() < min_expiration_) {
        resetTimerfd();
    }
}

void bounce::TimerQueue::deleteTimer(
        const bounce::TimerQueue::TimerPtr& timer) {
    loop_->doTaskInThread(std::bind(
            &TimerQueue::deleteTimerInLoop, this, timer));
}

void bounce::TimerQueue::deleteTimerInLoop(
        const bounce::TimerQueue::TimerPtr& timer) {
    auto lower_it = timer_map_repeat_.lower_bound(timer->expiration());
    for (auto it = lower_it; it != timer_map_repeat_.end(); ++it) {
        if (it->second->expiration() > timer->expiration()) {
            break;
        }
        if (it->second == timer) {
            timer_map_repeat_.erase(it);
            break;
        }
    }
}

void bounce::TimerQueue::handleRead(time_t) {
    uint64_t for_read;
    ssize_t ret = ::read(timer_fd_, &for_read, sizeof(for_read));
    if (ret != sizeof(for_read)) {
        Logger::get("bounce_file_log")->error("file:{}, line:{}, function:{}  "
                "timer read failed, errno is {}", FILENAME(__FILE__), __LINE__,
                __FUNCTION__, errno);
    }
    // find timers who is timeout.
    // get the first iterator bigger than now.
    auto now = std::chrono::system_clock::now();
    auto upper_it = timer_map_once_.upper_bound(now);
    for (auto it = timer_map_once_.begin(); it != upper_it; ++it) {
        it->second->execCallback();
    }
    timer_map_once_.erase(timer_map_once_.begin(), upper_it);
    // deal the repeat timer.
    upper_it = timer_map_repeat_.upper_bound(now);
    std::vector<TimerPtr> to_update;
    for (auto it = timer_map_repeat_.begin(); it != upper_it; ++it) {
        it->second->execCallback();
        to_update.push_back(it->second);
    }
    timer_map_repeat_.erase(timer_map_repeat_.begin(), upper_it);
    for (auto it : to_update) {
        auto new_time = it->timerUpdate();
        timer_map_repeat_.insert(std::make_pair(new_time, it));
    }
    resetTimerfd();
}

void bounce::TimerQueue::resetTimerfd() {
    TimePoint once_expiration = TimePoint::max();
    TimePoint repeat_expiration = TimePoint::max();
    if (!timer_map_once_.empty()) {
        once_expiration = timer_map_once_.begin()->second->expiration();
    }
    if (!timer_map_repeat_.empty()) {
        repeat_expiration = timer_map_repeat_.begin()->second->expiration();
    }
    min_expiration_ =
            once_expiration < repeat_expiration ?
            once_expiration : repeat_expiration;
    if (min_expiration_ != TimePoint::max()) {
        // wake up loop by timerfd_settime()
        struct itimerspec new_value{timespec{0, 0}, timespec{0, 0}};
        struct itimerspec old_value{timespec{0, 0}, timespec{0, 0}};
        auto diff = min_expiration_ - std::chrono::system_clock::now();
        if (diff < NanoSeconds(10000000)) { // 0.01ms
            diff = NanoSeconds(10000000);
        }
        new_value.it_value.tv_sec = diff.count() / NanoSecondsPerSecond;
        new_value.it_value.tv_nsec = diff.count() % NanoSecondsPerSecond;
        // old_value can be nullptr, we don't care.
        int ret = ::timerfd_settime(timer_fd_, 0, &new_value, &old_value);
        if (ret != 0) {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  "
                    "timerfd_settime failed, errno is {}",
                    FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
        }
    }
}