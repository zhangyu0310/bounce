/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of EventLoopThread.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/event_loop_thread.h>
#include <bounce/logger.h>

bounce::EventLoopThread::EventLoopThread(
        const bounce::EventLoopThread::LoopFunction &func) :
        loop_func_(func) {}

void bounce::EventLoopThread::run() {
    if (loop_func_ == nullptr) {
        spdlog::get("bounce_file_log")->critical(
                "file:{}, line:{}, function:{}  thread loop function is null",
                FILENAME(__FILE__), __LINE__, __FUNCTION__);
        exit(1);
    }
    thread_ = std::unique_ptr<std::thread>(new std::thread(loop_func_));
}