  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 

#include "helper.h"

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

int main(int argc, char *argv[])
{
  int sockfd;
  connectSocket(&sockfd, HOSTNAME, PORT);
  char rcvBuffer[RCVBUFSIZE];

  if ( argc == 1 ){
		writeSocket(sockfd, "create_new_nimbus");
	} else if ( argc >= 2 ) {	
		if ( (strcmp(argv[1],"-h") & strcmp(argv[1],"--help")) == 0 || argc > 2 ) {
			fprintf(stderr, "Usage: \n%s\n%s /path/to/file\n%s <nimbus_id>\n", argv[0], argv[0], argv[0]);
		} else if ( fileExists(argv[1]) )
			fprintf(stdout, "Creating new nimbus from file %s\n", argv[1]);
		else {
			fprintf(stdout, "Asking server if %s is a valid nimbus id...\n", argv[1]);
		}
	}  

  while (1) {
    readSocket(sockfd, rcvBuffer, RCVBUFSIZE);
  }
  
  //....

  close(sockfd);
  exit(0);
}