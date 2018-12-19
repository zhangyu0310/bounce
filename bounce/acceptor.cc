/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Acceptor.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/acceptor.h>

#include <sys/socket.h>

bounce::Acceptor::Acceptor(EventLoop* loop,
	const SockAddress& addr) : 
	listen_sock_(new Socket(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))),
	listen_channel_(new Channel(loop, listen_sock_->fd())),
	local_addr_(addr)
{
	listen_sock_->bind(local_addr_);
}

void bounce::Acceptor::startListen() {
	listen_channel_->setReadCallback(
		std::bind(&Acceptor::handleAccept, this, std::placeholders::_1));
	listen_channel_->enableReading();
	listen_sock_->listen();
}

void bounce::Acceptor::handleAccept(time_t time) {
	SockAddress new_addr;
	int new_fd = listen_sock_->accept(&new_addr);
	// accept_cb_ is from TcpServer.newConnection(), so it isn't null.
	accept_cb_(new_fd, new_addr);
}