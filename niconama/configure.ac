#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Nico-Cross-Streamer], [1.0], [github@yogpc.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([common/coreaudio.cpp])
AC_CONFIG_HEADERS([config.h])

AC_SUBST(CFLAGS)
CFLAGS="-Wall -Werror -O3 -march=native"
AC_SUBST(CXXFLAGS)
CXXFLAGS="-Wall -Werror -O3 -march=native"

AC_SUBST(WIN_LIBS)
AC_CANONICAL_HOST
AS_CASE([$host_os],
	[*mingw* | *cygwin*], [WIN_LIBS="-lws2_32 -lwinmm -lgdi32"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h malloc.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset socket strchr])

PKG_CHECK_MODULES([MAIN], [gtk+-3.0 sqlite3 fdk-aac librtmp])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
