/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Socket.
* A socket object is a encapsulation of file descriptor.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/socket.h>

#include <bounce/sockaddr.h>

// The size 5 is a recommended value.
static const int LISTEN_SIZE = 5;

// IPv4 only
void bounce::Socket::bind(SockAddress& addr) {
	// TODO: support IPv6
	// FIXME: check return of bind()
	::bind(fd_, addr.inetAddr(), addr.size());
}

void bounce::Socket::listen() {
	// FIXME: check return of listen().
	::listen(fd_, LISTEN_SIZE);
}

int bounce::Socket::accept(SockAddress* addr) {
	socklen_t len = addr->size();
	int fd = ::accept4(fd_, addr->inetAddr(), &len, SOCK_NONBLOCK);
	// FIXME: If accept error, fd < 0.
	return fd;
}