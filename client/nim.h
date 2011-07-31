#ifndef NIM_H
#define NIM_H

#include "helper.h" // Provides convenient abstractions on annoying shit.
#include "nimbus.h" // Provides wrapper functions for interfacing with a nimbus.

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

// Contexts of the program
#define TERM 0
#define CMND 1
#define EDIT 2
#define CHAT 3

// Globals
int prevContext;  // Temporarily holds prev context
int NimContext = TERM; // The context of the program

char cmndBuffer[64];

int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

// Callbacks
void onSocketData();
void onKeyData();

#endif /* NIM_H */