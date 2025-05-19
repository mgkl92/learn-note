#include "InetAddress.hpp"

#include <strings.h>
#include <cstring>

InetAddress::InetAddress(uint16_t port, std::string ip) {
    ::memset(&addr_, 0, sizeof(addr_));

    // 指定网络地址协议族为 IPv4
    addr_.sin_family = AF_INET;

    // Host to Network Short
    // 将主机字节序转化为网络字节序（大端序，高位字节低地址）
    addr_.sin_port = ::htons(port);

    // 将点分十进制 IP 地址解析为 32 网络序字节，仅支持 IPv4
    // 支持 IPv6 使用 ::inet_pton
    addr_.sin_addr.s_addr = ::inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const {
    char buf[64] = { 0 };

    // 将 IP 地址转化为网络字节序
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));

    return buf;
}

std::string InetAddress::toIpPort() const {
    char buf[64] = { 0 };

    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    size_t end = ::strlen(buf);
    uint16_t port = ::ntohs(addr_.sin_port);
    sprintf(buf + end, ":%u", port);

    return buf;
}

uint16_t InetAddress::toPort() const {
    // Network to Host Short
    return ::ntohs(addr_.sin_port);
}