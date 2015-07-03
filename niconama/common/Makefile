CC=gcc
CFLAGS=-Wall -Werror -O3 -march=native -pipe -flto
CCLD=$(CC)
LDFLAGS=-s

SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
DEPS:=$(SRCS:%.c=%.d)
PCLIBS:=sqlite3 libpcre
ifneq ($(SYS),mingw)
PCLIBS+=libcrypto
endif
LIBS:=$(shell pkg-config --libs $(PCLIBS))
CCFLAGS:=$(CFLAGS) $(shell pkg-config --cflags $(PCLIBS))
LLDFLAGS:=$(LDFLAGS)
ifeq ($(SYS), mingw)
EXT=.exe
LIBS+=-lcrypt32 -lws2_32
LLDFLAGS+=-static
CCFLAGS+=-DPCRE_STATIC
endif

all: alert$(EXT) getpublishstatus$(EXT) editstream$(EXT) comment$(EXT) getthreads$(EXT) fixedhn$(EXT) dl$(EXT)

-include $(DEPS)

getpublishstatus$(EXT): getpublishstatus.o xml.o n_sock.o cookie.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
getplayerstatus$(EXT): getplayerstatus.o xml.o n_sock.o cookie.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
getthreads$(EXT): getthreads.o xml.o n_sock.o cookie.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
editstream$(EXT): editstream.o xml.o n_sock.o cookie.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
alert$(EXT): _alert.o xml.o n_sock.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
comment$(EXT): _comment.o getplayerstatus.o xml.o n_sock.o cookie.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
fixedhn$(EXT): fixedhn.o xml.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
dl$(EXT): dl.o n_sock.o cookie.o
	$(CCLD) -o $@ $(LLDFLAGS) $^ $(LIBS)
.c.o:
	$(CC) -c -MMD -MP -o $@ $(CCFLAGS) $<
clean:
	rm -f $(OBJS) $(DEPS) {alert,comment,getpublishstatus,getplayerstatus,getthreads,editstream,fixedhn,dl}$(EXT)
.PHONY: clean
