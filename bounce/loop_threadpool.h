/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the LoopThreadPool.
* This is an internal header file, users should not include this.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_LOOP_THREADPOOL_H
#define BOUNCE_LOOP_THREADPOOL_H

#include <functional>
#include <memory>
#include <vector>

#include <bounce/event_loop.h>
#include <ThreadPool/ThreadPool.h>

namespace bounce {

class LoopThreadPool {
    typedef std::shared_ptr<EventLoop> EventLoopPtr;
    typedef std::function<void(EventLoop*)> InitThreadCallback;
public:
    LoopThreadPool(EventLoop* loop, uint32_t thread_num);
    ~LoopThreadPool() {
        // nothing to do. EventLoops are shared_ptr.
        // vector :loops_ destruct, EventLoops will destruct auto.
    }
    LoopThreadPool(const LoopThreadPool&) = delete;
    LoopThreadPool& operator=(const LoopThreadPool&) = delete;

    void start();
    void addThreadNumber(uint32_t num);
    void addInitThreadCallback(const InitThreadCallback& cb)
    { thread_cb_ = cb; }
    EventLoop* getLoopForNewConnection();
    uint32_t getThreadNumber() const { return thread_num_; }

private:
    bool started_;
    EventLoop* base_loop_;
    uint32_t thread_num_;
    uint32_t next_loop_index_;
    InitThreadCallback thread_cb_;
    std::unique_ptr<ThreadPool> thread_pool_;
    std::vector<EventLoopPtr> loops_;
};

} // namespace bounce

#endif //BOUNCE_LOOP_THREADPOOL_H
