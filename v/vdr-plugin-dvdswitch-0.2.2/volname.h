/*
 * dvdswitch plugin for VDR (C++)
 *
 * (C) 2010 Andreas Brachold <vdr07 AT deltab de>
 *
 * This dvdswitch plugin is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 2 of the License.
 *
 * See the files README and COPYING for details.
 *
 */

#ifndef __VOLNAME_DVDSWITCH_H
#define __VOLNAME_DVDSWITCH_H

int volname(const char* device, char* volname, size_t volsize);

#endif // __VOLNAME_DVDSWITCH_H
