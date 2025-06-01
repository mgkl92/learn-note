#include "ThreadPool.h"

namespace mgkl {
ThreadPool::ThreadPool(size_t nthreads) :
    exit_flag(false) {
    if (nthreads == 0) {
        nthreads = std::thread::hardware_concurrency();
    }

    for (size_t i = 0; i < nthreads; ++i) {
        auto worker = std::thread(&ThreadPool::work, this);
        workers.push_back(std::move(worker));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mut);
        exit_flag = true;
    }

    cond.notify_all();

    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::work() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mut, std::defer_lock);
            cond.wait(lock, [&] {
                return exit_flag || !tasks.empty();
            });

            if (exit_flag && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

template <typename F, typename... Args>
auto ThreadPool::submit(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto res = task_ptr->get_future();
    {
        std::lock_guard<std::mutex> loclck(mut);
        tasks.emplace([task_ptr]() {
            (*task_ptr)();
        });
    }

    cond.notify_one();

    return res;
}

} // namespace mgkl

#include <iostream>

int main(int argc, char const *argv[]) {
    auto f = [](int i) {
        std::cout << "Id is " << i << std::endl;
        return 0;
    };
    mgkl::ThreadPool pool(2);

    for (size_t i = 0; i < 10; ++i) {
        pool.submit(f, i);
    }

    return 0;
}
