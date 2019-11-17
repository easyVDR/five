/*
 * bitmap.h: bitmap handling
 *
 * Copyright (C) 2004        Andreas Brugger
 * Copyright (C) 2005 - 2006 _Frank_ @ vdrportal.de
 * Copyright (C) 2007 - 2008 Christoph Haubrich
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
 *
 * See the README file how to reach the author.
 *
 * $Id$
 */
//changed original from bitmap.h from osdimage plugin by Andreas Brugger

#ifndef _OSDIMAGE_BITMAP_H_
#define _OSDIMAGE_BITMAP_H_

#define X_DISPLAY_MISSING

#include <Magick++.h>
#include "config.h"
#include <vdr/osd.h>
#include <vdr/skins.h>

using namespace Magick;

class cOSDImageBitmap
{
public:
   cOSDImageBitmap();
   ~cOSDImageBitmap();
   bool LoadZoomed(const char *file, int zoomWidth, int zoomHeight, int zoomLeft, int zoomTop);
   void Render(cBitmap &bmp, int wWindow, int hWindow, int colors, bool dither);
   inline int Width() { return width; }
   inline int Height() { return height; }

private:
   bool LoadImageMagick(Image &imgkLoad, const char *file);
   void QuantizeImageMagick(Image &imgkQuant, int colors, bool dither);
   void ConvertImgk2Bmp(cBitmap &bmp, Image &imgkConv, int colors);
   Image imgkZoom;
   int ZoomWidth, ZoomHeight, ZoomLeft, ZoomTop;
   int origWidth, origHeight;
   bool loadingFailed;
   int width, height;
};

#endif
