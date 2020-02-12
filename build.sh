#!/usr/bin/env bash
#
# build.sh - script to build a group of programs.
#
# Copyright 2020 Robert L (Bob) Parker rlp1938@gmail.com
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.# See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#
#gcc -Wall -Wextra -g -O0 -c -D_GNU_SOURCE=1 avltree.c
#gcc -Wall -Wextra -g -O0 -c -D_GNU_SOURCE=1 testtree.c
#gcc avltree.o testtree.o -o testtree

#gcc -Wall -Wextra -g gnutree.c -o gnutree
#gcc -Wall -Wextra -g dups.c -lmhash -o dups
gcc -Wall -Wextra -O0 -g -c newdups.c
gcc -Wall -Wextra -O0 -g -c calcmd5.c
gcc newdups.o calcmd5.o -lmhash -o dups


#gcc -Wall -Wextra -g -O0 -c testmd5.c
#gcc -Wall -Wextra -g -O0 -c calcmd5.c
#gcc testmd5.o calcmd5.o -lmhash -o testmd5
rm *.o
#clear
#ls -tlh

