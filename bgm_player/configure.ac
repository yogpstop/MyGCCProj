AC_PREREQ([2.69])
AC_INIT([BGM-Player], [1.0], [github@yogpc.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_ARG_ENABLE(debug, [ --enable-debug], [DEBUG="yes"], [DEBUG="no"])
AM_CONDITIONAL(DEBUG, test $DEBUG = yes)

if test $DEBUG = yes; then
	CFLAGS="-Wall -O0 -ggdb"
	CXXFLAGS="-Wall -O0 -ggdb"
else
	CFLAGS="-Wall -O3 -march=native"
	CXXFLAGS="-Wall -O3 -march=native"
fi

AC_PROG_CC

AC_USE_SYSTEM_EXTENSIONS

AC_CHECK_HEADERS([FLAC/stream_decoder.h alsa/asoundlib.h dirent.h malloc.h poll.h pthread.h signal.h stdint.h stdio.h stdio_ext.h stdlib.h string.h sys/stat.h sys/types.h termios.h time.h unistd.h])

AC_CHECK_LIB([FLAC], [FLAC__stream_decoder_new])
AC_CHECK_LIB([asound], [snd_pcm_open])
AC_CHECK_LIB([pthread], [pthread_create])

AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([getcwd memmove memset strrchr])

gl_EARLY
gl_INIT

AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile])
AC_OUTPUT
