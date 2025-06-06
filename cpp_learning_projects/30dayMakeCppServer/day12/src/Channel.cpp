#include "Channel.h"
// #include "Epoll.h"
#include "EventLoop.h"
#include <unistd.h>
#include <sys/epoll.h>

// Channel::Channel(Epoll *ep_, int fd_) :
//     ep(ep_), fd(fd_), events(0), revents(0), inEpoll(false) {
// }

Channel::Channel(EventLoop *loop_, int fd_) :
    loop(loop_), fd(fd_), events(0), revents(0), inEpoll(false) {
}

Channel::~Channel() {
    if (fd != -1) {
        ::close(fd);
        fd = -1;
    }
}

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    // ep->updateChannel(this);
    loop->updateChannel(this);
}


int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}

bool Channel::getInEpoll() {
    return inEpoll;
}

void Channel::setInEpoll(bool inEpoll_) {
    inEpoll = inEpoll_;
}

void Channel::setRevents(uint32_t revents_) {
    revents = revents_;
}

void Channel::handleEvent() {
    if (revents & (EPOLLIN | EPOLLPRI)) {
        readCallback();
    }

    if (revents & EPOLLOUT) {
        writeCallback();
    }
}

void Channel::setReadCallback(std::function<void()> readCallback_) {
    readCallback = readCallback_;
}