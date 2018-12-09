/*
* 主要测试Channel、Poller与EventLoop的基本功能
* 测试方法：监听一个端口，有连接到来，触发回调，打印当前时间戳。
*/

#include <sys/socket.h>

#include <chrono>
#include <iostream>
#include <string>

#include <bounce/channel.h>
#include <bounce/event_loop.h>
#include <bounce/poller.h>
#include <bounce/poll_poller.h>
#include <bounce/sockaddr.h>
#include <bounce/socket.h>
using namespace bounce;

void read_cb(time_t time) {
	std::cout << "Connecting... " << time << std::endl;
	sleep(2);
}

int main() {
	int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	Socket listen_sock(listen_fd);
	SockAddress local_addr("127.0.0.1", 9281);
	listen_sock.bind(local_addr);
	listen_sock.listen();
	EventLoop loop;
	Channel listen_channel(&loop, listen_fd);
	//listen_channel.setEvents(POLLIN | POLLPRI);
	listen_channel.enableReading();
	listen_channel.setReadCallback(read_cb);
	loop.updateChannel(&listen_channel);
	loop.loop();
}