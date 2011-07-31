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

  context.current = TERM;
  configTerminal(NB_ENABLE);
  startGui();
  do {
    switch(poll(ufds, 2, -1)) {
      case -1:{ perror("poll()"); break; }
      default:{
        if (ufds[0].revents & POLLIN)
          onSocketData();
        if (ufds[1].revents & POLLIN)
          onKeyData();
      }
    }
  } while(Running);
  endwin();
  configTerminal(NB_DISABLE);
  close(sockfd);
  exit(0);
}

void onSocketData() {
  char buffer[BIGBUF];
  int bytes;
  memset(buffer, 0, BIGBUF);
  bytes = readSocket(sockfd, buffer, BIGBUF);
  // printf("SOCKET: Received %d bytes. Data: %s END SOCKET\n", bytes, buffer);
}

void onKeyData() { 
  char c;
  char str[2] = " \0";
  int bytes;
  if ((bytes = read(STDIN_FILENO, &c, 1)) >= 0) {
    str[0] = c;
    if (c == CTRL_C) Running = 0;
    mvprintw(0, 0, "keycode: %d character: %s   ", c, str); 
    switch (context.current) {
      case EDIT:{
        switch (c) {
          case ESCAPE:{
            switchContext(PREVIOUS);
            break;
          }
        }
        // allow normal character to fall through to the editor
        break;
      }
      case CHAT:{
        switch (c) {
          case ESCAPE:{
            switchContext(PREVIOUS);
            break;
          }
        }
        // allow normal character to fall through to the chat message buffer
        break;
      }
      case TERM:{ 
        switch (c) {
          case ':':{
            strcpy(cmnd.buffer, str); // prepare the buffer
            printBottomLeft(cmnd.buffer);
            switchContext(CMND);
            // draw cmndBuffer and cursor at the bottom left like vim does
            break;
          }
        }
        break;
      }
      case CMND:{
        switch (c) {
          case ESCAPE:{
            switchContext(PREVIOUS);
            break;
          }
          case RETURN:
          case NEWLINE:{
            executeCommand(cmnd.buffer);
            switchContext(PREVIOUS); // pop back into previous context
            break;
          }
          case BACKSPACE:{
            backSpaceBuffer(cmnd.buffer, LINES-1);
            break;
          }
          default:{
            if (c >= 32) {
              if (strlen(cmnd.buffer) < CMND_BUF_SIZE)
                strcat(cmnd.buffer, str);
              else cmnd.buffer[0] = '\0'; // avoid buffer overflow crash
            } // else printf("unmapped key in cmnd mode:\n keycode: %d character: %s\n", c, str);
          }
        }
        printBottomLeft(cmnd.buffer);
        break;
      }
    }
    refresh();
    writeSocket(sockfd, str);
    if (context.current == CMND)
      writeSocket(sockfd, cmnd.buffer);
  } else perror("read(stdin)");
}

void executeCommand(char *str) {
  // : Command mode
  // :q Quit
  // :w Save
  // :x Save and quit
  if (strcmp(str, ":q") == 0) {
    Running = 0; // flip killswitch
    writeSocket(sockfd, "BYE");
  }
}
// i Input mode
// ? Chat mode
// Esc Return to normal mode

void switchContext(int n) {
  if (n == PREVIOUS) {
    int temp = context.current;
    context.current = context.previous;
    context.previous = temp;
  } else {
    context.previous = context.current;
    context.current = n;
  }
}


