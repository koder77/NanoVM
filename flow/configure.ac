#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([flow], [0.5.5], [jay-t@gmx.net])
AM_INIT_AUTOMAKE([1.9.6])

AC_CONFIG_SRCDIR([mem.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lSDL':
AC_CHECK_LIB([SDL], [main])
# FIXME: Replace `main' with a function in `-lSDL_gfx':
AC_CHECK_LIB([SDL_gfx], [main])
# FIXME: Replace `main' with a function in `-lSDL_image':
AC_CHECK_LIB([SDL_image], [main])
# FIXME: Replace `main' with a function in `-lSDL_net':
AC_CHECK_LIB([SDL_net], [main])
# FIXME: Replace `main' with a function in `-lSDL_ttf':
AC_CHECK_LIB([SDL_ttf], [main])
# FIXME: Replace `main' with a function in `-lSDL_mixer':
AC_CHECK_LIB([SDL_mixer], [main])
# FIXME: Replace `main' with a function in `-lSDLmain':
AC_CHECK_LIB([SDLmain], [main])


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/ioctl.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
