/*
 * Fritz!Box plugin for VDR
 *
 * Copyright (C) 2007-2012 Joachim Wilke <vdr@joachim-wilke.de>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef NOTIFYOSD_H_
#define NOTIFYOSD_H_

#include <string>
#include <vector>
#include <vdr/osdbase.h>
#include "fritzeventhandler.h"

class cNotifyOsd : public cOsdObject
{
private:
	cOsd *osd;
	const cFont *font;
	cFritzEventHandler *event;
	static bool open;
	std::vector<std::string> lines;
	time_t lastUpdate;
public:
	cNotifyOsd(cFritzEventHandler *event);
	virtual ~cNotifyOsd();
	bool GenerateOsdText();
	virtual void Show(void);
	virtual eOSState ProcessKey(eKeys Key);
	static bool isOpen() { return open; }
};

#endif /*NOTIFYOSD_H_*/
