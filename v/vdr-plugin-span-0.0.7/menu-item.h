/*
 * menu-item.h: A menu item
 */
 
/*
 * Spectrum Analyzer plugin for VDR (C++)
 *
 * (C) 2006 Christian Leuschen <christian.leuschen@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _MENU_ITEM__H
#define _MENU_ITEM__H

#include <vdr/osdbase.h>

#include "defines.h"
#include "menu.h"

enum eSpanOsdItem
{
	HEADING,
	PROVIDER,
	CLIENT
};

// cSpanOsdItem ##################################################################
class cSpanOsdItem : public cOsdItem
{
protected:
	eSpanOsdItem m_item;
private:
	bool isActive;
	bool isRunning;
public:
	cSpanOsdItem(){ isActive = false; isRunning = false; };
	cSpanOsdItem(const char* text, eSpanOsdItem item, bool sIsActive=false, bool sIsRunning=false);
	virtual ~cSpanOsdItem(void);
};

#endif //_MENU_ITEM__H
