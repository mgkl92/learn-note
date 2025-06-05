#pragma once

class EventLoop;
class Socket;
class Server {
    EventLoop *loop;

public:
    Server(EventLoop *);

    ~Server();

    void handReadEvent(int);
    
    void newConnection(Socket *serv_sock);
};