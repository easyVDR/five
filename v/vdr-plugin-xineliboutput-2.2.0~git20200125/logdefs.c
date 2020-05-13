/*
 * logdefs.c: Logging and debug output
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#include "logdefs.h"

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#if defined(__WIN32__)
#  include <windows.h>
#else
#  include <syslog.h>
#  if !defined(__APPLE__) && !defined(__FreeBSD__)
#    include <sys/syscall.h>
#    include <linux/unistd.h> /* syscall(__NR_gettid) */
#  endif
#endif

/* next symbol is dynamically linked from input plugin */
int LogToSysLog __attribute__((visibility("default"))) = 1; /* log to syslog instead of console */

void x_syslog(int level, const char *module, const char *fmt, ...)
{ 
  va_list argp;
  char buf[512];

  va_start(argp, fmt);
  vsnprintf(buf, sizeof(buf), fmt, argp);
  buf[sizeof(buf)-1] = 0;

#if defined(__WIN32__)

  fprintf(stderr, "[%lu] %s%s\n", (unsigned long)GetCurrentThreadId(), module, buf);

#elif !defined(__APPLE__) && !defined(__FreeBSD__)

  if(!LogToSysLog) {
    fprintf(stderr,"[%ld] %s%s\n", (long int)syscall(__NR_gettid), module, buf);
  } else {
    syslog(level, "[%ld] %s%s", (long int)syscall(__NR_gettid), module, buf);
  }

#else

  if(!LogToSysLog) {
    fprintf(stderr, "%s%s\n", module, buf);
  } else {
    syslog(level, "%s%s", module, buf);
  }

#endif

  va_end(argp);
}

