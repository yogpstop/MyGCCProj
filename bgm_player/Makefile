CC := gcc
CCLD := $(CC)
CFLAGS := -Wall -Werror -march=native -O3 -pipe -flto
LDFLAGS := -s
LIBS := -lFLAC -lmpg123 -lfaad
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
DEPS := $(SRCS:%.c=%.d)

ifeq ($(SYS), mingw)
EXT:=.exe
LDFLAGS+=-static
LIBS+=-lole32
SND:=mmdev
else
LIBS+=-lasound -lpthread
SND:=alsa
endif

all: bgm_player$(EXT)

-include $(DEPS)

bgm_player$(EXT): $(OBJS)
	$(CCLD) -o $@ $(LDFLAGS) $^ $(LIBS)
.c.o:
	$(CC) -DSNDAPI=$(SND) -DSND$(SND) -o $@ -c -MMD -MP $(CFLAGS) $<
clean:
	rm -f *.d *.o bgm_player$(EXT)

.PHONY: clean
