#pragma once

#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
    EventLoop *mainReactor;

    Acceptor *acceptor;

    std::map<int, Connection *> connections;

    std::vector<EventLoop *> subReactors;

    ThreadPool *threadPool;

public:
    Server(EventLoop *);

    ~Server();

    void handReadEvent(int);

    void newConnection(Socket *);

    // void deleteConnection(Socket *);
    void deleteConnection(int sockfd);
};