#include "qotdd.h"

void handler(int signo)
{
	switch (signo) {
	case SIGINT:
		resume = 0;
		sigInt = 1;
		break;
	default: /*Should never get this case*/
		break;
	}
}

struct hostData * parseHost(char *host)
{
	struct hostData *hostInfo = malloc(sizeof(struct hostData));
	memoryCheck(hostInfo);
	hostInfo->host = calloc(strlen(host)+1,sizeof(char));
	memoryCheck(hostInfo->host);
	hostInfo->port = calloc(strlen(host)+1,sizeof(char));
	memoryCheck(hostInfo->port);
	hostInfo->path = calloc(strlen(host)+1,sizeof(char));
	memoryCheck(hostInfo->path);

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

	/*if (hostInfo->path[0] == '\0')
	{
		strcpy(hostInfo->path,"/");
	}*/

	return hostInfo;
}

int main(int argc, char *argv[])
{
	if(!argv[2] || argv[3]){
		printf("Usage: %s host[:port]/path key\n", basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	struct sigaction pSig;
	pSig.sa_handler = handler;
	sigemptyset(&pSig.sa_mask);
	pSig.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &pSig, NULL);

	struct hostData *hostInfo = parseHost(argv[1]);

	testClientConnection(hostInfo);

	resume = 1;
	sigInt = 0;

	int serverfdOut;

	serverConnect(&serverfdOut);


	while(resume)
	{
		struct sockaddr_storage client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		while (waitpid(-1, NULL, WNOHANG) > 0);

		int cfd = accept(serverfdOut, (struct sockaddr *)&client_addr, &client_addr_len);

		if (fork() == 0)
		{
			resume = 0;
			char *msg = clientReq(hostInfo,argv[2]);
			int written = send(cfd, msg, strlen(msg), 0);
			if (written != strlen(msg))
			{
				if (!sigInt) fprintf(stderr, "write\n");
				if (strcmp(msg,""))
				{
					free(msg);
				}
				freeHostData(hostInfo);
				exit(EXIT_FAILURE);
			}
			if (strcmp(msg,""))
			{
				free(msg);
			}
		}
		close(cfd);

	}
	freeHostData(hostInfo);

	return 0;
}
