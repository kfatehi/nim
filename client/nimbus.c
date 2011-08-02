#include "nimbus.h"

void editorSeeded() {
  clearLine(LINES-1);
  printBottomLeft("Editor seeded!");
}
void newNimbusCreated() {
  char msg[48] = "New empty nimbus created: ";
  strcat(msg, id);
  clearLine(LINES-1);
  printBottomLeft(msg);
}
int loadAndSeedFromFile(char *filepath){
  char head[32] = "seed_buffer:";
  strcat(head, id);
  strcat(">")
  
  FILE *pFile;
  long lSize;
  char *buffer;
  size_t result;

  pFile = fopen(filepath, "r");
  if (pFile == NULL) return -1;

  // get file size
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  // allocate memory to hold contents
  buffer = (char *) malloc(sizeof(char)*lSize);
  if (buffer == NULL) return -2;

  // copy file into buffer
  result = fread(buffer, 1, lSize, pFile);
  if (result != lSize) return -3;

  writeSocket(sockfd, buffer);

  fclose(pFile);
  free(buffer);
  
  return 0;
}
