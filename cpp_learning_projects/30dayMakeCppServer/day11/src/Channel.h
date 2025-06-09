#pragma once

#include <sys/epoll.h>
#include <functional>

class Epoll;
class EventLoop;

class Channel {
    EventLoop *loop;

    int fd;

    uint32_t events;

    uint32_t revents;

    bool inEpoll;

    bool useThreadPool;

    std::function<void()> readCallback;

    std::function<void()> writeCallback;

public:
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

    void setReadCallback(std::function<void()>);

    void setUseThreadPoll(bool = true);

    void useET();
};