#pragma once

#include <future>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace mgkl {
class ThreadPool {
public:
    // 创建并启动线程池
    ThreadPool(size_t nthreads = 0);

    ~ThreadPool();

    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:   
    std::vector<std::thread> workers;
    void work();

    std::queue<std::function<void()>> tasks;

    std::mutex mut;
    std::condition_variable cond;

    bool exit_flag;
};
} // namespace mgkl