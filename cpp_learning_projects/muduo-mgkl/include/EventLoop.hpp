#pragma once

#include "NonCopyable.hpp"
#include "Timestamp.hpp"
#include "CurrentThread.hpp"

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

class Channel;
class Poller;

// 事件循环类 (Reactor)
// One Poller Per Thread
// 使用主从 Reactor 模型时，要保证 Main Reactor 与 Sub Reactor 之间的通信
class EventLoop: NonCopyable {
public:
    using Functor = std::function<void()>;

    EventLoop();

    ~EventLoop();

    // 开始事件循环
    void loop();

    // 退出事件循环
    void quit();

    Timestamp pollReturnTime() const {
        return pollReturnTime_;
    }

    // 在当前 EventLoop 中执行回调函数
    void runInLoop(Functor cb);

    // 将回调函数放入到队列，唤醒 EventLoop 所在线程并执行回调函数
    void queueInLoop(Functor cb);

    // 通过 eventfd 唤醒 EventLoop 所在线程
    void wakeup();

    // 交由 EPollPoler 更新相应 Channel 的状态
    void updateChannel(Channel * channel);

    void removeChannel(Channel * channel);

    bool hasChannel(Channel * channel);

    // 判断 EventLoop 对象是否在自己线程内
    bool isInLoopThread() const {
        return threadId_ == CurrentThread::tid();
    }

private:
    void handleRead();

    // 执行上层回调函数
    void doPendingFunctors();

    using ChannelList = std::vector<Channel *>;

    std::atomic_bool looping_;

    std::atomic_bool quit_;

    // 标识 EventLoop 所属线程
    const pid_t threadId_;

    // 返回事件发生时间戳
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;

    //  当 Main Ractor 获取新的 Channel 时，以轮询方式选择一个 Sub Reactor 并 wakeupFd_ 将其唤醒来处理 Channel
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    // 标识当前 EventLoop 是否有需要执行的回调函数
    std::atomic_bool callingPendingFunctors_;

    // 存储 EventLoop 需要执行的回调函数
    std::vector<Functor> pendingFunctors_;

    // 保护对 pendingFunctor_ 的访问
    std::mutex mutex_;
};