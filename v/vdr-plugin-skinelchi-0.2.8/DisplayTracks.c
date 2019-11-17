/*
 * DisplayTracks.c: Audio track display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <vdr/device.h>

#include "DisplayTracks.h"
#include "setup.h"
#include "common.h"

#include "symbols/576/audioleft.xpm"
#include "symbols/576/audioright.xpm"
#include "symbols/576/audiostereo.xpm"
#include "symbols/576/arrowdown.xpm"
#include "symbols/576/arrowup.xpm"

#include "symbols/720/Baudioleft.xpm"
#include "symbols/720/Baudioright.xpm"
#include "symbols/720/Baudiostereo.xpm"
#include "symbols/720/Barrowdown.xpm"
#include "symbols/720/Barrowup.xpm"

#include "symbols/1080/Caudioleft.xpm"
#include "symbols/1080/Caudioright.xpm"
#include "symbols/1080/Caudiostereo.xpm"
#include "symbols/1080/Carrowdown.xpm"
#include "symbols/1080/Carrowup.xpm"

// --- cSkinElchiDisplayTracks ---------------------------------------------
cBitmap cSkinElchiDisplayTracks::bmAAudioLeft(audioleft_xpm);
cBitmap cSkinElchiDisplayTracks::bmAAudioRight(audioright_xpm);
cBitmap cSkinElchiDisplayTracks::bmAAudioStereo(audiostereo_xpm);
cBitmap cSkinElchiDisplayTracks::bmAArrowDown(arrowdown_xpm);
cBitmap cSkinElchiDisplayTracks::bmAArrowUp(arrowup_xpm);

cBitmap cSkinElchiDisplayTracks::bmBAudioLeft(audioleft_xpm);
cBitmap cSkinElchiDisplayTracks::bmBAudioRight(audioright_xpm);
cBitmap cSkinElchiDisplayTracks::bmBAudioStereo(audiostereo_xpm);
cBitmap cSkinElchiDisplayTracks::bmBArrowDown(arrowdown_xpm);
cBitmap cSkinElchiDisplayTracks::bmBArrowUp(arrowup_xpm);

cBitmap cSkinElchiDisplayTracks::bmCAudioLeft(Caudioleft_xpm);
cBitmap cSkinElchiDisplayTracks::bmCAudioRight(Caudioright_xpm);
cBitmap cSkinElchiDisplayTracks::bmCAudioStereo(Caudiostereo_xpm);
cBitmap cSkinElchiDisplayTracks::bmCArrowDown(Carrowdown_xpm);
cBitmap cSkinElchiDisplayTracks::bmCArrowUp(Carrowup_xpm);

cSkinElchiDisplayTracks::cSkinElchiDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
   showMessage = changed = false;
   const cFont *font = cFont::GetFont(fontOsd);
   const cFont *smallfont = cFont::GetFont(fontSml);
   lh = font->Height();
   lhh = lh / 2;
   currentIndex = -1;
   index = -1;
   offset = 0;
   numTracks = NumTracks;
   int ItemsWidth = font->Width(Title);
   for (int i = 0; i < NumTracks; i++)
      ItemsWidth = max(ItemsWidth, font->Width(Tracks[i]));
   ItemsWidth += 10;
   x0 = 0;
   x1 = x0 + lhh;

   tOSDsize OSDsize;

   OSDsize.left   = cOsd::OsdLeft();
   OSDsize.top    = cOsd::OsdTop();
   OSDsize.width  = cOsd::OsdWidth();
   OSDsize.height = cOsd::OsdHeight();

   int OSDHeight = 576;
   int OSDWidth = 720;
   double OSDAspect = 4.0/3.0;
#if VDRVERSNUM >= 10708   
   cDevice::PrimaryDevice()->GetOsdSize(OSDWidth, OSDHeight, OSDAspect);
#endif

   DSYSLOG("skinelchi: OSDsize Tracks %dx%d left=%d/%d top=%d/%d width=%d/%d heigth=%d/%d",
           OSDWidth, OSDHeight, OSDsize.left, Setup.OSDLeft, OSDsize.top, Setup.OSDTop, OSDsize.width, Setup.OSDWidth, OSDsize.height, Setup.OSDHeight);

   if (OSDHeight >= 1080) {
      bmAudioLeft   = &bmCAudioLeft;
      bmAudioRight  = &bmCAudioRight;
      bmAudioStereo = &bmCAudioStereo;
      bmArrowDown   = &bmCArrowDown;
      bmArrowUp     = &bmCArrowUp;
   } else if (OSDHeight >= 720) {
      bmAudioLeft   = &bmBAudioLeft;
      bmAudioRight  = &bmBAudioRight;
      bmAudioStereo = &bmBAudioStereo;
      bmArrowDown   = &bmBArrowDown;
      bmArrowUp     = &bmBArrowUp;
   } else {   // <  720 incl. 576
      bmAudioLeft   = &bmAAudioLeft;
      bmAudioRight  = &bmAAudioRight;
      bmAudioStereo = &bmAAudioStereo;
      bmArrowDown   = &bmAArrowDown;
      bmArrowUp     = &bmAArrowUp;
   }

   x3 = x0 + OSDsize.width;
   x2 = x3 - lhh;
   int d = x3 - x0;
   if (d > (ItemsWidth + lh)) {
      d = (d - ItemsWidth - lh) & ~0x07; // must be multiple of 8
      x2 -= d;
      x3 -= d;
   }
   y0 = 0;
   y1 = y0 + lh;
   y2 = y1 + lh;
   y3 = y2 + NumTracks * lh;
   y4 = y3 + lh;
   y5 = y4 + lh;

   if (y5 > (y0 + OSDsize.height)) {
      y5 = y0 + OSDsize.height;
      y4 = y5 - lh;
      y3 = y4 - lh;
   }
   osd = cOsdProvider::NewOsd(OSDsize.left, OSDsize.top + (Setup.ChannelInfoPos ? 0 : OSDsize.height - y5));
   tArea Areas[] = { { x0, y0, x3 - 1, y5 - 1, 4 } };
   osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));

   tColor bg = Theme.Color(clrBackground);
   tColor mtbg = Theme.Color(clrMenuTitleBg);
   tColor mtfg = Theme.Color(clrMenuTitleFg);

   osd->DrawPixel(x0, y0, clrTransparent);
   osd->DrawPixel(x0, y0, bg);
   osd->DrawPixel(x0, y0, mtbg);
   osd->DrawPixel(x0, y0, mtfg);
   osd->DrawPixel(x0, y0, Theme.Color(clrMenuItemSelectable));
   osd->DrawPixel(x0, y0, Theme.Color(clrMenuItemCurrentFg));
   osd->DrawPixel(x0, y0, Theme.Color(clrMenuItemCurrentBg));

   osd->DrawRectangle(x0, y0, x1 - 1, y1 - lhh - 1, clrTransparent);
   osd->DrawEllipse(x0, y0, x1 - 1, y1 - lhh - 1, mtbg, 2);
   osd->DrawRectangle(x0, y0 + lhh , x1 - 1, y1 - 1, mtbg);
   osd->DrawRectangle(x1, y0, x2 - 1, y1 - 1, mtbg);
   osd->DrawText(x1, y0, Title, mtfg, clrTransparent, smallfont, x2 - x1);
   osd->DrawRectangle(x2, y0, x3 - 1, y1 - lhh - 1, clrTransparent);
   osd->DrawEllipse(x2, y0, x3 - 1, y1 - lhh  - 1, mtbg, 1);
   osd->DrawRectangle(x2, y0 + lhh , x3 - 1, y1 - 1, mtbg);
   osd->DrawRectangle(x0, y4, x1 - 1, y5 - lhh - 1, mtbg);
   osd->DrawRectangle(x0, y4 + lhh, x1 - 1, y5 - 1, clrTransparent);
   osd->DrawEllipse(x0, y4 + lhh , x1 - 1, y5 - 1, mtbg, 3);
   osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, mtbg);
   osd->DrawRectangle(x2, y4, x3 - 1, y5 - lhh - 1, mtbg);
   osd->DrawRectangle(x2, y4 + lhh, x3 - 1, y5 - 1, clrTransparent);
   osd->DrawEllipse(x2, y4 + lhh , x3 - 1, y5 - 1, mtbg, 4);
   osd->DrawRectangle(x0, y1, x3 - 1, y2 - 1, bg);
   osd->DrawRectangle(x0, y3, x3 - 1, y4 - 1, bg);

   int MaxTracks = MaxItems();
   Clear();
   if (MaxTracks > (numTracks - offset))
   MaxTracks = numTracks - offset;
   for (int i = offset; i < offset + MaxTracks; i++)
      SetItem(Tracks[i], i, false);
   if (numTracks > (offset + MaxTracks))
      osd->DrawBitmap(x2, y3 + (y4 - y3 - bmArrowDown->Height()) / 2, *bmArrowDown, Theme.Color(clrMenuItemSelectable), Theme.Color(clrBackground));

   MessageScroll = NULL;
}

cSkinElchiDisplayTracks::~cSkinElchiDisplayTracks()
{
   showMessage = changed = false;
   DELETENULL(osd);
}

int cSkinElchiDisplayTracks::MaxItems(void)
{
   return (y3 - y2) / lh;
}

void cSkinElchiDisplayTracks::Clear(void)
{
   changed = true;
   int MaxTracks = MaxItems();
   tColor bg = Theme.Color(clrBackground);
   tColor mis = Theme.Color(clrMenuItemSelectable);
   osd->DrawBitmap(x2, y1 + (y2 - y1 - bmArrowUp->Height()) / 2, *bmArrowUp, (offset > 0) ? mis : bg, bg);
   osd->DrawBitmap(x2, y3 + (y4 - y3 - bmArrowDown->Height()) / 2, *bmArrowDown, (numTracks > (offset + MaxTracks)) ? mis : bg, bg);
   osd->DrawRectangle(x0, y2, x3 - 1, y3 - 1, Theme.Color(clrBackground));
}

void cSkinElchiDisplayTracks::SetItem(const char *Text, int Index, bool Current)
{
   int y = y2 + Index * lh - offset * lh;
   tColor fg, bg;
   if (Current) {
      fg = Theme.Color(clrMenuItemCurrentFg);
      bg = Theme.Color(clrMenuItemCurrentBg);
      currentIndex = Index;
   }
   else {
      fg = Theme.Color(clrMenuItemSelectable);
      bg = Theme.Color(clrBackground);
   }
   osd->DrawText(x1, y, Text, fg, bg, cFont::GetFont(fontOsd), x2 - x1);
   if (currentIndex == Index) {
      osd->DrawEllipse(x0, y, x0 + lhh - 1, y + lh - 1, bg, 7);
      osd->DrawEllipse(x3 - lhh, y, x3 - 1, y + lh - 1, bg, 5);
   }
}

void cSkinElchiDisplayTracks::SetTrack(int Index, const char * const *Tracks)
{
   changed = true;
   if (index != Index) {
      tColor bg = Theme.Color(clrBackground);
      osd->DrawRectangle(x0, y2, x1 - 1, y3 - 1, bg);
      osd->DrawRectangle(x2, y2, x3 - 1, y3 - 1, bg);
      if (currentIndex >= 0) {
         int MaxTracks = MaxItems();
         if ((Index - offset) >= MaxTracks) {
            offset += MaxTracks - 1;
            Clear();
            if (MaxTracks > (numTracks - offset))
               MaxTracks = numTracks - offset;
            for (int i = offset; i < offset + MaxTracks; i++)
               SetItem(Tracks[i], i, false);
         }
         else {
            if (Index < offset) {
               offset -= MaxTracks - 1;
               Clear();
               if (MaxTracks > (numTracks - offset))
                  MaxTracks = numTracks - offset;
               for (int i = offset; i < offset + MaxTracks; i++)
                  SetItem(Tracks[i], i, false);
            }
            else
               SetItem(Tracks[currentIndex], currentIndex, false);
         }
      }
      else {
         int MaxTracks = MaxItems();
         if ((Index + 1) > MaxTracks) {
            int temp = Index + 1;
            while (temp > MaxTracks) {
               offset += MaxTracks - 1;
               temp   -= MaxTracks - 1;
            }
            Clear();
            if (MaxTracks > (numTracks - offset))
               MaxTracks = numTracks - offset;
            for (int i = offset; i < offset + MaxTracks; i++)
               SetItem(Tracks[i], i, false);
         }
      }
      SetItem(Tracks[Index], Index, true);
      index = Index;
   }
}

void cSkinElchiDisplayTracks::SetAudioChannel(int AudioChannel)
{
   changed = true;
   cBitmap *bm = NULL;
   switch (AudioChannel) {
     case 0: bm = bmAudioStereo; break;
     case 1: bm = bmAudioLeft;   break;
     case 2: bm = bmAudioRight;  break;
   }
   if (bm)
      osd->DrawBitmap(x1, y4 + (y5 - y4 - bm->Height()) / 2, *bm, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg));
   else
      osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, Theme.Color(clrMenuTitleBg));
}

void cSkinElchiDisplayTracks::SetMessage(eMessageType Type, const char *Text)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayTracks::SetMessage(%d,%s)", (int)Type, Text)
   if (Text) {
      showMessage = changed = true;
      tColor msg_fg = Theme.Color(clrMessageStatusFg + 2 * Type);
      tColor msg_bg = Theme.Color(clrMessageStatusBg + 2 * Type);
      osd->DrawEllipse(x0, y3, x1 - 1, y4 - 1, msg_bg, 7);

      const cFont *font = cFont::GetFont(fontOsd);
      MessageScroll = new cCharScroll(x1, y3, x2 - x1, font->Height(), osd, msg_fg, msg_bg);
      MessageScroll->SetText(Text, font);
      osd->DrawEllipse(x2, y3, x3 - 1, y4 - 1, msg_bg, 5);
   }
   else {
      if (showMessage) {
         changed = true;
         DELETENULL(MessageScroll);
         showMessage = false;
         osd->DrawRectangle(x0, y3, x3 - 1, y4 - 1, Theme.Color(clrBackground));
      }
   }
}

void cSkinElchiDisplayTracks::Flush(void)
{
   if (MessageScroll) changed |= MessageScroll->DrawIfNeeded();

   if (changed) {
      DSYSLOG("skinelchi: cSkinElchiDisplayTracks::(Flush)")
      osd->Flush();
      changed = false;
   }
}
