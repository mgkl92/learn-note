#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"

#include <unistd.h>
#include <string.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *loop_, Socket *sock_) :
    loop(loop_), sock(sock_), channel(nullptr), inBuffer(new std::string()), readBuffer(nullptr) {
    channel = new Channel(loop, sock->getFd());
    std::function<void()> callback = std::bind(&Connection::echo, this, sock->getFd());

    channel->setCallback(callback);
    channel->enableReading();

    readBuffer = new Buffer();
}

Connection::~Connection() {
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::echo(int sockfd) {
    char buf[READ_BUFFER];

    while (true) {
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = ::read(sockfd, buf, sizeof(buf));

        if (read_bytes > 0) {
            readBuffer->append(buf, read_bytes);
        } else if (read_bytes == -1 && (errno == EINTR)) {
            printf("continue reading");
            continue;
        } else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            printf("finish reading once\n");
            printf("message from client fd %d: %s \n", sockfd, readBuffer->c_str());
            errif(::write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            readBuffer->clear();
            break;
        } else if (read_bytes == 0) {
            printf("EOF, client fd %d disconnected\n", sockfd);
            // ::close(sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> deleteConnectionCallback_) {
    deleteConnectionCallback = deleteConnectionCallback_;
}
