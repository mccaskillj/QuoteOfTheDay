#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct hostData
{
	char *host;
	char *port;
	char *path;
};

void freeHostData(struct hostData *hostData);
void memoryCheck(void * retVal);

#endif