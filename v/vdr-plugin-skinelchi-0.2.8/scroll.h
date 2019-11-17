/*
 * scroll.h: Scrolling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __SCROLL_H
#define __SCROLL_H

#include "setup.h"
#include <vdr/skins.h>
#include <vdr/tools.h>

class cCharScroll
{
private:
   int Delay;
   cBitmap *bmp;
   cTimeMs Time;
   char *Text;
   tColor Fg;
   tColor Bg;
   int xOff;
   int yOff;
   int DestWidth;
   int Width;
   int Height;
   cOsd *osd;
   int ColorDepth;
   int pos;
   int dir;
   int alignment;
   int delay;
   int intervall;
   int xOffset;
   bool IsDrawNeeded(void);
public:
   cCharScroll(int x, int y, int width, int height, cOsd *Osd, tColor FG = 0xFFFFFFFF, tColor BG = 0xFF000000, int Alignment = taDefault, int Delay = 5);
   ~cCharScroll();
   bool SetText(const char *text, const cFont *Font = NULL, int offset = 0);
   bool DrawIfNeeded(void);
};

#endif //__SCROLL_H
