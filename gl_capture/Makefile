CC32:=i686-w64-mingw32-gcc
CCLD32:=$(CC32)
CC64:=x86_64-w64-mingw32-gcc
CCLD64:=$(CC64)
CC:=$(CC64)
CCLD:=$(CCLD64)
CFLAGS:=-Wall -Werror -O3 -march=native -pipe -flto
LDFLAGS:=-s -static-libgcc
MH_SRCS:=$(wildcard minhook/src/*.c minhook/src/HDE/*.c)
MH_OBJS32:=$(MH_SRCS:%.c=%32.o)
MH_OBJS64:=$(MH_SRCS:%.c=%64.o)
MH_DEPS32:=$(MH_SRCS:%.c=%32.d)
MH_DEPS64:=$(MH_SRCS:%.c=%64.d)

all: hook32.dll hook64.dll hook32.exe hook64.exe glc.exe core.exe dump.exe

-include $(MH_DEPS32) $(MH_DEPS64)

hook32.dll: hook32.o hook_ogl32.o hook_d3d932.o dbg32.o $(MH_OBJS32)
	$(CCLD32) -shared -o $@ $(LDFLAGS) $^
hook64.dll: hook64.o hook_ogl64.o hook_d3d964.o dbg64.o $(MH_OBJS64)
	$(CCLD64) -shared -o $@ $(LDFLAGS) $^
hook32.exe: injector32.o
	$(CCLD32) -o $@ $(LDFLAGS) $^
hook64.exe: injector64.o
	$(CCLD64) -o $@ $(LDFLAGS) $^
glc.exe: main.o vfw.o avi.o dbg.o
	$(CCLD) -o $@ $(LDFLAGS) $^ -lvfw32
core.exe: coreaudio.o
	$(CCLD) -o $@ $(LDFLAGS) $^ -lole32
dump.exe: dump.o
	$(CCLD) -o $@ $(LDFLAGS) $^
.c.o:
	$(CC) -c -MMD -MP -Iminhook/include -o $@ $(CFLAGS) $<
.c32.o:
	$(CC32) -c -MMD -MP -Iminhook/include -o $@ $(CFLAGS) $<
.c64.o:
	$(CC64) -c -MMD -MP -Iminhook/include -o $@ $(CFLAGS) $<
clean:
	rm -f {,minhook/src/{,HDE/}}*.{o,d} *.exe *.dll
.PHONY: clean
.SUFFIXES: 32.o 64.o
