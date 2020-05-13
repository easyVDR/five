/*
 * xine_frontend_lirc.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINE_FRONTEND_LIRC_H
#define XINE_FRONTEND_LIRC_H

struct frontend_s;

typedef struct input_lirc_s input_lirc_t;

input_lirc_t *lirc_start(struct frontend_s *fe, const char *lirc_dev, int repeat_emu, int gui_hotkeys);
void lirc_stop(input_lirc_t **);

#endif /* XINE_FRONTEND_LIRC_H */
