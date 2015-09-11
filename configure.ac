#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.13])
AC_INIT([nanovm], [3.3.3], [jay-t@gmx.net])
AM_INIT_AUTOMAKE([1.9.6])
# AM_GNU_GETTEXT_VERSION([0.19.4])
# AM_GNU_GETTEXT([external])
IT_PROG_INTLTOOL(0.50.2)

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h malloc.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gethostbyaddr gethostbyname gethostname inet_ntoa memchr memset pow socket sqrt strchr])

AC_CONFIG_FILES([Makefile po/Makefile.in
                 assemb/Makefile
                 comp/Makefile
                 include/Makefile
                 lib/Makefile
                 m4/Makefile
                 vm/Makefile])
AC_OUTPUT
