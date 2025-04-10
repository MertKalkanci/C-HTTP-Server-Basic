#ifndef __SERVER_H__
#define __SERVER_H__

#include "include.h"
#include "requesthandler.h"
#include "queue.h"
#include "route.h"

int run_server(void);
int accept_connection(int listenfd);

#endif