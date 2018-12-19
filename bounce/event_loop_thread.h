/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the EventLoopThread.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_EVENT_LOOP_THREAD_H
#define BOUNCE_EVENT_LOOP_THREAD_H

#include <functional>
#include <memory>
#include <thread>

namespace bounce {

class LoopThreadPool;

class EventLoopThread {
    typedef std::function<void(void)> LoopFunction;
public:
    explicit EventLoopThread(const LoopFunction& func);
    ~EventLoopThread() = default;
    EventLoopThread(const EventLoopThread&) = delete;
    EventLoopThread(EventLoopThread&&) = delete;
    EventLoopThread& operator=(const EventLoopThread&) = delete;
    EventLoopThread& operator=(EventLoopThread&&) = delete;

    void run();

private:
    LoopFunction loop_func_;
    std::unique_ptr<std::thread> thread_;
};

} // namespace bounce

#endif //BOUNCE_EVENT_LOOP_THREAD_H