  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 
#include "nim.h"

int socketMode;

int main(int argc, char *argv[]) {
  struct pollfd ufds[2];
  configTerminal(NB_ENABLE);
  initscr();
  switchContext(ROOT);
  printTopCenter("NIM");
  connectSocket(&sockfd, HOSTNAME, PORT);
  ufds[0].fd = sockfd;
  ufds[0].events = POLLIN;
  ufds[1].fd = STDIN_FILENO;
  ufds[1].events = POLLIN;
  startupArgumentsHandler(argc, argv);
  refresh();
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
  if (socketMode == OVERSIZE) {
    // Use socketMode to capture long seeds that exceed BIGBUF
  } else {
    char delims[3] = ":>";
    char *result = NULL;
    result = strtok(buffer, delims);
    int i;
    for (i = 0; ((result != NULL) && (i > -1)); i++) {
      result = strtok(NULL, delims);
      if strcmp(result
    }
  }

  refresh();
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
      case ROOT:{
        switch (c) {
          case 'n':{ // navigation buttons.
            switchContext(CMND);
            break;
          }
          case ':':{
            switchContext(CMND);
            break;
          }
          case '?':{
            switchContext(CHAT);
            break;
          }
          case 'i':{
            switchContext(EDIT);
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
            if (NUMLETTER(c)) {
              // If there is space in the terminal, add the character to the command buffer
              if (strlen(cmnd.buffer) < COLS-1) strcat(cmnd.buffer, str);
              // FIXME make this behave like vim (move the other shit up one line, etc)
              // For now we just don't let editing happen past the end of the row
            }
          }
        }
        if (context.current == CMND) // Make sure we didn't switch out of CMND mode already.
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
  // :w Save
  // :x Save and quit
  if (strcmp(str, ":q") == 0)
    Running = 0; // flip killswitch
}

void switchContext(int n) {
  clearLine(LINES-1);
  if (n == PREVIOUS) {
    int temp = context.current;
    context.current = context.previous;
    context.previous = temp;
  } else if (n == CMND) {
    context.previous = context.current;
    context.current = CMND;
    // Setup command buffer
    cmnd.buffer[0] = ':';
    cmnd.buffer[1] = '\0';
    printBottomLeft(cmnd.buffer);
  } else if (n == EDIT) {
    context.previous = context.current;
    context.current = EDIT;
    // Setup or get into the edit buffer...
    // I would want to maintain cursor last position as well.
  } else if (n == CHAT) {
    context.previous = context.current;
    context.current = CHAT;
    // Dont know how i want to do this yet.
    // Perhaps a new ncurses window overlay we can hide/show would be good.
  } else if (n == ROOT) {
    context.current = n;
  }
  if (context.current == CMND) printTopRight("CMND");
  else if (context.current == EDIT) printTopRight("EDIT");
  else if (context.current == CHAT) printTopRight("CHAT");
  else if (context.current == ROOT) printTopRight("ROOT");
}


