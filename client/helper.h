#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/poll.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>  /* addrinfo stuff and select() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>			/* for struct addrinfo */
#include <unistd.h>     /* for close() */
#include <errno.h>    

#define STDIN 0
#define NIMBUS_ID_LEN 7
#define RCVBUFSIZE 64

extern int sockfd;
extern char id[NIMBUS_ID_LEN];

// Network
void connectSocket(int *sockfd, char *hostname, char *port);
int writeSocket(const int sock, const char *buffer);
int readSocket(const int sock, char *buffer, const unsigned int buf_size);

// Misc
int fileExists(const char *filename);
void dieWithError(char *errorMessage);

#endif /* HELPER_H */
