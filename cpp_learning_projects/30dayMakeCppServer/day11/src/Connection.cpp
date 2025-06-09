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
    channel->enableReading();
    channel->useET();

    std::function<void()> callback = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(callback);

    channel->setUseThreadPoll();
    readBuffer = new Buffer();
}

Connection::~Connection() {
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::echo(int sockfd) {
    char buf[1024];

    while (true) {
        bzero(&buf, sizeof(buf));

        ssize_t read_bytes = ::read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            readBuffer->append(buf, read_bytes);
        } else if (read_bytes == -1 && errno == EINTR) {
            printf("continue reading.");
            continue;
        } else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            send(sockfd);

            readBuffer->clear();
            break;
        } else if (read_bytes == 0) {
            printf("EOF, client fd %d disconnected\n", sockfd);
            // deleteConnectionCallback(sockfd);
            break;
        } else {
            printf("Connection reset by peer\n");
            // deleteConnectionCallback(sockfd);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> deleteConnectionCallback_) {
    deleteConnectionCallback = deleteConnectionCallback_;
}

void Connection::send(int sockfd) {
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int data_size = readBuffer->size();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t write_bytes = ::write(sockfd, buf + data_size - data_left, data_left);
        if (write_bytes == -1 && errno == EAGAIN) {
            break;
        }

        data_left -= write_bytes;
    }
}