#ifndef NIM_H
#define NIM_H

#include "helper.h" // Provides convenient abstractions on annoying shit.
#include "nimbus.h" // Provides wrapper functions for interfacing with a nimbus.
#include "keys.h"

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

// Contexts
#define ROOT 0
#define CMND 1
#define EDIT 2
#define CHAT 3
#define PREVIOUS -1

#define CMND_BUF_SIZE 512

// Structs
struct _terminal { // i may use this for catching escape sequences
  // hold the last character, so that we
  // can detect and react correctly to escape sequences.
  // without losing function of the escape key alone
  char escFlag; //
  char prevkey;
};

struct _edit { // Used in the editor context
  char *buffer;
};

struct _cmnd { // Used in the command context
  char *history;
  char buffer[CMND_BUF_SIZE-1];
};

struct _context {
  int previous;
  int current;
};

// Globals
char Running = 1; // flipping this kills the loop
struct _terminal;
struct _context context;
struct _edit edit;
struct _cmnd cmnd;

int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

void onSocketData(void);
void onKeyData(void);

void executeCommand(char *str);
void switchContext(int n);

#endif /* NIM_H */