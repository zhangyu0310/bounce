/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Socket.
* A socket object is a encapsulation of file descriptor.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/socket.h>

#include <bounce/logger.h>
#include <bounce/sockaddr.h>

// The size 5 is a recommended value.
static const int LISTEN_SIZE = 5;

// IPv4 only
void bounce::Socket::bind(SockAddress& addr) {
	// TODO: support IPv6
	if (::bind(fd_, addr.inetAddr(), addr.size()) != 0) {
		Logger::get("bounce_file_log")->critical(
				"file:{}, line:{}, function:{}  bind failed. errno is {}",
				FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
		exit(1);
	}
}

void bounce::Socket::listen() {
	if (::listen(fd_, LISTEN_SIZE) != 0) {
		Logger::get("bounce_file_log")->critical(
				"file:{}, line:{}, function:{}  listen failed. errno is {}",
				FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
		exit(1);
	}
}

int bounce::Socket::accept(SockAddress* addr) {
	socklen_t len = addr->size();
	int fd = ::accept4(fd_, addr->inetAddr(), &len, SOCK_NONBLOCK);
	if (fd < 0) {
		Logger::get("bounce_file_log")->error(
				"file:{}, line:{}, function:{}  accept fd < 0. errno is {}",
				FILENAME(__FILE__), __LINE__, __FUNCTION__, errno);
	}
	return fd;
}