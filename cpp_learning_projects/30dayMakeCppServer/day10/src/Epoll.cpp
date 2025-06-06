#include "Epoll.h"
#include "Channel.h"
#include "util.h"

#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 1024

Epoll::Epoll() :
    epfd(epoll_create1(0)), events(nullptr) {
    errif(epfd == -1, "epoll create error");

    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(events));
}

Epoll::~Epoll() {
    if (epfd != -1) {
        ::close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

// std::vector<epoll_event> Epoll::poll(int timeout) {
//     std::vector<epoll_event> activeEvents;

//     int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
//     errif(nfds == -1, "epoll wait error");

//     for (int i = 0; i < nfds; ++i) {
//         activeEvents.push_back(events[i]);
//     }

//     return activeEvents;
// }

std::vector<Channel *> Epoll::poll(int timeout) {
    std::vector<Channel *> activeChannels;

    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");

    for (size_t i = 0; i < nfds; ++i) {
        Channel *channel = (Channel *) events[i].data.ptr;
        channel->setRevents(events[i].events);
        activeChannels.push_back(channel);
    }

    return activeChannels;
}

void Epoll::updateChannel(Channel * channel) {
    int fd = channel->getFd();
    
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.data.ptr = channel;
    ev.events = channel->getEvents();

    if (!channel->getInEpoll()) {
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->setInEpoll();
    } else {
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}