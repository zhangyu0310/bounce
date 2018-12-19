/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Timer.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/timer.h>

bounce::Timer::TimePoint bounce::Timer::timerUpdate() {
    if (duration_ != NanoSeconds::zero())
        expiration_ += duration_;
    return expiration_;
}