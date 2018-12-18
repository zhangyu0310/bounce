/*
* 主要测试TcpServer功能是否正常
* 建立连接，创建TcpServer对象，有消息到来，主动shutdown
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

void conn_cb(const std::shared_ptr<TcpConnection>& conn) {
    if (conn->state() == TcpConnection::connected) {
        std::cout << "New Connnection..." << std::endl;
        std::cout << "This thread_id is ";
        std::cout << std::this_thread::get_id() << std::endl;
        conn->shutdown();
        std::cout << "shut down! hahahahaha..." << std::endl;
        std::cout << std::endl;
    } else if (conn->state() == TcpConnection::disconnecting) {
        std::cout << "DISCONNECTING!!!!!!!!!!" << std::endl;
    } else if (conn->state() == TcpConnection::disconnected) {
        std:: cout << "Connection is over..." << std::endl;
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
    //spdlog::get("bounce_file_log")->info("First log.");
    bounce::log_level = spdlog::level::err;
    std::cout << "pthread_id: " << std::this_thread::get_id() << std::endl;
    Logger::get("bounce_file_log")->info(
            "file:{}, line:{}, function:{}  I am in main",
            FILENAME(__FILE__), __LINE__, __FUNCTION__);
    EventLoop loop;
    TcpServer server(&loop, "127.0.0.1", 9281, 3);
    server.setConnectionCallback(conn_cb);
    server.setMessageCallback(read_cb);
    server.setWriteCompleteCallback(write_cb);
    server.start();
    loop.loop();
}