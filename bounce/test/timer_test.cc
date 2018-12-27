/*
* 主要测试TimerQueue定时功能是否正常
* runEvery 执行发送 Hello! 给连接上来的客户端
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
#include <bounce/event_loop.h>
#include <bounce/tcp_connection.h>
#include <bounce/tcp_server.h>

using namespace bounce;

void time_to_send(const std::shared_ptr<TcpConnection>& conn) {
    spdlog::info("Hello! Timer");
    conn->send("Hello! Timer!\n");
}

std::vector<EventLoop::TimerPtr> timers;

void conn_cb(const std::shared_ptr<TcpConnection>& conn) {
    EventLoop* loop = conn->getLoop();

    if (conn->state() == TcpConnection::connected) {
        std::cout << "New Connnection..." << std::endl;
        std::cout << "This thread_id is ";
        std::cout << std::this_thread::get_id() << std::endl;
        std::cout << std::endl;
        std::chrono::microseconds micro_sec(1000);
        /*auto count = std::chrono::duration_cast<
                EventLoop::NanoSeconds>(micro_sec).count();
        std::cout << "The count is " << count << std::endl;*/
        auto timer = loop->runEvery(
                std::chrono::duration_cast<EventLoop::NanoSeconds>(micro_sec),
                std::bind(time_to_send, conn));
        /*auto timer = loop->runAfter(
                std::chrono::duration_cast<EventLoop::NanoSeconds>(sec),
                std::bind(time_to_send, conn));*/
        /*auto timer = loop->runAt(
                std::chrono::system_clock::now() +
                std::chrono::duration_cast<EventLoop::NanoSeconds>(sec),
                std::bind(time_to_send, conn));*/
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
    bounce::log_level = spdlog::level::err;
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