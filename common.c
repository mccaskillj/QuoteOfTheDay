/*-----------------------------------------------------------------------------
* common.c
* Jordan McCaskill
* CMPT 361
*
* This file holds a couple functions which are used by multiple C files
-----------------------------------------------------------------------------*/
#include "common.h"

/*-----------------------------------------------------------------
* Function: freeHostData()
* Purpose: Free all of the data malloc'd in the hostData struct
* Parameters: hostData - hostData struct which holds the
*                        information for the HTTP connection
* Return: void
-----------------------------------------------------------------*/
void freeHostData(struct hostData *hostData)
{
	/*free all of the char arrays internal*/
	free(hostData->host);
	free(hostData->port);
	free(hostData->path);
	/*free the struct*/
	free(hostData);
}

/*-----------------------------------------------------------------
* Function: memoryCheck()
* Purpose: Check the pointer to see if malloc and calloc were
*          successful
* Parameters: retVal - void pointer to a location in memory
* Return: void
-----------------------------------------------------------------*/
void memoryCheck(void *retVal)
{
	/*check if pointer is null*/
	if (retVal == NULL) {
		/*Return error and exit program*/
		fprintf(stderr, "Error Allocating Memory\n");
		exit(EXIT_FAILURE);
	}
}
