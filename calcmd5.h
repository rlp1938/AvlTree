#ifndef	_CALCMD5_H
#define	_CALCMD5_H 1

#include <stdlib.h>
#include <mhash.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void
calcmd5(FILE *fpi, char *sumout);

#endif /* calcmd5.h  */
