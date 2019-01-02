/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the LoopThreadPool.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_LOOP_THREADPOOL_H
#define BOUNCE_LOOP_THREADPOOL_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

#include <bounce/event_loop.h>
#include <bounce/event_loop_thread.h>
//#include <ThreadPool/ThreadPool.h>

namespace bounce {

class LoopThreadPool {
    typedef std::shared_ptr<EventLoopThread> EventLoopThreadPtr;
    typedef std::function<void(EventLoop*)> InitThreadCallback;
public:
    LoopThreadPool(EventLoop* loop, uint32_t thread_num);
    ~LoopThreadPool() = default;
    LoopThreadPool(const LoopThreadPool&) = delete;
    LoopThreadPool& operator=(const LoopThreadPool&) = delete;

    void start();
    void setThreadNumber(uint32_t num);
    void addInitThreadCallback(const InitThreadCallback& cb)
    { thread_cb_ = cb; }
    EventLoop* getLoopForNewConnection();
    uint32_t getThreadNumber() const { return thread_num_; }
    void threadLoop();

private:
    bool started_;
    EventLoop* base_loop_;
    uint32_t thread_num_;
    uint32_t next_loop_index_;
    InitThreadCallback thread_cb_;
    std::vector<EventLoopThreadPtr> threads_;
    std::mutex mutex_;
    std::atomic<bool> loop_inited_;
    std::condition_variable condition_;
    std::vector<EventLoop*> loops_;
};

} // namespace bounce

#endif //BOUNCE_LOOP_THREADPOOL_H
