#include "InetAddress.h"

#include <arpa/inet.h>
#include <string.h>

InetAddress::InetAddress() :
    addr_len(sizeof(addr)) {
    bzero(&addr, addr_len);
}

InetAddress::InetAddress(const char *ip, uint16_t port) :
    addr_len(sizeof(addr)) {
    bzero(&addr, addr_len);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ::inet_addr(ip);
    addr.sin_port = ::htons(port);
}

InetAddress::~InetAddress() {
}

void InetAddress::setInetAddr(sockaddr_in addr_, socklen_t addr_len_) {
    addr = addr_;
    addr_len = addr_len_;
}

sockaddr_in InetAddress::getAddr() {
    return addr;
}

socklen_t InetAddress::getAddrLen() {
    return addr_len;
}