#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"

#include <sys/epoll.h>

// 空事件
const int Channel::kNoneEvent = 0;

// （紧急）读事件
const int Channel::kReadEvent = (EPOLLIN | EPOLLPRI);

// 写事件
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop * loop, int fd):
    loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    tied_(false) {}

Channel::~Channel() {}

void Channel::tie(const std::shared_ptr<void> & obj) {
    tie_ = obj;
    tied_ = true;
}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
    if (tied_) {
        // 将 weak_ptr 提升为 shared_ptr
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    } else {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    LOG_INFO("channel handleEvent revents:%d\n", revents_);

    // shutdown
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    // error
    if (revents_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    // read
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    // write
    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}