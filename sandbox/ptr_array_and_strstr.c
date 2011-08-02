#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  char *ptr[50]; // 50 strings can be held here
  char poo[] = "poo"; // heres a new string
  ptr[0] = (char *)&poo; // stick the string into our pointer at index 0
  if (strcmp("poo", ptr[0]) == 0)
    printf("equal");
  return 0;
}