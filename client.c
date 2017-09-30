#include "client.h"

char* createRequest(struct hostData *hostInfo){
	char * request = malloc(26+strlen(hostInfo->host)+strlen(hostInfo->path)+strlen(hostInfo->port));
	memoryCheck(request);
	
	strcpy(request,"GET ");
	strcpy(&request[strlen(request)], hostInfo->path);
	strcpy(&request[strlen(request)], " HTTP/1.1\r\nHost: ");
	strcpy(&request[strlen(request)], hostInfo->host);
	strcpy(&request[strlen(request)], hostInfo->port);
	strcpy(&request[strlen(request)], "\r\n\r\n");
	return request;
}

int clientInternal(int * clientfd, int * cErr, struct addrinfo *cRes)
{
	struct addrinfo *cCur;
	for (cCur = cRes; cCur != NULL; cCur = cCur->ai_next)
	{
		*clientfd = socket(cCur->ai_family, cCur->ai_socktype, cCur->ai_protocol);
		if (*clientfd < 0)
		{
			perror("socket");
			continue;
		}

		*cErr = connect(*clientfd, cCur->ai_addr, cCur->ai_addrlen);
		if (*cErr == -1)
		{
			perror("connect");
			close(*clientfd);
			*clientfd = -1;
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

	return 1;
	
}

char * clientReq(struct hostData *hostInfo, char * key){
	struct addrinfo cHints, *cRes;
	memset(&cHints, 0, sizeof(cHints));
	cHints.ai_family =	AF_INET6;
	cHints.ai_socktype = SOCK_STREAM;
	cHints.ai_flags = AI_V4MAPPED;

	/*http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html*/
	int cErr = getaddrinfo(hostInfo->host, "http", &cHints, &cRes);
	if (cErr != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(cErr));
		exit(EXIT_FAILURE);
	}

	int clientfd;

	clientInternal(&clientfd, &cErr,cRes);

	char *getReq = createRequest(hostInfo);

	int written = send(clientfd, getReq, strlen(getReq), 0);

	if(written == 0)
	{
		perror("HTTP connection closed by host");
		exit(EXIT_FAILURE);
	}

	char * jsonRet = readFD(clientfd, key);

	if (clientfd > 0){
		close(clientfd);
	}

	free(getReq);

	return jsonRet;
}


void testClientConnection(struct hostData *hostInfo)
{
	struct addrinfo cHints, *cRes;
	memset(&cHints, 0, sizeof(cHints));
	cHints.ai_family =	AF_INET6;
	cHints.ai_socktype = SOCK_STREAM;
	cHints.ai_flags = AI_V4MAPPED;

	/*http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html*/
	int cErr = getaddrinfo(hostInfo->host, "http", &cHints, &cRes);
	if (cErr != 0)
	{
		fprintf(stderr, "Client does not exist - Please check URL\n");
		freeHostData(hostInfo);
		exit(EXIT_FAILURE);
	}
}