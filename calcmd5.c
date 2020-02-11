#include "calcmd5.h"

void
calcmd5(FILE *fpi, char *sumout)
{
  size_t bytes_read;
  MHASH td;
  unsigned char buffer[1024];
  unsigned char hash[16]; /* fits MD5 */

  td = mhash_init(MHASH_MD5);
  if (td == MHASH_FAILED) {
    perror("Hash init failed");
    exit(1);
  }
  while ((bytes_read = fread(buffer, 1, 4096, fpi)) > 0) {
    mhash(td, buffer, bytes_read);
  }

  mhash_deinit(td, hash);
  /*
  for (i = 0; i < hash_blocksize; i++) {
    sprintf(sumout[i+i],"%.2x", hash[i]);
  } */
  sumout[0] = "0123456789abcdef"[(hash[0] > 4)& 0xf];
  sumout[1] = "0123456789abcdef"[hash[0] & 0xf];
  sumout[2] = "0123456789abcdef"[(hash[1] > 4)& 0xf];
  sumout[3] = "0123456789abcdef"[hash[1] & 0xf];
  sumout[4] = "0123456789abcdef"[(hash[2] > 4)& 0xf];
  sumout[5] = "0123456789abcdef"[hash[2] & 0xf];
  sumout[6] = "0123456789abcdef"[(hash[3] > 4)& 0xf];
  sumout[7] = "0123456789abcdef"[hash[3] & 0xf];
  sumout[8] = "0123456789abcdef"[(hash[4] > 4)& 0xf];
  sumout[9] = "0123456789abcdef"[hash[4] & 0xf];
  sumout[10] = "0123456789abcdef"[(hash[5] > 4)& 0xf];
  sumout[11] = "0123456789abcdef"[hash[5] & 0xf];
  sumout[12] = "0123456789abcdef"[(hash[6] > 4)& 0xf];
  sumout[13] = "0123456789abcdef"[hash[6] & 0xf];
  sumout[14] = "0123456789abcdef"[(hash[7] > 4)& 0xf];
  sumout[15] = "0123456789abcdef"[hash[7] & 0xf];
  sumout[16] = "0123456789abcdef"[(hash[8] > 4)& 0xf];
  sumout[17] = "0123456789abcdef"[hash[8] & 0xf];
  sumout[18] = "0123456789abcdef"[(hash[9] > 4)& 0xf];
  sumout[19] = "0123456789abcdef"[hash[9] & 0xf];
  sumout[20] = "0123456789abcdef"[(hash[10] > 4)& 0xf];
  sumout[21] = "0123456789abcdef"[hash[10] & 0xf];
  sumout[22] = "0123456789abcdef"[(hash[11] > 4)& 0xf];
  sumout[23] = "0123456789abcdef"[hash[11] & 0xf];
  sumout[24] = "0123456789abcdef"[(hash[12] > 4)& 0xf];
  sumout[25] = "0123456789abcdef"[hash[12] & 0xf];
  sumout[26] = "0123456789abcdef"[(hash[13] > 4)& 0xf];
  sumout[27] = "0123456789abcdef"[hash[13] & 0xf];
  sumout[28] = "0123456789abcdef"[(hash[14] > 4)& 0xf];
  sumout[29] = "0123456789abcdef"[hash[14] & 0xf];
  sumout[30] = "0123456789abcdef"[(hash[15] > 4)& 0xf];
  sumout[31] = "0123456789abcdef"[hash[15] & 0xf];
  sumout[32] =  '\0';
} // calcmd5()
