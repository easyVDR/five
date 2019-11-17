/*
 * scroll.c: Scrolling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "common.h"
#include "scroll.h"

cCharScroll::cCharScroll(int x, int y, int width, int height, cOsd *Osd, tColor FG, tColor BG, int Alignment, int inDelay)
{
   DSYSLOG("skinelchi: cCharScroll::cCharScroll(x=%d,y=%d,w=%d)", x, y, width)
   //isyslog("skinelchi: cCharScroll::cCharScroll(x=%d,y=%d,w=%d)", x, y, width);
   xOff = x;
   yOff = y;
   osd = Osd;
   Text = NULL;
   bmp = NULL;
   Width = width;
   Height = height;
   DestWidth = Width;
   ColorDepth = 1;
   Bg = BG;
   Fg = FG;
   alignment = Alignment;
   delay = inDelay;
   intervall = 20;
   xOffset = 0;
   dir = 0;
}

cCharScroll::~cCharScroll()
{
   free(Text);
   DELETENULL(bmp);
}

bool cCharScroll::SetText(const char *text, const cFont *Font, int offset)
{
   DSYSLOG("skinelchi: cCharScroll::SetText(%s,%d)", text?text:"NULL", offset)
   //isyslog("skinelchi: cCharScroll::SetText(\"%s\",%d)", text?text:"NULL", offset);

   xOffset = offset;
   dir = 0;
   DestWidth = Width - xOffset;

   //isyslog("skinelchi-scroll: text=%0x Text=\"%s\"", text, Text?Text:"-");
   if (!text || isempty(text)) { // clear area if no text is given
      osd->DrawRectangle(xOff + xOffset, yOff, xOff + DestWidth, yOff - 1 + Height, Bg);
      return false;
   }
   else { // text is given, draw it
      //take changes only if string differs to avoid flickering if already scrolling
      //no check possible: in DisplayChannel stations with same event would stay clear

      //if (Text && !strcmp(Text, text)) return false; // RC not neccessarily correct! (but currently unused)
      free(Text);
      Text = strdup(text);
      osd->DrawText(xOff + xOffset, yOff, Text, Fg, Bg, Font, DestWidth, Font->Height(Text), alignment);
   }

   if (!DestWidth || !ElchiConfig.useScrolling)
      return false;

   int textwidth = Font->Width(Text);
   //isyslog("skinelchi: cCharScroll::SetText(%s,%d):%d", text?text:"NULL", offset, textwidth > DestWidth);
   if (textwidth > DestWidth) {
      pos = 0;
      dir = -1;
      Time.Set();

      int textheight = Font->Height();
      if (bmp)
         bmp->SetSize(textwidth, textheight);
      else
         bmp = new cBitmap(textwidth, textheight, ColorDepth);
      bmp->DrawText(0, 0, Text, Fg, Bg, Font);

      Delay = delay;
      return true;
   }

   return false;
}


bool cCharScroll::IsDrawNeeded(void)
{
   if (bmp) {
      int elapsed = ((int)Time.Elapsed()) / intervall;
      ///isyslog("skinelchi IsDrawNeeded: %d", elapsed);
      if (elapsed) {
         if (Delay > 0) {
            int x = elapsed / Delay;
            Delay -= elapsed;
            if (Delay <= 0)
               pos -= dir * x;
            else {
               Time.Set();
               return false;
            }
         }
         else
            pos -= dir * elapsed;

         if (pos < 0) {
            pos = 0;
            dir = -1;
         }
         else {
            int x = bmp->Width() - DestWidth;
            if (pos > x) {
               pos = x;
               dir = 1;
            }
         }
         int x = bmp->Width() - DestWidth - pos;
         Delay = delay - ((x > pos) ? pos : x);
         Time.Set();
         return true;
      }
   }
   return false;
}


bool cCharScroll::DrawIfNeeded(void)
{
   DSYSLOG("skinelchi: cCharScroll::DrawIfNeeded")
   //isyslog("skinelchi: cCharScroll::DrawIfNeeded");

   bool changed = false;
   if (dir && (changed = IsDrawNeeded())) {
      for (int i = 0; i < DestWidth; i++) {
         for (int j = 0; j < bmp->Height(); j++) {
            osd->DrawPixel(xOff + xOffset + i, yOff + j, bmp->Color(*(bmp->Data(i + pos, j))));
         }
      }
   }

   //isyslog("skinelchi: cCharScroll::DrawIfNeeded: %s", changed?"yes":"no");
   return changed;
}
