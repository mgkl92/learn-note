#include "ThreadPool.h"

ThreadPool::ThreadPool(int size) {
    for (int i = 0; i < size; ++i) {
        threads.emplace_back(std::thread([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx, std::defer_lock);
                    cond.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty()) {
                        return;
                    }

                    task = tasks.front();
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }

    cond.notify_all();
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

// void ThreadPool::add(std::function<void()> func) {
//     {
//         std::unique_lock<std::mutex> lock(tasks_mtx);
//         if (stop) {
//             throw std::runtime_error("ThreadPool already stoped, can't add task any more!");
//         }

//         cond.notify_one();
//     }
// }