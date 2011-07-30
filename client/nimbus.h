#ifndef NIMBUS_H
#define NIMBUS_H

#include "helper.h"

int sendToNimbus(char *msg);
int getFromNimbus(char *msg);
// int onDataFromNimbus(const int sock); // called from the select() when it detects activity on the sock fd
// int seedNimbus(const int sock, char *id, filepath);

#endif /* NIMBUS_H */
