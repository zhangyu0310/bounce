/*
* 主要测试性能
* 建立连接，创建TcpServer对象，有消息到来，触发回调回射消息。
* 不打印消息，单纯为了测试是否能承担大流量。
*/

#include <assert.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <chrono>
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
        spdlog::info("A new Connection...");
    } else if (conn->state() == TcpConnection::disconnected) {
        spdlog::info("A Connection down...");
    }
}

void read_cb(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer, time_t time) {
    std::string message = buffer->readAllAsString();
    conn->send(message);
}

/*int main() {
    bounce::log_level = spdlog::level::err;
    EventLoop loop;
    TcpServer server(&loop, "127.0.0.1", 9999, 3);
    server.setConnectionCallback(conn_cb);
    server.setMessageCallback(read_cb);
    server.start();
    loop.loop();
}*/

int main() {
    EventLoop loop; // base_loop
    TcpServer server(&loop, "127.0.0.1", 9999, 3);
    server.setMessageCallback([](
            const TcpServer::TcpConnectionPtr& conn,
            Buffer* buffer, time_t time) {
        conn->send(buffer->readAllAsString());
    });
    server.start();
    loop.loop();
}