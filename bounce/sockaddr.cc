/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of SockAddress.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/sockaddr.h>

#include <arpa/inet.h>
#include <string.h>

bounce::SockAddress::SockAddress(uint16_t port, bool loopback, bool ipv6) {
	bzero(&addr6_, sizeof(addr6_));
	addr6_.sin6_port = htons(port);
	if (ipv6) {
		addr6_.sin6_family = AF_INET6;
		addr6_.sin6_addr = loopback ? in6addr_loopback : in6addr_any;
	} else {
		addr_.sin_family = AF_INET;
		in_addr_t ip = loopback ? INADDR_LOOPBACK : INADDR_ANY;
		addr_.sin_addr.s_addr = htonl(ip);
	}
}

bounce::SockAddress::SockAddress(
		const std::string& ip,
		uint16_t port,
		bool ipv6) {
	bzero(&addr6_, sizeof(addr6_));
	addr6_.sin6_port = htons(port);
	if (ipv6) {
		addr6_.sin6_family = AF_INET6;
		inet_pton(AF_INET6, ip.c_str(), &addr6_.sin6_addr);
	} else {
		addr_.sin_family = AF_INET;
		inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
	}
}

std::string bounce::SockAddress::ip() {
	char buf[64] = "";
	socklen_t size = sizeof(buf);
	if (addr6_.sin6_family == AF_INET) {
		inet_ntop(AF_INET, &addr_.sin_addr, buf, size);
	} else if (addr6_.sin6_family == AF_INET6) {
		inet_ntop(AF_INET6, &addr6_.sin6_addr, buf, size);
	}
	return buf;
}

void bounce::SockAddress::setScopeId(uint32_t scope_id) {
	if (family() == AF_INET6) {
		addr6_.sin6_scope_id = scope_id;
	}
}