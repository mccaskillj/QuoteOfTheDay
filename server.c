/*-----------------------------------------------------------------------------
* server.c
* Jordan McCaskill
* CMPT 361
*
* This file contains the functions necessary for running the server side of
* the qotdd
-----------------------------------------------------------------------------*/

/*System includes*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*File includes*/
#include "server.h"

int serverConnectInternal(int *serverfdOut, int *sErr, struct addrinfo *sRes)
{
	/*Create a temporary placeholder for use in the for loop*/
	struct addrinfo *sCur;

	/*loop through all of the values
	* from getaddrinfo() to make a connection*/
	for (sCur = sRes; sCur != NULL; sCur = sCur->ai_next) {
		/*create a socket connection*/
		*serverfdOut = socket(sCur->ai_family, sCur->ai_socktype,
					sCur->ai_protocol);
		/*check for error creating a socket*/
		if (*serverfdOut < 0) {
			perror("socket");
			continue;
		}

		/*Set up the socket for the connection*/
		int val = 1;
		*sErr = setsockopt(*serverfdOut, SOL_SOCKET,
				SO_REUSEADDR, &val, sizeof(val));
		/*check for error in socket set-up*/
		if (*sErr == -1) {
			perror("setsockopt");
			close(*serverfdOut);
			continue;
		}

		/*Bind the socket for use*/
		*sErr = bind(*serverfdOut, sCur->ai_addr, sCur->ai_addrlen);
		/*check for error in the socket bind*/
		if (*sErr == -1) {
			perror("bind");
			close(*serverfdOut);
			continue;
		}

		/*listen for connections on the socket*/
		*sErr = listen(*serverfdOut, BACKLOG);
		/*Check for errors listening*/
		if (*sErr == -1) {
			perror("listen");
			close(*serverfdOut);
			continue;
		}

		break;
	}

	/*break out of the program if there are no other possible connections
	* from getaddrinfo()*/
	if (sCur == NULL) {
		fprintf(stderr, "Could not create server\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int serverConnect(int *serverfdOut)
{
	/*set up variables for use in getaddrinfo()*/
	struct addrinfo sHints, *sRes;

	memset(&sHints, 0, sizeof(sHints));
	sHints.ai_family = AF_INET6;
	sHints.ai_socktype = SOCK_STREAM;
	sHints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

	/*run getaddrinfo() to do the DNS lookup*/
	int sErr = getaddrinfo(NULL, "1028", &sHints, &sRes);

	if (sErr != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sErr));
		exit(EXIT_FAILURE);
	}

	/*make the server connection*/
	serverConnectInternal(serverfdOut, &sErr, sRes);

	/*free for addrinfo struct*/
	freeaddrinfo(sRes);


	return 0;
}
