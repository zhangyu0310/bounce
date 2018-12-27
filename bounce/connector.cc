/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Connector.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/connector.h>

#include <sys/socket.h>

#include <utility>

#include <bounce/event_loop.h>
#include <bounce/logger.h>
#include <bounce/tcp_server.h>

bounce::Connector::Connector(bounce::EventLoop* loop,
        bounce::TcpServer* server) :
        loop_(loop),
        tcp_server_(server) { }

void bounce::Connector::connect(const bounce::SockAddress& addr) {
    if (!tcp_server_->started()) {
        return;
    }
    loop_->doTaskInThread(std::bind(&Connector::connectInLoop, this, addr));
}

void bounce::Connector::connectInLoop(const bounce::SockAddress& addr) {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ChannelPtr channel_ptr(new Channel(loop_, fd));
    int ret = ::connect(fd, addr.constInetAddr(), addr.size());
    if (ret == 0) {
        handleWrite(fd, addr);
        Logger::get("bounce_file_log")->info(
                "file:{}, line:{}, function:{}  "
                "connect success, fd is {}",
                FILENAME(__FILE__), __LINE__, __FUNCTION__, fd);
    } else if (errno == EINPROGRESS) {
        conn_map_.insert(std::make_pair(fd, channel_ptr));
        channel_ptr->setWriteCallback(std::bind(
                &Connector::handleWrite, this, fd, addr));
        channel_ptr->enableWriting();
        Logger::get("bounce_file_log")->debug(
                "file:{}, line:{}, function:{}  "
                "connect EINPROGRESS, fd is {}",
                FILENAME(__FILE__), __LINE__, __FUNCTION__, fd);
    } else {
        Logger::get("bounce_file_log")->error(
                "file:{}, line:{}, function:{}  "
                "connect error, errno is {}",
                FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
    }
}

void bounce::Connector::handleWrite(int fd, const bounce::SockAddress& addr) {
    auto it = conn_map_.find(fd);
    if (it == conn_map_.end()) {
        tcp_server_->setNewConnection(fd, addr);
    } else { // Means handleWrite was called by channel.
        int error = 0;
        bool success = true;
        socklen_t len = sizeof(error);
        if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  "
                    "errno is {}",
                    FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
            success = false;
        }
        if (error != 0) {
            Logger::get("bounce_file_log")->error(
                    "file:{}, line:{}, function:{}  "
                    "sockopt error != 0, error is {}",
                    FILENAME(__FILE__), __LINE__, __FUNCTION__, error);
            success = false;
        }
        it->second->remove();
        conn_map_.erase(it);
        if (success) {
            tcp_server_->setNewConnection(fd, addr);
        } else {
            ::close(fd);
            if (error_cb_) {
                int err = errno;
                error_cb_(addr, err);
            }
        }
    }
}