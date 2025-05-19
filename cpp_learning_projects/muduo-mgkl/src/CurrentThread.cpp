#include "CurrentThread.hpp"

namespace CurrentThread {
    // 线程局部变量
    __thread int t_cachedTid = 0;

    void cacheTid() {
        if (t_cachedTid == 0) {
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
} // namespace CurrentThread