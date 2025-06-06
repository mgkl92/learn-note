#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"

#include <unistd.h>
#include <string.h>
#include <iostream>

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
    if (channel) {
        delete channel;
    }

    if (sock) {
        delete sock;
    }

    if (inBuffer) {
        delete inBuffer;
    }

    if (readBuffer) {
        delete readBuffer;
    }
}

// void Connection::echo(int sockfd) {
//     char buf[READ_BUFFER];
//     while (true) { // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//         bzero(&buf, sizeof(buf));
//         ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
//         if (bytes_read > 0) {
//             printf("message from client fd %d: %s\n", sockfd, buf);
//             ::write(sockfd, buf, sizeof(buf));
//         } else if (bytes_read == -1 && errno == EINTR) { // 客户端正常中断、继续读取
//             printf("continue reading");
//             continue;
//         } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { // 非阻塞IO，这个条件表示数据全部读取完毕
//             printf("finish reading once, errno: %d\n", errno);
//             break;
//         } else if (bytes_read == 0) { // EOF，客户端断开连接
//             printf("EOF, client fd %d disconnected\n", sockfd);
//             // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
//             deleteConnectionCallback(sock);
//             break;
//         }
//     }
// }

void Connection::echo(int sockfd) {
    char buf[1024]; // 这个buf大小无所谓
    while (true) {  // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            readBuffer->append(buf, bytes_read);
        } else if (bytes_read == -1 && errno == EINTR) { // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            errif(::write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            readBuffer->clear();
            break;
        } else if (bytes_read == 0) { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            deleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> deleteConnectionCallback_) {
    deleteConnectionCallback = deleteConnectionCallback_;
}
