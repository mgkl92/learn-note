#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace mgkl {
class ThreadPool {
public:
    // 线程池初始化
    ThreadPool(size_t nthreads);

    // 向任务队列中添加任务
    //
    template <class F, class... Args>
    auto push(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

    // 线程池析构
    ~ThreadPool();

private:
    // 线程池管理的工作线程
    std::vector<std::thread> workers;

    // 线程池管理的任务队列
    // FIFO 任务调度策略
    std::queue<std::function<void()>> tasks;

    // 主线程与子线程互斥访问任务队列
    std::mutex mut;

    // 条件变量用去唤醒被被阻塞（等待任务）的子线程
    std::condition_variable cond;

    // 线程池启标识
    bool stop;
};

} // namespace mgkl