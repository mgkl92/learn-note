#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <vector>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void handleReadEvent(int);

int main(int argc, char const *argv[]) {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();

    Epoll *ep = new Epoll();
    // ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
    serv_sock->setnonblocking();

    Channel *serv_channel = new Channel(ep, serv_sock->getFd());
    serv_channel->enableReading();
    
    while (true) {
        std::vector<Channel *> activeChannels = ep->poll();
        int nfds = activeChannels.size();
        for (size_t i = 0; i < nfds; ++i) {
            int chfd = activeChannels[i]->getFd();
            if (chfd == serv_sock->getFd()) {
                InetAddress *client_addr = new InetAddress();
                Socket *client_sock = new Socket(serv_sock->accept(client_addr));

                printf("New Client fd %d ! IP: %s, Port: %d\n", client_sock->getFd(), ::inet_ntoa(client_addr->addr.sin_addr), ::ntohs(client_addr->addr.sin_port));
                client_sock->setnonblocking();
                
                Channel *client_channel = new Channel(ep, client_sock->getFd());
                client_channel->enableReading();
            } else if (activeChannels[i]->getRevents() & EPOLLIN) {
                // 处理读事件
                handleReadEvent(activeChannels[i]->getFd());
            } else {
            }
        }
    }

    delete serv_sock;
    delete serv_addr;
    return 0;
}

void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));

        ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));

        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            ::write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}