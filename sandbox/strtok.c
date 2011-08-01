#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  char buffer[] = "new_nimbus:ko24p2>blablabla";
  
  char delims[3] = ":>";
  char *result = NULL;
  result = strtok(buffer, delims);
  while (result != NULL) {
    printf( "result is \"%s\"\n", result );
    result = strtok( NULL, delims );
  }
  return 0;
}