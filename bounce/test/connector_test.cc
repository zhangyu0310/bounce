/*
* 主要测试Connector建立连接是否正常
* 写一个客户端程序，去连接echo server，观察是否连接成功
*/

#include <bounce/tcp_server.h>
#include <bounce/logger.h>
#include <bounce/event_loop.h>
#include <bounce/connector.h>
#include <bounce/tcp_connection.h>
using namespace bounce;

void conn_cb(const std::shared_ptr<TcpConnection>& conn) {
    if (conn->state() == TcpConnection::connected) {
        conn->send("hello!\n");
    } else if (conn->state() == TcpConnection::disconnected) {

    }
}

void read_cb(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer, time_t time) {
    std::string message = buffer->readAllAsString();
    conn->send(message);
}

int main() {
    EventLoop loop;
    TcpServer server(&loop, "127.0.0.1", 9999, 1);
    server.setConnectionCallback(conn_cb);
    server.setMessageCallback(read_cb);
    Connector connector(&loop, &server);
    SockAddress addr("127.0.0.1", 9281);
    SockAddress addr2("127.0.0.1", 9282);
    server.start();
    connector.connect(addr);
    connector.connect(addr2);
    loop.loop();
}