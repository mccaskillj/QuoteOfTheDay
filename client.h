#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "reader.h"


char* createRequest(struct hostData *hostInfo);
int clientInternal(int * clientfd, int * cErr, struct addrinfo *cRes);
char * clientReq(struct hostData *hostInfo, char * key);
void testClientConnection(struct hostData *hostInfo);

#endif