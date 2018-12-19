/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Timer.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_TIMER_H
#define BOUNCE_TIMER_H

#include <chrono>
#include <functional>

namespace bounce {

class Timer {
    typedef std::chrono::nanoseconds NanoSeconds;
    typedef std::chrono::system_clock SystemClock;
    typedef std::chrono::time_point<SystemClock, NanoSeconds> TimePoint;
    typedef std::function<void()> TimeOutCallback;
public:
    Timer(const TimePoint& time,
            const NanoSeconds& duration,
            TimeOutCallback&& cb) :
            expiration_(time),
            duration_(duration),
            time_out_cb_(std::move(cb))
    { }
    ~Timer() = default;
    Timer(const Timer&) = default;
    Timer(Timer&&) = default;

    TimePoint expiration() { return expiration_; }
    NanoSeconds duration() { return duration_; }
    // duration is not zero, mean timer is a repeat timer.
    bool repeat() { return duration_ != NanoSeconds::zero(); }
    void execCallback() { time_out_cb_(); }
    TimePoint timerUpdate(); // just for duration_ != zero

private:
    TimePoint expiration_;
    NanoSeconds duration_;
    TimeOutCallback time_out_cb_;
};

} // namespace bounce

#endif //BOUNCE_TIMER_H
