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
        next_loop_index_(0) { }

void bounce::LoopThreadPool::threadLoop() {
    EventLoop event_loop;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        loops_.push_back(&event_loop);
    }
    if (thread_cb_ != nullptr) {
        thread_cb_(&event_loop);
    }
    event_loop.loop();
}

void bounce::LoopThreadPool::start() {
    started_ = true;
    //thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(thread_num_));
    for (uint32_t i = 0; i < thread_num_; ++i) {
        //thread_pool_->enqueue(std::bind(&LoopThreadPool::threadLoop, this));
        EventLoopThreadPtr loop_thread(
               new EventLoopThread(std::bind(&LoopThreadPool::threadLoop, this)));
        loop_thread->run();
        threads_.push_back(loop_thread);
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