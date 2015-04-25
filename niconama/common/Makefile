CC=gcc
CFLAGS=-Wall -Werror -O3 -march=native -pipe -flto
LD=gcc
LDFLAGS=-s

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)
LIBS=$(shell pkg-config --libs sqlite3 libpcre)

ifeq ($(OS),Windows_NT)
EXEEXT=.exe
LIBS+=-lcrypt32 -lws2_32
endif

CFLAGS+=$(shell pkg-config --cflags sqlite3 libpcre)

all: alert$(EXEEXT) getpublishstatus$(EXEEXT) getplayerstatus$(EXEEXT) editstream$(EXEEXT)

-include $(DEPS)

getpublishstatus$(EXEEXT): getpublishstatus.o xml.o n_socket.o cookie.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
getplayerstatus$(EXEEXT): getplayerstatus.o xml.o n_socket.o cookie.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
editstream$(EXEEXT): editstream.o xml.o n_socket.o cookie.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
alert$(EXEEXT): _alert.o xml.o n_socket.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS)
.c.o:
	$(CC) -c -MMD -MP -o $@ $(CFLAGS) $<
clean:
	rm -f $(OBJS) $(DEPS) {alert,getpublishstatus,getplayerstatus,editstream}$(EXEEXT)
.PHONY: clean
