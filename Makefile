CC:=gcc
CCLD:=$(CC)

ifeq ($(SYS), mingw)
EXT:=.exe
endif

all: dist/dummy$(EXT)
dist/dummy$(EXT): src/dummy.c
	$(CCLD) -o $@ -O3 -s -ffreestanding -nostdlib $^
clean:
	rm -f dist/dummy$(EXT)
