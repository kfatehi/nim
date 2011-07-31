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
  // char buffer[SMALLBUF];
  // int bytes;
  // memset(buffer, 0, SMALLBUF);
  // bytes = read(STDIN_FILENO, buffer, SMALLBUF);
  // printf("STDIN: Receieved %d bytes. Data: %s END STDIN\n", bytes, buffer);
  char c;
  char str[2] = " \0";
  int bytes;
  if ((bytes = read(STDIN_FILENO, &c, 1)) >= 0) {
    str[0] = c;
    printf("keycode: %d character: %s\n", c, str);
    switch (NimContext) {
      case EDIT:{
        // allow normal character to fall through to the editor
        break;
      }
      case CHAT:{
        // allow normal character to fall through to the chat message buffer
        break;
      }
      case TERM:{
        switch (c) {
          case ':':{
            prevContext = NimContext;
            NimContext = CMND;
            strcpy(cmndBuffer, ""); // prepare the buffer  
            printf("CMND MODE\n");
            // draw ':' and cursor at the bottom left like vim does
            break;
          }
        }
        break;
      }
      case CMND:{
        switch (c) {
          case '\e':{ // ESCAPE
            NimContext = prevContext;
            printf("END CMND MODE\n");
            break;
          }
          case '\n':{
            // "execute" the built up command buffer
            // clear the command buffer
            NimContext = prevContext; // pop back into previous context
            break;
          }
          default:{
            if (c >= 32) {
              strcat(cmndBuffer, str);
              printf("CMNDBUFFER: %s\n", cmndBuffer);
            } else printf("unmapped key in cmnd mode:\n keycode: %d character: %s\n", c, str);
          }
        }
        break;
      }
    }
  } else perror("read(stdin)");
}

// i Input mode
// : Command mode
// :q Quit
// :w Save
// :x Save and quit
// ? Chat mode
// Esc Return to normal mode




