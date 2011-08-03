#include "nimbus.h"

void editorSeeded() {
  printBottomLeft("Editor seeded!");
}
void initialSyncNotice(int method) {
  char out[COLS];
  if (method == BLANK)
    strcpy(out, "Blank nimbus created. ID: ");
  else if (method == FROM_FILE)
    strcpy(out, "New nimbus created from file. ID: ");
  else if (method == JOINED)
    strcpy(out, "Joined existing nimbus. ID: ");
  strcat(out, id);
  printTopCenter(id);
  printBottomLeft(out);
}
void newNimbusCreated() {
  char out[COLS];

}
int loadAndSeedFromFile(char *filepath){
  char head[32] = "seed_buffer:";
  strcat(head, id);
  strcat(head, ">");
  
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
  

  writeSocket(sockfd, head);
  writeSocket(sockfd, buffer);
  
  fclose(pFile);
  free(buffer);
  
  return 0;
}
