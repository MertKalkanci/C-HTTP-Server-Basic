#include "include.h"
#include "requesthandler.h"
#include "queue.h"

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex;
pthread_cond_t sleep_condition;

int listenfd, connfd;
volatile bool close_program = false;

void handle_interrupt(int sig);
void *thread_function(void *arg);
int accept_connection(int listenfd);

int main(void)
{
    //print settings
    printf("DEBUG_REQUEST_IP_ADDRESS: %s\n", DEBUG_REQUEST_IP_ADDRESS ? "true" : "false");
    printf("DEBUG_INCOMING_REQUEST_VERBOSELY: %s\n", DEBUG_INCOMING_REQUEST_VERBOSELY ? "true" : "false");
    printf("DEBUG_REQUEST_HANDLER: %s\n", DEBUG_REQUEST_HANDLER ? "true" : "false");
    printf("SINGLE_THREAD: %s\n", SINGLE_THREAD ? "true" : "false");
    if (!SINGLE_THREAD)
    {
        printf("THREAD_POOL_SIZE: %d\n", THREAD_POOL_SIZE);
    }

    //interrupt handling
    struct sigaction interrupt_handler;
    interrupt_handler.sa_handler = handle_interrupt;

    sigaction(SIGINT, &interrupt_handler, NULL);
    sigaction(SIGTSTP, &interrupt_handler, NULL);
    
    //initialize thread pool
    if(!SINGLE_THREAD)
    {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&sleep_condition, NULL);
        for (int i = 0; i < THREAD_POOL_SIZE; i++)
        {
            pthread_create(&thread_pool[i], NULL, thread_function, NULL);
        }
    }

    //initialize socket
    struct sockaddr_in server_addr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket creation error");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) 
    {
        perror("setsockopt error");
        exit(1);
    }

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

    if (SINGLE_THREAD)
    {
        while (!close_program)
        {
            connfd = accept_connection(listenfd);
                    
            int *p_connfd = malloc(sizeof(int));
            *p_connfd = connfd;
            
            handle_request(p_connfd);
        }
    }
    else
    {
        while (!close_program)
        {
            connfd = accept_connection(listenfd);
                    
            int *p_connfd = malloc(sizeof(int));
            *p_connfd = connfd;
            
            pthread_mutex_lock(&mutex);
            enqueue(p_connfd);
            pthread_cond_signal(&sleep_condition);
            pthread_mutex_unlock(&mutex); 
        }
    }

    exit(0);
}

int accept_connection(int listenfd)
{
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    char client_addr_str[MAXLINE];

    connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (connfd < 0)
    {
        perror("accept error");
        return -1;
    }

    inet_ntop(AF_INET, &client_addr, client_addr_str, MAXLINE);
    if(DEBUG_REQUEST_IP_ADDRESS)
    {
        printf("Client connected: %s\n", client_addr_str);
    }

    return connfd;
}

void *thread_function(void *arg)
{
    printf("Thread %ld started...\n", pthread_self());
    while (!close_program)
    {
        pthread_mutex_lock(&mutex);
        int *p_connfd = dequeue();
        if (p_connfd == NULL)
        {
            pthread_cond_wait(&sleep_condition, &mutex);

            p_connfd = dequeue();
        }
        pthread_mutex_unlock(&mutex);

        if (p_connfd != NULL)
        {
            handle_request(p_connfd);
        }
    }

    printf("Thread %ld exiting...\n", pthread_self());
    pthread_exit(NULL);
}

void handle_interrupt(int sig)
{
    write(STDOUT_FILENO,"\nReceived an interrupt signal, trying to close program properly...\n", 68);
    close_program = true;

    if(!SINGLE_THREAD)
    {
        for (int i = 0; i < THREAD_POOL_SIZE; i++)
        {
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&sleep_condition);
        }
    
        for (int i = 0; i < THREAD_POOL_SIZE; i++)
        {
            pthread_join(thread_pool[i], NULL);
        }
        write(STDOUT_FILENO,"All threads have exited, closing sockets...\n", 45);
    }
    
    close(listenfd);
    close(connfd);

    write(STDOUT_FILENO,"Sockets closed, exiting program...\n", 36);
    exit(0);
}