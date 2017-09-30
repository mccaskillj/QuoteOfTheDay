#ifndef READER_H
#define READER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"
#include "jsmn/jsmn.h"

int jsoneq(const char *json, jsmntok_t *tok, const char *s);
char * jsonParse(char * json, char * key);
char * readFD(int fd, char *key);

#endif