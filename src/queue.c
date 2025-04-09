#include "queue.h"

node_t *head = NULL;
node_t *tail = NULL;

void enqueue(int *connfd)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->connfd = connfd;
    new_node->next = NULL;

    if (head == NULL)
    {
        head = new_node;
        tail = new_node;
    }
    else
    {
        tail->next = new_node;
        tail = new_node;
    }
}

int *dequeue()
{
    if (head == NULL)
    {
        return NULL;
    }

    node_t *temp = head;
    int *connfd = temp->connfd;

    head = head->next;
    if (head == NULL)
    {
        tail = NULL;
    }

    free(temp);
    return connfd;
}