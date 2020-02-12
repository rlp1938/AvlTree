#include <stdlib.h>
#include <mhash.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "calcmd5.h"

int
main(int argc, char **argv)
{
  
  if (argc < 2) {
    fputs("Requires an argument\n", stderr);
    exit(1);
  }
  char *path = argv[1];
  FILE *fpi = fopen(path, "r");
  if (!fpi) {
    perror(path);
    exit(1);
  }

char *sumout = calcmd5(fpi);
  printf("%s\n", sumout);
  return 0;
} // main()
