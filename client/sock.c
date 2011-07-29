#include "sock.h"

int connectSocket(int *sockfd, char *hostname, char *port) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints); // make sure struct is empty
	hints.ai_family = AF_UNSPEC; // be ipv4/ipv6 agnostic
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	if (getaddrinfo(hostname, port, &hints, &res) != 0)
		DieWithError("Failed to resolve host");
	// FIXME walk the "res" linked list for a valid entry, first one may be invalid
	if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		DieWithError("Failed to initialize socket file descriptor");
	if (connect(*sockfd, res->ai_addr, res->ai_addrlen) != 0)
		DieWithError("Failed to connect to server");
	freeaddrinfo(res);
  return 0;
}