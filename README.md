## Bounce

**解决C++ Linux网络编程中的偶发问题，通过简单易用的接口，简化网络编程。**

[![Build Status](https://travis-ci.com/zhangyu0310/bounce.svg?branch=master)](https://travis-ci.com/zhangyu0310/bounce)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/zhangyu0310/bounce/blob/master/LICENSE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)

### 使用方法介绍

在bounce项目的上级目录创建build文件夹 <br>
```shell
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX={path} ../bounce
sudo make install
```
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

---
### 可以直接引用的头文件
带有\*的头文件，说明必须引用（实现网络基本功能）。
不在列表中的文件，不建议直接引用。

| 头文件 | 说明 |
|:------------------|:---------------------------------|
| buffer.h          | 消息回调、send函数，使用的Buffer类 |
| connector.h       | 连接器，用于发起连接，类似于客户端  |
| event_loop.h*     | 事件循环类，是整个网络库的基础      |
| logger.h          | 单例类的用法，需要打印日志可以使用  |
| sockaddr.h        | 封装了Socket地址信息（支持IPv6）   |
| tcp_connection.h* | 封装了一条Tcp连接                 |
| tcp_server.h*     | 用于启动整个网络服务，自定义回调    |
| timer.h           | 使用定时功能时需要引用             |

> 注：单独使用 ```EventLoop``` 和 ```timer```，
可以直接实现定时器的功能。非常轻松方便。定时接口在EventLoop类中。

### 其他注意事项
* 日志使用spdlog，两个已经注册使用的名称为```bounce_console```
和```bounce_file_log```。前者会将日志打印到命令行，后者会将日志打印到
文件中。如果不想使用这两个注册日志，请使用其它名称，避免冲突。
* 文件日志的默认路径为```./logs```，如需修改，应当在第一次调用```Logger::get()```
前，调用```Logger::setBounceLogPath(path)```
* 文件日志的文件名称为bounce_log.txt，无法修改。
* 设置日志等级和刷新策略的方法：
```cpp
Logger::get("bounce_file_log")->set_level(spdlog::level::xxx);
Logger::get("bounce_file_log")->flush_on(spdlog::level::xxx);
```
* bounce网络库为了实现线程唤醒，每个EventLoop都使用了一个文件描述符。
* 日志会占用单独的线程，除了日志，没有其它用户不知晓的线程启动。
