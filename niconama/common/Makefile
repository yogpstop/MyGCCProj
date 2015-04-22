CC=gcc
CFLAGS=-Wall -Werror -O3 -march=native -pipe -flto
LD=gcc
LDFLAGS=-s

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)
LIBS=-lws2_32

ifeq ($(OS),Windows_NT)
EXEEXT=.exe
endif

-include $(DEPS)

all: alert$(EXEEXT)
alert$(EXEEXT): _alert.o xml.o n_socket.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
.c.o:
	$(CC) -c -MMD -MP -o $@ $(CFLAGS) $<
clean:
	rm -f $(OBJS) $(DEPS) alert$(EXEEXT)
.PHONY: clean
