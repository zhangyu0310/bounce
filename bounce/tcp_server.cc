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
	uint16_t port,
	uint32_t thread_num) :
	loop_(loop),
	acceptor_(loop, SockAddress(ip, port)),
	thread_pool_(new LoopThreadPool(loop_, thread_num))
{
	acceptor_.setAcceptCallback(std::bind(&TcpServer::newConnection,
		this, std::placeholders::_1, std::placeholders::_2));
}

void bounce::TcpServer::start() {
	thread_pool_->start();
	acceptor_.startListen();
}

void bounce::TcpServer::newConnection(int fd, const SockAddress& addr) {
	// FIXME: loop is a thread pool. Not only main loop.
	EventLoop* conn_loop = nullptr;
	if (thread_pool_->getThreadNumber() == 0) {
		conn_loop = loop_;
	} else {
		// If there is no thread in threadpool. Use main loop.
		conn_loop = thread_pool_->getLoopForNewConnection();
	}
	std::shared_ptr<TcpConnection> conn(new TcpConnection(conn_loop, fd));
	conn->setConnectCallback(connect_cb_);
	conn->setMessageCallback(message_cb_);
	conn->setWriteCompleteCallback(write_cb_);
	conn->setCloseCallback(close_cb_);
	// TODO: setErrorCallback()
	connection_map_.insert(std::make_pair(fd, conn));
	conn->connectComplete();
}