#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

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

    void add(std::function<void()>);
};