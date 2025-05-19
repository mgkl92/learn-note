#pragma once

#include "NonCopyable.hpp"
#include "Timestamp.hpp"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

// 实现 I/O 多路复用的抽象 Poller 基类
class Poller {
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop * loop);

    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList * activeChannels) = 0;
    virtual void updateChannel(Channel * channel) = 0;
    virtual void removeChannel(Channel * channel) = 0;

    bool hasChannel(Channel * channel) const;

    static Poller * newDefaultPoller(EventLoop * loop);

protected:
    using ChannelMap = std::unordered_map<int, Channel *>;

    // 对 Channel 封装的 fd 进行管理
    ChannelMap channels_;

private:
    // Poller 所属 EventLoop 对象
    EventLoop * ownerLoop_;
};