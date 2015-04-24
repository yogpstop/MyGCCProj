CC=gcc
CFLAGS=-Wall -Werror -O3 -march=native -pipe -flto
LD=gcc
LDFLAGS=-s

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)
LIBS=-lws2_32 -lcrypt32

ifeq ($(OS),Windows_NT)
EXEEXT=.exe
endif

-include $(DEPS)

all: alert$(EXEEXT) getpublishstatus$(EXEEXT) getplayerstatus$(EXEEXT)
getpublishstatus$(EXEEXT): getpublishstatus.o xml.o n_socket.o cookie.o ../libs/sqlite3.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
getplayerstatus$(EXEEXT): getplayerstatus.o xml.o n_socket.o cookie.o ../libs/sqlite3.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
alert$(EXEEXT): _alert.o xml.o n_socket.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
../libs/sqlite3.o: ../libs/sqlite3.c
	$(CC) -c -MMD -MP -I../libs -o $@ $(CFLAGS:-Werror=) $<
.c.o:
	$(CC) -c -MMD -MP -I../libs -o $@ $(CFLAGS) $<
clean:
	rm -f $(OBJS) $(DEPS) alert$(EXEEXT)
.PHONY: clean
