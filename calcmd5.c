#include "calcmd5.h"

char
*calcmd5(FILE *fpi)
{
  size_t bytes_read;
  MHASH td;
  unsigned char buffer[1024];
  unsigned char hash[16]; /* fits MD5 */
  static char result[33];

  td = mhash_init(MHASH_MD5);
  if (td == MHASH_FAILED) {
    perror("Hash init failed");
    exit(1);
  }
  while ((bytes_read = fread(buffer, 1, 1024, fpi)) > 0) {
    mhash(td, buffer, bytes_read);
  }

  mhash_deinit(td, hash);
  int i, j;
  for (i = 0, j = 0; i < 16; i++, j += 2) {
    sprintf(&result[j], "%.2x", hash[i]);
  }
  return result;
} // calcmd5()
