/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of EventLoop.
* It's a public class, users can use this class.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/event_loop.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/eventfd.h>

#include <bounce/channel.h>
#include <bounce/poll_poller.h>

bounce::EventLoop::EventLoop() :
	looping_(false),
	stop_(false),
	doing_the_tasks_(false),
	thread_id_(std::this_thread::get_id()),
	poller_(new PollPoller(this)),
	timer_queue_(new TimerQueue(this)),
	weakup_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
	weak_up_channel_(new Channel(this, weakup_fd_)) {
	if (weakup_fd_ == -1) {
		Logger::get("bounce_file_log")->critical(
		        "file:{}, line:{}, function:{}  Weak up channel fd is -1",
                FILENAME(__FILE__), __LINE__, __FUNCTION__);
		exit(-1);
	}
	weak_up_channel_->setReadCallback(std::bind(
			&EventLoop::weakupCallback,
			this, std::placeholders::_1));
	weak_up_channel_->enableReading();
}

void bounce::EventLoop::loop() {
	looping_ = true;
	while (!stop_) {
		active_channels_.clear();
		time_t recv_time = poller_->poll(-1, &active_channels_);
		for (auto it = active_channels_.begin();
			it != active_channels_.end(); ++it) {
			cur_channel_ = *it;
			cur_channel_->handleChannel(recv_time);
		}
		cur_channel_ = nullptr;
		doTheTasks();
	}
}

bounce::EventLoop::TimerPtr bounce::EventLoop::runAt(
		const bounce::EventLoop::TimePoint& expiration,
		bounce::EventLoop::TimeOutCallback&& cb) {
	return timer_queue_->addTimer(expiration,
			NanoSeconds::zero(), std::move(cb));
}

bounce::EventLoop::TimerPtr bounce::EventLoop::runAfter(
		const bounce::EventLoop::NanoSeconds& delay,
		bounce::EventLoop::TimeOutCallback&& cb) {
	return timer_queue_->addTimer(
			SystemClock::now() + delay, NanoSeconds::zero(), std::move(cb));
}

bounce::EventLoop::TimerPtr bounce::EventLoop::runAfter(
		long delay_ns, bounce::EventLoop::TimeOutCallback &&cb) {
	return timer_queue_->addTimer(
			SystemClock::now() + NanoSeconds(delay_ns),
			NanoSeconds::zero(), std::move(cb));
}

bounce::EventLoop::TimerPtr bounce::EventLoop::runEvery(
		const bounce::EventLoop::NanoSeconds &interval,
		bounce::EventLoop::TimeOutCallback &&cb) {
	return timer_queue_->addTimer(
			SystemClock::now() + interval, interval, std::move(cb));
}

void bounce::EventLoop::deleteTimer(bounce::EventLoop::TimerPtr timer_ptr) {
	timer_queue_->deleteTimer(timer_ptr);
}

void bounce::EventLoop::updateChannel(Channel* channel) {
	poller_->updateChannel(channel);
}

void bounce::EventLoop::removeChannel(Channel* channel) {
	poller_->removeChannel(channel);
}

void bounce::EventLoop::doTaskInThread(bounce::EventLoop::Functor& func) {
	if(thread_id_ == std::this_thread::get_id()) {
		func();
	} else {
		queueTaskInThread(func);
	}
}

void bounce::EventLoop::queueTaskInThread(Functor& func) {
	{
		std::lock_guard<std::mutex> guard(mutex_);
		task_vec_.push_back(func);
	}
	// wake up thread
    if ((std::this_thread::get_id() != thread_id_) && !doing_the_tasks_) {
        uint64_t one = 1;
        ssize_t n = ::write(weakup_fd_, &one, sizeof one);
        if (n != sizeof one)
        {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  "
					"write {} bytes instead of 8.",
                    FILENAME(__FILE__), __LINE__, __FUNCTION__, n);
        }
    }
}

void bounce::EventLoop::doTaskInThread(bounce::EventLoop::Functor&& func) {
	if(thread_id_ == std::this_thread::get_id()) {
		func();
	} else {
		queueTaskInThread(std::move(func));
	}
}

void bounce::EventLoop::queueTaskInThread(Functor&& func) {
	{
		std::lock_guard<std::mutex> guard(mutex_);
		task_vec_.push_back(std::move(func));
	}
	// wake up thread
    if ((std::this_thread::get_id() != thread_id_) && !doing_the_tasks_) {
        uint64_t one = 1;
        ssize_t n = ::write(weakup_fd_, &one, sizeof one);
        if (n != sizeof one)
        {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  "
					"write {} bytes instead of 8.",
                    FILENAME(__FILE__), __LINE__, __FUNCTION__, n);
        }
    }
}

void bounce::EventLoop::doTheTasks() {
	doing_the_tasks_ = true;
	std::vector<Functor> tmp_vec;
	{
		std::lock_guard<std::mutex> guard(mutex_);
		task_vec_.swap(tmp_vec);
	}
	for (auto& task : tmp_vec) {
		task();
	}
	doing_the_tasks_ = false;
}

void bounce::EventLoop::weakupCallback(time_t) {
	uint64_t one = 1;
	ssize_t n = ::read(weakup_fd_, &one, sizeof one);
	if (n != sizeof one)
	{
		Logger::get("bounce_file_log")->error(
				"file:{}, line:{}, function:{}  "
	            "reads {} bytes instead of 8.",
				FILENAME(__FILE__), __LINE__, __FUNCTION__, n);
	}
}