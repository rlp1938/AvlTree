/*
 * procdups.c
 * 
 * Copyright 2020 Robert L (Bob) Parker <rlp1938@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct fdata {
  char *begin;
  char *finis;
} fdata;

static fdata
*readfile(const char *path);
static void
*xcalloc(size_t count, size_t size);

int main(void)
{
  fdata *fd = readfile("dups.lst");
  return 0;
}

fdata
*readfile(const char *path)
{ /* eats a file in one lump. */
  fdata *fd = xcalloc(1, sizeof(struct fdata));
  struct stat sb;
  if (stat(path, &sb) == -1 ) {
    perror(path);
    exit(EXIT_FAILURE);
  }
  if (!(S_ISREG(sb.st_mode))) {
    fprintf(stderr, "Not a regular file: %s\n", path);
    exit(EXIT_FAILURE);
  }
  size_t _s = sb.st_size;
  void *p = xcalloc(1, _s);
  if (!p) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }
  FILE *fpi = fopen(path, "r");
  if (!fpi) {
    perror(path);
    exit(EXIT_FAILURE);
  }
  size_t bytes_read = fread(p, 1, _s, fpi);
  if (bytes_read != _s) {
    perror("fread");
    exit(EXIT_FAILURE);
  }
  fd->begin = p;
  fd->finis = p + _s;
  return fd;
} // readfile()

static void
*xcalloc(size_t _nmemb, size_t _size)
{ /* calloc() with error handling */
  void *p = calloc(_nmemb, _size);
  if (!p) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }
  return p;
} // xcalloc()
