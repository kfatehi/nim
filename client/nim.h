#ifndef NIM_H
#define NIM_H

#include "helper.h" // Provides convenient abstractions on annoying shit.
#include "nimbus.h" // Provides wrapper functions for interfacing with a nimbus.


// Structs
struct _terminal { // i may use this for catching escape sequences
  // hold the last character, so that we
  // can detect and react correctly to escape sequences.
  // without losing function of the escape key alone
  char escFlag; //
  char prevkey;
};

struct _edit { // Used in the editor context
  char buffer[EDITOR_BUF_SIZE]; // i dont want a limit on file size, nor do i want to hardcode, damn
  // maybe we should use a pointer array to lines ? hmmm a doubly linked list even....
  // this could get complicated.....
};

struct _cmnd { // Used in the command context
  char *history[50]; // 50 strings can be held. FIXME use a linked list
  char buffer[512];
};

struct _context {
  int previous;
  int current;
};

// Globals
char socketPrecondition = NONE; // used when receiving socket data
char Running = 1; // flipping this kills the loop
char filePath[256];
struct _terminal;
struct _context context;
struct _cmnd cmnd;
struct _edit edit;

int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

void onSocketData(void);
void onKeyData(void);

void executeCommand(char *str);
void switchContext(int n);

#endif /* NIM_H */