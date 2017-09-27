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

#include "jsmn/jsmn.h"

const static int BACKLOG = 10;
int resume;

struct hostData
{
	char *host;
	char *port;
	char *path;
};

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

struct hostData * parseHost(char *host)
{
	struct hostData *hostInfo = malloc(sizeof(struct hostData));
	hostInfo->host = calloc(strlen(host)+1,sizeof(char));
	hostInfo->port = calloc(strlen(host)+1,sizeof(char));
	hostInfo->path = calloc(strlen(host)+1,sizeof(char));

	char * front = host;
	int i;
	for (i = 0; front[i] != '/' && i < strlen(front); i++);
		if (front[i] == '/')
		{
			strcpy(hostInfo->path,&front[i]);
			front[i] = '\0';
		}

	for (i = 0; front[i] != ':' && i < strlen(front); i++);
		if (front[i] == ':')
		{
			strcpy(hostInfo->port,&front[i]);
			front[i] = '\0';
		}

	strcpy(hostInfo->host,front);

	return hostInfo;
}

char* createRequest(struct hostData *hostInfo){
	char * request = malloc(26+strlen(hostInfo->host)+strlen(hostInfo->path)+strlen(hostInfo->port));
	
	strcpy(request,"GET ");
	strcpy(&request[strlen(request)], hostInfo->path);
	strcpy(&request[strlen(request)], " HTTP/1.1\r\nHost: ");
	strcpy(&request[strlen(request)], hostInfo->host);
	strcpy(&request[strlen(request)], hostInfo->port);
	strcpy(&request[strlen(request)], "\r\n\r\n");
	return request;
}

int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0)
	{
		return 0;
	}
	return -1;
}

char * jsonParse(char * json, char * key)
{
	/*portions of this code are adapted from the Jsmn library examples*/
	int i, r;
	jsmn_parser parser;
	jsmntok_t tok[128];

	jsmn_init(&parser);
	r = jsmn_parse(&parser, json, strlen(json), tok, sizeof(tok)/sizeof(tok[0]));
	if (r<0)
	{
		perror("Could not parse Json");
		return "";
	}

	if (r < 1 || tok[0].type != JSMN_OBJECT)
	{
		perror("Object expected");
		return "";
	}

	char * start;
	for (i = 1; i < r; i++)
	{
		if (jsoneq(json,&tok[i],key)==0)
		{
			json[tok[i+1].end] = '\0';
			start = &json[tok[i+1].start];
			char * jsonOut = malloc(sizeof(char) * strlen(start)+2);
			strcpy(jsonOut, start);
			jsonOut[strlen(jsonOut)+1] = '\0';
			jsonOut[strlen(jsonOut)] = '\n';
			return jsonOut;
		}
	}

	return "";

}

char * readFD(int fd, char *key)
{
	char temp[1024];
	char message[1024] = "";
	char * retVal;

	int recieved;

	while ((recieved = recv(fd, temp, 1023, 0))!=0)
	{
		temp[recieved] = '\0';
		strcat(message,temp);
	}

	char * i;
	for (i = message;i[3] != '\0' && (i[0] != '\r' ||
		i[1] != '\n' || i[2] != '\r' || i[3] != '\n'); i++);

	if (i[3] == '\0') return "";

	retVal = jsonParse(i+4, key);

	return retVal;
	
}

char * clientReq(struct hostData *hostInfo, char * key){
	struct addrinfo cHints, *cRes, *cCur;
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

int serverConnectInternal(int *serverfdOut, int *sErr, struct addrinfo *sRes)
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

	if (sCur == NULL)
	{
		fprintf(stderr, "Could not create server\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int serverConnect(int *serverfdOut)
{
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

	serverConnectInternal(serverfdOut,&sErr,sRes);
	
	freeaddrinfo(sRes);


	return 0;
}


int main(int argc, char *argv[])
{
	if(!argv[2] || argv[3]){
		printf("Usage: %s host[:port]/path key\n", basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	struct hostData *hostInfo = parseHost(argv[1]);

	struct sigaction pSig;
	pSig.sa_handler = handler;
	sigemptyset(&pSig.sa_mask);
	pSig.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &pSig, NULL);

	resume = 1;

	int serverfdOut;

	serverConnect(&serverfdOut);


	while(resume)
	{
		struct sockaddr_storage client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		int cfd = accept(serverfdOut, (struct sockaddr *)&client_addr, &client_addr_len);

		if (resume)
		{
			char *msg = clientReq(hostInfo,argv[2]);
			int written = send(cfd, msg, strlen(msg), 0);
			if (written != strlen(msg))
			{
				fprintf(stderr, "write\n");
				exit(EXIT_FAILURE);
			}
		}
		if(cfd <= 0)
		{
			close(cfd);
		}

		if (resume) 
		{
			close(cfd);
		} 
		else 
		{
			printf("Exiting program\n");
		}
	}

	return 0;
}
