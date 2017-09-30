#include "reader.h"

/*This function is taken from the jsmn usage examples*/
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
			memoryCheck(jsonOut);
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
	if (message[9] != '2')
	{
		retVal = calloc(25,sizeof(char));
		memoryCheck(retVal);
		strcpy(retVal, "cannot obtain quote: ");
		strncpy(&retVal[21], &message[9], 3);
		return retVal;
	}

	for (i = message;i[3] != '\0' && (i[0] != '\r' ||
		i[1] != '\n' || i[2] != '\r' || i[3] != '\n'); i++);

	if (i[3] == '\0') return "";

	retVal = jsonParse(i+4, key);

	return retVal;
	
}