#include "Channel.h"
// #include "Epoll.h"
#include "EventLoop.h"

// Channel::Channel(Epoll *ep_, int fd_) :
//     ep(ep_), fd(fd_), events(0), revents(0), inEpoll(false) {
// }

Channel::Channel(EventLoop *loop_, int fd_) :
    loop(loop_), fd(fd_), events(0), revents(0), inEpoll(false) {
}
Channel::~Channel() {
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

void Channel::setInEpoll() {
    inEpoll = true;
}

void Channel::setRevents(uint32_t revents_) {
    revents = revents_;
}

void Channel::handleEvent() {
    if (callback) {
        callback();
    }
}

void Channel::setCallback(std::function<void()> callback_) {
    callback = callback_;
}