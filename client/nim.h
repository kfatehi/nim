// Only nim.c should include this.
#ifndef NIM_H
#define NIM_H

#include "helper.h" // Provides convenient abstractions on annoying shit.
#include "gui.h" // All the nasty ncurses stuff can hopefully hide here

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

// Globals
int sockfd; // socket file descriptor
char id[NIMBUS_ID_LEN]; // nimbus id

// Callbacks
void onSocketData();
void onKeyData();

#endif /* NIM_H */