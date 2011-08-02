  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Nim (a.k.a "Network Vim")
   * http://github.com/keyvanfatehi/nim
   * Copyright (c) Keyvan Fatehi 2011
  \* * * * * * * * * * * * * * * * * * * * * * */ 
#include "nim.h"

int main(int argc, char *argv[]) {
  struct pollfd ufds[2];
  ufds[0].fd = sockfd;
  ufds[0].events = POLLIN;
  ufds[1].fd = STDIN_FILENO;
  ufds[1].events = POLLIN;
  connectSocket(&sockfd, HOSTNAME, PORT);
  if ( argc == 1 ){ // no args, create blank new nimbus
    startgui();
    printBottomLeft("Creating new nimbus, please wait.");
		writeSocket(sockfd, "create_new_nimbus");
	} else if ( argc >= 2 ) {	
		if ( (strcmp(argv[1],"-h") & strcmp(argv[1],"--help")) == 0 || argc > 2 ) {
			fprintf(stderr, "Usage: \n%s\n%s /path/to/file\n%s <nimbus_id>\n", argv[0], argv[0], argv[0]);
      exit(1);
		} else {
		  if ( fileExists(argv[1]) == 1 ) {
        socketPrecondition = WAITING_TO_SEED;
  		  fprintf(stdout, "Creating new nimbus from file %s\n", argv[1]);
        strcpy(filePath, argv[1]);
        startgui();
        writeSocket(sockfd, "create_new_nimbus");
  		} else {
  			fprintf(stdout, "Asking server if %s is a valid nimbus id...\nnot implemented\n", argv[1]);
  			// not implemented
  		}
		}
	}
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

  switch (socketPrecondition) {
    case WAITING_TO_SEED:
    case NONE: {
      char *message = NULL;
      message = strtok(buffer, ":");
      if (strcmp(message, "new_nimbus") == 0) {          
        strcpy(id, strtok(NULL, ":"));
        if (socketPrecondition == WAITING_TO_SEED) {
          loadAndSeedFromFile();
          socketPrecondition = NONE;
        }
        newNimbusCreated();
      } else if (strcmp(message, "seed_buffer") == 0) {
        edit.buffer[0] = '\0';
        if (strstr(message, "end_seed")) { // buffer contains :end_seed
          buffer[bytes-9] = '\0'; // remove :end_seed
          editorSeeded();
        } else socketPrecondition = OVERFLOW_SEED;
        strcat(edit.buffer, buffer);
      } else if (strcmp(message, "error") == 0) {
        char error[64] = "error: ";
        strcat(error, strtok(NULL, ":"));
        printBottomLeft(error);
      }
      break;
    }
    case OVERFLOW_SEED: {
      if (strstr(buffer, "end_seed")) { // buffer contains :end_seed
        buffer[bytes-9] = '\0'; // remove :end_seed
        socketPrecondition = NONE;
        editorSeeded();
      } // FIXME problems if a break occurs between the :end_seed so we never see it
      if (strlen(edit.buffer) > (EDITOR_BUF_SIZE-32)) // FIXME bad solution for buffer overflow
        strcat(edit.buffer, buffer); 
      else {
        strcat(edit.buffer, " ~BUFFER OVERFLOW~ ");
        socketPrecondition = NONE; editorSeeded();
      }
      break;
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


