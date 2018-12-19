/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Socket.
* This is an internal header file, users should not include this.
* A socket object is a encapsulation of file descriptor.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_SOCKET_H
#define BOUNCE_SOCKET_H

#include <sys/socket.h>
#include <unistd.h>

namespace bounce {

class SockAddress;

// This class can't be copy.
// because it's control life of fd.
class Socket {
public:
	explicit Socket(int fd) : fd_(fd) {}
	~Socket() { ::close(fd_); }
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	void bind(SockAddress& addr); // bindOrDie;
	void listen(); // listenOrDie;
	int accept(SockAddress* addr);
	void shutdownWrite() { ::shutdown(fd_, SHUT_WR); }
	void shutdownRead() { ::shutdown(fd_, SHUT_RD); }
	int fd() { return fd_; }

private:
	int fd_;
};

} // namespace bounce

#endif // !BOUNCE_SOCKET_H