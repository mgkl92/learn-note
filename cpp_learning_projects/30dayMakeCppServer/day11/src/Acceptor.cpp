#include "Acceptor.h"

#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"

#include <arpa/inet.h>

Acceptor::Acceptor(EventLoop *loop_) :
    loop(loop_) {
    sock = new Socket();
    addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);

    sock->listen();
    sock->setnonblocking();
    
    std::function<void()> callback = std::bind(&Acceptor::acceptConnection, this);

    acceptChannel = new Channel(loop, sock->getFd());
    acceptChannel->setReadCallback(callback);
    acceptChannel->enableReading();
    acceptChannel->setUseThreadPoll(false);
}

Acceptor::~Acceptor() {
    delete sock;
    delete addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection() {
    InetAddress *client_addr = new InetAddress();
    Socket *client_sock = new Socket(sock->accept(client_addr));

    printf("New client fd %d! IP: %s, Port: %d\n", client_sock->getFd(), ::inet_ntoa(client_addr->getAddr().sin_addr), ::ntohs(client_addr->getAddr().sin_port));
    
    client_sock->setnonblocking();
    newConnectionCallback(client_sock);

    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> newConnectionCallback_) {
    newConnectionCallback = newConnectionCallback_;
}