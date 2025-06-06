#pragma once

#include <sys/epoll.h>
#include <functional>

class Epoll;
class EventLoop;

class Channel {
    // Epoll *ep;
    EventLoop *loop;

    int fd;

    uint32_t events;

    uint32_t revents;

    bool inEpoll;

    // std::function<void()> callback;
    
    std::function<void()> readCallback;
    
    std::function<void()> writeCallback;

public:

    // Channel(Epoll *ep_, int fd_);
    Channel(EventLoop *, int);

    ~Channel();

    void enableReading();

    int getFd();

    uint32_t getEvents();

    uint32_t getRevents();

    bool getInEpoll();

    void setInEpoll(bool = true);

    void setRevents(uint32_t);

    void handleEvent();

    // void setCallback(std::function<void()>);
    void setReadCallback(std::function<void()>);
};