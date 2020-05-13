/*
 * endian_compat.h
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_ENDIAN_H_
#define XINELIBOUTPUT_ENDIAN_H_

#if defined(__APPLE__) || defined (__FreeBSD__)
#  include <machine/endian.h>
#elif defined(_WIN32)
#  include <sys/param.h>
#else
# include <endian.h>
#endif

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
#elif defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
#elif defined(BYTE_ORDER) && BYTE_ORDER == LITTLE_ENDIAN
#else
#  error __BYTE_ORDER not defined
#endif

#if ((defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) || (defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN))
#  define XINELIBOUTPUT_BIG_ENDIAN 1
#else
#  define XINELIBOUTPUT_BIG_ENDIAN 0
#endif

#endif // XINELIBOUTPUT_ENDIAN_H_
