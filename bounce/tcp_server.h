/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the TcpServer.
* You must include this file if you want to use TcpServer.
* It's a public class, users can use this class.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_TCPSERVER_H
#define BOUNCE_TCPSERVER_H

#include <map>

#include <bounce/acceptor.h>
#include <bounce/sockaddr.h>
#include <bounce/tcp_connection.h>

namespace bounce {

class EventLoop;

class TcpServer {
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
	typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
	typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef std::function<void(const TcpConnectionPtr&, Buffer*, time_t)> MessageCallback;
public:
	TcpServer(EventLoop* loop, const std::string& ip, uint16_t port);
	//~TcpServer();
	void start();
	void setConnectionCallback(const ConnectionCallback& cb) { connect_cb_ = cb; }
	void setMessageCallback(const MessageCallback& cb) { message_cb_ = cb; }
	void setWriteCallback(const WriteCompleteCallback& cb) { write_cb_ = cb; }
	void setCloseCallback(const CloseCallback& cb) { close_cb_ = cb; }
	
private:
	void newConnection(int fd, const SockAddress& addr);

	EventLoop* loop_;
	Acceptor acceptor_;
	ConnectionCallback connect_cb_;
	MessageCallback message_cb_;
	WriteCompleteCallback write_cb_;
	CloseCallback close_cb_;
	std::map<int, TcpConnectionPtr> connection_map_;
};

} // namespace bounce

#endif // !BOUNCE_TCPSERVER_H