/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of PollPoller.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/poll_poller.h>

#include <chrono>

#include <bounce/channel.h>

std::time_t bounce::PollPoller::poll(int timeout, 
	ChannelList* active_channels) {
	int r_num = ::poll(pollfds_.data(), pollfds_.size(), timeout);
	if (r_num > 0) {
		// Fill channels -- revents and active_channels.
		int count = 0;
		for (auto it = pollfds_.begin(); it != pollfds_.end(); ++it) {
			if (count >= r_num) {
				break;
			}
			if (it->revents > 0) {
				auto m_it = channels_.find(it->fd);
				(m_it->second)->setRevents(it->revents);
				active_channels->push_back(m_it->second);
			}
		}
	} else if (r_num == 0) {
		// time out
	} else {
		// FIXME: r_num < 0  show the error.
	}
	using std::chrono::system_clock;
	return system_clock::to_time_t(system_clock::now());
}

void bounce::PollPoller::updateChannel(Channel* channel) {
	// the arg 'channel' is already exists. update the channel.
	if (channels_.find(channel->fd()) != channels_.end()) {
		// FIXME: I don't know how to do.
	} else {
		// Insert channel map and pollfds.
		channels_.insert(std::make_pair(channel->fd(), channel));
		struct pollfd tmp;
		tmp.fd = channel->fd();
		tmp.events = channel->events();
		tmp.revents = 0;
		pollfds_.push_back(tmp);
	}
}

void bounce::PollPoller::removeChannel(Channel* channel) {
	// TODO:
}