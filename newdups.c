/*
 * newdups.c
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
#include <mhash.h>

typedef struct filerec {
  char *path;
  ino_t inode;
  size_t fsize;
  char *md5;
  int delete_flag;
} filerec;

int main(int argc, char **argv)
{
  if (argc > 2) { // 0 or 1 user input dirs is acceptable.
    fprintf(stderr, "Extraneous parameter input: %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  // get the real path to process.
  // compare = compare_intp_uniq;
  char *rootpath;
  if (argv[1]) {
    rootpath = realpath(argv[1], NULL);
  } else {
    rootpath = realpath("./", NULL);
  }
  struct stat sb;
  if (stat(rootpath, &sb) == -1) {
    perror(rootpath);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "%s\n", rootpath);
  // recurse the rootpath and count number of regular files.
  rec_count = 0;
  recursedir_simple(rootpath);
  fprintf(stderr, "files %d\n", rec_count);

  return 0;
}

