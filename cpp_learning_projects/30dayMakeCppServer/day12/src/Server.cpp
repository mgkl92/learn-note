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

Server::Server(EventLoop *loop_) : mainReactor(loop_), acceptor(nullptr) {
    using namespace std::placeholders;

    acceptor = new Acceptor(mainReactor);

    std::function<void(Socket*)> callback = std::bind(&Server::newConnection, this, _1);
    acceptor->setNewConnectionCallback(callback);

    int size = std::thread::hardware_concurrency();
    threadPool = new ThreadPool(size);

    for (int i = 0; i < size; ++i) {
        subReactors.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        threadPool->add(sub_loop);
    }
}

Server::~Server() {
    delete acceptor;
    delete threadPool;
}

void Server::handReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while (true) { // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));

        ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));

        if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf);
            ::write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) { // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (bytes_read == 0) { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            ::close(sockfd); // 关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
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
        auto it = connections.find(sockfd);
        if (it != connections.end()) {
            Connection *conn = connections[sockfd];
            connections.erase(sockfd);
            ::close(sockfd);
            
            if (conn) {
                delete conn;
            }
        }
    }
}