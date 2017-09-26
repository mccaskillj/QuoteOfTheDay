#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <libgen.h>

const static int BACKLOG = 10;
int resume;

void handler(int signo)
{
	switch (signo) {
	case SIGINT:
		resume = 0;
		break;
	default: /*Should never get this case*/
		break;
	}
}

char* createRequest(char * host, char * path, char * port){
	char * request = malloc(26+strlen(host)+strlen(path)+strlen(port));
	
	strcpy(request,"GET ");
	strcpy(&request[strlen(request)], path);
	strcpy(&request[strlen(request)], " HTTP/1.1\r\nHost: ");
	strcpy(&request[strlen(request)], host);
	strcpy(&request[strlen(request)], port);
	strcpy(&request[strlen(request)], "\r\n\r\n");
	return request;
}

int clientReq(char * host, char * path, char * port){
	struct addrinfo cHints, *cRes, *cCur;
	memset(&cHints, 0, sizeof(cHints));
	cHints.ai_family =	AF_INET6;
	cHints.ai_socktype = SOCK_STREAM;
	cHints.ai_flags = AI_V4MAPPED;

	/*http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html*/
	int cErr = getaddrinfo(host, "http", &cHints, &cRes);
	if (cErr != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(cErr));
		exit(EXIT_FAILURE);
	}

	int clientfd;			
	for (cCur = cRes; cCur != NULL; cCur = cCur->ai_next)
	{
		clientfd = socket(cCur->ai_family, cCur->ai_socktype, cCur->ai_protocol);
		if (clientfd < 0)
		{
			perror("socket");
			continue;
		}

		cErr = connect(clientfd, cCur->ai_addr, cCur->ai_addrlen);
		if (cErr == -1)
		{
			perror("connect");
			close(clientfd);
			clientfd = -1;
			continue;
		}

		break;
	}

	freeaddrinfo(cRes);

	if (cCur == NULL)
	{
		fprintf(stderr, "Could not connect client\n");
		exit(EXIT_FAILURE);
	}

	char *getReq = createRequest("api.adviceslip.com","/advice","");

	char response[3000];
	int written = send(clientfd, getReq, strlen(getReq), 0);
	printf("%s\n", getReq);
	if(written == 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	printf("wrote\n");
	int recieved = recv(clientfd, response,3000-1,0);
	printf("read\n");
	response[recieved] = '\0';
	printf("%s\n", response);

	if (clientfd > 0){
		close(clientfd);
	}

	free(getReq);

	return 0;
}

int serverConnect(struct addrinfo *sRes, int *serverfdOut, int *sErr)
{
	struct addrinfo *sCur;

	for (sCur = sRes; sCur != NULL; sCur = sCur->ai_next)
	{
		*serverfdOut = socket(sCur->ai_family, sCur->ai_socktype, sCur->ai_protocol);
		if (*serverfdOut < 0)
		{
			perror("socket");
			continue;
		}

		int val = 1;
		*sErr = setsockopt(*serverfdOut, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
		if (*sErr == -1)
		{
			perror("setsockopt");
			close(*serverfdOut);
			continue;
		}

		*sErr = bind(*serverfdOut, sCur->ai_addr, sCur->ai_addrlen);
		if (*sErr == -1)
		{
			perror("bind");
			close(*serverfdOut);
			continue;
		}

		*sErr = listen(*serverfdOut, BACKLOG);
		if (*sErr == -1)
		{
			perror("listen");
			close(*serverfdOut);
			continue;
		}

		break;
	}
	freeaddrinfo(sRes);

	if (sCur == NULL)
	{
		fprintf(stderr, "Could not create server\n");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char *argv[])
{
	if(!argv[2] || argv[3]){
		printf("Usage: %s host[:port]/path key\n", basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	resume = 1;
	struct sigaction pSig;

	pSig.sa_handler = handler;
	sigemptyset(&pSig.sa_mask);
	pSig.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &pSig, NULL);

	struct addrinfo sHints, *sRes;
	memset(&sHints, 0, sizeof(sHints));
	sHints.ai_family = AF_INET6;
	sHints.ai_socktype = SOCK_STREAM;
	sHints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

	int sErr = getaddrinfo(NULL, "1025", &sHints, &sRes);
	if (sErr != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sErr));
		exit(EXIT_FAILURE);
	}

	int serverfdOut;

	serverConnect(sRes,&serverfdOut,&sErr);


	while(0)
	{
		struct sockaddr_storage client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		int cfd = accept(serverfdOut, (struct sockaddr *)&client_addr, &client_addr_len);

		if (resume)
		{
			printf("Accept connection...\n");

			char *msg = "Congrats\n";
			int written = send(cfd, msg, strlen(msg), 0);
			if (written != strlen(msg))
			{
				fprintf(stderr, "write\n");
				exit(EXIT_FAILURE);
			}
		}
		if(&cfd != NULL)
		{
			close(cfd);
		}

		if (resume) 
		{
			printf("Closed connection...\n");
		} 
		else 
		{
			printf("Exiting program\n");
		}
	}

	clientReq("api.adviceslip.com","/advice","");

	return 0;
}
