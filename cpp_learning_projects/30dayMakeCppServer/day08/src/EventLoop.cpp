#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

#include <vector>

EventLoop::EventLoop() :
    ep(nullptr), quit(false) {
    ep = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel *> activeChannels = ep->poll();
        for (size_t i = 0; i < activeChannels.size(); ++i) {
            activeChannels[i]->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel) {
    ep->updateChannel(channel);
}