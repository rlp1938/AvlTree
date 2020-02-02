#include <stdlib.h>
#include <mhash.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char **argv)
{
  size_t i, j, hash_blocksize, bytes_read;
  MHASH td;
  unsigned char buffer[1024];
  unsigned char hash[16]; /* fits MD5 */
  char sum[33];

  td = mhash_init(MHASH_MD5);
  if (td == MHASH_FAILED) {
    perror("Hash init failed");
    exit(EXIT_FAILURE);
  }
  hash_blocksize = mhash_get_block_size(MHASH_MD5);
  while ((bytes_read = fread(buffer, 1, 1024, stdin)) > 0) {
    mhash(td, buffer, bytes_read);
  }


  mhash_deinit(td, hash);
/*
  for (i = 0; i < hash_blocksize; i++) {
    printf("%.2x", hash[i]);
  }
  printf("\n");
  */
  snprintf("sum", 2, "%.2x", hash[0]);
  snprintf("sum+2", 2, "%.2x", hash[1]);
  snprintf("sum+4", 2, "%.2x", hash[2]);
  snprintf("sum+6", 2, "%.2x", hash[3]);
  snprintf("sum+8", 2, "%.2x", hash[4]);
  snprintf("sum+10", 2, "%.2x", hash[5]);
  snprintf("sum+12", 2, "%.2x", hash[6]);
  snprintf("sum+14", 2, "%.2x", hash[7]);
  snprintf("sum+14", 2, "%.2x", hash[8]);
  snprintf("sum+18", 2, "%.2x", hash[9]);
  snprintf("sum+20", 2, "%.2x", hash[10]);
  snprintf("sum+22", 2, "%.2x", hash[11]);
  snprintf("sum+24", 2, "%.2x", hash[12]);
  snprintf("sum+28", 2, "%.2x", hash[13]);
  snprintf("sum+30", 2, "%.2x", hash[14]);
  snprintf("sum+32", 2, "%.2x", hash[15]);
  sum[32] = 0;
  printf("%s\n", sum);
  return 0;
} // main()
