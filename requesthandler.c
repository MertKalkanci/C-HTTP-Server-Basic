#include "include.h"
#include "requesthandler.h"

void handle_request(int *p_connfd)
{
    int connfd = *p_connfd;
    free(p_connfd);

    int n;
    char buff[MAXLINE];
    char recvline[MAXLINE];

    memset(recvline, 0, sizeof(recvline));
    
    while ((n = recv(connfd, recvline, MAXLINE-1, 0)) > 0)
    {
        if(DEBUG_PRINT_INCOMING_REQUESTS)
        {
            printf("%s\n", recvline); //to debug
        }

        // Check if the last character is \r\n\r\n at the future
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
        return;
    }
    
    snprintf((char*)buff, sizeof(buff), "HTTP /1.1 200 OK\r\n\r\n<h1>Hello</h1>");

    send(connfd, buff, strlen(buff), 0);
    close(connfd);
    return;
}