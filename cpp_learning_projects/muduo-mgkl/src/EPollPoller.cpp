#include "EPollPoller.hpp"
#include "Channel.hpp"
#include "Logger.hpp"

#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

// 与 Channel 中 index_ 成员变量相关，表示 Channel 与 Poller 之间的状态
// Channel 尚未添加到 Poller
const int kNew = -1;

// Channel 已经添加到 Poller
const int kAdded = 1;

// Channel 已经从 Poller 删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop * loop):
    Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)), events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        LOG_FATAL("epoll_create error:%d \n", errno);
    }
}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList * activeChannels) {
    LOG_INFO("func=%s => fd total count:%lu\n", __FUNCTION__, channels_.size());

    int numEvents = ::epoll_wait(epollfd_, &(*events_.begin()), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;

    Timestamp now(Timestamp::now());

    if (numEvents > 0) {
        LOG_INFO("%d events happend\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);

        // 由 Poller 监听的所有事件均发生！
        if (numEvents == events_.size()) {
            // 扩容
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        // 超时
        LOG_DEBUG("%s timeout!\n", __FUNCTION__);
    } else {
        // 检查错误码是否为外部中断
        if (saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() error!");
        }
    }

    return now;
}

// updateChannel 只是对 epoll 监听的 Channel 进行监听状态的更新
// 并不会将 Channel 从 Poller 管理的 channels_ 中移除
// 这意味着 channels_ 的大小要大于等于 Poller 实际监听的 Channel 数量
void EPollPoller::updateChannel(Channel * channel) {
    const int index = channel->index();

    LOG_INFO("func=%s => fd=%d events=%d index=%d\n", __FUNCTION__, channel->fd(), channel->events(), index);

    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        } else {
            // kDeleted 的对象已经在 channel_ 中，无需处理
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        // 当前 Channel 无感兴趣事件
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            // 更新感兴趣事件集合
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel * channel) {
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_INFO("func=%s => fd=%d\n", __FUNCTION__, fd);

    int index = channel->index();
    // 如果当前 channel 处于被监听状态，则将其删除
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList * activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        Channel * channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel * channel) {
    epoll_event event;

    ::memset(&event, 0, sizeof(event));

    int fd = channel->fd();

    event.events = channel->events();
    // event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        } else {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}