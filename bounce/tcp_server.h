/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the TcpServer.
* You must include this file if you want to use TcpServer.
* It's a public class, users can use this class.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_TCPSERVER_H
#define BOUNCE_TCPSERVER_H

#include <map>

#include <bounce/acceptor.h>
#include <bounce/sockaddr.h>
#include <bounce/tcp_connection.h>
#include <bounce/loop_threadpool.h>

namespace bounce {

class EventLoop;

class TcpServer {
public:
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(EventLoop*)> ThreadInitCallback;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
	// Close call back is no use, Connection close should call connection back.
	//typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
	typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef std::function<void(const TcpConnectionPtr&)> ErrorCallback;
	typedef std::function<void(const TcpConnectionPtr&, Buffer*, time_t)> MessageCallback;
public:
	TcpServer(EventLoop* loop,
	        const std::string& ip,
	        uint16_t port,
	        uint32_t thread_num = 0);
	~TcpServer() = default;
	TcpServer(const TcpServer&) = delete;
	TcpServer& operator=(const TcpServer&) = delete;
	TcpServer(TcpServer&&) = delete;
	TcpServer& operator=(TcpServer&&) = delete;

	void start();
	void setThreadNumber(uint32_t num);
	void setThreadInitCallback(const ThreadInitCallback& cb) { 
	    thread_init_cb_ = cb; 
	}
	void setConnectionCallback(const ConnectionCallback& cb){ 
	    connect_cb_ = cb; 
	}
	void setMessageCallback(const MessageCallback& cb) { 
	    message_cb_ = cb; 
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) { 
	    write_cb_ = cb; 
	}
	void setErrorCallback(const ErrorCallback& cb) {
		error_cb_ = cb;
	}
	void setNewConnection(int fd, const SockAddress& addr) {
		loop_->queueTaskInThread(std::bind(
		        &TcpServer::newConnection, this, fd, addr));
	}

	uint32_t getThreadNumber() { 
	    return thread_pool_->getThreadNumber(); 
	}
	bool started() {
	    return started_;
	}
	
private:
	void newConnection(int fd, const SockAddress& addr);
	void removeConnection(const TcpConnectionPtr&);
	void removeConnectionInLoop(const TcpConnectionPtr&);

	bool started_;
	EventLoop* loop_;
	Acceptor acceptor_;
	ThreadInitCallback thread_init_cb_;
	ConnectionCallback connect_cb_;
	MessageCallback message_cb_;
	WriteCompleteCallback write_cb_;
	ErrorCallback error_cb_;

	std::unique_ptr<LoopThreadPool> thread_pool_;
	std::map<int, TcpConnectionPtr> connection_map_;
};

} // namespace bounce

#endif // !BOUNCE_TCPSERVER_H