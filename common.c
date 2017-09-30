#include "common.h"

void freeHostData(struct hostData *hostData)
{
	free(hostData->host);
	free(hostData->port);
	free(hostData->path);
	free(hostData);
}

void memoryCheck(void * retVal)
{
	if (retVal == NULL)
	{
		fprintf(stderr, "Error Allocating Memory\n");
		exit(EXIT_FAILURE);
	}
}