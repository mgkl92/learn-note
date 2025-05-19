#include "Acceptor.hpp"
#include "Logger.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

static int createNonblocking() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

    if (sockfd < 0) {
    }

    return sockfd;
}

Acceptor::Acceptor(EventLoop * loop, const InetAddress & listenAddr, bool reuseport):
    loop_(loop),
    acceptSocket_(createNonblocking()),
    acceptChannel_(loop, acceptSocket_.fd()),
    listenning_(false) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setResuePort(true);
    acceptSocket_.bindAddress(listenAddr);

    // TcpServer::start() -> Acceptor.listen()
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen() {
    listenning_ = true;
    acceptSocket_.listen();

    // 将 Channel 注册到 Poller
    acceptChannel_.enableReading();
}

// 读事件发生 -> 新用户连接
void Acceptor::handleRead() {
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);

    if (connfd >= 0) {
        if (NewConnectionCallback_) {
            // 轮询找到 Sub Reactor 唤醒并分发当前新客户端的 Channel
            NewConnectionCallback_(connfd, peerAddr);
        } else {
            ::close(connfd);
        }
    } else {
        LOG_ERROR("%s:%s:%d accept err:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        if (errno == EMFILE) {
            LOG_ERROR("%s:%s:%d sockfd reached limit\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}