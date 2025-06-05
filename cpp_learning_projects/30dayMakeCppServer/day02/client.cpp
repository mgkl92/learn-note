#include "util.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
    server_addr.sin_port = ::htons(8888);

    connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr));

while(true){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        scanf("%s", buf);

        ssize_t write_bytes = ::write(sockfd, buf, sizeof(buf));
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = ::read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            printf("message from server: %s\n", buf);
        }else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){
            ::close(sockfd);
            errif(true, "socket read error");
        }
    }
    
    close(sockfd);
    return 0;
}
