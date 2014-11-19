#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([flow2], [2.0.0], [jay-t@gmx.net])
AM_INIT_AUTOMAKE([1.9.6])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([SDL2], [main])
# FIXME: Replace `main' with a function in `-lSDL_gfx':
AC_CHECK_LIB([SDL2_gfx], [main])
# FIXME: Replace `main' with a function in `-lSDLmain':
AC_CHECK_LIB([SD2Lmain], [main])

AC_CHECK_LIB([SDL2_ttf], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset socket])

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
