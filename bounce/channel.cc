/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Channel.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/channel.h>
#include <bounce/event_loop.h>

#include <poll.h>

const short int bounce::Channel::kNoneEvent = 0;
const short int bounce::Channel::kReadEvent = POLLIN | POLLPRI;
const short int bounce::Channel::kWriteEvent = POLLOUT;

void bounce::Channel::handleChannel(time_t recv_time) {
	// fd is closed and nothing to read.
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
		if (close_cb_) close_cb_();
	}
	// fd is error.
	if (revents_ & (POLLERR | POLLNVAL)) {
		if (error_cb_) error_cb_();
	}
	// fd can be read or RDHUP.
	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
		if (read_cb_) read_cb_(recv_time);
	}
	if (revents_ & POLLOUT) {
		if (write_cb_) write_cb_();
	}
}

void bounce::Channel::update() {
	loop_->updateChannel(this);
}

void bounce::Channel::remove() {
	loop_->removeChannel(this);
}