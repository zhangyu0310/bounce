//
// Created by zhangyu on 19-1-16.
//

#include <bounce/socket.h>
#include <bounce/sockaddr.h>
#include <bounce/tcp_connection.h>
#include <gtest/gtest.h>

#include <iostream>

TEST(SockAddr, test) {
    bounce::SockAddress addr;
    bounce::SockAddress addr1(10000, true, false);
    bounce::SockAddress addr2(9999, true, true);
    bounce::SockAddress addr3("fe80::1778:fdc1:16f9:b739", 8888, true);
    bounce::SockAddress addr4("120.24.89.156", 7777);

    EXPECT_EQ(addr.family(), AF_INET);
    EXPECT_EQ(addr1.family(), AF_INET);
    EXPECT_EQ(addr2.family(), AF_INET6);
    EXPECT_EQ(addr3.family(), AF_INET6);
    EXPECT_EQ(addr4.family(), AF_INET);

    EXPECT_STREQ(addr.ip().c_str(), "0.0.0.0");
    EXPECT_STREQ(addr1.ip().c_str(), "127.0.0.1");
    EXPECT_STREQ(addr2.ip().c_str(), "::1");
    EXPECT_STREQ(addr3.ip().c_str(), "fe80::1778:fdc1:16f9:b739");
    EXPECT_STREQ(addr4.ip().c_str(), "120.24.89.156");

    EXPECT_EQ(addr.port(), 0);
    EXPECT_EQ(addr1.port(), 10000);
    EXPECT_EQ(addr2.port(), 9999);
    EXPECT_EQ(addr3.port(), 8888);
    EXPECT_EQ(addr4.port(), 7777);

    int fd = ::socket(AF_INET6, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_GT(fd, 0);
    bounce::Socket sock(fd);
    //sock.bind(addr1);
    //addr2.setScopeId(0x10);
    //sock.bind(addr2);
    addr3.setScopeId(2);
    sock.bind(addr3);
    sock.listen();
    sleep(3);
    bounce::SockAddress new_addr;
    int ret = sock.accept(&new_addr);
    ASSERT_GT(ret, 0);
    std::cout << new_addr.ip() << std::endl;
    std::cout << new_addr.port() << std::endl;
}