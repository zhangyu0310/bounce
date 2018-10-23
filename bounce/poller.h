/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Poller.
* Poller is an abstract class.
* This is an internal header file, users should not include this.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_POLLER_H
#define BOUNCE_POLLER_H

#include <chrono>
#include <map>

#include <bounce/channel.h>
#include <bounce/event_loop.h>

namespace bounce {

class Poller {
public:
	Poller();
	virtual ~Poller();

	virtual std::time_t poll() = 0;
	virtual void updateChannel() = 0;
	virtual void removeChannel() = 0;

protected:
	typedef std::map<int, Channel> ChannelMap;
	ChannelMap channels_;

private:
	EventLoop* loop_;
};

} // namespace bounce

#endif // !BOUNCE_POLLER_H