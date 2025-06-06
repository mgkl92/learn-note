#include "Acceptor.h"

#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"

Acceptor::Acceptor(EventLoop *loop_) :
    loop(loop_) {
    sock = new Socket();
    addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);

    sock->listen();
    sock->setnonblocking();

    acceptChannel = new Channel(loop, sock->getFd());
    std::function<void()> callback = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setCallback(callback);

    acceptChannel->enableReading();
}

Acceptor::~Acceptor() {
    if (sock) {
        delete sock;
    }

    if (addr) {
        delete addr;
    }

    if (acceptChannel) {
        delete acceptChannel;
    }
}

void Acceptor::acceptConnection() {
    newConnectionCallback(sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> newConnectionCallback_) {
    newConnectionCallback = newConnectionCallback_;
}