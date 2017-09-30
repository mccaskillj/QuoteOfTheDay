#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

const static int BACKLOG = 10;

int serverConnectInternal(int *serverfdOut, int *sErr, struct addrinfo *sRes);
int serverConnect(int *serverfdOut);

#endif