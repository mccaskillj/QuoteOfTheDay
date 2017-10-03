/*-----------------------------------------------------------------------------
* reader.c
* Jordan McCaskill
* CMPT 361
*
* File containing all of the functions related with reading from the HTTP
* connection
-----------------------------------------------------------------------------*/
#include "reader.h"

/*-----------------------------------------------------------------
* Function: jsoneq()
* Purpose: This function is taken from the jsmn usage examples for
*          parsing the files and remains unchanged from the one in
*          the example
* Parameters: json - json string
*             tok - list of the locations of the json tokens
*             s - the key which is being searched for
* Return: return values for success or failure
* Reference: https://github.com/zserge/jsmn/blob/
*                    master/example/simple.c
-----------------------------------------------------------------*/
int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
	/*Check to see if the token found from tok matches the key*/
	if (tok->type == JSMN_STRING &&
		(int) strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

/*-----------------------------------------------------------------
* Function: jsonParse()
* Purpose: parse the json string to remove the desired value. Parts
*          of the function and adapted from the jsmn library 
*          examples
* Parameters: json - the jason string to parse
*             key - the key to be found in the json string
* Return: the value associated with the key if found or an empty
*         string if not
* Reference: https://github.com/zserge/jsmn/blob/
*                    master/example/simple.c
-----------------------------------------------------------------*/
char *jsonParse(char *json, char *key)
{
	/*set up variables for the tokenizer*/
	int i, r;
	jsmn_parser parser;

	r = jsmn_parse(&parser, json, strlen(json),
			NULL, 0);
	if (r < 0) {
		perror("Could not parse Json");
		return "";
	}
	jsmntok_t tok[r];

	/*initialize the parser*/
	jsmn_init(&parser);

	/*run the parser on the json string*/
	r = jsmn_parse(&parser, json, strlen(json),
			tok, sizeof(tok)/sizeof(tok[0]));
	/*check for errors in parsing the json*/
	if (r < 0) {
		perror("Could not parse Json");
		return "";
	}

	/*check to make sure that the json parsed into the appropriate form*/
	if (r < 1 || tok[0].type != JSMN_OBJECT) {
		perror("Object expected");
		return "";
	}

	/*look for the json key and retrieve the value*/
	char * start;

	for (i = 1; i < r; i++) {
		/*check if the tok struct at pos i contains the key*/
		if (jsoneq(json, &tok[i], key) == 0) {
			/*put a null terminator at the end point*/
			json[tok[i+1].end] = '\0';
			/*set the start point for the value*/
			start = &json[tok[i+1].start];
			/*set up for the storage of the value*/
			char *jsonOut = malloc(sizeof(char) * strlen(start)+2);

			memoryCheck(jsonOut);
			/*Copy the value into the array and set
			* the end to a new line for printing*/
			strcpy(jsonOut, start);
			jsonOut[strlen(jsonOut)+1] = '\0';
			jsonOut[strlen(jsonOut)] = '\n';
			return jsonOut;
		}
	}

	return "";

}

/*-----------------------------------------------------------------
* Function: readFD()
* Purpose: read the data contained in the connection file
*          descriptor and parses it
* Parameters: fd - a file descriptor for the connection
*             key - the key being searched for in the json
* Return: the value from the json associated with the key passed
-----------------------------------------------------------------*/
char *readFD(int fd, char *key)
{
	/*set up temporary arrays*/
	char temp[10000];
	char message[10000] = "";
	char *retVal;
	int recieved;

	/*get data from the socket*/
	while ((recieved = recv(fd, temp, 9999, 0)) != 0) {
		/*add the new data to the existing data*/
		temp[recieved] = '\0';
		strcat(message, temp);
	}

	/*check for a 200 value return*/
	char *i;

	if (message[9] != '2') {
		/*assign space to hold the error string*/
		retVal = calloc(25, sizeof(char));
		memoryCheck(retVal);
		/*Copy in the message and the error code*/
		strcpy(retVal, "cannot obtain quote: ");
		strncpy(&retVal[21], &message[9], 3);
		return retVal;
	}

	/*look for the double carriage return/newline*/
	for (i = message; i[3] != '\0' && (i[0] != '\r' ||
		i[1] != '\n' || i[2] != '\r' || i[3] != '\n'); i++);

	/*check to see if responce was not correct*/
	if (i[3] == '\0')
		return "";

	/*parse the json*/
	retVal = jsonParse(i+4, key);

	return retVal;

}
