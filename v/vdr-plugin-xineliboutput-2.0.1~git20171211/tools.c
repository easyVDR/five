/*
 * tools.c: VDR/C++ wrapper for common tools
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <vdr/tools.h>
#include "logdefs.h"

//#include "tools/vdrdiscovery.c"

#include "tools/pes.c"
#include "tools/mpeg.c"
#include "tools/h264.c"
#include "tools/h265.c"
#include "tools/ts.c"
