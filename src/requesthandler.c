#include "include.h"
#include "route.h"
#include "requesthandler.h"

//extern route_t *root;
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
    char recvline[MAXLINE];

    char request_path[MAXLINE];
    char request_file_path[MAXLINE];
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

    if (DEBUG_REQUEST_HANDLER)
    {
        printf("====\nRequest path: %s\n", request_path);
        printf("Is GET request: %s\n", is_get_request ? "true" : "false");
    }

    // check if route exists
    route_t *route = search_route(root, request_path);

    if (route == NULL)
    {            
        return_routed_page(connfd, route_404, html_404_header);
        
        if(DEBUG_REQUEST_HANDLER)
        {
            printf("404 Route Not Found: %s\n====\n", request_path);
        }
    }
    else
    {
        int status = return_routed_page(connfd, route, html_success_header);
        if(status != 0)
        {
            return_routed_page(connfd, route_404, html_404_header);

            if(DEBUG_REQUEST_HANDLER)
            {
                printf("404 Page Not Found: %s\n====\n", route->value);
            }
        }
        else
        {
            if (DEBUG_REQUEST_HANDLER)
            {
                printf("200 OK: %s\n====\n", route->value);
            }
        }
    }

    close(connfd);
    return;
}

int return_routed_page(int connfd, route_t *route, char *request_header)
{
    char full_path[MAXLINE];
    char buff[MAXLINE];
    
    snprintf(full_path, sizeof(full_path), "%s%s\0", file_path, route->value);

    FILE *page_file = fopen(full_path, "r");
    if(page_file == NULL)
    {
        // file not found
        return 1;
    }
    // send header
    snprintf(buff, sizeof(buff), "%s", request_header);
    send(connfd, buff, strlen(buff), 0);

    // read page_file and send to client
    while (fgets(buff, sizeof(buff), page_file) != NULL)
    {
        send(connfd, buff, strlen(buff), 0);
    }
    // close file
    fclose(page_file);
    return 0;
}