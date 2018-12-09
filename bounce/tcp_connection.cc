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
#include <bounce/event_loop.h>
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

void bounce::TcpConnection::send(const std::string& message) {
    // This mean the fd is free. Maybe can write now.
    // So we can have a try. If it can't write.
    // We will call enableWrite(). To watch writable event.
    ssize_t wrote_bytes = 0;
    std::string::size_type send_bytes = message.size();
    bool send_error = false;

    if (!channel_->isWriting() && output_buffer_.readableBytes() == 0) {
        wrote_bytes = ::send(channel_->fd(), message.c_str(), message.size(), 0);
        // send success and finish.
        if (wrote_bytes == send_bytes) {
            if (write_cb_) {
                // The function TcpConnection::send() may be called by other thread.
                // Why do not use doTaskInThread? Write complate callback maybe block the thread.
                // HAHA! return of std::bind is lval. I must write a lval function.
                loop_->queueTaskInThread(std::bind(write_cb_, shared_from_this()));
            }
            send_bytes = 0;
        } else if (wrote_bytes > 0) {
            // send success but not finish.
            send_bytes -= wrote_bytes;
        } else {
            wrote_bytes = 0;
            if (errno != EWOULDBLOCK)
            {
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    send_error = true;
                }
            }
        }

        if (!send_error && send_bytes > 0) {
            const char* str = message.c_str();
            output_buffer_.append(str + wrote_bytes, send_bytes);
            if (!channel_->isWriting()) {
                channel_->enableWriting();
            }
        }
    }
}

void bounce::TcpConnection::handleRead(time_t time) {
	int error;
	ssize_t  ret = input_buffer_.readFd(socket_->fd(), &error);
	if (ret > 0) {
        if (message_cb_ != nullptr) {
            message_cb_(shared_from_this(), &input_buffer_, time);
        }
    } else if (ret == 0) {
	    handleClose();
	} else {
	    //FIXME: Do something to deal error.
	}
}

void bounce::TcpConnection::handleWrite() {
    // if (channel_->isWriting())  Why check this?
	if (write_cb_ != nullptr) {
		write_cb_(shared_from_this());
	}
	ssize_t wrote_bytes = ::send(channel_->fd(),
	        output_buffer_.peek(),
	        output_buffer_.readableBytes(), 0);
	if (wrote_bytes > 0) {
	    output_buffer_.retrieve(wrote_bytes);
	    if (output_buffer_.readableBytes() == 0) {
	        // send finish, delete watch and call callback function.
	        channel_->disableWriting();
	        if (write_cb_) {
                loop_->queueTaskInThread(std::bind(write_cb_, shared_from_this()));
	        }
	        // FIXME: Proactively close the connection.
	        // Make sure message send finish,
	        // need check connection state.
	        // e.g. if (state == disconnected) { shutdown(); }
	    }
	    // nothing to do, wait next loop.
	}
	// FIXME: To deal wrote_bytes < 0
}

void bounce::TcpConnection::handleClose() {
	if (close_cb_ != nullptr) {
		close_cb_(shared_from_this());
	}
}

void bounce::TcpConnection::handleError() {
}