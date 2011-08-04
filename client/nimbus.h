#ifndef NIMBUS_H
#define NIMBUS_H

#include "helper.h"

void editorSeeded(void);
void initialSyncNotice(int method);
int loadAndSeedFromFile(const char *filepath);

#endif /* NIMBUS_H */
