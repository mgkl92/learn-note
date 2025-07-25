#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
private:
    std::vector<std::thread> threads;

    std::queue<std::function<void()>> tasks;

    std::mutex tasks_mtx;

    std::condition_variable cond;

    bool stop;

public:
    ThreadPool(int = 10);

    ~ThreadPool();

    template <class F, class... Args>
    auto add(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
auto ThreadPool::add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(tasks_mtx);

        if (stop) {
            throw std::runtime_error("ThreadPool already stoped, can't add task any more!");
        }

        tasks.emplace([task]() { (*task)(); });
    }
    cond.notify_one();

    return res;
}
