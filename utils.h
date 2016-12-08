#ifndef _UTILS_H
#define _UTILS_H

#include <netinet/in.h>
#include "qtree.h"

#define MAX_NAME 128
#define INPUT_ARG_MAX_NUM 3
#define DELIM " \r\n"
#define BUFFER_SIZE 128
/*
 * Τhe definition of the following structure is copied directly from the
 * muffinman server (http://www.cdf.toronto.edu/~ajr/209/a4/muffinman.c).
 * You might need to add more fields - such as a seoarate buffer for each client and 
 * the current position in the buffer - as explained in A 4 handout.
 */
typedef struct client {
    int fd;
    int *answer;
    // before he enters the name, he cannot issue command
    int state; 	// 0 new, 1 name, 2 waiting, 3-n questions  
    char username[MAX_NAME];
    // each client has its own buffer 
    char buf [BUFFER_SIZE];
    // a pointer to the current end-of-buf position
    int inbuf;
    struct in_addr ipaddr;
    struct client *next;
} Client;

/* 
 * Print a formatted error message to stderr.
 */
void error(char *);

/* 
 * Read and process commands
 */
int process_args(int, char **, QNode **, Node *, struct client *, struct client *);


/*
 * Tokenize the string stored in cmd.
 * Return the number of tokens, and store the tokens in cmd_argv.
 */
int tokenize(char *, char **);

#endif /* _UTILS_H */