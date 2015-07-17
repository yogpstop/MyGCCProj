CC := x86_64-w64-mingw32-gcc
CCLD := $(CC)
WRES := x86_64-w64-mingw32-windres
CFLAGS := -Wall -Werror -O3 -march=sandybridge -flto -pipe
LDFLAGS := -s
all: ../dist/flv_norm.exe ../dist/flv_cutter.exe ../dist/wol.exe
-include *.d
../dist/flv_norm.exe: flv_norm.res flv_norm.o flv_core_le64.o
	$(CCLD) -o $@ $(LDFLAGS) $^ -lcomdlg32
../dist/flv_cutter.exe: flv_cutter.o flv_core_le64.o
	$(CCLD) -o $@ $(LDFLAGS) $^
../dist/wol.exe: wol.o
	$(CCLD) -o $@ $(LDFLAGS) $^ -lws2_32
.rc.res:
	$(WRES) -o $@ --input-format=rc --output-format=coff $<
.c.o:
	$(CC) -o $@ -c -MMD -MP -fexec-charset=cp932 $(CFLAGS) $<
clean:
	rm -f *.d *.o *.res ../dist/{wol,flv_{norm,cutter}}.exe
.PHONY: clean
.SUFFIXES: .rc .res
