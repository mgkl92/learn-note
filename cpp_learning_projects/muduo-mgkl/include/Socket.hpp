#pragma once

#include "InetAddress.hpp"
#include "NonCopyable.hpp"

class InetAddress;

// 管理 TCP 连接对应的 sockfd 的生命周期
class Socket: NonCopyable {
public:
    explicit Socket(int sockfd):
        sockfd_(sockfd) {}

    ~Socket();

    int fd() const {
        return sockfd_;
    }

    // 绑定 sockfd
    void bindAddress(const InetAddress & localaddr);

    void listen();

    // 接受连接
    int accept(InetAddress * peeraddr);

    // 设置半关闭状态
    void shutdownWrite();

    // 设置 Nagel 算法
    void setTcpNoDelay(bool on);

    // 地址复用
    void setReuseAddr(bool on);

    // 端口复用
    void setResuePort(bool on);

    // 长连接
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};