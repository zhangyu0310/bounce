/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of TcpConnection.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/tcp_server.h>

bounce::TcpServer::TcpServer(EventLoop* loop,
	const std::string& ip,
	uint16_t port) :
	loop_(loop),
	acceptor_(loop, SockAddress(ip, port))
{
	acceptor_.setAcceptCallback(std::bind(&TcpServer::newConnection,
		this, std::placeholders::_1, std::placeholders::_2));
}

void bounce::TcpServer::start() {
	acceptor_.startListen();
}

void bounce::TcpServer::newConnection(int fd, const SockAddress& addr) {
	std::shared_ptr<TcpConnection> conn(new TcpConnection(loop_, fd));
	conn->setConnectCallback(connect_cb_);
	conn->setMessageCallback(message_cb_);
	connection_map_.insert(std::make_pair(fd, conn));
	conn->connectComplete();
}