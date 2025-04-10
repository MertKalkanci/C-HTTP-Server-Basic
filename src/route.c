#include "route.h"

route_t *init_route(char *key, char* value)
{
    route_t *new_route = (route_t *)malloc(sizeof(route_t));
    new_route->key = strdup(key);
    new_route->value = strdup(value);
    new_route->left = NULL;
    new_route->right = NULL;
    return new_route;
}

route_t *insert_route(route_t *root, route_t *insert)
{
    if (root == NULL)
    {
        return insert;
    }
    if (strcmp(insert->key, root->key) < 0)
    {
        root->left = insert_route(root->left, insert);
    }
    else if (strcmp(insert->key, root->key) > 0)
    {
        root->right = insert_route(root->right, insert);
    }
    return root;
}

route_t *search_route(route_t *root, char *key)
{
    if(root == NULL)
    {
        return NULL;
    }
    else if (strcmp(key, root->key) == 0)
    {
        return root;
    }
    else if (strcmp(key, root->key) < 0)
    {
        return search_route(root->left, key);
    }
    else
    {
        return search_route(root->right, key);
    }
}