/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * compat.h: Compatibility stuff
 *
 * Copyright (C) 2015 Oliver Endriss <o.endriss@gmx.de>
 *
 * cSocketRemote was copied from cSocket (svdrp.h, VDR 2.2.0):
 * Copyright (C) 2000, 2003, 2006, 2008, 2013 Klaus Schmidinger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */

#ifndef __PLUGIN_REMOTE_COMPAT_H
#define __PLUGIN_REMOTE_COMPAT_H

#if VDRVERSNUM >= 20300

class cSocketRemote {
private:
  int port;
  int sock;
  int queue;
  void Close(void);
public:
  cSocketRemote(int Port, int Queue = 1);
  ~cSocketRemote();
  bool Open(void);
  int Accept(void);
  };

#else // VDRVERSNUM >= 20300

#include <vdr/svdrp.h>
typedef cSocket	cSocketRemote;

#endif // VDRVERSNUM >= 20300

#endif //__PLUGIN_REMOTE_COMPAT_H
