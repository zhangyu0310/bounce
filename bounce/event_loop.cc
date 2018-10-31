/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of EventLoop.
* It's a public class, users can use this class.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/event_loop.h>

#include <bounce/channel.h>
#include <bounce/poll_poller.h>

bounce::EventLoop::EventLoop() :
	looping_(false),
	stop_(false),
	poller_(new PollPoller(this))
{}

void bounce::EventLoop::loop() {
	looping_ = true;
	while (!stop_) {
		active_channels_.clear();
		// TODO: timeout can be set.
		time_t recv_time = poller_->poll(-1, &active_channels_);
		for (auto it = active_channels_.begin();
			it != active_channels_.end(); ++it) {
			cur_channel_ = *it;
			cur_channel_->handleChannel(recv_time);
		}
		cur_channel_ = NULL;
	}
}

void bounce::EventLoop::updateChannel(Channel* channel) {
	poller_->updateChannel(channel);
}