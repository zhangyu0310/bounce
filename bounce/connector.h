/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Connector.
* This is a public header file, users can include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_CONNECTOR_H
#define BOUNCE_CONNECTOR_H

#include <functional>
#include <map>
#include <memory>

#include <bounce/channel.h>
#include <bounce/socket.h>
#include <bounce/sockaddr.h>

namespace bounce {

class EventLoop;
class TcpServer;

class Connector {
    typedef std::function<void(SockAddress, int)> ErrorCallback;
    typedef std::shared_ptr<Channel> ChannelPtr;
public:
    Connector(EventLoop* loop, TcpServer* server);
    Connector(const Connector&) = delete;
    Connector& operator=(const Connector&) = delete;

    void connect(const SockAddress& addr);
    void connectInLoop(const SockAddress& addr);

    void setErrorCallback(const ErrorCallback& cb) {
        error_cb_ = cb;
    }

private:
    void handleWrite(int fd, const bounce::SockAddress& addr);

    EventLoop* loop_;
    TcpServer* tcp_server_;
    ErrorCallback error_cb_;
    std::map<int, ChannelPtr> conn_map_;
};

} // namespace bounce

#endif //BOUNCE_CONNECTOR_H
