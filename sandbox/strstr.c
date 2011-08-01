#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  char poo[] = "poo";
  if (strcmp("poo", poo) == 0)
    printf("equal");
  return 0;
}