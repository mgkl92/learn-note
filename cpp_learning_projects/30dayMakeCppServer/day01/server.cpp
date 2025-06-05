#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr));

    listen(sockfd, SOMAXCONN);

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    bzero(&client_addr, len);

    int client_sockfd = accept(sockfd, (sockaddr *)&client_addr, &len);

    printf("New Client fd %d! IP: %s, Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    return 0;
}
