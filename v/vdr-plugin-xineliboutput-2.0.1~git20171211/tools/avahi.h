/*
 * avahi.h: mDNS announce
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_AVAHI_H_
#define XINELIBOUTPUT_AVAHI_H_

void x_avahi_stop(void *h);
void *x_avahi_start(int port, int rtsp, int http);

#endif /* XINELIBOUTPUT_AVAHI_H_ */
