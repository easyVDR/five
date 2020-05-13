/*
 * xvdr_plugin.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef _XVDR_PLUGIN_H_
#define _XVDR_PLUGIN_H_

#include <xine.h>

#if XINE_VERSION_CODE > 10209 || defined(PLUGIN_XINE_MODULE)
void *input_xvdr_init_class (xine_t *xine, const void *data);
void *demux_xvdr_init_class (xine_t *xine, const void *data);
#else
void *input_xvdr_init_class (xine_t *xine, void *data);
void *demux_xvdr_init_class (xine_t *xine, void *data);
#endif


#endif /* _XVDR_PLUGIN_H_ */

