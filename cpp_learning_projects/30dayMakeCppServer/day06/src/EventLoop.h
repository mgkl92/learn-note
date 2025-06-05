#pragma once

class Epoll;
class Channel;

class EventLoop {
    Epoll *ep;
    bool quit;

public:
    EventLoop();
    
    ~EventLoop();

    void loop();

    void updateChannel(Channel *);
};