#include "include.h"
#include "requesthandler.h"
 #include "queue.h"

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex;

void *thread_function(void *arg);

int main(void)
{
    int listenfd, connfd;
    struct sockaddr_in server_addr;

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }

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
        char client_addr_str[MAXLINE];

        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);

        inet_ntop(AF_INET, &client_addr, client_addr_str, MAXLINE);
        printf("Client connected: %s\n", client_addr_str);
                
        int *p_connfd = malloc(sizeof(int));
        *p_connfd = connfd;
        
        pthread_mutex_lock(&mutex);
        enqueue(p_connfd);
        pthread_mutex_unlock(&mutex);
        /*
        pthread_t tid;

        if (pthread_create(&tid, NULL, handle_request, p_connfd) != 0)
        {
            perror("pthread_create error");
            close(connfd);
            free(p_connfd);
            continue;
        }
        */
    }
    exit(0);
}


void *thread_function(void *arg)
{
    while (true)
    {
        //sem_wait(&sem);
        pthread_mutex_lock(&mutex);
        int *p_connfd = dequeue();
        pthread_mutex_unlock(&mutex);
        if (p_connfd != NULL)
        {
            handle_request(p_connfd);
        }
    }
}