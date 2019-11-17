#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# This was initialy created by autoscan.
#

AC_PREREQ(2.69)

# The package version, when autotool files are regenerated
m4_define([VERSION_NUMBER],[m4_esyscmd([./getVersion.sh])])
AC_INIT(yard2tools, m4_defn([VERSION_NUMBER]), yard@rg-software.de)

AC_CONFIG_SRCDIR(yard2srvd.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(foreign)

# rewrite version with version from ChangeLog
VERSION=$([[ -f ./getVersion.sh ]] && ./getVersion.sh; [[ -f ../getVersion.sh ]] && ../getVersion.sh)
PACKAGE_VERSION="${VERSION}"
PACKAGE_STRING="yard2tools ${VERSION}"

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_CPP
AC_PROG_MKDIR_P

AC_CHECK_PROG(PROG_GIT,git,yes,no)
if test "${PROG_GIT}" = "no" ; then
    AC_MSG_WARN([git not installed, using date/time.])
fi
AM_CONDITIONAL(PROG_GIT, [test "${PROG_GIT}" = "yes"])

# Checks for libraries.
AC_CHECK_LIB(rt, aio_cancel)
AC_CHECK_LIB(usb, usb_close, ,[AC_MSG_ERROR([Couldn't find libusb... try to install libusb-dev])])
AC_CHECK_LIB(bsd, pidfile_open, ,[AC_MSG_ERROR([Couldn't find libbsd... try to install libbsd-dev])])

# Checks for header files; Specific for this package
AC_CHECK_HEADERS(usb.h, ,[AC_MSG_ERROR([Couldn't find usb.h... try to install libusb-dev])])

# Checks for header files; Specific for this package
AC_CHECK_HEADERS(bsd/libutil.h, ,[AC_MSG_ERROR([Couldn't find libutil.h... try to install libbsd-dev])])

# Checks for header files; normaly available on every system
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(limits.h)
AC_CHECK_HEADERS(netinet/in.h)
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(sys/socket.h)
AC_CHECK_HEADERS(sys/time.h)
AC_CHECK_HEADERS(sys/stat.h)
AC_CHECK_HEADERS(termios.h)
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(getopt.h)
AC_CHECK_HEADERS(stdarg.h)
AC_CHECK_HEADERS(syslog.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS(bzero)
AC_CHECK_FUNCS(clock_gettime)
AC_CHECK_FUNCS(getdelim)
AC_CHECK_FUNCS(gettimeofday)
AC_CHECK_FUNCS(memset)
AC_CHECK_FUNCS(mkdir)
AC_CHECK_FUNCS(select)
AC_CHECK_FUNCS(socket)
AC_CHECK_FUNCS(strchr)
AC_CHECK_FUNCS(strerror)
AC_CHECK_FUNCS(strspn)
AC_CHECK_FUNCS(strstr)
AC_CHECK_FUNCS(strtol)
AC_CHECK_FUNCS(strtoull)

# Debug option
AC_ARG_WITH(debug,
            [  --with-debug=ARG        type debug [[yes,no]]],
            [DEBUG="$withval"],[DEBUG="no"])
AC_SUBST(DEBUG)

if test "$DEBUG" = "yes" ; then
   CPPFLAGS="$CPPFLAGS -DDEBUG=1"
fi

AC_CONFIG_FILES(
  Makefile
  yard2flash/Makefile
  addon/Makefile
)
AC_OUTPUT
