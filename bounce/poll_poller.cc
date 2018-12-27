/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of PollPoller.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/poll_poller.h>

#include <algorithm>
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
		// If channel events == NoneEvent. ignore it.
		// Why set fd = -fd?
		// man poll :
		/*
		 * The field events is an input  parameter,  a  bit  mask  specifying  the
		 * events  the  application  is  interested in for the file descriptor fd.
		 * This field may be specified as zero, in which case the only events that
		 * can  be  returned  in  revents  are POLLHUP, POLLERR, and POLLNVAL (see
		 * below).
		 */
		if (channel->isNoneEvent()) {
			// if tmp.fd == 0, must sub 1.
			tmp.fd = -(tmp.fd) - 1;
		}
	} else {
		// Insert channel map and pollfds.
		channels_.insert(std::make_pair(channel->fd(), channel));
		struct pollfd tmp{-1, 0, 0};
		tmp.fd = channel->fd();
		tmp.events = channel->events();
		tmp.revents = 0;
		pollfds_.push_back(tmp);
		channel->setIndex(pollfds_.size() - 1);
	}
}

void bounce::PollPoller::removeChannel(Channel* channel) {
	auto it = channels_.find(channel->fd());
	if (it != channels_.end()) {
		channels_.erase(it);
		unsigned long idx = channel->index();
		auto last = pollfds_.size() - 1;
		// if idx == last - 1 pop_back directly.
		if (idx != last) {
			// Give last channel the drop index.
			// Fill in space.
			channels_[pollfds_[last].fd]->setIndex(idx);
			std::iter_swap(&pollfds_[idx], &pollfds_[last]);
		}
		pollfds_.pop_back();
	} else {
		Logger::get("bounce_file_log")->critical(
				"file:{}, line:{}, function:{}  removeChannel can't find",
				FILENAME(__FILE__), __LINE__, __FUNCTION__);
	}
}