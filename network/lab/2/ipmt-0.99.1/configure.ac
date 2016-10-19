#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# $Id: configure.ac 87 2009-01-21 13:36:58Z gberger $

AC_PREREQ(2.59)
AC_INIT(ipmt, 0.99.1, Gilles.Berger-Sabbatel@imag.fr)
AM_INIT_AUTOMAKE([foreign])
#color-tests
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_C_CONST

# Checks for libraries.

AC_CHECK_LIB(socket, socket)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h netdb.h netinet/in.h netinet/tcp.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h memory.h, sys/select.h])

AC_CONFIG_MACRO_DIR([m4])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_SOCKLEN_T
AC_TYPE_IN_PORT_T
AC_DECL_H_ERRNO

AC_PROG_YACC
AM_PROG_LEX

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gettimeofday socket strerror strtol gai_strerror memcpy])
#AC_REPLACE_FUNCS(memset)

AC_CHECK_FUNCS(getaddrinfo getnameinfo)
if test "$ac_cv_func_getaddrinfo$ac_cv_func_getnameinfo" != yesyes ; then
    AC_LIBOBJ([getaddrinfo])
    AC_LIBSOURCE(getaddrinfo.h)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
