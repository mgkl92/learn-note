#include "EventLoop.hpp"
#include "Channel.hpp"
#include "Poller.hpp"
#include "Logger.hpp"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory>

// one Loop per Thread
__thread EventLoop * t_loopInThisThread = nullptr;

// 默认超时事件：10s
const int kPollTimeMs = 10e4;

// 创建 wakeupFd 用来唤醒 Sub Reactor 来处理新的 Channel
int createEventfd() {
    int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    if (eventfd < 0) {
        LOG_FATAL("eventfd error:%d\n", errno);
    }

    return eventfd;
}

EventLoop::EventLoop():
    looping_(false),
    quit_(false),
    threadId_(CurrentThread::tid()),
    poller_(Poller::newDefaultPoller(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    callingPendingFunctors_(false) {
    LOG_DEBUG("EventLoop created %p in thread %d\n", this, threadId_);

    if (t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d\n", t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }

    // 保证 Main Reactor 能够唤醒 Sub Ractor
    // 设置 wapkeuFd 的事件类型及发生事件后的回调函数
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();

    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping\n", this);

    while (!quit_) {
        activeChannels_.clear();

        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

        // Poller 监听那些 Channel 有事件发生， 然后让 EventLoop 通知相应 Channel 进行事件处理
        for (Channel * channel : activeChannels_) {
            channel->handleEvent(pollReturnTime_);
        }

        // 执行当前 EventLoop 事件循环需要处理的回调操作
        doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping.\n", this);

    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;

    // 如果在 Sub Ractor 中调用了 Main Reactor 的 quit 方法
    // 将会唤醒 Main Reactor
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    // 数据不重要，主要用于唤醒 Sub Reactor
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8\n", n);
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8\n", n);
    }
}

void EventLoop::updateChannel(Channel * channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel * channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel * channel) {
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor & functor : functors) {
        functor();
    }

    callingPendingFunctors_ = false;
}