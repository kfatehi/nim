#ifndef SOCK_H
#define SOCK_H

#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>			/* for struct addrinfo */
#include <unistd.h>     /* for close() */
#include <errno.h>

int connectSocket(int *sockfd, char *hostname, char *port);

#endif /* SOCK_H */
