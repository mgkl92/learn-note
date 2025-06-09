#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"

#include <functional>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *loop_) :
    loop(loop_),
    acceptor(nullptr) {
    using namespace std::placeholders;
    
    acceptor = new Acceptor(loop);
    std::function<void(Socket *)> callback = std::bind(&Server::newConnection, this, _1);
    acceptor->setNewConnectionCallback(callback);
}

Server::~Server() {
    delete acceptor;
}

void Server::newConnection(Socket *serv_sock) {
    using namespace std::placeholders;

    Connection *conn = new Connection(loop, serv_sock);
    std::function<void(Socket *)> callback = std::bind(&Server::deleteConnection, this, _1);
    conn->setDeleteConnectionCallback(callback);
    serv_sock->setnonblocking();
    
    connections[serv_sock->getFd()] = conn;
}

void Server::deleteConnection(Socket *sock) {
    Connection *conn = connections[sock->getFd()];
    if (connections.find(sock->getFd()) != connections.end()) {
        connections.erase(sock->getFd());
        delete conn;
    }
}