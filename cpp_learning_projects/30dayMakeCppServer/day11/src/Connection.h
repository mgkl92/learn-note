#pragma once

#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
private:
    EventLoop *loop;

    Socket *sock;

    Channel *channel;

    std::function<void(int)> deleteConnectionCallback;

    std::string *inBuffer;

    Buffer *readBuffer;
    
public:
    Connection(EventLoop *, Socket *);
    
    ~Connection();

    void echo(int);

    void setDeleteConnectionCallback(std::function<void(int)>);

    void send(int sockfd);
};