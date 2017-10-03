/*-----------------------------------------------------------------------------
* qotdd.c
* Jordan McCaskill
* CMPT 361
*
* This file holds the main program function and sets up the main connection
* loops and forks the server
-----------------------------------------------------------------------------*/
/*System includes*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <libgen.h>
#include <errno.h>

/*header file includes*/
#include "common.h"
#include "client.h"
#include "server.h"
#include "qotdd.h"

void handler(int signo)
{
	/*switch statement to determine signal*/
	switch (signo) {
	case SIGINT:
		/*set the globals*/
		resume = 0;
		sigInt = 1;
		break;
	case SIGCHLD:
		while (waitpid(-1, NULL, WNOHANG) > 0);
		break;
	default: /*Should never get this case*/
		break;
	}
}

struct hostData *parseHost(char *host)
{
	/*allocate space for the hostdata struct and its internal pieces*/
	struct hostData *hostInfo = malloc(sizeof(struct hostData));
	memoryCheck(hostInfo);
	hostInfo->host = calloc(strlen(host)+1, sizeof(char));
	memoryCheck(hostInfo->host);
	hostInfo->port = calloc(strlen(host)+1, sizeof(char));
	memoryCheck(hostInfo->port);
	hostInfo->path = calloc(strlen(host)+1, sizeof(char));
	memoryCheck(hostInfo->path);

	/*set a front marker to modify*/
	char *front = host;
	/*loop through the array*/
	int slash = 0;
	int i;

	for (i = 0; front[i] != '/' && i < strlen(front); i++);
	/*check for the first occurance of '/'*/
	if (front[i] == '/' && slash == 0) {
		/*copy from the '/' until the end of the string*/
		strcpy(hostInfo->path, &front[i]);
		front[i] = '\0';
		slash++;
	}

	/*search is in seperate loop because '/' needs to be found first*/
	int colon = 0;

	for (i = 0; front[i] != ':' && i < strlen(front); i++);
	/*check for the first occurance of ':'*/
	if (front[i] == ':' && colon == 0) {
		/*copy from the ':' until the new end*/
		strcpy(hostInfo->port, &front[i]);
		front[i] = '\0';
		colon++;
	}

	/*copy the host data*/
	strcpy(hostInfo->host, front);

	/*if path is an empty string then replace with '/'*/
	if (hostInfo->path[0] == '\0')
		strcpy(hostInfo->path, "/");

	return hostInfo;
}

int main(int argc, char *argv[])
{
	/*check for correct number of arguments*/
	if (!argv[2] || argv[3]) {
		/*print the usage message*/
		printf("Usage: %s host[:port]/path key\n", basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	/*set up structures for signal handling*/
	struct sigaction pSig;
	pSig.sa_handler = handler;
	sigemptyset(&pSig.sa_mask);
	pSig.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &pSig, NULL);
	sigaction(SIGCHLD, &pSig, NULL);

	/*set up struct to hold host data*/
	struct hostData *hostInfo = parseHost(argv[1]);

	/*test the connection to the client to make sure URL is good*/
	testClientConnection(hostInfo);

	/*set the globals*/
	resume = 1;
	sigInt = 0;

	/*make file descriptor int*/
	int serverfdOut;

	/*make connection*/
	serverConnect(&serverfdOut);

	/*while loop for getting more than one connection*/
	while (resume) {
		/*set up the struct for holding info from getaddrinfo()*/
		struct sockaddr_storage client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		/*wait for connections*/
		int cfd = accept(serverfdOut, (struct sockaddr *)&client_addr,
				&client_addr_len);

		if (cfd < 0 && errno != EINTR) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		/*fork the server*/
		if (cfd >= 0 && fork() == 0) {
			/*set the resume to 0 so children terminate*/
			resume = 0;
			/*set up the connection and get the return from HTTP*/
			char *msg = clientReq(hostInfo, argv[2]);

			/*send the message back to the client*/
			int written = send(cfd, msg, strlen(msg), 0);

			/*check for errors in writting*/
			if (written != strlen(msg)) {
				if (!sigInt)
					fprintf(stderr, "write\n");
				if (strcmp(msg, ""))
					free(msg);

				/*free the host data struct*/
				freeHostData(hostInfo);
				exit(EXIT_FAILURE);
			}
			/*free if there was an error getting message*/
			if (strcmp(msg, ""))
				free(msg);

		}
		/*clkoe the file descriptor*/
		close(cfd);

	}
	/*free host data*/
	freeHostData(hostInfo);

	return 0;
}
