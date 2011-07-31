#ifndef NIM_H
#define NIM_H

#include "helper.h" // Provides convenient abstractions on annoying shit.
#include "nimbus.h" // Provides wrapper functions for interfacing with a nimbus.
#include "keys.h"

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

// Contexts
#define TERM 0
#define CMND 1
#define EDIT 2
#define CHAT 3
#define PREVIOUS -1

#define CMND_BUF_SIZE 64

// Structs
struct _terminal {
  // hold the last character, so that we
  // can detect and react correctly to escape sequences.
  // without losing function of the escape key alone
  char escFlag; //
  char prevkey;
};

struct _cmnd { // Used for CMND Mode
  char *history;
  char buffer[CMND_BUF_SIZE-1];
  char res;
};

struct _context {
  int previous;
  int current;
};

// Globals
char Running = 1; // flipping this kills the loop
struct _terminal;
struct _context context;
struct _cmnd cmnd;

int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

void onSocketData(void);
void onKeyData(void);

void executeCommand(char *str);
void switchContext(int n);

#endif /* NIM_H */