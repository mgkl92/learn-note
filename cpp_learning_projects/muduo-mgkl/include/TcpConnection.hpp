#pragma once

#include "Callbacks.hpp"
#include "NonCopyable.hpp"
#include "InetAddress.hpp"
#include "Buffer.hpp"
#include "Timestamp.hpp"
#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"

#include <memory>
#include <string>
#include <atomic>

class Channel;
class EventLoop;
class Socket;

class TcpConnection: NonCopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop * loop, const std::string & nameArg, int sockfd, const InetAddress & localAddr, const InetAddress & peerAddr);

    ~TcpConnection();

    EventLoop * getLoop() const {
        return loop_;
    }

    const std::string & name() const {
        return name_;
    }

    const InetAddress & localAddress() const {
        return localAddr_;
    }

    const InetAddress & peerAddress() const {
        return peerAddr_;
    }

    bool connected() const {
        return state_ == kConnected;
    }

    void send(const std::string & buf);

    void sendFile(int fileDescriptor, off_t offset, size_t count);

    void shutdown();

    void setConnectionCallback(const ConnectionCallback & cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback & cb) {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback & cb) {
        writeCompleteCallback_ = cb;
    }

    void setCloseCallback(const CloseCallback & cb) {
        closeCallback_ = cb;
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback & cb, size_t highWaterMark) {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    void connectEstablished();

    void connectDestroted();

private:
    enum StateE {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    };

    void setState(StateE state) {
        state_ = state;
    }

    void handleRead(Timestamp receiveTime);

    void handleWrite();

    void handleClose();

    void handleError();

    void sendInLoop(const void * data, size_t len);

    void shutdownInLoop();

    void sendFileInLoop(int fileDescriptor, off_t offset, size_t count);

    EventLoop * loop_;

    const std::string name_;

    std::atomic_int state_;

    bool reading_;

    std::unique_ptr<Socket> socket_;

    std::unique_ptr<Channel> channel_;

    const InetAddress localAddr_;

    const InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;

    MessageCallback messageCallback_;

    WriteCompleteCallback writeCompleteCallback_;

    HighWaterMarkCallback highWaterMarkCallback_;

    CloseCallback closeCallback_;

    size_t highWaterMark_;

    Buffer inputBuffer_;

    Buffer outputBuffer_;
};