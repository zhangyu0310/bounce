/*
* 主要测试TcpConnection功能是否正常
* 建立连接，创建TcpConnection对象，有消息到来，触发回调即可
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

using namespace bounce;

std::vector<std::shared_ptr<TcpConnection>> connection_vec;
int listen_fd;
EventLoop loop;

void conn_cb(const std::shared_ptr<TcpConnection>& conn) {
	std::cout << "New Connnection..." << std::endl;
}

void read_cb(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer, time_t time) {
	std::cout << "Get Message at " << time << std::endl;
	std::cout << buffer->readAllAsString() << std::endl;
}

void acceptor_cb(time_t time) {
	sockaddr_in new_addr;
	socklen_t len;
	len = sizeof(new_addr);
	int new_fd = accept4(listen_fd, (sockaddr*)&new_addr, &len, SOCK_NONBLOCK);
	//assert(new_fd > 0);
	if (new_fd < 0) {
		std::cout << ::strerror(errno) << std::endl;
		return;
	}
	std::shared_ptr<TcpConnection> conn(new TcpConnection(&loop, new_fd));
	conn->setMessageCallback(read_cb);
	conn->setConnectCallback(conn_cb);
	conn->connectComplete();
	connection_vec.push_back(conn);
}

int main() {
	// Acceptor
	listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	Socket listen_sock(listen_fd);
	SockAddress local_addr("127.0.0.1", 9281);
	listen_sock.bind(local_addr);
	listen_sock.listen();
	Channel listen_channel(&loop, listen_fd);
	listen_channel.enableReading();
	listen_channel.setReadCallback(acceptor_cb);

	// EventLoop
	loop.updateChannel(&listen_channel);
	loop.loop();
}