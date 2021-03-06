/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of LoopThreadPool.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <utility>

#include <bounce/loop_threadpool.h>

#include <bounce/event_loop.h>

bounce::LoopThreadPool::LoopThreadPool(
        bounce::EventLoop *loop,
        uint32_t thread_num) :
        started_(false),
        base_loop_(loop),
        thread_num_(thread_num),
        next_loop_index_(0),
        loop_inited_(false) { }

void bounce::LoopThreadPool::threadLoop() {
    EventLoop event_loop;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        loops_.push_back(&event_loop);
        loop_inited_ = true;
    }
    condition_.notify_one();
    if (thread_cb_ != nullptr) {
        thread_cb_(&event_loop);
    }
    event_loop.loop();
}

void bounce::LoopThreadPool::start() {
    started_ = true;
    for (uint32_t i = 0; i < thread_num_; ++i) {
        EventLoopThreadPtr loop_thread(
               new EventLoopThread(std::bind(&LoopThreadPool::threadLoop, this)));
        loop_thread->run();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]{ return loop_inited_.load(); });
            threads_.push_back(loop_thread);
            loop_inited_ = false;
        }
    }
}

void bounce::LoopThreadPool::setThreadNumber(uint32_t num) {
    if (started_) return;
    thread_num_ = num;
}

bounce::EventLoop* bounce::LoopThreadPool::getLoopForNewConnection() {
    if (next_loop_index_ >= thread_num_) { // > is impossible
        next_loop_index_ = 0;
    }
    return loops_[next_loop_index_++];
}