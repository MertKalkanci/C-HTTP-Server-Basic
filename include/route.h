#include "include.h"
#ifndef __ROUTE_H__
#define __ROUTE_H__

typedef struct route
{
    char *key;
    char *value;
    struct route *left, *right;
} route_t;

extern route_t *root, *route_404;
route_t *init_route(char *key, char* value);
route_t *insert_route(route_t *root, route_t *insert);
route_t *search_route(route_t *root, char *key);

#endif