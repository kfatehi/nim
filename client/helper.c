#include "helper.h"

void printTopCenter(char *msg) {
  mvprintw(0, (COLS-strlen(msg))/2, "%s", msg);
}

void printTopLeft(char *msg) {
  mvprintw(0, 0, "%s", msg);
}

void printTopRight(char *msg) {
  
}

void printBottomLeft(char *msg) {
  mvprintw(LINES-1, 0, "%s", msg);
}

void backSpaceBuffer(char *buffer, int last_line_no) {
  // Backspaces a buffer 
  int new_len = strlen(buffer)-1;
  buffer[new_len] = '\0';
  mvaddch(last_line_no, new_len, ' ');
}

void startGui() {  
  initscr();
  printTopLeft("NIM");
  refresh();
}

void startupArgumentsHandler(int argc, char *argv[]) {
  if ( argc == 1 ){ // no args, create blank new nimbus
		writeSocket(sockfd, "create_new_nimbus");
    sleep(1);
	} else if ( argc >= 2 ) {	
		if ( (strcmp(argv[1],"-h") & strcmp(argv[1],"--help")) == 0 || argc > 2 ) {
			fprintf(stderr, "Usage: \n%s\n%s /path/to/file\n%s <nimbus_id>\n", argv[0], argv[0], argv[0]);
		} else if ( fileExists(argv[1]) == 1 )
			fprintf(stdout, "Creating new nimbus from file %s\n", argv[1]);
		else {
			fprintf(stdout, "Asking server if %s is a valid nimbus id...\n", argv[1]);
		}
	}
}

void configTerminal(int state) {
  struct termios config;
  if (!isatty(STDIN_FILENO) || (tcgetattr(STDIN_FILENO, &config) < 0))
    perror("configTerminal()");
  // get the terminal state
  tcgetattr(STDIN_FILENO, &config);
  if (state==NB_ENABLE) {
    // remove echo, line buffer, and canonical mode flags
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    // minimum number of input to read.
    config.c_cc[VMIN] = 1;
  } else if (state==NB_DISABLE) {
    // set the flags back to normal
    config.c_lflag &= (ECHO | ECHONL | ICANON | IEXTEN | ISIG);
  }
  // set the terminal attributes.
  if(tcsetattr(STDIN_FILENO, TCSANOW, &config) < 0) perror("configTerminal()");
}

void connectSocket(int *sockfd, char *hostname, char *port) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints); // make sure struct is empty
	hints.ai_family = AF_UNSPEC; // be ipv4/ipv6 agnostic
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	if (getaddrinfo(hostname, port, &hints, &res) != 0)
		dieWithError("Failed to resolve host");
	// FIXME walk the "res" linked list for a valid entry, first one may be invalid
	if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		dieWithError("Failed to create socket descriptor");
  fprintf(stdout, "Connecting to %s on port %s\n", hostname, port);
	if (connect(*sockfd, res->ai_addr, res->ai_addrlen) != 0)
	  dieWithError("Failed to connect");
	freeaddrinfo(res);
}

int writeSocket(const int sock, const char *buffer) {
  int bytesSent;
  unsigned int len = strlen(buffer);        /* Determine input length */
  if ((bytesSent = send(sock, buffer, len, 0)) != len) {    /* Send the string to the server */
    fprintf(stderr, "send() sent a different number of bytes than expected");
    return -1;
  } else return bytesSent;
}

int readSocket(const int sock, char *buffer, const unsigned int buf_size) {
  int bytesRcvd;
  if ((bytesRcvd = recv(sock, buffer, buf_size - 1, 0)) <= 0)
    fprintf(stderr, "recv() failed or connection closed prematurely");
  buffer[bytesRcvd] = '\0';  /* Terminate the string! */
  return bytesRcvd;
}

int fileExists(const char *filename) {
	FILE *file;
	if (file = fopen(filename, "r"))  {
		fclose(file);
		return 0;
	}
	return 1;
}

// int getFileContents(const char *filename) {
//   FILE *f;
//  
//   char buffer[11];
//   if (f = fopen("fred.txt", "rt"))
//   {
//     fread(buffer, 1, 10, f);
//     buffer[10] = 0;
//     fclose(f);
//     printf("first 10 characters of the file:\n%s\n", buffer);
//   }
// }

void dieWithError(char *errorMessage) {
  /* Error handling function */
	fprintf(stderr, "%s\n", errorMessage);
  fprintf(stderr, "Error code? %d\n", errno);
	exit(1);
}