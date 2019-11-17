/*
 * xine_frontend_kbd.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINE_FRONTEND_KBD_H
#define XINE_FRONTEND_KBD_H

struct frontend_s;

void kbd_start(struct frontend_s *fe, int slave_mode);
void kbd_stop(void);

#endif /* XINE_FRONTEND_KBD_H */
