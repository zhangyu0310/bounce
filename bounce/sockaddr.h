/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the SockAddress.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_SOCKADDR_H
#define BOUNCE_SOCKADDR_H

//#include <sys/socket.h>
#include <netinet/in.h>

#include <string>

namespace bounce {

// only IPv4. TODO: support IPv6
class SockAddress {
public:
	SockAddress() = default; // Only for Acceptor
	SockAddress(const std::string& ip, uint16_t port);
	~SockAddress() = default;
	SockAddress(const SockAddress&) = default;
	SockAddress& operator=(const SockAddress&) = default;

	sockaddr* inetAddr() { return (sockaddr*)&addr_; }
	const sockaddr* constInetAddr() const { return (sockaddr*)&addr_; }
	socklen_t size() const { return sizeof(addr_); }

private:
	// IPv4 address struct.
	struct sockaddr_in addr_;
};

} // namespace bounce
#endif // !BOUNCE_SOCKADDR_H