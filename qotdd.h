#ifndef QOTDD_H
#define QOTDD_H

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

#include "server.h"
#include "client.h"

static int resume;
static int sigInt;

#endif