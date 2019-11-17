/*
 * bitmap.c: bitmap handling
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
//changed original from bitmap.c from osdimage plugin by Andreas Brugger

#include "bitmap.h"

using namespace std; //need ???
using namespace Magick;


cOSDImageBitmap::cOSDImageBitmap() {
}


cOSDImageBitmap::~cOSDImageBitmap() {
}

bool cOSDImageBitmap::LoadZoomed(const char *file, int zoomWidth, int zoomHeight, int zoomLeft, int zoomTop) {
   bool status;
   status = LoadImageMagick(imgkZoom, file);
   if (zoomWidth != 0)
      imgkZoom.crop(Geometry(zoomWidth, zoomHeight, zoomLeft, zoomTop));
   height = imgkZoom.rows();
   width = imgkZoom.columns();
   return status;
}

void cOSDImageBitmap::Render(cBitmap &bmp, int wWindow, int hWindow, int colors, bool dither) {
   int w = wWindow;
   int h = hWindow;
   int wNew, hNew;
   wNew = wWindow;
   hNew = hWindow;
   if (!loadingFailed) {
      Image imgkRender = imgkZoom;
      width = imgkRender.columns();
      height = imgkRender.rows();
      if (height != h || width != w) {
         switch (ElchiConfig.ImgResizeAlgo) {
               case 0:
                  imgkRender.sample(Geometry(wNew, hNew, 0, 0) );
                  break;
               case 1:
                  imgkRender.scale(Geometry(wNew, hNew, 0, 0) );
                  break;
               case 2:
                  imgkRender.zoom(Geometry(wNew, hNew, 0, 0) );
         }
         width = imgkRender.columns();
         height = imgkRender.rows();
         //if (colors == 16 && (height != wWindow || width != hWindow))
         //   colors = 15;
         if (height != wWindow || width != hWindow)
            colors -= 1;
      }
      QuantizeImageMagick(imgkRender, colors, dither);
      ConvertImgk2Bmp(bmp, imgkRender, colors);
   } else { // loading failed: print Error message
      bmp.SetSize(w, h);
      bmp.SetBpp((colors <= 16) ? 4 : 8);
      width = w;
      height = h;
      const cFont *font = cFont::GetFont(fontSml);
      int smalllineHeight = font->Height();
      bmp.DrawRectangle(0, 0, width - 1, height - 1, Theme.Color(clrBackground));
      bmp.DrawText(0, h - smalllineHeight + 1, tr("Error"), Theme.Color(clrButtonRedBg), Theme.Color(clrBackground), font, w, smalllineHeight, taCenter);
   }
}

bool cOSDImageBitmap::LoadImageMagick(Image &imgkLoad, const char *file) {
   try {
      imgkLoad.read(file);
      if (imgkLoad.fileSize() == 0) {
         loadingFailed = true;
         return false;
      } else {
         height = imgkLoad.baseRows();
         width = imgkLoad.baseColumns();
         origWidth = width;
         origHeight = height;
         loadingFailed = false;
         return true;
      }
   }

   catch(exception &error)
   {
      loadingFailed = true;
      return false;
   }
}

void cOSDImageBitmap::QuantizeImageMagick(Image &imgkQuant, int colors, bool dither) {
   imgkQuant.quantizeColors(colors);
   imgkQuant.quantizeDither(dither);
   imgkQuant.quantize();
}

void cOSDImageBitmap::ConvertImgk2Bmp(cBitmap &bmp, Image &imgkConv, int colors) {
   int w = Width();
   int h = Height();
   tColor col;
   if ((w < 120) && (h < 90)) { // smaller than image => must be channel logo ... !?!??
      int x = 64;
      int y = 48;
      //if (ElchiConfig.logousemagick) {
      //   x = ElchiConfig.logox;
      //   y = ElchiConfig.logoy;
      //}
      //imgkConv.write("/vdr/logo.xpm");
      bmp.SetSize(x, y);
      int xoff = (x - w) / 2;
      int yoff = (y - h) / 2;
      bmp.SetBpp((colors <= 16) ? 4 : 8);
      const PixelPacket *pixels = imgkConv.getConstPixels(0, 0, w, h);
      col = (0xFF << 24)
            | ( (pixels->green * 255 / MaxRGB) << 8)
            | ( (pixels->red * 255 / MaxRGB) << 16)
            | ( (pixels->blue * 255 / MaxRGB) );
      for (int iy = 0; iy < yoff; iy++)
         for (int ix = 0; ix < x; ix++)
            bmp.DrawPixel(ix, iy, col);
      for (int iy = yoff; iy < (h + yoff); iy++)
         for (int ix = 0; ix < xoff; ix++)
            bmp.DrawPixel(ix, iy, col);
      for (int iy = yoff; iy < (h + yoff); iy++)
         for (int ix = w + xoff; ix < x; ix++)
            bmp.DrawPixel(ix, iy, col);
      for (int iy = yoff + h; iy < y; iy++)
         for (int ix = 0; ix < x; ix++)
            bmp.DrawPixel(ix, iy, col);
      for (int iy = yoff; iy < (h + yoff); iy++) {
         for (int ix = xoff; ix < (w + xoff); ix++) {
            col = (0xFF << 24)
               | ( (pixels->green * 255 / MaxRGB) << 8)
               | ( (pixels->red * 255 / MaxRGB) << 16)
               | ( (pixels->blue * 255 / MaxRGB) );
            bmp.DrawPixel(ix, iy, col);
            pixels++;
         }
      }
   } else { // not a channel logo
      //imgkConv.write("/tmp/test.xpm");
      
      bmp.SetSize(w, h);
      bmp.SetBpp((colors <= 16) ? 4 : 8);
      const PixelPacket *pixels = imgkConv.getConstPixels(0, 0, w, h);
      for (int iy = 0; iy < h; iy++) {
         for (int ix = 0; ix < w; ix++) {
            col = (0xFF << 24)
               | ( (pixels->green * 255 / MaxRGB) << 8)
               | ( (pixels->red * 255 / MaxRGB) << 16)
               | ( (pixels->blue * 255 / MaxRGB) );
            bmp.DrawPixel(ix, iy, col);
            pixels++;
         }
      }
   }
}

