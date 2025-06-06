#include "Acceptor.h"

#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"

Acceptor::Acceptor(EventLoop *loop_) :
    loop(loop_) {
    sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);

    sock->listen();
    // sock->setnonblocking();

    acceptChannel = new Channel(loop, sock->getFd());
    std::function<void()> callback = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setReadCallback(callback);

    acceptChannel->enableReading();
}

Acceptor::~Acceptor() {
   delete sock;
   delete acceptChannel;
}

void Acceptor::acceptConnection() {
    InetAddress *client_addr = new InetAddress();
    Socket *client_sock = new Socket(sock->accept(client_addr));
    printf("New client fd %d! IP: %s Port: %d\n", client_sock->getFd(), client_addr->getIp(), client_addr->getPort());
    client_sock->setnonblocking();

    newConnectionCallback(client_sock);

    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> newConnectionCallback_) {
    newConnectionCallback = newConnectionCallback_;
}