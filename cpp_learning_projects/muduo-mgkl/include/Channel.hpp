#pragma once

#include "NonCopyable.hpp"
#include "Timestamp.hpp"

#include <functional>
#include <memory>

// 使用前置类型声明，在实际 C++ 文件中包含相应的头文件，能够减少头文件内容的暴露
class EventLoop;

using std::move;
using std::shared_ptr;

/**
 * 封装 sockfd 和其感兴趣的 event, 如 EPOLLIN，EPOLLOUT 等
 *
 * 绑定 Poller 返回的具体事件？
 */
class Channel: NonCopyable {
public:
    using EventCallBack = std::function<void()>;

    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop * loop, int fd);

    ~Channel();

    // 事件处理
    void handleEvent(Timestamp receiveTime);

    // 设置回调函数
    void setReadCallback(ReadEventCallback rcb) {
        readCallback_ = move(rcb);
    }

    void setWriteCallback(EventCallBack wcb) {
        writeCallback_ = move(wcb);
    }

    void setCloseCallback(EventCallBack ccb) {
        closeCallback_ = move(ccb);
    }

    void setErrorCallback(EventCallBack ecb) {
        errorCallback_ = move(ecb);
    }

    // 防止当前 Channel 被手动移除后，Channel 对象还在执行回调函数
    // 使用 shared_ptr + weak_ptr 避免形成循环引用
    void tie(const shared_ptr<void> &);

    int fd() const {
        return fd_;
    }

    int events() const {
        return events_;
    }

    // 由 Poller 设置具体发生事件的集合
    void set_revents(int revents) {
        revents_ = revents;
    }

    // 启用/禁用事件
    void enableReading() {
        events_ |= kReadEvent;
        update();
    }

    void disableReading() {
        events_ &= ~kReadEvent;
        update();
    }

    void enableWriting() {
        events_ |= kWriteEvent;
        update();
    }

    void disableWriting() {
        events_ &= ~kWriteEvent;
        update();
    }

    // 禁用所有事件
    void disableAll() {
        events_ = kNoneEvent;
        update();
    }

    // 返回 sockfd 当前事件状态
    bool isNoneEvent() const {
        return events_ == kNoneEvent;
    }

    bool isWriting() const {
        return events_ & kWriteEvent;
    }

    bool isReading() const {
        return events_ & kReadEvent;
    }

    int index() const {
        return index_;
    }

    void set_index(int index) {
        index_ = index;
    }

    // Channel 所属 EventLoop
    EventLoop * ownerLoop() {
        return loop_;
    }

    // 通过本 Channel 所属 EventLoop 移除该 Channel
    void remove();

private:
    // 通过 Channel 所属 EventLoop 中的 Poller 进行事件注册
    void update();

    // 事件处理
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;  // 不对任何事件感兴趣
    static const int kReadEvent;  // 读事件
    static const int kWriteEvent; // 写事件

    // fd 所属事件循环对象
    EventLoop * loop_;

    // Poller 监听的 fd
    // 或者用于 Main Reator 标识 Sub Reactor 的唤醒 eventfd
    const int fd_;

    // fd 感兴趣事件集合
    int events_;

    // Poller 监听到的具体事件的集合
    int revents_;

    // 记录当前 Channel 与 Poller 之间的关系
    // kNew = -1, kAdded = 1, kDeleted = 2
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    // 存储事件发生后相应的回调函数
    // Channel 根据 Poller 返回的事件集合来执行回调函数
    ReadEventCallback readCallback_;
    EventCallBack writeCallback_;
    EventCallBack closeCallback_;
    EventCallBack errorCallback_;
};