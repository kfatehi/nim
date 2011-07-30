  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 
#include "nim.h"

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

void onSocketData() {
  char buffer[RCVBUFSIZE];
  int bytes;
  bytes = readSocket(sockfd, buffer, RCVBUFSIZE);
  printf("Received %d bytes. Data: %s\n", bytes, buffer);
}

void onKeyData() {
  printf("Got key data, but it's line buffered so LOLOLOLOLOL all over your screen!\n");
}