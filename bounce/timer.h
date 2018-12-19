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
    typedef std::chrono::system_clock::time_point TimePoint;
    typedef std::chrono::milliseconds MilliSeconds;
    typedef std::function<void()> TimeOutCallback;
public:
    Timer(const TimePoint& time,
            const MilliSeconds& duration,
            TimeOutCallback&& cb) :
            time_(time),
            duration_(duration),
            time_out_cb_(std::move(cb))
    { }
    ~Timer() = default;
    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;

private:
    TimePoint time_;
    MilliSeconds duration_;
    TimeOutCallback time_out_cb_;
};

} // namespace bounce

#endif //BOUNCE_TIMER_H
