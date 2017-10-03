/*-----------------------------------------------------------------------------
* client.c
* Jordan McCaskill
* CMPT 361
*
* This file is the code file for the creation of the client HTTP connections
* which are established when a connection to the server is made
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
#include "client.h"

/*-----------------------------------------------------------------
* Function: createRequest()
* Purpose: Creates the properly formatted get request using the
*          host URL information passed to it
* Parameters: hostInfo - hostData struct which holds all the
*                        information for host, port and path
* Return: request - The properly formatted get request
-----------------------------------------------------------------*/
char *createRequest(struct hostData *hostInfo)
{
	/*get space for the GET request*/
	char *request = malloc(45+strlen(hostInfo->host)+
		strlen(hostInfo->path)+strlen(hostInfo->port));
	memoryCheck(request);

	/*copy in each of the parts of the GET request in the correct order*/
	strcpy(request, "GET ");
	strcpy(&request[strlen(request)], hostInfo->path);
	strcpy(&request[strlen(request)], " HTTP/1.1\r\nHost: ");
	strcpy(&request[strlen(request)], hostInfo->host);
	strcpy(&request[strlen(request)], hostInfo->port);
	strcpy(&request[strlen(request)], "\r\nConnection: close");
	strcpy(&request[strlen(request)], "\r\n\r\n");
	return request;
}

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
int clientInternal(int *clientfd, int *cErr, struct addrinfo *cRes)
{
	/*set up a placeholder struct for use in the for loop*/
	struct addrinfo *cCur;

	for (cCur = cRes; cCur != NULL; cCur = cCur->ai_next) {
		/*create the socket for use in the connection*/
		*clientfd = socket(cCur->ai_family,
				cCur->ai_socktype, cCur->ai_protocol);
		/*check for errors in socket creation*/
		if (*clientfd < 0) {
			perror("socket");
			continue;
		}
		
		/*Create the connection for the HTTP request*/
		*cErr = connect(*clientfd, cCur->ai_addr, cCur->ai_addrlen);
		/*check for errors connecting*/
		if (*cErr == -1) {
			perror("connect");
			close(*clientfd);
			*clientfd = -1;
			continue;
		}

		break;
	}

	/*if there are no more possible connections then abort and terminate*/
	if (cCur == NULL) {
		fprintf(stderr, "Could not connect client\n");
		exit(EXIT_FAILURE);
	}

	/*free addrinfo struct*/
	freeaddrinfo(cRes);

	return 1;

}

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
char *clientReq(struct hostData *hostInfo, char *key)
{
	/*set up for call to getaddrinfo()*/
	struct addrinfo cHints, *cRes;

	memset(&cHints, 0, sizeof(cHints));
	cHints.ai_family =	AF_UNSPEC;
	cHints.ai_socktype = SOCK_STREAM;
	cHints.ai_flags = AI_V4MAPPED;

	/*make the call to getaddrinfo()*/
	/*http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html*/
	int cErr = getaddrinfo(hostInfo->host, "http", &cHints, &cRes);
	/*check for errors*/
	if (cErr != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(cErr));
		exit(EXIT_FAILURE);
	}

	/*make the int for the file descriptor*/
	int clientfd;

	/*create the client connection from addrinfo*/
	clientInternal(&clientfd, &cErr, cRes);

	/*make the GET request*/
	char *getReq = createRequest(hostInfo);

	/*send the GET request*/
	int written = send(clientfd, getReq, strlen(getReq), 0);
	/*check for error sending the GET request*/
	if (written == 0) {
		perror("HTTP connection closed by host");
		exit(EXIT_FAILURE);
	}

	/*read the response from the HTTP connection*/
	char *jsonRet = readFD(clientfd, key);

	/*close the client if the connection was made*/
	if (clientfd > 0)
		close(clientfd);

	/*free the GET request*/
	free(getReq);

	return jsonRet;
}

/*-----------------------------------------------------------------
* Function: testClientConnection()
* Purpose: tests the information passed to determine if the URL
*          entered on the command line is valid
* Parameters: hostInfo - hostData struct which holds the
*                        information for making the HTTP connection
* Return: void
-----------------------------------------------------------------*/
void testClientConnection(struct hostData *hostInfo)
{
	/*set up for call to getaddrinfo()*/
	struct addrinfo cHints, *cRes;

	memset(&cHints, 0, sizeof(cHints));
	cHints.ai_family =	AF_INET6;
	cHints.ai_socktype = SOCK_STREAM;
	cHints.ai_flags = AI_V4MAPPED;

	/*Make call to getaddrinfo()*/
	/*http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html*/
	int cErr = getaddrinfo(hostInfo->host, "http", &cHints, &cRes);
	/*check for errors from getaddrinfo()*/
	if (cErr != 0) {
		fprintf(stderr, "Client does not exist - Please check URL\n");
		freeHostData(hostInfo);
		exit(EXIT_FAILURE);
	}
}
