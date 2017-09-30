#include "server.h"

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