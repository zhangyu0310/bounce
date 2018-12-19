/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the PollPoller.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_POLLPOLLER_H
#define BOUNCE_POLLPOLLER_H

#include <poll.h>

#include <list>
#include <vector>

#include <bounce/poller.h>

namespace bounce {

class Channel;
class EventLoop;

class PollPoller : public Poller {
public:
	explicit  PollPoller(EventLoop* loop) :
		Poller(loop) {}
	~PollPoller() final = default;
	PollPoller(const PollPoller&) = delete;
	PollPoller& operator=(const PollPoller&) = delete;

	std::time_t poll(int timeout, ChannelList* active_channels) final;
	void updateChannel(Channel* channel) final;
	void removeChannel(Channel* channel) final;

private:
	std::vector<struct pollfd> pollfds_;
};

} // namespace bounce

#endif // !BOUNCE_POLLPOLLER_H