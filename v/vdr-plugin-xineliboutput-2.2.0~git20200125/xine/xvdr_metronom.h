/*
 * xvdr_metronom.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XVDR_METRONOM_H
#define XVDR_METRONOM_H

#include <xine/metronom.h>

#ifdef METRONOM_INTERNAL
#  error METRONOM_INTERNAL defined, struct xvdr_metronom_s size will be incorrect
#endif

#define XVDR_METRONOM_OPTION_BASE  0x1001
#define XVDR_METRONOM_LAST_VO_PTS  (XVDR_METRONOM_OPTION_BASE)
#define XVDR_METRONOM_TRICK_SPEED  (XVDR_METRONOM_OPTION_BASE + 1)
#define XVDR_METRONOM_STILL_MODE   (XVDR_METRONOM_OPTION_BASE + 2)
#define XVDR_METRONOM_ID           (XVDR_METRONOM_OPTION_BASE + 3)

#define XVDR_METRONOM_LIVE_BUFFERING   (XVDR_METRONOM_OPTION_BASE + 4)
#define XVDR_METRONOM_STREAM_START     (XVDR_METRONOM_OPTION_BASE + 5)


typedef struct xvdr_metronom_s xvdr_metronom_t;

struct adjustable_scr_s;

struct xvdr_metronom_s {
  /* xine-lib metronom interface */
  metronom_t     metronom;

  /* management interface */
  void (*dispose)     (xvdr_metronom_t *);

  void (*wire)          (xvdr_metronom_t *);
  void (*unwire)        (xvdr_metronom_t *);
};

xvdr_metronom_t *xvdr_metronom_init(xine_stream_t *);


#endif /* XVDR_METRONOM_H */
