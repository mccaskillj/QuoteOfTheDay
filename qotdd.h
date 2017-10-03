#ifndef QOTDD_H
#define QOTDD_H

/*-----------------------------------------------------------------------------
* qotdd.h
* Jordan McCaskill
* CMPT 361
*
* Header file for qotdd.h holding includes and prototypes
-----------------------------------------------------------------------------*/

/*static globals needed for signal handling*/
static int resume;
static int sigInt;

/*-----------------------------------------------------------------
* Function: handler()
* Purpose: Signal handler for handling SIGINT
* Parameters: signo - integer value of the signal
* Return: void
-----------------------------------------------------------------*/
void handler(int signo);

/*-----------------------------------------------------------------
* Function: parseHost()
* Purpose: Parse the host URL which is passed through the command
*          line into its host, port and path values
* Parameters: host - string of the full path passed from argv[1]
* Return: hostData struct with the values seperated
-----------------------------------------------------------------*/
struct hostData *parseHost(char *host);

/*-----------------------------------------------------------------
* Function: main()
* Purpose: main program function which starts the initial set-up of
*          the server and handles the outgoing connections and fork
* Parameters: argc - number of arguments
*             argv - array of the arguments
* Return: return value of int for success or failure
-----------------------------------------------------------------*/
int main(int argc, char *argv[]);

/* QOTDD_H */
#endif
