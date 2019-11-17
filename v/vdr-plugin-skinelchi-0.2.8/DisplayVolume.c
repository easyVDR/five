/*
 * DisplayVolume.c: Volume display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <vdr/device.h>

#include "DisplayVolume.h"
#include "setup.h"
#include "common.h"

#include "symbols/576/mute.xpm"
#include "symbols/576/volume.xpm"

#include "symbols/720/Bmute.xpm"
#include "symbols/720/Bvolume.xpm"

#include "symbols/1080/Cmute.xpm"
#include "symbols/1080/Cvolume.xpm"

// --- cSkinElchiDisplayVolume ---------------------------------------------
cBitmap cSkinElchiDisplayVolume::bmAMute(mute_xpm);
cBitmap cSkinElchiDisplayVolume::bmAVolume(volume_xpm);

cBitmap cSkinElchiDisplayVolume::bmBMute(Bmute_xpm);
cBitmap cSkinElchiDisplayVolume::bmBVolume(Bvolume_xpm);

cBitmap cSkinElchiDisplayVolume::bmCMute(Cmute_xpm);
cBitmap cSkinElchiDisplayVolume::bmCVolume(Cvolume_xpm);

cSkinElchiDisplayVolume::cSkinElchiDisplayVolume(void)
{
   changed = showMessage = false;
   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();
   lhh = lh / 2;

   OSDsize.left   = cOsd::OsdLeft();
   OSDsize.top    = cOsd::OsdTop();
   OSDsize.width  = cOsd::OsdWidth();
   OSDsize.height = cOsd::OsdHeight();

   int OSDHeight = 576;
#if VDRVERSNUM >= 10708   
   int OSDWidth = 720;
   double OSDAspect = 4.0/3.0;
   cDevice::PrimaryDevice()->GetOsdSize(OSDWidth, OSDHeight, OSDAspect);
#endif

   DSYSLOG("skinelchi: OSDsize Volume %dx%d left=%d/%d top=%d/%d width=%d/%d heigth=%d/%d",
           OSDWidth, OSDHeight, OSDsize.left, Setup.OSDLeft, OSDsize.top, Setup.OSDTop, OSDsize.width, Setup.OSDWidth, OSDsize.height, Setup.OSDHeight);

   if (OSDHeight >= 1080) {
      bmMute   = &bmCMute;
      bmVolume = &bmCVolume;
   } else if (OSDHeight >= 720) {
      bmMute   = &bmBMute;
      bmVolume = &bmBVolume;
   } else {  // <  720 incl. 576
      bmMute   = &bmAMute;
      bmVolume = &bmAVolume;
   }

   osd = cOsdProvider::NewOsd(OSDsize.left, OSDsize.top + OSDsize.height - 2 * lh);
   tArea Areas[] = { { 0, 0, OSDsize.width - 1, 2 * lh - 1, 4 } };
   osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));

   osd->DrawPixel(0, 0, clrTransparent);
   osd->DrawPixel(0, 0, Theme.Color(clrBackground));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumeSymbolMuteFg));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumeSymbolMuteBg));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumeBarLower));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumeBarUpper));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumePrompt));

   mute = -1;
   current = -1;
   MessageScroll = NULL;
}

cSkinElchiDisplayVolume::~cSkinElchiDisplayVolume()
{
   changed = showMessage = true;
   DELETENULL(osd);
}

void cSkinElchiDisplayVolume::SetVolume(int Current, int Total, bool Mute)
{
   const cFont *font = cFont::GetFont(fontOsd);
   if (Mute) {
      if (mute != 1){
         current = 0;
         changed = true;
         osd->DrawRectangle(0, lh, osd->Width() - 1, osd->Height() - 1, clrTransparent);
         char Prompt[4];
         snprintf(Prompt, 4,"%3d", Current * 100 / Total);
         int l = font->Width(Prompt) + lhh;
         osd->DrawText(lhh + 3, lh + 1, Prompt, Theme.Color(clrBackground), clrTransparent, font);
         osd->DrawText(lhh, lh - 2, Prompt, Theme.Color(clrVolumePrompt), clrTransparent, font);
         osd->DrawBitmap(l + 3, lh + (lh - bmMute->Height()) / 2, *bmMute, Theme.Color(clrVolumeSymbolMuteFg), Theme.Color(clrVolumeSymbolMuteBg));
         mute = 1;
      }
   }
   else {
      int w = osd->Width();
      int h = osd->Height();
      if (mute != 0) {
         changed = true;
         tColor vbl = Theme.Color(clrVolumeBarLower);
         int w1 = w - 1;
         int h1 = h - 1;
         osd->DrawRectangle(0, lh, w1, h1, clrTransparent);
         osd->DrawEllipse(0, lh, lhh - 1, h1, vbl, 7);
         osd->DrawRectangle(lhh, lh, w1 - lhh, h1, vbl);
         osd->DrawEllipse(w - lhh, lh, w1, h1, vbl, 5);
         osd->DrawRectangle(0, 0, w1, lh - 1, clrTransparent);
         const char *Prompt = trVDR("Volume ");
         int l = font->Width(Prompt) + lhh;
         osd->DrawText(lhh + 3, 1, Prompt, Theme.Color(clrBackground), clrTransparent, font);
         osd->DrawText(lhh, -2, Prompt, Theme.Color(clrVolumePrompt), clrTransparent, font);
         osd->DrawBitmap(l + 3, (lh - bmVolume->Height()) / 2, *bmVolume, Theme.Color(clrVolumeSymbolVolumeFg), clrTransparent);
         mute = 0;
      }
      if (current != Current){
         changed = true;
         int p = (w - lh) * Current / Total;
         int h5 = h - 5;
         int lh4 = lh + 4;
         int lhhp = lhh + p;
         osd->DrawRectangle(lhh, lh4, lhhp - 1, h5, Theme.Color(clrVolumeBarUpper));
         osd->DrawRectangle(lhhp, lh4, w - lhh - 1, h5, Theme.Color(clrVolumeBarLower));
         current = Current;
      }
   }
}

void cSkinElchiDisplayVolume::SetMessage(eMessageType Type, const char *Text)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayVolume::SetMessage(%d,%s)", (int)Type, Text)
   if (Text) {
      {
         showMessage = changed = true;
         tColor msg_fg = Theme.Color(clrMessageStatusFg + 2 * Type);
         tColor msg_bg = Theme.Color(clrMessageStatusBg + 2 * Type);
         osd->SaveRegion(0, 0, OSDsize.width - 1, 2 * lh - 1);
         osd->DrawEllipse(0, lh, lhh - 1, 2 * lh - 1, msg_bg, 7);

         const cFont *font = cFont::GetFont(fontOsd);
         MessageScroll = new cCharScroll(0, lh, OSDsize.width, font->Height(), osd, msg_fg, msg_bg);
         MessageScroll->SetText(Text, font);
         osd->DrawEllipse(OSDsize.width - lhh, lh, OSDsize.width - 1, 2 * lh - 1, msg_bg, 5);
         }
      }
   else {
      if (showMessage) {
         changed = true;
         DELETENULL(MessageScroll);
         showMessage = false;
         osd->RestoreRegion();
      }
   }
}

void cSkinElchiDisplayVolume::Flush(void)
{
   if (MessageScroll) changed |= MessageScroll->DrawIfNeeded();

   if (changed) {
      DSYSLOG("skinelchi: cSkinElchiDisplayVolume::Flush()")
      osd->Flush();
      changed = false;
   }
}
