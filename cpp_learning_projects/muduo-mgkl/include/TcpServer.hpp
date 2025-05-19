#pragma once

#include "EventLoop.hpp"
#include "Acceptor.hpp"
#include "InetAddress.hpp"
#include "NonCopyable.hpp"
#include "EventLoopThreadPool.hpp"
#include "Callbacks.hpp"
#include "TcpConnection.hpp"
#include "Buffer.hpp"

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <atomic>

class TcpServer {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    enum Option {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop * loop,
              const InetAddress & listenAddr,
              const std::string & namArg,
              Option option = kNoReusePort);

    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback & cb) {
        threadInitCallback_ = cb;
    }

    void setConnectionCallback(const ConnectionCallback & cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback & cb) {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback & cb) {
        writeCompleteCallback_ = cb;
    }

    void setThreadNum(int numThreads);

    void start();

private:
    void newConnection(int sockfd, const InetAddress & peerAddr);

    void removeConnection(const TcpConnectionPtr & connection);

    void removeConnectionInLoop(const TcpConnectionPtr & connect);

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop * loop_;

    const std::string ipPort_;

    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;

    std::unique_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;

    MessageCallback messageCallback_;

    WriteCompleteCallback writeCompleteCallback_;

    ThreadInitCallback threadInitCallback_;

    int numThreads_;

    std::atomic_int started_;

    int nextConnId_;

    ConnectionMap connections_;
};