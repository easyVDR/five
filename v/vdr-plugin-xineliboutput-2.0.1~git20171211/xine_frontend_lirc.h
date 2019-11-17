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

void lirc_start(struct frontend_s *fe, const char *lirc_dev, int repeat_emu);
void lirc_stop(void);

#endif /* XINE_FRONTEND_LIRC_H */
