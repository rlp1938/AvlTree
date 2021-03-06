/*
 * test.c
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

#define _GNU_SOURCE     /* Expose declaration of tdestroy() */
#include <search.h>
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

typedef struct filerec {
  char *path;
  ino_t inode;
  size_t fsize;
  char md5[33];
} filerec;

int main(int argc, char **argv)
{
  fprintf(stdout,"%lu\n", sizeof(filerec));
  filerec *fr = calloc(10, sizeof(struct filerec));
  fprintf(stdout,"%lu\n", sizeof(fr));  
  return 0;
}

