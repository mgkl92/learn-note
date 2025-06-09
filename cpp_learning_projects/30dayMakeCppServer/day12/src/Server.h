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

    ThreadPool *thpool;

public:
    Server(EventLoop *);

    ~Server();

    void newConnection(Socket *);

    void deleteConnection(int);
};