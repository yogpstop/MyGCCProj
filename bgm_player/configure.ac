AC_PREREQ([2.69])
AC_INIT([BGM-Player], [1.0], [github@yogpc.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_SUBST(CFLAGS)
CFLAGS=" -Wall -O3 -march=native"
AC_SUBST(CXXFLAGS)
CXXFLAGS=" -Wall -O3 -march=native"

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C_O

gl_EARLY
gl_INIT

AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile])
AC_OUTPUT
