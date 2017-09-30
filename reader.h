#ifndef READER_H
#define READER_H

/*-----------------------------------------------------------------------------
# reader.h
# Jordan McCaskill
# CMPT 361
# 
# Header file for reader.h holding includes and prototypes
-----------------------------------------------------------------------------*/

/*System includes*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

/*header file includes*/
#include "common.h"
#include "jsmn/jsmn.h"

/*-----------------------------------------------------------------
# Function: jsoneq()
# Purpose: This function is taken from the jsmn usage examples for
#          parsing the files and remains unchanged from the one in
#          the example
# Parameters: json - json string
#             tok - list of the locations of the json tokens
#             s - the key which is being searched for
# Return: return values for success or failure
# Reference: https://github.com/zserge/jsmn/blob/
#                    master/example/simple.c
-----------------------------------------------------------------*/
int jsoneq(const char *json, jsmntok_t *tok, const char *s);

/*-----------------------------------------------------------------
# Function: jsonParse()
# Purpose: parse the json string to remove the desired value. Parts
#          of the function and adapted from the jsmn library 
#          examples
# Parameters: json - the jason string to parse
#             key - the key to be found in the json string
# Return: the value associated with the key if found or an empty
#         string if not
# Reference: https://github.com/zserge/jsmn/blob/
#                    master/example/simple.c
-----------------------------------------------------------------*/
char * jsonParse(char * json, char * key);

/*-----------------------------------------------------------------
# Function: readFD()
# Purpose: read the data contained in the connection file 
#          descriptor and parses it
# Parameters: fd - a file descriptor for the connection
#             key - the key being searched for in the json
# Return: the value from the json associated with the key passed
-----------------------------------------------------------------*/
char * readFD(int fd, char *key);

/* READER_H */
#endif