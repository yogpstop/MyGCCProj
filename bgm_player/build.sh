#!/bin/bash
CFLAGSR="-march=native -mtune=native -O3 -s -pipe -flto"
CFLAGSG="-O0 -g -ggdb"
gcc -Wall -Werror $CFLAGSR *.c -lFLAC -lasound -lpthread
