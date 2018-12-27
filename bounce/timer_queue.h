/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the TimerQueue.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_TIMER_QUEUE_H
#define BOUNCE_TIMER_QUEUE_H

#include <sys/timerfd.h>

#include <chrono>
#include <map>
#include <memory>

#include <bounce/channel.h>
#include <bounce/logger.h>
#include <bounce/timer.h>

namespace bounce {

class EventLoop;

class TimerQueue {
    typedef std::chrono::nanoseconds NanoSeconds;
    typedef std::chrono::system_clock SystemClock;
    typedef std::chrono::time_point<SystemClock, NanoSeconds> TimePoint;
    typedef std::function<void()> TimeOutCallback;
    typedef std::shared_ptr<Timer> TimerPtr;
    static const int NanoSecondsPerSecond = 1000000000;
public:
    explicit TimerQueue(EventLoop* loop) :
        loop_(loop),
        timer_fd_(timerfd_create(CLOCK_MONOTONIC,
                TFD_NONBLOCK | TFD_CLOEXEC)),
        timer_channel_(loop_, timer_fd_),
        min_expiration_(TimePoint::max()) {
        if (timer_fd_ < 0) {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  "
                    "timerfd_create failed, errno is {}",
                    FILENAME(__FILE__), __LINE__,
                    __FUNCTION__, errno);
        }
        timer_channel_.setReadCallback(
                std::bind(&TimerQueue::handleRead,
                        this, std::placeholders::_1));
        timer_channel_.enableReading();
    }
    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    TimerPtr addTimer(const TimePoint&,
            const NanoSeconds&,
            TimeOutCallback&&);
    void addTimerInLoop(const TimerPtr&);
    void deleteTimer(const TimerPtr&);
    void deleteTimerInLoop(const TimerPtr&);

private:
    void handleRead(time_t);
    void resetTimerfd();

    EventLoop* loop_;
    int timer_fd_;
    Channel timer_channel_;
    TimePoint min_expiration_;
    std::multimap<TimePoint, TimerPtr> timer_map_once_;
    std::multimap<TimePoint, TimerPtr> timer_map_repeat_;
};

}
#endif //BOUNCE_TIMER_QUEUE_H
