/*
 * time_ms.h: simple current time interface
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_TIME_MS_H_
#define XINELIBOUTPUT_TIME_MS_H_

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>

static inline uint64_t time_ms(void)
{
  struct timeval t;
#ifdef XINEUTILS_H
  if (xine_monotonic_clock(&t, NULL) == 0)
#else
  if (gettimeofday(&t, NULL) == 0)
#endif
     return ((uint64_t)t.tv_sec) * 1000ULL + t.tv_usec / 1000ULL;
  return 0;
}

static inline uint64_t elapsed(uint64_t t)
{
  return time_ms() - t;
}

#endif /* XINELIBOUTPUT_TIME_MS_H_ */
