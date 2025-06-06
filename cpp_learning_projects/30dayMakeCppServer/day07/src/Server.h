#pragma once

class EventLoop;
class Socket;
class Acceptor;

class Server {
    EventLoop *loop;

    Acceptor *acceptor;

public:
    Server(EventLoop *);

    ~Server();

    void handReadEvent(int);
    
    void newConnection(Socket *);
};