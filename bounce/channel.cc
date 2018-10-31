/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Channel.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/channel.h>

#include <poll.h>

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