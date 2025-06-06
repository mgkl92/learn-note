#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"

#include <vector>

EventLoop::EventLoop() :
    ep(nullptr), quit(false), threadPool(nullptr) {
    ep = new Epoll();
    threadPool = new ThreadPool();
}

EventLoop::~EventLoop() {
    if (ep) {
        delete ep;
    }

    if (threadPool) {
        delete threadPool;
    }
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

void EventLoop::addThread(std::function<void()> func) {
    threadPool->add(func);
}