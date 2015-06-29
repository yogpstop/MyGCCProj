CC:=x86_64-w64-mingw32-gcc
CFLAGS:=-Wall -Werror -O3 -march=native -pipe -flto
CLDFLAGS:=-s
MH_SRCS:=$(wildcard minhook/src/*.c minhook/src/HDE/*.c)
MH_OBJS:=$(MH_SRCS:%.c=%.o)
MH_DEPS:=$(MH_SRCS:%.c=%.d)

all: glc.dll glc.exe core.exe dump.exe
glc.dll: apih.o dbg.o $(MH_OBJS)
	$(CC) -shared -o $@ $(CLDFLAGS) $^ -lopengl32
glc.exe: dlli.o vfw.o avi.o dbg.o
	$(CC) -o $@ $(CLDFLAGS) $^ -lvfw32
core.exe: coreaudio.o
	$(CC) -o $@ $(CLDFLAGS) $^ -lole32
dump.exe: dump.o
	$(CC) -o $@ $(CLDFLAGS) $^
.c.o:
	$(CC) -c -MMD -MP -Iminhook/include -o $@ $(CFLAGS) $<
clean:
	rm -f {,minhook/src/{,HDE/}}*.{o,d} *.exe *.dll
.PHONY: clean
