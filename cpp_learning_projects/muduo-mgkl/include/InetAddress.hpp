#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

// 封装网络套接字 <IP ADDRESS>:<PORT>
class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");

    // 拷贝构造
    explicit InetAddress(const sockaddr_in & addr):
        addr_(addr) {}

    std::string toIp() const;

    std::string toIpPort() const;

    uint16_t toPort() const;

    const sockaddr_in * getSockAddr() const {
        return &addr_;
    }

    void setSockAddr(const sockaddr_in & addr) {
        addr_ = addr;
    }

private:
    // 用于标识网络套接字的结构体
    sockaddr_in addr_;
};