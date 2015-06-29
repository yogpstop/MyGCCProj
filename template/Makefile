NAME := win_gui
CC := gcc
CCLD := $(CC)
WRES := windres
CFLAGS := -Wall -Werror -O3 -march=native -flto -pipe
LDFLAGS := -s
all: ../dist/$(NAME).exe
-include *.d
../dist/$(NAME).exe: $(NAME).res $(NAME).o
	$(CCLD) -o $@ -mwindows $(LDFLAGS) $^
.rc.res:
	$(WRES) -o $@ --input-format=rc --output-format=coff $<
.c.o:
	$(CC) -o $@ -c -MMD -MP $(CFLAGS) $<
clean:
	rm -f *.d *.o *.res ../dist/$(NAME).exe
.PHONY: clean
.SUFFIXES: .rc .res
