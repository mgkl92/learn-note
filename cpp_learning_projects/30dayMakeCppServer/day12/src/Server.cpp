#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "EventLoop.h"

#include <functional>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define READ_BUFFER 1024

Server::Server(EventLoop *loop_) :
    mainReactor(loop_),
    acceptor(nullptr) {
    using namespace std::placeholders;

    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket *)> callback = std::bind(&Server::newConnection, this, _1);
    acceptor->setNewConnectionCallback(callback);

    int size = std::thread::hardware_concurrency();

    printf("size is %d\n", size);
    
    thpool = new ThreadPool(size);

    for (int i = 0; i < size; ++i) {
        subReactors.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        thpool->add(sub_loop);
    }
}

Server::~Server() {
    delete acceptor;
    delete thpool;
}

void Server::newConnection(Socket *sock) {
    using namespace std::placeholders;

    if (sock->getFd() != -1) {
        int random = sock->getFd() % subReactors.size();
        Connection *conn = new Connection(subReactors[random], sock);
        std::function<void(int)> callback = std::bind(&Server::deleteConnection, this, _1);
        conn->setDeleteConnectionCallback(callback);
        connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(int sockfd) {
    if (sockfd != -1) {
        if (connections.find(sockfd) != connections.end()) {
            Connection *conn = connections[sockfd];
            ::close(sockfd);
            // delete conn;
        }
    }
}