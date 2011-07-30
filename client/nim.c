  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 

#include "helper.h" // convenient abstractions on annoying shit
#include "nimbus.h" // for communication with nimbus

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

int main(int argc, char *argv[]) {
  struct pollfd ufds[2];
  
  fprintf(stdout, "Connecting to nimbus provider at %s on port %s\n", HOSTNAME, PORT);
  connectSocket(&sockfd, HOSTNAME, PORT);

  if ( argc == 1 ){ // no args, create blank new nimbus
		//writeSocket(sockfd, "create_new_nimbus");
    sendToNimbus("create_new_nimbus");
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

  // let's use poll() instead of select. 
  // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#pollman
  // fd_set readfds;
  // FD_ZERO(&readfds);
  // FD_SET(STDIN, &readfds);
  // FD_SET(sockfd, &readfds);
  // 
  // // don't care about writefds and exceptfds:
  // while (select(sockfd+1, &readfds, NULL, NULL, NULL) >= 0) {
  //   if (FD_ISSET(STDIN, &readfds))
  //     printf("A key was pressed!\n");
  //   // else if (FD_ISSET(sockfd, &readfs))
  //   //   printf("A message is waiting!\n");
  //   else
  //     printf("Timed out.\n"); // this should not happen, as we arent passing a timeval to select
  //   FD_ZERO(&readfds);
  //   
  // }
  close(sockfd);
  exit(0);
}