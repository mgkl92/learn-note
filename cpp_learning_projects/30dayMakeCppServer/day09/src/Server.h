#pragma once

#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server {
    EventLoop *loop;

    Acceptor *acceptor;

    std::map<int, Connection *> connections;

public:
    Server(EventLoop *);

    ~Server();

    void handReadEvent(int);

    void newConnection(Socket *);

    void deleteConnection(Socket *);
};