/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Channel.
* This is an internal header file, users should not include this.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_CHANNEL_H
#define BOUNCE_CHANNEL_H

#include <chrono>
#include <functional>

namespace bounce {

class EventLoop;

class Channel {
	typedef std::function<void(void)> EventCallback;
	typedef std::function<void(time_t)> ReadEventCallback;
public:
	Channel(EventLoop* loop, int fd) :
	    fd_(fd),
		loop_(loop) 
	{}
	~Channel() {}
	Channel(const Channel&) = delete;
	Channel& operator=(const Channel&) = delete;

	void handleChannel(time_t recv_time);
	int fd() const { return fd_; }
	short int events() const { return events_; }
	short int revents() const { return revents_; }
	void setEvents(short int events) { events_ = events; }
	void setRevents(short int revents) { revents_ = revents; }
	void setReadCallback(ReadEventCallback cb) { read_cb_ = cb; }
	void setWriteCallback(EventCallback cb) { write_cb_ = cb; }
	void setCloseCallback(EventCallback cb) { close_cb_ = cb; }
	void setErrorCallback(EventCallback cb) { error_cb_ = cb; }

private:
	const int fd_;
	short int events_;
	short int revents_;
	EventLoop* loop_;

	ReadEventCallback read_cb_;
	EventCallback write_cb_;
	EventCallback close_cb_;
	EventCallback error_cb_;
};

} // namespace bounce

#endif // !BOUNCE_CHANNEL_H