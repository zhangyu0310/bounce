/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the TcpConnection.
* You must include this file if you want to use TcpConnection.
* It's a public class, users can use this class.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_TCPCONNECTION_H
#define BOUNCE_TCPCONNECTION_H

#include <atomic>
#include <any/any.hpp>
#include <functional>
#include <memory>

#include <bounce/buffer.h>
#include <bounce/channel.h>
#include <bounce/socket.h>
#include <bounce/logger.h>

namespace bounce {

class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
	typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
	typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef std::function<void(const TcpConnectionPtr&)> ErrorCallback;
	typedef std::function<void(const TcpConnectionPtr&, Buffer*, time_t)> MessageCallback;
public:
	enum ConnectState {
		connecting,
		connected,
		disconnecting,
		disconnected };
public:
	TcpConnection(EventLoop* loop, int fd);
	~TcpConnection() = default;
	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(const TcpConnection&) = delete;

	EventLoop* getLoop() const { return loop_; }
	int getFd() const { return socket_->fd(); }
	ConnectState state() const { return state_; }

	void setConnectCallback(const ConnectionCallback& cb) {
		connect_cb_ = cb;
	}
	void setMessageCallback(const MessageCallback& cb) {
		message_cb_ = cb;
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
		write_cb_ = cb;
	}
	void setCloseCallback(const CloseCallback& cb) {
		close_cb_ = cb;
	}
	void setErrorCallback(const ErrorCallback& cb) {
		error_cb_ = cb;
	}

	void connectComplete();
	void destroyConnection();

	void send(const std::string& message);
	void send(Buffer& buffer);
	void sendInLoop(const std::string& message);
	void shutdown();
	void shutdownInLoop();
	void forceClose();
	//void forceCloseDelay(size_t seconds);
	void forceCloseInLoop();

	void setContext(const linb::any& context) {
		context_ = context;
	}
	const linb::any& getContext() const {
		return context_;
	}

private:
	void handleRead(time_t time);
	void handleWrite();
	void handleClose();
	void handleError();

	std::atomic<ConnectState> state_;
	EventLoop* loop_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	Buffer input_buffer_;
	Buffer output_buffer_;
	ConnectionCallback connect_cb_;
	MessageCallback message_cb_;
	WriteCompleteCallback write_cb_;
	CloseCallback close_cb_;
	ErrorCallback error_cb_;
	//TODO: add context. Type: std::any
	// This is from C++17 not 11.
	// std::any context_;
	linb::any context_;
};

} // namespace bounce

#endif // !BOUNCE_TCPCONNECTION_H