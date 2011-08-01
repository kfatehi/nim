#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  int res;
  char buffer[512] = "new_nimbus:f426kf";
  printf("Loaded a string: %s\n", buffer);
  res = strcspn(buffer, ":"); 
  printf("Result: %d\n", res);
  // strtok is better
  return 0;
}