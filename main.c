#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SERVER_PORT 8080
#define MAXLINE 1024

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    int sendbytes;
    struct sockaddr_in server_addr;
    char buff[MAXLINE+1];
    char recvline[MAXLINE+1];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket creation error");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("bind error");
        exit(1);
    }

    if(listen(listenfd, 10) < 0)
    {
        perror("listen error");
        exit(1);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;

        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);

        memset(recvline, 0, sizeof(recvline));
        while ((n = recv(connfd, recvline, MAXLINE-1, 0)) > 0)
        {
            printf("%s\n", recvline);
            
            // Check if the last character is \r\n\r\n with strcmp
            if (recvline[n-1] == '\n')
            {   
                break;
            }
            memset(recvline, 0, MAXLINE);
        }
        if (n < 0)
        {
            perror("recv error");
            close(connfd);
            continue;
        }
        
        snprintf((char*)buff, sizeof(buff), "HTTP /1.1 200 OK\r\n\r\n<h1>Hello</h1>");

        send(connfd, buff, strlen(buff), 0);
        close(connfd);
    }
    exit(0);
}