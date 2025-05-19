#include "Poller.hpp"
#include "Channel.hpp"

Poller::Poller(EventLoop * loop):
    ownerLoop_(loop) {
}

bool Poller::hasChannel(Channel * channel) const {
    auto it = channels_.find(channel->fd());

    // it->second == channel 用于避免哈希碰撞
    return (it != channels_.end()) && it->second == channel;
}