#ifndef COMMON_H
#define COMMON_H

/*-----------------------------------------------------------------------------
# common.h
# Jordan McCaskill
# CMPT 361
# 
# Header file for common.h holding includes, structs and prototypes
-----------------------------------------------------------------------------*/

/*System includes*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*-----------------------------------------------------------------
# Struct which is used to hold the host, path and port associated
# with the HTTP connection
-----------------------------------------------------------------*/
struct hostData
{
	char *host;
	char *port;
	char *path;
};

/*-----------------------------------------------------------------
# Function: freeHostData()
# Purpose: Free all of the data malloc'd in the hostData struct
# Parameters: hostData - hostData struct which holds the
#                        information for the HTTP connection
# Return: void
-----------------------------------------------------------------*/
void freeHostData(struct hostData *hostData);

/*-----------------------------------------------------------------
# Function: memoryCheck()
# Purpose: Check the pointer to see if malloc and calloc were
#          successful
# Parameters: retVal - void pointer to a location in memory
# Return: void
-----------------------------------------------------------------*/
void memoryCheck(void * retVal);

/* COMMON_H */
#endif