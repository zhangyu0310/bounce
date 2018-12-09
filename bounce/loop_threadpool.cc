/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of LoopThreadPool.
*
* Use of this source code is governed by the GNU Lesser General Public.
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

void bounce::LoopThreadPool::start() {
    started_ = true;
    thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(thread_num_));
    for (int i = 0; i < thread_num_; ++i) {
        auto loop_ptr = std::make_shared<EventLoop>();
        loops_.push_back(loop_ptr);
        if (thread_cb_ != nullptr) {
            thread_cb_(loop_ptr.get());
        }
        thread_pool_->enqueue([loop_ptr] { loop_ptr->loop(); });
    }
}

void bounce::LoopThreadPool::addThreadNumber(uint32_t num) {
    if (started_) return;
    thread_num_ = num;
}

bounce::EventLoop* bounce::LoopThreadPool::getLoopForNewConnection() {
    if (next_loop_index_ >= thread_num_) { // > is impossible
        next_loop_index_ = 0;
    }
    return loops_[next_loop_index_++].get();
}