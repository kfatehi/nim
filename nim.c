/*
 * Nim client + ncurses, written in C
 */
// TCP STUFF

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <ncurses.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage) {
  /* Error handling function */  
}

int main(int argc, char *argv[])
{
  int sock;                        /* Socket descriptor */
  struct sockaddr_in echoServAddr; /* Echo server address */
  unsigned short echoServPort;     /* Echo server port */
  char servIP[16] = "127.0.0.1";   /* Server IP address (dotted quad) */
  char *echoString;                /* String to send to echo server */
  char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
  unsigned int echoStringLen;      /* Length of string to echo */
  int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                      and total bytes read */
  if ( argc >= 2 && (strcmp(argv[1],"-h") & strcmp(argv[1],"--help")) == 0 ) {
    fprintf(stderr, "Usage:\n \
      %s /path/to/file\n \
      %s <nimbus_id>\n \
      %s\n", argv[0], argv[0], argv[0]);
    exit(1);
  }  

  echoString = "create_new_nimbus";

  if (argc == 4)
    echoServPort = atoi(argv[3]); /* Use given port, if any */
  else
    echoServPort = 7;  /* 7 is the well-known port for the echo service */

  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError("socket() failed");

  /* Construct the server address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
  echoServAddr.sin_family      = AF_INET;             /* Internet address family */
  echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
  echoServAddr.sin_port        = htons(echoServPort); /* Server port */

  /* Establish the connection to the echo server */
  if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    DieWithError("connect() failed");

  echoStringLen = strlen(echoString);          /* Determine input length */

  /* Send the string to the server */
  if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
    DieWithError("send() sent a different number of bytes than expected");

  /* Receive the same string back from the server */
  totalBytesRcvd = 0;
  printf("Received: ");                /* Setup to print the echoed string */
  while (totalBytesRcvd < echoStringLen) {
    /* Receive up to the buffer size (minus 1 to leave space for
    a null terminator) bytes from the sender */
    if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
      DieWithError("recv() failed or connection closed prematurely");
    totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
    echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
    printf("%s", echoBuffer);      /* Print the echo buffer */
  }

  printf("\n");    /* Print a final linefeed */

  close(sock);
  exit(0);
}
 
 
int curstart()
{  
  char mesg[] = "Just a string";
  char str[80];
  int row, col;
  
  initscr();    // Start curses mode
  raw();        // Disable line buffering
  noecho();     // Suppress automatic key echo
  keypad(stdscr, TRUE); // Enable reading of function keys, arrow keys, etc
  
  getmaxyx(stdscr, row, col); // get the number of rows and columns
  mvprintw(row/2, (col-strlen(mesg))/2, "%s", mesg);

  getstr(str);
  mvprintw(LINES - 2, 0, "You entered: %s", str);

  getch();     /* Wait for user input */
  endwin();     /* End curses mode      */
  return 0;
}
