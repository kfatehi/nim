#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  char buffer[] = "new_nimbus:ko24p2>blablablah:end_seed";
  
  
  buffer[strlen(buffer)-9] = '\0';
  
  printf("%s\n", buffer);
  
  //char delims[3] = ":>";
  char *result = NULL;
  result = strtok(buffer, ":");
  while (result != NULL) {
    if (strcmp(result, "new_nimbus")==0) 
      printf("compare worked: %s\n", result);
    //printf( "result is \"%s\"\n", result );
    result = strtok( NULL, ":" );
  }
  return 0;
}