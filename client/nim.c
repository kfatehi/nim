  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 
#include "nim.h"

int main(int argc, char *argv[]) {
  struct pollfd ufds[2];
  connectSocket(&sockfd, HOSTNAME, PORT);
  startupArgumentsHandler(argc, argv);

  ufds[0].fd = sockfd;
  ufds[0].events = POLLIN;
  ufds[1].fd = STDIN_FILENO;
  ufds[1].events = POLLIN;

  configTerminal(NB_ENABLE);
  while(1) {
    switch(poll(ufds, 2, -1)) {
      case -1:{ perror("poll()"); break; }
      default:{
        if (ufds[0].revents & POLLIN) onSocketData();
        if (ufds[1].revents & POLLIN) onKeyData();
      }
    }
  }
  close(sockfd);
  configTerminal(NB_DISABLE);
  exit(0);
}

void onSocketData() {
  char buffer[BIGBUF];
  int bytes;
  memset(buffer, 0, BIGBUF);
  bytes = readSocket(sockfd, buffer, BIGBUF);
  printf("SOCKET: Received %d bytes. Data: %s END SOCKET\n", bytes, buffer);
}

void onKeyData() {
  char buffer[SMALLBUF];
  int bytes;
  memset(buffer, 0, SMALLBUF);
  bytes = read(STDIN_FILENO, buffer, SMALLBUF);
  printf("STDIN: Receieved %d bytes. Data: %s END STDIN\n", bytes, buffer);
}