#include "util.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(int argc, char const *argv[]) {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
    serv_addr.sin_port = ::htons(8888);

    errif(::bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    errif(::listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == sockfd) {
                // 新连接
                struct sockaddr_in client_addr;
                bzero(&client_addr, sizeof(client_addr));
                socklen_t len = sizeof(client_addr);

                int client_sockfd = accept(sockfd, (sockaddr *)&client_addr, &len);
                errif(client_sockfd == -1, "socket accept error");
                printf("New Client fd %d! IP: %s, Port: %d\n", client_sockfd, ::inet_ntoa(client_addr.sin_addr), ::ntohs(client_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(client_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
            } else if (events[i].events & EPOLLIN) {
                // 读事件
                char buf[READ_BUFFER];
                while (true) {
                    bzero(&buf, sizeof(buf));

                    ssize_t read_bytes = ::read(events[i].data.fd, buf, sizeof(buf));
                    if (read_bytes > 0) {
                        printf("Message from client fd %d: %s\n", events[i].data.fd, buf);
                        ::write(events[i].data.fd, buf, sizeof(buf));
                    } else if (read_bytes == -1 && errno == EINTR) {
                        // 客户端正常中断：继续读取
                        printf("Continue reading");
                        continue;
                    } else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                        // 数据已完成读取
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if (read_bytes == 0) {
                        // 客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        ::close(events[i].data.fd);
                        break;
                    }
                }
            } else {
            }
        }
    }
    ::close(sockfd);
    return 0;
}
