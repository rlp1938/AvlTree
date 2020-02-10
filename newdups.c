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

static void
show(const char *);
static void
shown(const char *, int);
static void
*xcalloc(size_t numelm, size_t elemsize);
static void
recursedir_simple(const char *dirname);
static void
recursedir_act(const char *dirname, filerec *fr);
static int
cmpfsize_inodep(const void *p1, const void *p2);

// Global vars
int rec_count, idx;

int main(int argc, char **argv)
{
  if (argc > 2) { // 0 or 1 user input dirs is acceptable.
    fprintf(stderr, "Extraneous parameter input: %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  // get the real path to process.
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
  show(rootpath);
  // recurse the rootpath and count number of regular files.
  rec_count = 0;
  recursedir_simple(rootpath);
  shown("File count", rec_count);
  filerec *fr = xcalloc(rec_count, sizeof(struct filerec));
  recursedir_act(rootpath, fr);
  free(rootpath);
  /* recursedir_simple counts regular files, recursedir_act records
   * only those regular files with size > 0. Consequently, there may
   * be a number of fr[] records which are null. After sorting on size,
   * these null records should float to the top of the list.
   * */
  qsort(fr, rec_count, sizeof(struct filerec), cmpfsize_inodep);
  /* Now inspect the file records, and mark those with unique size
   * for deletion from the list.*/
  if (fr[0].fsize != fr[1].fsize) fr[0].delete_flag = 1;
  int i;
  for (i = 1; i < rec_count-1; i++) {
    if (fr[i].fsize != fr[i-1].fsize && fr[i].fsize != fr[i+1].fsize) {
      fr[i].delete_flag = 1;
    }
  } // for(i...)
  if (fr[rec_count-1].fsize != fr[rec_count-2].fsize)
    fr[rec_count-1].delete_flag = 1;
  /* Now count the records to retain, those with size > 0, and have
   * delete flag not set. */
  int recs1 = 0;
  for (i = 0; i < rec_count; i++) {
    if (fr[i].fsize > 0 && fr[i].delete_flag == 0) recs1++;
  } // for(i...)
  shown("File records to retain", recs1);
  /* Records are in order of size, and inode as a secondary key. These
   * records will be placed in a second list, fr1 for further action. */
  filerec *fr1 = xcalloc(recs1, sizeof(struct filerec));
  int j = 0;
  for (i = 0; i < rec_count; i++) {
    if (fr[i].fsize > 0 && fr[i].delete_flag == 0) {
      fr1[j] = fr[i];
      j++;
    }
  } // for(i...)
  /* As well as unique file sizes, already dealt with, there may be
   * hard linked blocks of files which will manifest as blocks of
   * identical sizes paired with identical inodes. This next will
   * identify any such blocks.
   * NB only blocks comprising all identical file sizes paired with
   * identical inodes will be considered as singleton files. There are
   * more complex possibilties whereby identical files are in more than
   * one hard linked group. Any such groups will be identified at md5sum
   * processing time.
  */
  size_t starting_size = fr1[0].fsize;
  ino_t starting_inode = fr1[0].inode;
  int starting_index = 0;
  for (i = 1; i < recs1; i++) {
    if (fr1[i].fsize != starting_size) {
      if (fr1[i-1].inode == starting_inode) { // singleton inode block
        for (j = starting_index; j < i; j++) {
          fr1[j].delete_flag = 1;
        }
      }
    }
    starting_index = i;
    starting_inode = fr1[i].inode;
    starting_size = fr1[i].fsize;
  }
  // count the number of items to go to the next list.
  j = 0;
  for (i = 0; i < recs1; i++) {
    if (fr1[i].delete_flag == 0) j++;
  }
  int recs2 = j;
  return 0;
} // main()

static void
show(const char *s)
{ /* display it on stderr */
  fprintf(stderr, "%s\n", s);
}

static void
shown(const char *s, int n)
{ /* display it on stderr */
  fprintf(stderr, "%s %i\n", s, n);
}

static void
*xcalloc(size_t __nmemb, size_t __size)
{ /* Takes care of error handling. */
  void *p = calloc(__nmemb, __size);
  if (!p) {
    perror("xcalloc");
    exit(EXIT_FAILURE);
  }
  return p;
} // xcalloc()

static void
recursedir_simple(const char *path)
{ // count the number of regular files found.
  DIR *dp = opendir(path);
  if (!dp) {
    perror(path);
    exit(EXIT_FAILURE);
  }
  struct dirent *de;
  while ((de = readdir(dp))) {
    if (strcmp(de->d_name, ".") == 0 ) continue;
    if (strcmp(de->d_name, "..") == 0) continue;
    char joinbuf[PATH_MAX];
    strcpy(joinbuf, path);
    strcat(joinbuf, "/");
    strcat(joinbuf, de->d_name);
    switch (de->d_type) {
    case DT_DIR:
      recursedir_simple(joinbuf);
      break;
    case DT_REG:
      rec_count++;
      break;
    default:
      break;  // no interest in anything except regular files and dirs.
    } // switch()
  } // while()
  closedir(dp);
} // recursedir_simple()

static void
recursedir_act(const char *path, filerec *fr)
{ // record main data of regular files.
  DIR *dp = opendir(path);
  if (!dp) {
    perror(path);
    exit(EXIT_FAILURE);
  }
  struct dirent *de;
  while ((de = readdir(dp))) {
    if (strcmp(de->d_name, ".") == 0 ) continue;
    if (strcmp(de->d_name, "..") == 0) continue;
    char joinbuf[PATH_MAX];
    strcpy(joinbuf, path);
    strcat(joinbuf, "/");
    strcat(joinbuf, de->d_name);
    int res;
    struct stat sb;
    switch (de->d_type) {
    case DT_DIR:
      recursedir_act(joinbuf, fr);
      break;
    case DT_REG:
      res = stat(joinbuf, &sb);
      if (res == -1) {
        perror(joinbuf);
        exit(EXIT_FAILURE);
      }
      fr[idx].fsize = sb.st_size;
      if (sb.st_size == 0) continue;
      fr[idx].inode = sb.st_ino;
      fr[idx].path = strdup(joinbuf);
      idx++;
      break;
    default:
      break;  // no interest in anything except regular files and dirs.
    } // switch()
  } // while()
  closedir(dp);
} // recursedir_act()

static int
cmpfsize_inodep(const void *p1, const void *p2)
{ /* Will treat the inode number as a second place key. */
  filerec *frp1 = (filerec *)p1;
  filerec *frp2 = (filerec *)p2;

  /* I can not just rely on a simple subtaction because I am operating
   * on 8 byte numbers which can generate results that overflow an int.
  */
  if (frp1->fsize > frp2->fsize) {
    return 1;
  } else if (frp1->fsize < frp2->fsize) {
    return -1;
  } else {
    if (frp1->inode > frp2->inode) {
      return 1;
    } else if (frp1->inode < frp2->inode) {
      return -1;
    } else {
      return 0;
    }
  }
} // cmpfsize_inodep()
