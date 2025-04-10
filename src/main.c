#include "server.h"

route_t *root, *route_404;
void route_setup(void);

int main(void)
{
    // Initialize the route tree
    route_setup();

    run_server();
}

void route_setup(void)
{
    // Initialize the route tree
    root = NULL;

    route_t *route1 = init_route("/index.html", "/index.html");
    route_t *route2 = init_route("/", "/index.html");
    route_t *route3 = init_route("/404.html", "/404.html");
    route_t *route4 = init_route("/404", "/404.html");
    route_t *route5 = init_route("/test_internal_routing_error", "/non_existent_page.html");

    root = insert_route(root, route1);
    root = insert_route(root, route2);
    root = insert_route(root, route3);
    root = insert_route(root, route4);
    root = insert_route(root, route5);

    route_404 = init_route("/404", "/404.html");
}