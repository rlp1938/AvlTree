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

static void *root = NULL;
static void
action(const void *nodep, const VISIT which, const int depth);
static int
compare_intp_uniq(const void *pa, const void *pb);
static int
compare_intp(const void *pa, const void *pb);
static int
(*compare) (const void *pa, const void *pb);
static void *
xmalloc(unsigned n);
static void
recursedir_simple(const char *dirname);
static void
recursedir_act(const char *dirname, filerec *fr);
static void
printNodes(filerec *fr, int count);
static void
printnode(filerec node);


// Global vars
static int rec_count, idx;

int
main(int argc, char **argv)
{
  int i, *ptr;
  void *val;
  // get the  
  compare = compare_intp_uniq;
  char *rootpath;
  if (argv[1]) {
    rootpath = realpath(argv[1], NULL);
  } else {
    rootpath = realpath("./", NULL);
  }
  printf("%s\n", rootpath);
  // recurse the rootpath and count number of regular files.
  rec_count = 0;
  recursedir_simple(rootpath);
  fprintf(stdout, "files %d\n", rec_count);
  filerec *fr = xmalloc(sizeof(struct filerec) * (rec_count + 1));
  filerec lfr = {0};
  int ind;
  for (i = 0; ind < rec_count; ind++) {
    fr[ind] = lfr;
  }
  recursedir_act(rootpath, fr);
  free(rootpath);
  ind = 0;
  printNodes(fr, rec_count);

  exit(0);
  int nums[12] = {12, 13, 5, 10, 25, 1, 3, 10, 55, 2, 7, 5 };
  for (i = 0; i < 12; i++) {
    
    ptr = xmalloc(sizeof(int));
    //*ptr = rand() & 0xff;
    *ptr = nums[i];
    val = tsearch((void *) ptr, &root, compare);
    if (val == NULL)
      exit(EXIT_FAILURE);
    else if ((*(int **) val) != ptr)
      free(ptr);
  }
  twalk(root, action);
  tdestroy(root, free);
  exit(EXIT_SUCCESS);
}

static void *
xmalloc(unsigned n)
{
  void *p;
  p = malloc(n);
  if (p)
    return p;
  fprintf(stderr, "insufficient memory\n");
  exit(EXIT_FAILURE);
}

static int
compare_intp(const void *pa, const void *pb)
{
  if (*(int *) pa < *(int *) pb)
    return -1;
  if (*(int *) pa >= *(int *) pb)
    return 1;
  return 0;
}

static int
compare_intp_uniq(const void *pa, const void *pb)
{
  if (*(int *) pa < *(int *) pb)
    return -1;
  if (*(int *) pa > *(int *) pb)
    return 1;
  return 0;
}

static void
action(const void *nodep, const VISIT which, const int depth)
{
  int *datap;

  switch (which) {
  case preorder:
    break;
  case postorder:
    datap = *(int **) nodep;
    printf("%6d\n", *datap);
    break;
  case endorder:
    break;
  case leaf:
    datap = *(int **) nodep;
    printf("%6d\n", *datap);
    break;
  }
}

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

static void
printNodes(filerec *fr, int count)
{
  int i;
  for (i = 0; i < count; i++) {
    printnode(fr[i]);
  }
} // printNodes()

static void
printnode(filerec node)
{
  if (!node.path) return;
  fprintf(stdout, "%s\n", node.path);
  fprintf(stdout, "inode %lu\tsize %lu\n", node.inode, node.fsize);
}
