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

class SockAddress {
public:
	explicit SockAddress(uint16_t port = 0,
			bool loopback = false,
			bool ipv6 = false);
	SockAddress(const std::string& ip, uint16_t port, bool ipv6 = false);
	~SockAddress() = default;
	SockAddress(const SockAddress&) = default;
	SockAddress& operator=(const SockAddress&) = default;

	sockaddr* inetAddr() { return (sockaddr*)&addr6_; }
	const sockaddr* constInetAddr() const { return (sockaddr*)&addr6_; }
	socklen_t size() const { return sizeof(addr6_); }

	sa_family_t family() { return addr6_.sin6_family; }
	std::string ip();
	uint16_t port() { return ntohs(addr6_.sin6_port); }

	void setScopeId(uint32_t scope_id);

private:
	// IPv4 address struct.

	union {
		struct sockaddr_in addr_;
		struct sockaddr_in6 addr6_;
	};
};

} // namespace bounce
#endif // !BOUNCE_SOCKADDR_H