#pragma once

#include "NonCopyable.hpp"

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

class Thread: NonCopyable {
public:
    // 后续通过 std::bind 进行参数绑定
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string & name = std::string());

    ~Thread();

    void start();

    void join();

    bool started() {
        return started_;
    }

    pid_t tid() const {
        return tid_;
    }

    const std::string & name() const {
        return name_;
    }

    static int numCreated() {
        return numCreated_;
    }

private:
    void setDefaultName();

    bool started_;

    bool joined_;

    std::shared_ptr<std::thread> thread_;

    // 线程 id
    pid_t tid_;

    // 线程函数
    ThreadFunc func_;

    // 线程名称
    std::string name_;

    static std::atomic_int numCreated_;
};