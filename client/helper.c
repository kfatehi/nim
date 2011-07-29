#include "helper.h"

  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Start network helpers
  \* * * * * * * * * * * * * * * * * * * * * * */ 

int connectSocket(int *sockfd, char *hostname, char *port) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints); // make sure struct is empty
	hints.ai_family = AF_UNSPEC; // be ipv4/ipv6 agnostic
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	if (getaddrinfo(hostname, port, &hints, &res) != 0)
		dieWithError("Failed to resolve host");
	// FIXME walk the "res" linked list for a valid entry, first one may be invalid
	if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		dieWithError("Failed to create socket descriptor");
	if (connect(*sockfd, res->ai_addr, res->ai_addrlen) != 0)
	  dieWithError("Failed to connect");
	freeaddrinfo(res);
  return 0;
}
// FIXME refactor the following 2 functions
void writeSocket(const int sock, const char *buffer) {
  unsigned int len = strlen(buffer);        /* Determine input length */
  if (send(sock, buffer, len, 0) != len)    /* Send the string to the server */
    fprintf(stderr, "send() sent a different number of bytes than expected");
}
void readSocket(const int sock, char *buffer, const unsigned int buf_size) {
  int bytesRcvd;
  int totalBytesRcvd = 0;
  if ((bytesRcvd = recv(sock, buffer, buf_size - 1, 0)) <= 0)
    fprintf(stderr, "recv() failed or connection closed prematurely");
  buffer[bytesRcvd] = '\0';  /* Terminate the string! */
  fprintf(stdout, "Bytes received: %d\n", bytesRcvd);
  fprintf(stdout, "Received: %s\n", buffer);
}

  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * End network helpers
  \* * * * * * * * * * * * * * * * * * * * * * */ 

int fileExists(const char *filename) {
	FILE *file;
	if (file = fopen(filename, "r"))  {
		fclose(file);
		return -1;
	}
	return 0;
}

void dieWithError(char *errorMessage) {
  /* Error handling function */
	fprintf(stderr, "%s\n", errorMessage);
  fprintf(stderr, "Error code? %d\n", errno);
	exit(1);
}