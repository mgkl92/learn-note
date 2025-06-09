#include "Channel.h"
#include "EventLoop.h"

#include <unistd.h>

Channel::Channel(EventLoop *loop_, int fd_) :
    loop(loop_), fd(fd_), events(0), revents(0), inEpoll(false), useThreadPool(true) {
}

Channel::~Channel() {
    if (fd != -1) {
        ::close(fd);
        fd = -1;
    }
}

void Channel::enableReading() {
    events |= (EPOLLIN | EPOLLPRI);
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
    if (revents &(EPOLLIN | EPOLLPRI)) {
        if (useThreadPool) {
            loop->addThread(readCallback);
        } else {
            readCallback();
        }
    }

    if (revents & EPOLLOUT) {
        if (useThreadPool) {
            loop->addThread(writeCallback);
        } else {
            writeCallback();
        }
    }
}

void Channel::setReadCallback(std::function<void()> readCallback_) {
    readCallback = readCallback_;
}

void Channel::setUseThreadPoll(bool use) {
    useThreadPool = use;
}

void Channel::useET() {
    events |= EPOLLET;
    loop->updateChannel(this);
}