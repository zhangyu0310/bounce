/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the TcpConnection.
* You must include this file if you want to use TcpConnection.
* It's a public class, users can use this class.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_TCPCONNECTION_H
#define BOUNCE_TCPCONNECTION_H

#include <functional>
#include <memory>

#include <bounce/buffer.h>
#include <bounce/channel.h>
#include <bounce/socket.h>

namespace bounce {

class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
	typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
	typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef std::function<void(const TcpConnectionPtr&, Buffer*, time_t)> MessageCallback;
public:
	TcpConnection(EventLoop* loop, int fd);
	// ~TcpConnection();
	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(const TcpConnection&) = delete;
	EventLoop* getLoop() const { return loop_; }

	void setConnectCallback(const ConnectionCallback& cb)
	{ connect_cb_ = cb; }
	void setMessageCallback(const MessageCallback& cb)
	{ message_cb_ = cb; }
	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{ write_cb_ = cb; }
	void setCloseCallback(const CloseCallback& cb) 
	{ close_cb_ = cb; }

	void connectComplete();
	// void disconnectComplete();

	void send(const std::string& message);

private:
	void handleRead(time_t time);
	void handleWrite();
	void handleClose();
	void handleError();

	// FIXME: State of connection.
	EventLoop* loop_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	Buffer input_buffer_;
	Buffer output_buffer_;
	ConnectionCallback connect_cb_;
	MessageCallback message_cb_;
	WriteCompleteCallback write_cb_;
	CloseCallback close_cb_;
	// error_cb_;
	//FIXME: add context. Type: std::any
	// std::any context_;
};

} // namespace bounce

#endif // !BOUNCE_TCPCONNECTION_H