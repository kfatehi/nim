#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>  /* i think getaddrinfo needs this */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>			/* for struct addrinfo */
#include <unistd.h>     /* for close() */
#include <errno.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */

int connectSocket(int *sockfd, char *hostname, char *port);
// FIXME refactor the following 2 functions
void writeSocket(const int sock, const char *buffer);
void readSocket(const int sock, char *buffer, const unsigned int buf_size);

int fileExists(const char *filename);
void dieWithError(char *errorMessage);

#endif /* HELPER_H */
