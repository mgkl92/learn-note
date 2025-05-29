#include "ThreadPool.h"

#include <iostream>

namespace mgkl {
ThreadPool::ThreadPool(size_t nthreads) {
    for (size_t i = 0; i < nthreads; ++i) {
        workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;

                {
                    // 使用 unique_lock 而非 lock_guard
                    // 由条件变量来控制加锁和释放锁的过程
                    std::unique_lock<std::mutex> lock(this->mut);
                    this->cond.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });

                    // 保证任务队列中的所有任务都能够得到执行
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }

                    // 子线程从任务队列中获取单个任务
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                // 子线程执行任务
                task();
            }
        });
    }
}

template <class F, class... Args>
auto ThreadPool::push(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    // 绑定任务与参数列表
    // auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto task = std::make_unique<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mut);
        if (stop) {
            throw std::runtime_error("ThreadPool stoped.");
        }

        // tasks.emplace([task]() { (*task)(); });
        task.push(std::move(task));
    }

    cond.notify_one();
    return res;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mut);
        stop = true;
    }
    cond.notify_all();
    for (auto &worker : workers) {
        worker.join();
    }
}

} // namespace mgkl

int main() {
    auto f = []() { return 1; };
    std::queue<std::function<void()>> q;
    auto task = std::make_shared<std::packaged_task<int()>>(f);
    std::future<int> res = task->get_future();
    q.emplace([task]() { (*task)(); });

    q.front()();
    q.pop();
    std::cout << "result of task : " << res.get() << std::endl;
    return 0;
}
