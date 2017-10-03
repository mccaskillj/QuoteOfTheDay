/*-----------------------------------------------------------------------------
* common.c
* Jordan McCaskill
* CMPT 361
*
* This file holds a couple functions which are used by multiple C files
-----------------------------------------------------------------------------*/

/*System includes*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*file includes*/
#include "common.h"

void freeHostData(struct hostData *hostData)
{
	/*free all of the char arrays internal*/
	free(hostData->host);
	free(hostData->port);
	free(hostData->path);
	/*free the struct*/
	free(hostData);
}

void memoryCheck(void *retVal)
{
	/*check if pointer is null*/
	if (retVal == NULL) {
		/*Return error and exit program*/
		fprintf(stderr, "Error Allocating Memory\n");
		exit(EXIT_FAILURE);
	}
}
