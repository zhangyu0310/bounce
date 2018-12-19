/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Channel.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
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
	static const short int kNoneEvent;
	static const short int kReadEvent;
	static const short int kWriteEvent;
public:
	Channel(EventLoop* loop, int fd) :
	    fd_(fd),
	    events_(0),
	    revents_(0),
		loop_(loop),
		index_(0)
	{}
	~Channel() = default;
	Channel(const Channel&) = delete;
	Channel& operator=(const Channel&) = delete;

	void handleChannel(time_t recv_time);
	int fd() const { return fd_; }
	short int events() const { return events_; }
	short int revents() const { return revents_; }

	// Be replaced by enableXX & disableXX.
	//void setEvents(short int events) { events_ = events; }
	void setRevents(short int revents) { revents_ = revents; }
	void setReadCallback(const ReadEventCallback& cb) { read_cb_ = cb; }
	void setWriteCallback(const EventCallback& cb) { write_cb_ = cb; }
	void setCloseCallback(const EventCallback& cb) { close_cb_ = cb; }
	void setErrorCallback(const EventCallback& cb) { error_cb_ = cb; }
	void setIndex(size_t index) { index_ = index; }

	void enableReading() { events_ |= kReadEvent; update(); }
	void disableReading() { events_ &= ~kReadEvent; update(); }
	void enableWriting() { events_ |= kWriteEvent; update(); }
	void disableWriting() { events_ &= ~kWriteEvent; update(); }
	void disableAll() { events_ = kNoneEvent; update(); }
	bool isWriting() const { return static_cast<bool>(events_ & kWriteEvent); }
	bool isReading() const { return static_cast<bool>(events_ & kReadEvent); }
	bool isNoneEvent() const { return events_ == kNoneEvent; }
	size_t index() { return index_; }

	void remove();

private:
	void update();

	const int fd_;
	short int events_;
	short int revents_;
	EventLoop* loop_;
	size_t index_;

	ReadEventCallback read_cb_;
	EventCallback write_cb_;
	EventCallback close_cb_;
	EventCallback error_cb_;
};

} // namespace bounce

#endif // !BOUNCE_CHANNEL_H