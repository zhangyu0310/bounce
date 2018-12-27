/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of TcpConnection.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/channel.h>
#include <bounce/event_loop.h>
#include <bounce/socket.h>
#include <bounce/tcp_connection.h>


bounce::TcpConnection::TcpConnection(EventLoop* loop, int fd) :
    state_(connecting),
	loop_(loop),
	socket_(new Socket(fd)),
	channel_(new Channel(loop, fd)),
	context_()
{
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

void bounce::TcpConnection::connectComplete() {
    state_ = connected;
	channel_->enableReading();
	connect_cb_(shared_from_this());
}

void bounce::TcpConnection::destroyConnection() {
    if (state_ == connected) {
        state_ = disconnected;
        if (connect_cb_)
            connect_cb_(shared_from_this());
    }
    channel_->remove();
}

// Why must send in loop. If two loop want to send to this connection
// together. We must ensure the order of send. So must in a loop.
void bounce::TcpConnection::send(const std::string& message) {
    if (state_ == connected) {
        loop_->doTaskInThread(std::bind(
                &TcpConnection::sendInLoop, this, message));
    } else {
        Logger::get("bounce_file_log")->info(
                "file:{}, line:{}, function:{}  Connection is over, can't send.",
                FILENAME(__FILE__), __LINE__, __FUNCTION__);
    }
}

void bounce::TcpConnection::send(Buffer& buffer) {
    if (state_ == connected) {
        std::string message = buffer.readAllAsString();
        loop_->doTaskInThread(std::bind(
                &TcpConnection::sendInLoop, this, message));
    } else {
        Logger::get("bounce_file_log")->info(
                "file:{}, line:{}, function:{}  Connection is over, can't send.",
                FILENAME(__FILE__), __LINE__, __FUNCTION__);
    }
}

void bounce::TcpConnection::sendInLoop(const std::string& message) {
    if (state_ == disconnected) {
        Logger::get("bounce_file_log")->error(
                "file:{}, line:{}, function:{}  "
                "send failed, connected is disconnected.",
                FILENAME(__FILE__), __LINE__, __FUNCTION__);
        return;
    }
    // We can have a try to write.
    // If failed, we will call enableWrite(). To watch writable event.
    ssize_t wrote_bytes = 0;
    std::string::size_type send_bytes = message.size();
    bool send_error = false;

    // This mean the fd is free. Maybe can write now.
    if (!channel_->isWriting() && output_buffer_.readableBytes() == 0) {
        wrote_bytes = ::send(channel_->fd(), message.c_str(), message.size(), 0);
        // send success and finish.
        if (wrote_bytes == static_cast<ssize_t>(send_bytes)) {
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
            if (errno != EWOULDBLOCK) {
                if (errno == EPIPE || errno == ECONNRESET) {
                    Logger::get("bounce_file_log")->error(
                            "file:{}, line:{}, function:{}  "
                            "send message error, errno is {}",
                            FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
                    send_error = true;
                }
            }
        }
    }

    if (!send_error && send_bytes > 0) {
        //const char* str = message.c_str();
        const char* str = message.data();
        output_buffer_.append(str + wrote_bytes, send_bytes);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void bounce::TcpConnection::shutdown() {
    if (state_ != disconnected) {
        state_ = disconnecting;
        loop_->doTaskInThread(std::bind(
                &TcpConnection::shutdownInLoop, this));
    }
}

void bounce::TcpConnection::shutdownInLoop() {
    // something in the output_buffer, wait until they are sent.
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void bounce::TcpConnection::forceClose() {
    if (state_ != disconnected) {
        state_ = disconnecting;
        // must be shared_from_this()
        loop_->doTaskInThread(
                std::bind(&TcpConnection::forceCloseInLoop,
                        shared_from_this()));
    }
}

/*void bounce::TcpConnection::forceCloseDelay(size_t seconds) {
    if (state_ != disconnected) {
        loop_->runAfter(
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::seconds(seconds)),
                std::bind(&TcpConnection::forceCloseInLoop,
                          shared_from_this()));
    }
}*/

void bounce::TcpConnection::forceCloseInLoop() {
    if (state_ != disconnected)
        handleClose();
}

void bounce::TcpConnection::handleRead(time_t time) {
	int error;
	ssize_t ret = input_buffer_.readFd(socket_->fd(), &error);
	if (ret > 0) {
        if (message_cb_ != nullptr) {
            message_cb_(shared_from_this(), &input_buffer_, time);
        }
    } else if (ret == 0) {
	    handleClose();
	} else {
        Logger::get("bounce_file_log")->error(
                "file:{}, line:{}, function:{}  read fd failed. errno is {}",
                FILENAME(__FILE__), __LINE__, __FUNCTION__, error);
	}
}

void bounce::TcpConnection::handleWrite() {
    // if (channel_->isWriting())  Why check this?
    if (channel_->isWriting()) {
        ssize_t wrote_bytes = ::send(channel_->fd(),
                                     output_buffer_.peek(),
                                     output_buffer_.readableBytes(), 0);
        if (wrote_bytes > 0) {
            output_buffer_.retrieve(static_cast<size_t >(wrote_bytes));
            if (output_buffer_.readableBytes() == 0) {
                // send finish, delete watch and call callback function.
                channel_->disableWriting();
                if (write_cb_) {
                    loop_->queueTaskInThread(std::bind(
                            write_cb_, shared_from_this()));
                }
                // Make sure message send finish,
                // need check connection state.
                if (state_ == disconnecting) {
                    shutdownInLoop();
                }
            }
            // nothing to do, wait next loop.
        } else {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  send failed. errno is {}",
                    FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
        }
    } else {
        Logger::get("bounce_file_log")->error(
                "file:{}, line:{}, function:{}  not writing but handle write?"
                "errno is {}",
                FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
    }
}

void bounce::TcpConnection::handleClose() {
	state_ = disconnected;
	channel_->disableAll();
	// The last life of this connection.
	auto last_life(shared_from_this());
	if (connect_cb_)
	    connect_cb_(last_life);
	close_cb_(last_life);
}

void bounce::TcpConnection::handleError() {
    Logger::get("bounce_file_log")->error(
            "file:{}, line:{}, function:{}  errno is {}",
            FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
    if (error_cb_ != nullptr) {
        error_cb_(shared_from_this());
    }
}