#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <semaphore.h>

#define SERVER_PORT 8080
#define MAXLINE 1024
#define THREAD_POOL_SIZE 10

#endif