#ifndef __AxPluginGlobals_H__
#define __AxPluginGlobals_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   20.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//----- "multiple defs of swap() workaround" ----------------------------------
/*
 * The definition of swap() in $VDRDIR/tools.h collides with the definition
 * of swap in the STL when compiling on a Debian 3.0 / gcc 2.95.4.
 *
 * A (very ugly) workaround is to include a header of the C++ STL before
 * any other headers. So all c++-files of this plugin include this file
 * first and here a stl-header is included first.
 *
 * Sorry, I found no better workaround without changing vdr-sources
 * (and I want to avoid that).
 */
#include <list>

//----- vdr -------------------------------------------------------------------
#include <vdr/osdbase.h>
#include <vdr/config.h>

/** Stay on the PAGE_MARK after changing the state of a flag?
 */
#define STAY_ON_MARK_PAGE 1

#endif


