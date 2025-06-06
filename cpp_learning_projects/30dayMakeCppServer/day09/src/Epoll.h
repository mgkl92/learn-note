#pragma once

#include <vector>

#include <sys/epoll.h>

class Channel;

class Epoll {
    int epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);

    // std::vector<epoll_event> poll (int timeout = -1);
    std::vector<Channel *> poll(int timeout = -1);

    void updateChannel(Channel *);
};