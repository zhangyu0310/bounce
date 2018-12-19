/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Acceptor.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_ACCEPTOR_H
#define BOUNCE_ACCEPTOR_H

#include <functional>
#include <memory>

#include <bounce/channel.h>
#include <bounce/socket.h>
#include <bounce/sockaddr.h>

namespace bounce {

class EventLoop;

class Acceptor {
	typedef std::function<void(int fd, const SockAddress&)> AcceptCallback;
public:
	Acceptor(EventLoop*, const SockAddress&);
	~Acceptor() = default;
	Acceptor(const Acceptor&) = delete;
	Acceptor& operator=(const Acceptor&) = delete;
	void setAcceptCallback(const AcceptCallback& cb) { accept_cb_ = cb; }
	void startListen();

private:
	void handleAccept(time_t time);

	AcceptCallback accept_cb_;
	std::unique_ptr<Socket> listen_sock_;
	std::unique_ptr<Channel> listen_channel_;
	SockAddress local_addr_;
};

} // namespace bounce

#endif // !BOUNCE_ACCEPTOR_H