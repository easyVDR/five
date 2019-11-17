/*
 * common.h: Common definitions
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef VDR_SKINELCHI_COMMON_H
#define VDR_SKINELCHI_COMMON_H

#include <vdr/config.h>

#ifdef DEBUG
#	define ISYSLOG(x...)    isyslog(x);
#	define DSYSLOG(x...)    dsyslog(x);
#ifdef DEBUG2
#	define DSYSLOG2(x...)   dsyslog(x);
#else
#	define DSYSLOG2(x...)   ;
#endif
#else
#	define ISYSLOG(x...)    isyslog(x);
#	define DSYSLOG(x...)    ;
#	define DSYSLOG2(x...)   ;
#endif

// Avards Plugin (availability evaluated at runtime)
#include "services/avards_services.h"
#define PLUGIN_AVARDS "avards"

// EPGSearch Plugin (availability evaluated at runtime)
#include "services/epgsearch_services.h"
#define PLUGIN_EPGSEARCH "epgsearch"
#define EPGSEARCH_CONFLICTINFO "Epgsearch-lastconflictinfo-v1.0"

// Mailbox Plugin (availability evaluated at runtime)
#include "services/AxMailBoxServiceTypes.h"
#define PLUGIN_MAILBOX "mailbox"

struct tOSDsize {
  int left, top, width, height;
};

#endif // VDR_SKINELCHI_COMMON_H

