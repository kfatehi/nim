#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  FILE *pFile;
  long lSize;
  char *buffer;
  size_t result;
  
  pFile = fopen("README", "r");
  //if (pFile == NULL) {/* File error*/}
  
  // get file size
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);
  
  // allocate memory to hold contents
  buffer = (char *) malloc(sizeof(char)*lSize);
  //if (buffer == NULL) {/* Memory error*/}
  
  // copy file into buffer
  result = fread(buffer, 1, lSize, pFile);
  //if (result != lSize) {/* Read error*/}
  
  printf("%s\n", buffer);
  
  fclose(pFile);
  free(buffer);
  
  return 0;
}