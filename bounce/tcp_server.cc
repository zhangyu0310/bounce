/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of TcpConnection.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/tcp_server.h>

#include <signal.h>

bounce::TcpServer::TcpServer(EventLoop* loop,
	const std::string& ip,
	uint16_t port,
	uint32_t thread_num) :
	started_(false),
	loop_(loop),
	acceptor_(loop, SockAddress(ip, port)),
	thread_pool_(new LoopThreadPool(loop_, thread_num))
{
	::signal(SIGPIPE, SIG_IGN);
	acceptor_.setAcceptCallback(std::bind(&TcpServer::newConnection,
		this, std::placeholders::_1, std::placeholders::_2));
}

void bounce::TcpServer::start() {
	if (thread_init_cb_) {
		thread_pool_->addInitThreadCallback(
				std::bind(thread_init_cb_, std::placeholders::_1));
	}
	thread_pool_->start();
	acceptor_.startListen();
	started_ = true;
}

void bounce::TcpServer::setThreadNumber(uint32_t num) {
	if (!started_) {
		thread_pool_->setThreadNumber(num);
	}
}

void bounce::TcpServer::newConnection(int fd, const SockAddress& addr) {
	// loop is a thread pool. Not only main loop.
    // If there is no thread in threadpool. Use main loop.
	EventLoop* conn_loop = nullptr;
	if (thread_pool_->getThreadNumber() == 0) {
		conn_loop = loop_;
	} else {
		conn_loop = thread_pool_->getLoopForNewConnection();
	}
	if (conn_loop == nullptr) {
	    Logger::get("bounce_file_log")->error(
	            "file:{}, line:{}, function:{}  Can't get a connection loop.",
				FILENAME(__FILE__), __LINE__, __FUNCTION__);
	    conn_loop = loop_;
	}
	SockAddress local_addr = getLocalAddr(fd);
	std::shared_ptr<TcpConnection> conn(
	        new TcpConnection(conn_loop, fd, local_addr, addr));
	conn->setConnectCallback(connect_cb_);
	conn->setMessageCallback(message_cb_);
	conn->setWriteCompleteCallback(write_cb_);
	conn->setCloseCallback(std::bind(
			&TcpServer::removeConnection, this,
			std::placeholders::_1));
	conn->setErrorCallback(error_cb_);
	connection_map_.insert(std::make_pair(fd, conn));
	// This function is distribution of Connection.
	conn_loop->doTaskInThread(
	        std::bind(&TcpConnection::connectComplete, conn));
}

void bounce::TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    loop_->doTaskInThread(std::bind(
            &TcpServer::removeConnectionInLoop, this, conn));
}

void bounce::TcpServer::removeConnectionInLoop(
        const bounce::TcpServer::TcpConnectionPtr & conn) {
    auto it = connection_map_.find(conn->getFd());
    if (it != connection_map_.end()) {
        connection_map_.erase(it);
        // If there is only one thread, must use queueTaskInThread.
        conn->getLoop()->queueTaskInThread(
                std::bind(&TcpConnection::destroyConnection, conn));
    } else {
        Logger::get("bounce_file_log")->error(
                "file:{}, line:{}, function:{}  "
                "connection map can't find this connection.",
                FILENAME(__FILE__), __LINE__, __FUNCTION__);
    }
}
