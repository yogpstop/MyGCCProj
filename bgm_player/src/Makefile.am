bin_PROGRAMS=../BGM_Player
___BGM_Player_CPPFLAGS=$(MAIN_CFLAGS)
___BGM_Player_LDADD=$(MAIN_LIBS) $(top_builddir)/lib/libgnu.a \
                    -lasound -lFLAC -lpthread
___BGM_Player_LDFLAGS=-s
___BGM_Player_SOURCES=flac.c \
                      riff.c \
                      buffer.c \
                      playlist.c \
                      playback.c \
                      stdio.c \
                      main.c
