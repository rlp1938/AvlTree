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

  for (i = 0; i < hash_blocksize; i++) {
    printf("%.2x", hash[i]);
  }
  printf("\n");
  return 0;
} // main()
