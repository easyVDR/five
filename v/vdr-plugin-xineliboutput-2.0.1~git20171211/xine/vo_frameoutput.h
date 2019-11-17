/*
 * vo_frameoutput.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef _XINELIBOUTPUT_VO_FRAMEOUTPUT_H
#define _XINELIBOUTPUT_VO_FRAMEOUTPUT_H

vo_driver_t *vo_frameoutput_init(void *handle, void (*cb)(void*, vo_frame_t*));

#endif /* _XINELIBOUTPUT_VO_FRAMEOUTPUT_H */
