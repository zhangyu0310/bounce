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

#include <chrono>
#include <map>
#include <memory>

#include <bounce/timer.h>

namespace bounce {

class TimerQueue {
    typedef std::chrono::system_clock::time_point TimePoint;
    typedef std::chrono::milliseconds MilliSeconds;
    typedef std::function<void()> TimeOutCallback;
    typedef std::shared_ptr<Timer> TimerPtr;
public:
private:
    std::multimap<TimePoint, TimerPtr>  timer_map_;
};

}
#endif //BOUNCE_TIMER_QUEUE_H
