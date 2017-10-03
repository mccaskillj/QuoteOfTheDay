#ifndef CLIENT_H
#define CLIENT_H

/*-----------------------------------------------------------------------------
* client.h
* Jordan McCaskill
* CMPT 361
*
* Header file for client.h holding includes and prototypes
-----------------------------------------------------------------------------*/

/*System includes*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

/*header file includes*/
#include "common.h"
#include "reader.h"

/*-----------------------------------------------------------------
* Function: createRequest()
* Purpose: Creates the properly formatted get request using the
*          host URL information passed to it
* Parameters: hostInfo - hostData struct which holds all the
*                        information for host, port and path
* Return: request - The properly formatted get request
-----------------------------------------------------------------*/
char *createRequest(struct hostData *hostInfo);

/*-----------------------------------------------------------------
* Function: clientInternal()
* Purpose: Holds the loop for the client connection which attempts
*          to connect to the information from getaddrinfo()
* Parameters: clientfd - the client side file descriptor
*             cErr - an integer which holds client codes
*             cRes - the information of the connections gained
*                    from the call to getaddrinfo()
* Return: Integer success code
-----------------------------------------------------------------*/
int clientInternal(int *clientfd, int *cErr, struct addrinfo *cRes);

/*-----------------------------------------------------------------
* Function: clientReq()
* Purpose: Set up the HTTP connection, pass the GET request and
*          read from the socket
* Parameters: hostInfo - hostData struct which holds the
*                        information for making the HTTP connection
*             Key - The word which is being searched for in the
*                   Json returned from the HTTP request
* Return: The Json returned from the GET request
-----------------------------------------------------------------*/
char *clientReq(struct hostData *hostInfo, char *key);

/*-----------------------------------------------------------------
* Function: testClientConnection()
* Purpose: tests the information passed to determine if the URL
*          entered on the command line is valid
* Parameters: hostInfo - hostData struct which holds the
*                        information for making the HTTP connection
* Return: void
-----------------------------------------------------------------*/
void testClientConnection(struct hostData *hostInfo);

/* CLIENT_H */
#endif
