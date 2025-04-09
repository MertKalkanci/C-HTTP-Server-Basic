#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "include.h"
#include "requesthandler.h"

typedef struct node
{
    struct node *next;
    int *connfd;
} node_t;

void enqueue(int *connfd);
int *dequeue();

#endif