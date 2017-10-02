#ifndef SERVER_H
#define SERVER_H

/*-----------------------------------------------------------------------------
* reader.h
* Jordan McCaskill
* CMPT 361
*
* Header file for reader.h holding includes and prototypes
-----------------------------------------------------------------------------*/

/*System includes*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*static value for setting the connection backlog*/
const static int BACKLOG = 10;

/*-----------------------------------------------------------------
* Function: serverConnectInternal()
* Purpose: The internal connection loop for attmepting connections
*          on the info made by getaddrinfo()
* Parameters: serverfdOut - the servers outgoing connection file
*                           descriptor
*             sErr - integer for holding error codes
*             sRes - addrinfo struct for holding the information
*                    passed by getaddrinfo()
* Return: integer success code
-----------------------------------------------------------------*/
int serverConnectInternal(int *serverfdOut, int *sErr, struct addrinfo *sRes);

/*-----------------------------------------------------------------
* Function: serverConnect()
* Purpose: Set up the server side connection
* Parameters: serverfdOut - the servers outgoing file descriptor
* Return: Integer success code
-----------------------------------------------------------------*/
int serverConnect(int *serverfdOut);

/* SERVER _ H*/
#endif
