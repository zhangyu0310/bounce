/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the EventLoop.
* You must include this file if you want to use EventLoop.
* It's a public class, users can use this class.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_EVENTLOOP_H
#define BOUNCE_EVENTLOOP_H

#include <atomic>
#include <any/any.hpp>
#include <chrono>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <bounce/channel.h>
#include <bounce/logger.h>
#include <bounce/poller.h>
#include <bounce/timer_queue.h>

namespace bounce {

class Channel;

class EventLoop {
public:
	typedef std::chrono::nanoseconds NanoSeconds;
	typedef std::chrono::system_clock SystemClock;
	typedef std::chrono::time_point<SystemClock, NanoSeconds> TimePoint;
	typedef std::function<void()> TimeOutCallback;
	typedef std::shared_ptr<Timer> TimerPtr;
	typedef std::function<void()> Functor;
public:
	EventLoop();
	~EventLoop() = default;
	EventLoop(const EventLoop&) = delete;
	EventLoop& operator=(const EventLoop&) = delete;
	EventLoop(EventLoop&&) = delete;
	EventLoop& operator=(EventLoop&&) = delete;

	void loop();
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);

	TimerPtr runAt(const TimePoint& expiration, TimeOutCallback&& cb);
	TimerPtr runAfter(const NanoSeconds& delay, TimeOutCallback&& cb);
	TimerPtr runAfter(long delay_ns, TimeOutCallback&& cb);
	TimerPtr runEvery(const NanoSeconds& interval, TimeOutCallback&& cb);
	void deleteTimer(TimerPtr timer_ptr);

	void doTaskInThread(Functor& func);
	void queueTaskInThread(Functor& func);

	void doTaskInThread(Functor&& func);
	void queueTaskInThread(Functor&& func);

    void setContext(const linb::any& context) {
        context_ = context;
    }
    const linb::any& getContext() const {
        return context_;
    }

private:
	void doTheTasks();
	void weakupCallback(time_t);

	bool looping_;
	bool stop_;
	std::atomic<bool> doing_the_tasks_;
	std::thread::id thread_id_;
	std::unique_ptr<Poller> poller_;

	// for Poller
	typedef std::list<Channel*> ChannelList;
	ChannelList active_channels_;
	Channel* cur_channel_;

	// for timed task
	std::unique_ptr<TimerQueue> timer_queue_;

	// for async weak up
	std::mutex mutex_;
	std::vector<Functor> task_vec_;
	int weakup_fd_;
	std::unique_ptr<Channel> weak_up_channel_;
    //TODO: add context. Type: std::any
    // This is from C++17 not 11.
    // std::any context_;
    linb::any context_;
};

} // namespace bounce

#endif // BOUNCE_EVENTLOOP_H