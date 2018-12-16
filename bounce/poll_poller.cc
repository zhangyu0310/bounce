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
#include <bounce/logger.h>

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
		// FIXME: time out
	} else {
		Logger::get("bounce_file_log")->error(
				"file:{}, line:{}, function:{}  ::poll return {}, errno is {}",
				FILENAME(__FILE__), __LINE__, __FUNCTION__, r_num, errno);
	}
	using std::chrono::system_clock;
	return system_clock::to_time_t(system_clock::now());
}

void bounce::PollPoller::updateChannel(Channel* channel) {
	// the arg 'channel' is already exists. update the channel.
	if (channels_.find(channel->fd()) != channels_.end()) {
		struct pollfd &tmp = pollfds_[channel->index()];
		tmp.fd = channel->fd();
		tmp.events = channel->events();
		tmp.revents = 0;
		// TODO:If channel events == NoneEvent. ignore it.
		// tmp.fd = -tmp.fd
	} else {
		// Insert channel map and pollfds.
		channels_.insert(std::make_pair(channel->fd(), channel));
		struct pollfd tmp;
		tmp.fd = channel->fd();
		tmp.events = channel->events();
		tmp.revents = 0;
		pollfds_.push_back(tmp);
		channel->setIndex(pollfds_.size() - 1);
	}
}

void bounce::PollPoller::removeChannel(Channel* channel) {
	// FIXME:
}