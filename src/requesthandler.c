#include "include.h"
#include "requesthandler.h"

char html_success_header[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n"
                    "\r\n";
char html_404_header[] = "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n"
                    "\r\n";
char file_path[] = "./static";
char error_page_file_path[] = "./static/404.html";

void handle_request(int *p_connfd)
{
    int connfd = *p_connfd;
    free(p_connfd);

    int n;
    char buff[MAXLINE];
    char recvline[MAXLINE];

    char request_path[MAXLINE];
    bool is_get_request = false;

    memset(recvline, 0, sizeof(recvline));
    
    //recieve and parse request
    while ((n = recv(connfd, recvline, MAXLINE-1, 0)) > 0)
    {
        if(DEBUG_INCOMING_REQUEST_VERBOSELY)
        {
            printf("%s\n", recvline); //to debug
        }

        char *get_req = strstr(recvline, "GET");
        if(get_req != NULL)
        {
            is_get_request = true;
            //GET / HTTP/1.1
            //GET /index.html HTTP/1.1
            // select the path 
            char *path_start_p = get_req + strlen("GET ");
            char *path_end_p = strstr(path_start_p, "HTTP/1.1");

            if (path_end_p == NULL)
            {
                perror("incorrect request error");
                close(connfd);
                return;
            }

            int path_length = path_end_p - path_start_p - 1;

            strncpy(request_path, path_start_p, path_length);
            request_path[path_length] = '\0';
        }

        // Check if the last character is \r\n\r\n at the future
        if (strstr(recvline,"\r\n\r\n") != NULL)
        {   
            if (DEBUG_INCOMING_REQUEST_VERBOSELY)
            {
                printf("End of request\n");
            }
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
    
    // automatically match with index.html
    if(strcmp(request_path, "/") == 0)
    {
        strcpy(request_path, "/index.html");
    }

    if (DEBUG_REQUEST_HANDLER)
    {
        printf("====\nRequest path: %s\n", request_path);
        printf("Is GET request: %s\n", is_get_request ? "true" : "false");
    }

    // check if page_file exists
    char full_path[MAXLINE];
    snprintf(full_path, sizeof(full_path), "%s%s\0", file_path, request_path);

    FILE *page_file = fopen(full_path, "r");
    if (page_file == NULL)
    {        
        if (DEBUG_REQUEST_HANDLER)
        {
            printf("404 Not Found: %s\n====\n", full_path);
        }

        // send 404 header
        snprintf(buff, sizeof(buff), html_404_header);
        send(connfd, buff, strlen(buff), 0);

        // read error page file and send to client        
        FILE *error_page_file = fopen(error_page_file_path, "r");
        if (error_page_file == NULL)
        {
            //printf("Error 404 page on path: %s not found!\n",error_page_file_path);
            perror("Error 404 page at ./static/404.html path not found");
        }
        else
        {
            while (fgets(buff, sizeof(buff), error_page_file) != NULL)
            {
                send(connfd, buff, strlen(buff), 0);
            }
            fclose(error_page_file);
        }
    }
    else
    {
        // send header
        snprintf(buff, sizeof(buff), "%s", html_success_header);
        send(connfd, buff, strlen(buff), 0);

        // read page_file and send to client
        while (fgets(buff, sizeof(buff), page_file) != NULL)
        {
            send(connfd, buff, strlen(buff), 0);
        }

        if (DEBUG_REQUEST_HANDLER)
        {
            printf("200 OK: %s\n====\n", full_path);
        }
        fclose(page_file);
    }

    close(connfd);
    return;
}