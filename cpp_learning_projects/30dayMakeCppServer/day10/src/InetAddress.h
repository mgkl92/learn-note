#pragma once

#include <netinet/in.h>

class InetAddress {
private:
    struct sockaddr_in addr;

    socklen_t addr_len;

public:
    InetAddress();

    InetAddress(const char *, uint16_t);

    ~InetAddress();

    void setInetAddr(sockaddr_in, socklen_t);

    sockaddr_in getAddr();

    socklen_t getAddrLen();
};