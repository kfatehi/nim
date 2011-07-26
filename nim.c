/*
 * Nim client
 */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <unistd.h>     /* for close() */
#include <ncurses.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define IP_ADDRESS "127.0.0.1"
#define PORT 8000

void initGui(void);
bool fileExists(const char *filename);
void DieWithError(char *errorMessage);
const int establishConnection(const char *ip, const int port);
void writeSocket(const int sock, const char *buffer);
void readSocket(const int sock, char *buffer, const unsigned int buf_size);

int main(int argc, char *argv[])
{
  const int sock = establishConnection(IP_ADDRESS, PORT);
  char rcvBuffer[RCVBUFSIZE];

  if ( argc == 1 ) {
		// We are creating a blank, new nimbus
		
	} else if ( argc >= 2 ) {	
		if ( (strcmp(argv[1],"-h") & strcmp(argv[1],"--help")) == 0 || argc > 2 ) {
			char str[80] = "Usage: \n";
			strcat(str, argv[0]);
			strcat(str, " /path/to/file\n");
			strcat(str, argv[0]);
			strcat(str, " <nimbus_id>\n");
			strcat(str, argv[0]);
			DieWithError(str);
		} else if ( fileExists(argv[1]) )
			fprintf(stdout, "Creating new nimbus from file %s\n", argv[1]);
		else {
			fprintf(stdout, "Asking server if %s is a valid nimbus id...\n", argv[1]);
		}
	}  

  writeSocket(sock, "create_new_nimbus");
	
  readSocket(sock, rcvBuffer, RCVBUFSIZE);
  


  close(sock);
  exit(0);
}

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
const int establishConnection(const char *ip, const int port) {
  int sock;
  struct sockaddr_in servAddr; // Server address
  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError("socket() failed");
  /* Construct the server address structure */
  memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
  servAddr.sin_family      = AF_INET;             /* Internet address family */
  servAddr.sin_addr.s_addr = inet_addr(ip);   /* Server IP address */
  servAddr.sin_port        = htons(port); /* Server port */
  /* Establish the connection to the echo server */
  if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithError("Failed to connect to server"); 
  return sock;
}
void initGui() {  
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
}
bool fileExists(const char *filename) {
	FILE *file;
	if (file = fopen(filename, "r"))  {
		fclose(file);
		return true;
	}
	return false;
}
void DieWithError(char *errorMessage) {
  /* Error handling function */
	fprintf(stderr, "%s\n", errorMessage);  
	exit(1);
}
