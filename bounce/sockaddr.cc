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

bounce::SockAddress::SockAddress(const std::string& ip, uint16_t port) {
	bzero(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}