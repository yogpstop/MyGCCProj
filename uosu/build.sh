#!/bin/bash
CFLAGSR="-march=native -mtune=native -O3 -s -pipe -flto"
CFLAGSG="-O0 -g -ggdb"
PACKS="libpng libmpg123 alsa gl x11"
gcc -Wall -Werror -Wno-unused-result $CFLAGSR `pkg-config --cflags $PACKS` *.c `pkg-config --libs $PACKS` -lm -lpthread
