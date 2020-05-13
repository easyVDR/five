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

typedef struct input_kbd_s input_kbd_t;

input_kbd_t *kbd_start(struct frontend_s *fe, int slave_mode, int gui_hotkeys);
void kbd_stop(input_kbd_t **);

#endif /* XINE_FRONTEND_KBD_H */
