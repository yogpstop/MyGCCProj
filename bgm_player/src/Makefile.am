bin_PROGRAMS=../BGM_Player
___BGM_Player_LDADD=$(top_builddir)/lib/libgnu.a
if !DEBUG
___BGM_Player_LDFLAGS=-s
endif
___BGM_Player_SOURCES=flac.c \
                      riff.c \
                      buffer.c \
                      playlist.c \
                      playback.c \
                      stdio.c \
                      main.c
