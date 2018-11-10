/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Socket.
* This is an internal header file, users should not include this.
* A socket object is a encapsulation of file descriptor.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_SOCKET_H
#define BOUNCE_SOCKET_H

#include <sys/socket.h>
#include <unistd.h>

namespace bounce {

class SockAddress;

class Socket {
public:
	Socket(int fd) : fd_(fd) {}
	~Socket() { ::close(fd_); }
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	void bind(SockAddress& addr); // FIXME: bindOrDie();
	void listen(); // FIXME: listenOrDie();
	int accept(SockAddress* addr);
	void shutdownWrite() { ::shutdown(fd_, SHUT_WR); } // FIXME: If false
	void shutdownRead() { ::shutdown(fd_, SHUT_RD); } // FIXME: If false
	int fd() { return fd_; }

private:
	int fd_;
};

} // namespace bounce

#endif // !BOUNCE_SOCKET_H