  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 

#include "helper.h" // Provides convenient abstractions on annoying shit.
#include "nimbus.h" // Provides wrapper functions for interfacing with a nimbus.

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

int main(int argc, char *argv[]) {
  struct pollfd ufds[2];
  fprintf(stdout, "Connecting to nimbus provider at %s on port %s\n", HOSTNAME, PORT);
  connectSocket(&sockfd, HOSTNAME, PORT);
  startupArgumentsHandler(argc, argv);
  ufds[0].fd = sockfd;
  ufds[0].events = POLLIN;
  ufds[1].fd = STDIN;
  ufds[1].events = POLLIN;
  while(1) {
    switch(poll(ufds, 2, -1)) {
      case -1:{ perror("poll"); break; }
      default:{
        if (ufds[0].revents & POLLIN) onSocketData();
        if (ufds[1].revents & POLLIN) onKeyData();
      }
    }
  }
  close(sockfd);
  exit(0);
}