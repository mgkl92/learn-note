#include "Poller.hpp"
#include "EPollPoller.hpp"

#include <stdlib.h>

Poller * Poller::newDefaultPoller(EventLoop * loop) {
    if (::getenv("MODUO_USE_POLL")) {
        // 生成 Poll 实例
        return nullptr;
    } else {
        // 生成 EPoll 实例
        return new EPollPoller(loop);
    }
}