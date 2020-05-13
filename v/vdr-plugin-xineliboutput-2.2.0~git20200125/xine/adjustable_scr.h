/*
 * adjustable_scr.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_ADJUSTABLE_SCR_H_
#define XINELIBOUTPUT_ADJUSTABLE_SCR_H_

#include <stdint.h>

#include <xine/metronom.h>

/******************************* SCR *************************************
 *
 * unix System Clock Reference + fine tuning
 *
 * fine tuning is used to change playback speed in live mode
 * to keep in sync with mpeg source
 *************************************************************************/

typedef struct adjustable_scr_s adjustable_scr_t;

struct adjustable_scr_s {
  scr_plugin_t scr;

  void (*set_speed_tuning)(adjustable_scr_t *this, double factor);
  void (*set_speed_base)  (adjustable_scr_t *this, int hz);
  void (*jump)            (adjustable_scr_t *this, int pts);

  void (*set_buffering)   (adjustable_scr_t *this, int on);
  void (*got_pcr)         (adjustable_scr_t *this, int64_t pcr);

  void (*dispose)         (adjustable_scr_t *this);
};

adjustable_scr_t *adjustable_scr_start (xine_t *xine);


#endif /* XINELIBOUTPUT_ADJUSTABLE_SCR_H_ */
