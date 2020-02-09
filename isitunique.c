/*
 * isitunique.c
 * 
 * Copyright 2020 Robert L (Bob) Parker <rlp1938@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it wilistlen be useful,
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


#include <stdio.h>

int main(void)
{ /* work out how to identify unique size values. */
//  size_t sizes[] = {1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9};
//  size_t sizes[] = {1, 2, 2, 2, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9};
  size_t sizes[] = {1, 1, 1, 2, 3, 4, 4, 5, 5, 6, 7, 8, 8, 8};
  int i;
  for (i = 0; i < 14; i++) {
    printf("%lu ", sizes[i]);
  }
  printf("\n");
  FILE *fpo = fopen("list", "w");
  int listlen = 14;
  if (sizes[0] != sizes[1]) {
    fprintf(fpo, "%lu ", sizes[0]);
  }
  for (i = 1; i < listlen -1; i++) {
    if ((sizes[i] != sizes[i-1]) && (sizes[i] != sizes[i+1])) {
      fprintf(fpo, "%lu ", sizes[i]);
    }
  } // for()
  if (sizes[listlen-2] != sizes[listlen-1]) {
    fprintf(fpo, "%lu ", sizes[listlen-1]);
  }
  fprintf(fpo, "\n");
  fclose(fpo);
  return 0;
}

