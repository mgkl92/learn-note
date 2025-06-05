#include "util.h"

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
    server_addr.sin_port = ::htons(8888);

    errif(::bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1, "socket bind error");

    errif(::listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    bzero(&client_addr, len);

    int client_sockfd = ::accept(sockfd, (sockaddr *)&client_addr, &len);
    errif(client_sockfd == -1, "socket accept error");

    printf("New Client fd %d! IP: %s, Port: %d\n", client_sockfd, ::inet_ntoa(client_addr.sin_addr), ::ntohs(client_addr.sin_port));

    while (true) {
        char buf[1024];
        bzero(&buf, sizeof(buf));

        ssize_t read_bytes = read(client_sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client fd %d:%s\n", client_sockfd, buf);
            ::write(client_sockfd, buf, sizeof(buf));
        } else if (read_bytes == 0) {
            printf("clinet fd %d disconnected\n", client_sockfd);
            ::close(client_sockfd);
        } else if (read_bytes == -1) {
            ::close(client_sockfd);
            errif(true, "socket read error");
        }
    }

    ::close(sockfd);

    return 0;
}
