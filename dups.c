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

static void *
xmalloc(unsigned n);
static void
recursedir_simple(const char *dirname);
static void
recursedir_act(const char *dirname, filerec *fr);
static void
printNodes(const char *outfile, filerec *fr, int count);
static void
printnode(FILE *ofp, filerec node);
static int
cmpfsizep(const void *p1, const void *p2);
static void
mark_sizegroup_deleted(FILE *fpo, filerec *fr, int idxst, int idxend);
static int
cmpinodep(const void *p1, const void *p2);
static void
sum_md5(const char *path, char *thesum);
static int
cmpmd5_inodep(const void *p1, const void *p2);


// Global vars
static int rec_count, idx;

int
main(int argc, char **argv)
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
  printf("%s\n", rootpath);
  // recurse the rootpath and count number of regular files.
  rec_count = 0;
  recursedir_simple(rootpath);
  fprintf(stderr, "files %d\n", rec_count);
  filerec *fr = xmalloc(sizeof(struct filerec) * rec_count);
  filerec lfr = {0};
  int ind;
  for (ind = 0; ind < rec_count; ind++) {
    fr[ind] = lfr;
  }
  recursedir_act(rootpath, fr);
  free(rootpath);
  ind = 0;
  printNodes("/tmp/files_read", fr, rec_count);
  // sort on file size
  qsort(fr, rec_count, sizeof(struct filerec), cmpfsizep);
  printNodes("/tmp/files_sorted_on_size", fr, rec_count);
  // get rid of any files with unique file size
  FILE *fpo = fopen("/tmp/files_deletes", "w");
  if (fr[0].fsize != fr[1].fsize) {
    fr[0].delete_flag = 1;
    printnode(fpo, fr[0]);
  }
  for (ind = 1; ind < rec_count-1; ind++) {
    if (fr[ind].fsize != fr[ind-1].fsize &&
        fr[ind].fsize != fr[ind+1].fsize) {
      fr[ind].delete_flag = 1;
      printnode(fpo, fr[ind]);
    }
  } // for()
  if (fr[rec_count-2].fsize != fr[rec_count-1].fsize) {
    fr[rec_count-1].delete_flag = 1;
    printnode(fpo, fr[rec_count-1]);
  }
  fclose(fpo);
  // now create the revised list
  // how many nodes do I actually need in the next list?
  int rec_count1 = 0;
  for (ind = 0; ind < rec_count; ind++) {
    if (fr[ind].fsize > 0 && fr[ind].delete_flag == 0) {
      rec_count1++;
    }
  }
  rec_count1++;
  filerec *fr1 = calloc(rec_count1, sizeof(struct filerec));
  int ind1 = 0;
  for (ind = 0; ind < rec_count; ind++) {
    if (fr[ind].fsize != 0 && fr[ind].delete_flag != 1) {
      fr1[ind1] = fr[ind];
      ind1++;
    }
  }
  rec_count1 = ind1;
  fprintf(stderr, "non unique size files: %i\n", rec_count1);
  printNodes("/tmp/files_no_unique_sizes", fr1, rec_count1);
  /* Deal with hard linked files. There is the possibilty that
   * unique sizes exist that are used by only one inode. */
  fpo = fopen("/tmp/files_sizegroup_deletes", "w");
  ino_t inodestart, inodeend;
  int ind1start, ind1end;
  inodestart = fr1[0].inode;
  ind1start = 0;
  for (ind1 = 1; ind1 < rec_count1; ind1++) {
    if (fr1[ind1].fsize != fr1[ind1-1].fsize) {
      inodeend = fr1[ind1-1].inode;  // inode at end of size group.
      ind1end = ind1-1; // index at end of size group.
      if (inodestart == inodeend) {
        /* we have just 1 inode with this file size, so there are no
         * duplicated copies of this file.*/
        mark_sizegroup_deleted(fpo, fr1, ind1start, ind1end);
      } // if(inodestart...)
      // now reset the start markers
      inodestart = fr1[ind1].inode;
      ind1start = ind1;
    } // (fr1[ind]...)
  } // for()
  fclose(fpo);
  /* now create another list with the unique sizes using only 1 inode
   * deleted. Scan the list and count need list size. */
  int rec_count2 = 0;
  for (ind1 = 0; ind1 < rec_count1; ind1++) {
    if (fr1[ind1].delete_flag == 1) {
      rec_count2++;
    }
  }
  rec_count2++;
  filerec *fr2 = calloc(rec_count2, sizeof(struct filerec));
  int ind2 = 0;
  for (ind1 = 0; ind1 < rec_count1; ind1++) {
    if (fr1[ind1].delete_flag == 0) {
      fr2[ind2] = fr1[ind1];
      ind2++;
    }
  }
  rec_count2 = ind2;
  // now sort this new array on inode number.
  qsort(fr2, rec_count2, sizeof(struct filerec), cmpinodep);
  printNodes("/tmp/files_inode_sorted", fr2, rec_count2);
  // now calculate md5sum for all listed files
  char *thesum = calloc(33, 1);
  sum_md5(fr2[0].path, thesum);
  fr2[0].md5 = strdup(thesum);
  for (ind2 = 1; ind2 < rec_count2; ind2++) {
    if (fr2[ind2].inode == fr2[ind2-1].inode) {
      fr2[ind2].md5 = fr2[ind2-1].md5;
    } else {
      sum_md5(fr2[ind2].path, thesum);
      fr2[ind2].md5 = strdup(thesum);
    }
  } // for()
  // sort on m5sum + inode
  qsort(fr2, rec_count2, sizeof(struct filerec), cmpmd5_inodep);
  printNodes("/tmp/files_hasmd5_sorted", fr2, rec_count2);
  free(thesum);
  // now get rid of singleton md5sums
  if (strcmp(fr2[0].md5, fr2[1].md5)) {
    fr2[0].delete_flag = 1;
  }
  for (ind2 = 1; ind2 < rec_count2-1; ind2++) {
    if (strcmp(fr2[ind2].md5, fr2[ind2-1].md5) &&
        strcmp(fr2[ind2].md5, fr2[ind2+1].md5)) {
      fr2[ind2].delete_flag = 1;
    } // if()
  } // for()
  if (strcmp(fr2[ind2-2].md5, fr2[ind2-1].md5)) {
    fr2[ind2-1].delete_flag = 1;
  }

  filerec *fr3 = calloc(rec_count2, sizeof(struct filerec));
  int ind3 = 0;
  for (ind2 = 0; ind2 < rec_count2; ind2++) {
    if (fr2[ind2].delete_flag == 0) {
      fr3[ind3] = fr2[ind2];
      ind3++;
    }
  }
  int rec_count3 = ind3;
  // display the results
  printNodes("/tmp/files_hasmd5_grouped", fr3, rec_count3);
  
  return 0;
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
printNodes(const char *outfile, filerec *fr, int count)
{
  FILE *ofp = fopen(outfile, "w");
  int i;
  for (i = 0; i < count; i++) {
    printnode(ofp, fr[i]);
  }
  fclose(ofp);
} // printNodes()

static void
printnode(FILE *ofp, filerec node)
{
  if (!node.path) return;
  fprintf(ofp, "%s\n", node.path);
  fprintf(ofp, "inode %lu\tsize %lu\n", node.inode, node.fsize);    fprintf(ofp, "%s %d\n", node.md5, node.delete_flag);
} // printnode()

static int
cmpfsizep(const void *p1, const void *p2)
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
} // cmpfsizep()

static void
mark_sizegroup_deleted(FILE *fpo, filerec *fr, int idxst, int idxend)
{ /* A group of identical file sizes all have the same inode number,
   * so they are not duplicated copies, therefore they are to be
   * removed from consideration. */
  int i;
  for (i = idxst; i < idxend+1; i++) {
    fr[i].delete_flag = 1;
    printnode(fpo, fr[i]);
  }
} // mark_sizegroup_deleted()

static int
cmpinodep(const void *p1, const void *p2)
{
  filerec *frp1 = (filerec *)p1;
  filerec *frp2 = (filerec *)p2;

  /* I can not just rely on a simple subtaction because I am operating
   * on 8 byte numbers which can generate results that overflow an int.
  */
  if (frp1->inode > frp2->inode) {
    return 1;
  } else if (frp1->inode < frp2->inode) {
    return -1;
  } else {
    return 0;
  }
} // cmpinodep()

static void
sum_md5(const char *path, char *thesum)
{
  size_t i, hash_blocksize, bytes_read;
  MHASH td;
  unsigned char buffer[4096];
  unsigned char hash[16]; /* fits MD5 */

  td = mhash_init(MHASH_MD5);
  if (td == MHASH_FAILED) {
    perror("Hash init failed");
    exit(EXIT_FAILURE);
  }

  FILE *fpi = fopen(path, "r");
  if (!fpi) { // not fatal, stuff like browser cache files can go AWL.
    perror("sum_md5");
    thesum = (char *)NULL;
    return;
  }

  hash_blocksize = mhash_get_block_size(MHASH_MD5);
  while ((bytes_read = fread(buffer, 1, 4096, fpi)) > 0) {
    mhash(td, buffer, bytes_read);
  }

  mhash_deinit(td, hash);

  for (i = 0; i < hash_blocksize; i++) {
    sprintf(&thesum[i+i], "%.2x", hash[i]);
  }
  fclose(fpi);
} // sum_md5()

static int
cmpmd5_inodep(const void *p1, const void *p2)
{
  filerec *frp1 = (filerec *)p1;
  filerec *frp2 = (filerec *)p2;

  /* I can not just rely on a simple subtaction because I am operating
   * on 8 byte numbers which can generate results that overflow an int.
  */
  int res = strcmp(frp1->md5, frp2->md5);
  if (res > 0) {
    return 1;
  } else if (res < 0) {
    return -1;
  } else if (frp1->inode > frp2->inode) {
    return 1;
  } else if (frp1->inode < frp2->inode) {
    return -1;
  } else {
    return 0;
  }
} // cmpinodep()
