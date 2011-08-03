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
#include "constants.h"
#include "externs.h"

void startgui(void);
void printTopCenter(char *msg);
void printTopLeft(char *msg);
void printTopRight(char *msg);
void printBottomLeft(char *msg);
void backSpaceBuffer(char *buffer, int last_line_no);
void clearLine(int line_no);
void configTerminal(int state); // disable line buffering, echo, etc
void backSpaceBuffer(char *buffer, int last_line_no);
void clearLine(int line_no);

// Network
void connectSocket(int *sockfd, char *hostname, char *port);
int writeSocket(const int sock, const char *buffer);
int readSocket(const int sock, char *buffer, const unsigned int buf_size);

// Misc
void setUfds(struct pollfd *pUfds, int fd1, int fd2);
int fileExists(const char *filename);
void dieWithError(char *errorMessage);

#endif /* HELPER_H */
