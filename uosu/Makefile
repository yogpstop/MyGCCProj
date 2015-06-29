CC := gcc
CCLD := $(CC)
CFLAGS := -Wall -Werror -Wno-unused-result -march=native -O3 -pipe -flto
LDFLAGS := -s
PCLIBS := libpng libmpg123 alsa gl x11
CFLAGS += $(shell pkg-config --cflags $(PCLIBS))
LIBS := $(shell pkg-config --libs $(PCLIBS)) -lm -lpthread
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
DEPS := $(SRCS:%.c=%.d)

all: uosu

-include $(DEPS)

uosu: $(OBJS)
	$(CCLD) -o $@ $(LDFLAGS) $^ $(LIBS)
.c.o:
	$(CC) -o $@ -c -MMD -MP $(CFLAGS) $<
clean:
	rm -f *.d *.o uosu

.PHONY: clean
