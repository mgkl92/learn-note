#pragma once

#include "NonCopyable.hpp"
#include "Socket.hpp"
#include "Channel.hpp"

class EventLoop;
class InetAddress;

// 监听 sockfd 及相关处理方法
class Acceptor: NonCopyable {
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

    Acceptor(EventLoop * loop, const InetAddress & listenAddr, bool reuseport);

    ~Acceptor();

    // 设置回调函数
    void setNewConnectionCallback(const NewConnectionCallback & cb) {
        NewConnectionCallback_ = cb;
    }

    bool listenning() const {
        return listenning_;
    }

    // 监听本地端口
    void listen();

private:
    void handleRead();

    // main EventLoop
    EventLoop * loop_;
    Socket acceptSocket_;

    // 封装 sockfd 及其感兴趣的事件和处理函数
    Channel acceptChannel_;
    NewConnectionCallback NewConnectionCallback_;
    bool listenning_;
};