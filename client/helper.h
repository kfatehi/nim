#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>      /* for printf(), fprintf(), STDIN */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <termios.h>    /* for disabling line buffer on stdin */
#include <sys/poll.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>  /* addrinfo stuff and select() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>			/* for struct addrinfo */
#include <unistd.h>     /* for close() & STDIN_FILENO */
#include <errno.h>    
#include <ncurses.h>

#define NB_ENABLE 0
#define NB_DISABLE 1

#define NIMBUS_ID_LEN 7
#define BIGBUF 256
#define SMALLBUF 32

extern int sockfd;
extern char id[NIMBUS_ID_LEN];

void startupHandler(int argc, char *argv[]);
void configTerminal(int state); // disable line buffering, echo, etc

// Network
void connectSocket(int *sockfd, char *hostname, char *port);
int writeSocket(const int sock, const char *buffer);
int readSocket(const int sock, char *buffer, const unsigned int buf_size);

// Misc
int fileExists(const char *filename);
void dieWithError(char *errorMessage);

#endif /* HELPER_H */
