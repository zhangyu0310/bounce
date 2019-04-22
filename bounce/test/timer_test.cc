/*
* 主要测试TimerQueue定时功能是否正常
* runEvery 执行发送 Hello! 给连接上来的客户端
*/

#include <assert.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <bounce/buffer.h>
#include <bounce/event_loop.h>
#include <bounce/tcp_connection.h>
#include <bounce/tcp_server.h>

using namespace bounce;
using TcpConnectionPtr = TcpServer::TcpConnectionPtr;

std::atomic<int> count;
std::vector<EventLoop::TimerPtr> timers;

void time_to_send(const std::shared_ptr<TcpConnection>& conn) {
    spdlog::info("Hello! Timer");
    conn->send("Hello! Timer!\n");
    std::chrono::milliseconds milli_sec(150);
    if (count++ < 100) {
        conn->getLoop()->runAfter(
                milli_sec,
                std::bind(time_to_send, conn));
    }
}

void time_to_send_every(const TcpConnectionPtr& conn) {
    spdlog::info("Timer Every");
    conn->send("Timer Every!");
    conn->getLoop()->deleteTimer(timers[0]);
}

void conn_cb(const std::shared_ptr<TcpConnection>& conn) {
    EventLoop* loop = conn->getLoop();

    if (conn->state() == TcpConnection::connected) {
        std::cout << "New Connnection..." << std::endl;
        std::cout << "This thread_id is ";
        std::cout << std::this_thread::get_id() << std::endl;
        std::cout << std::endl;
        std::chrono::microseconds micro_sec(1000000);
        std::chrono::milliseconds milli_sec(150);

        auto timer = loop->runEvery(
                micro_sec,
                std::bind(time_to_send_every, conn));
        loop->runAfter(
                milli_sec,
                std::bind(time_to_send, conn));
        loop->runAt(
                std::chrono::system_clock::now() + milli_sec,
                std::bind(time_to_send, conn));
        loop->runAfter(1000000, std::bind(time_to_send, conn));
        timers.push_back(timer);
    } else if (conn->state() == TcpConnection::disconnected) {
        std:: cout << "Connection is over..." << std::endl;
        loop->deleteTimer(timers[0]);
        timers.pop_back();
    }
}

void read_cb(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer, time_t time) {
    std::cout << "Get Message at " << time << std::endl;
    std::string message = buffer->readAllAsString();
    std::cout << message << std::endl;
    conn->send(message);
    std::cout << "This thread_id is ";
    std::cout << std::this_thread::get_id() << std::endl;
    std::cout << std::endl;
}

void write_cb(const std::shared_ptr<TcpConnection>& conn) {
    std::cout << "Write back finish..." << std::endl;
    std::cout << std::endl;
}

int main() {
    Logger::get("bounce_file_log")->set_level(spdlog::level::err);
    std::cout << "pthread_id: " << std::this_thread::get_id() << std::endl;
    Logger::get("bounce_file_log")->info(
            "file:{}, line:{}, function:{}  I am in main",
            FILENAME(__FILE__), __LINE__, __FUNCTION__);
    EventLoop loop;
    TcpServer server(&loop, "127.0.0.1", 9282, 3);
    server.setConnectionCallback(conn_cb);
    server.setMessageCallback(read_cb);
    server.setWriteCompleteCallback(write_cb);
    server.start();
    loop.loop();
}