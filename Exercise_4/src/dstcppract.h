/**
 ****************************************************************************
 * All rights reserved (C) 2018-2019 by José María Foces Morán and
 * José María Foces Vivancos
 *
 * (In Linux, use the -lpthread gcc compiler flag -not necessary in OS-X)
 *
 * Stream-socket based server for basic TCP protocol analysis
 *
 *
 * Modified by Samuel Castrillo Domínguez
 ****************************************************************************
*/

#ifndef DSTCPPRACT_H
#define DSTCPPRACT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* DSTCPPRACT_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <memory.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include <limits.h>

#define REQ_SHUTDOWN "Shutdown server"
#define REQ_DATE "Send the date"
#define RESP_DATE "Server's date follows: "
#define RESP_SHUTDOWN "After receiving a Shutdown request, only the " \
"client-exclusive worker thread is being finished. " \
"Server thread will remain running.\n"

#define REQ_MULT_2 "Multiply integer by 2"
#define RESP_MULT_2 "Integer multiplied by 2"

#define RESP_OTHER "Unknown request. This server responds to DATE and SHUTDOWN requests, only\n"
#define RESP_ERROR "Socket receive error on delegate Socket"

#define TRUE 1
#define FALSE 0
#define DEFAULT_PORT 60001

// For listen() socket function
#define BACKLOG 100

#define MAX_REQ_SIZE 1024
#define MAX_RESP 1024

#define DEBUG_SOLUTION

#define MAXITERATIVEREQUESTS 5
#define PAUSEBETWEENREQUESTS 5

void *clientServerProtocol(void *ds);
