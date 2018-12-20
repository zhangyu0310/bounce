## Bounce

**解决C++ Linux网络编程中的偶发问题，通过简单易用的接口，简化网络编程。**

### 使用方法介绍

在bounce项目的上级目录创建build文件夹 <br>
mkdir build <br>
cd build <br>
cmake ../bounce/bounce <br>
make <br>

#### 实现简单的echo服务：

```cpp
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

int main() {
    bounce::log_level = spdlog::level::err;
    EventLoop loop;
    TcpServer server(&loop, "127.0.0.1", 9999, 3);
    server.setConnectionCallback(conn_cb);
    server.setMessageCallback(read_cb);
    server.start();
    loop.loop();
}
```
#### 也可以使用lambda表达式：
```cpp
// 头文件省略
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
```
#### 最好能使用类来封装TcpServer：
> 是组合，不使用继承

```cpp
class EchoServer {
public:
    EchoServer() : 
        ser_(&base_loop_, "127.0.0.1, 9999, 3") {
        ser_.setMessageCallback(std::bind(
            &EchoServer::read_cb, this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
    }
    
    void read_cb(const std::shared_ptr<TcpConnection>& conn,
         Buffer* buffer, time_t time) {
        conn->send(buffer->readAllAsString());
    }
    
    void start() {
        ser_.start();
        base_loop.loop();
    }
    
private:
    EventLoop base_loop_;
    TcpServer ser_;
};

int main() {
    EchoServer server;
    server.start();
}
```

#### 编译成链接库来使用 -lbounce 更方便