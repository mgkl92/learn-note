#include "Thread.hpp"
#include "CurrentThread.hpp"

#include <memory>
#include <semaphore.h>

std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string & name):
    started_(false),
    joined_(false),
    tid_(0),
    func_(std::move(func)),
    name_(name) {
}

Thread::~Thread() {
    // 确保在销毁线程对象前对其调用 join() 或者 detach()
    if (started_ && !joined_) {
        thread_->detach();
    }
}

void Thread::start() {
    started_ = true;

    // 信号量
    sem_t sem;
    sem_init(&sem, false, 0);

    thread_ = std::shared_ptr<std::thread>(
        new std::thread(
            [&]() {
                tid_ = CurrentThread::tid();
                sem_post(&sem);
                func_();
            }));

    // 因为线程之间执行无序，所以需要等待 tid_ 的生成
    sem_wait(&sem);
}

void Thread::join() {
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName() {
    int num = ++numCreated_;
    if (name_.empty()) {
        char buf[32] = { 0 };
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}
