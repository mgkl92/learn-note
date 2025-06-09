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


    std::vector<Channel *> poll(int timeout = -1);

    void updateChannel(Channel *);
};