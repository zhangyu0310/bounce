/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of TcpConnection.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/channel.h>
#include <bounce/socket.h>
#include <bounce/tcp_connection.h>


bounce::TcpConnection::TcpConnection(EventLoop* loop, int fd) :
	loop_(loop),
	socket_(new Socket(fd)),
	channel_(new Channel(loop, fd))
{
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

void bounce::TcpConnection::connectComplete() {
	channel_->enableReading();
	connect_cb_(shared_from_this());
}

void bounce::TcpConnection::handleRead(time_t time) {
	int error;
	input_buffer_.readFd(socket_->fd(), &error);
	if (message_cb_ != nullptr) {
		message_cb_(shared_from_this(), &input_buffer_, time);
	}
}

void bounce::TcpConnection::handleWrite() {
	if (write_cb_ != nullptr) {
		write_cb_(shared_from_this());
	}
}

void bounce::TcpConnection::handleClose() {
	if (close_cb_ != nullptr) {
		close_cb_(shared_from_this());
	}
}

void bounce::TcpConnection::handleError() {
}