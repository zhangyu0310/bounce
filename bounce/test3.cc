/*
* 主要测试TcpServer功能是否正常
* 建立连接，创建TcpServer对象，有消息到来，触发回调即可
*/

#include <assert.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <bounce/buffer.h>
#include <bounce/channel.h>
#include <bounce/event_loop.h>
#include <bounce/poller.h>
#include <bounce/poll_poller.h>
#include <bounce/sockaddr.h>
#include <bounce/socket.h>
#include <bounce/tcp_connection.h>
#include <bounce/tcp_server.h>

using namespace bounce;

void conn_cb(const std::shared_ptr<TcpConnection>& conn) {
	std::cout << "New Connnection..." << std::endl;
}

void read_cb(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer, time_t time) {
	std::cout << "Get Message at " << time << std::endl;
	std::cout << buffer->readAllAsString() << std::endl;
}

int main() {
	EventLoop loop;
	TcpServer server(&loop, "127.0.0.1", 9281);
	server.setConnectionCallback(conn_cb);
	server.setMessageCallback(read_cb);
	server.start();
	loop.loop();
}