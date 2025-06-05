#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll *ep_, int fd_) :
    ep(ep_), fd(fd_), events(0), revents(0), inEpoll(false) {
}

Channel::~Channel() {
}

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    ep->updateChannel(this);
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