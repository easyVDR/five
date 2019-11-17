/*
 * DisplayMessage.c: Message display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <vdr/device.h>

#include "DisplayMessage.h"
#include "setup.h"
#include "common.h"

// --- cSkinElchiDisplayMessage --------------------------------------------
cSkinElchiDisplayMessage::cSkinElchiDisplayMessage(void)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayMessage::cSkinElchiDisplayMessage()")

   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();
   lhh = lh / 2;

   tOSDsize OSDsize;

   OSDsize.left   = cOsd::OsdLeft();
   OSDsize.top    = cOsd::OsdTop();
   OSDsize.width  = cOsd::OsdWidth();
   OSDsize.height = cOsd::OsdHeight();

   DSYSLOG2("skinelchi: OSDsize Msg left=%d/%d top=%d/%d width=%d/%d heigth=%d/%d",
           OSDsize.left, Setup.OSDLeft, OSDsize.top, Setup.OSDTop, OSDsize.width, Setup.OSDWidth, OSDsize.height, Setup.OSDHeight)


   osd = cOsdProvider::NewOsd(OSDsize.left, OSDsize.top + OSDsize.height - lh);
   tArea Areas[] = { { 0, 0, OSDsize.width - 1, lh - 1, 2 } };
   osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));

   osd->DrawPixel(0, 0, clrTransparent);
   osd->DrawPixel(0, 0, Theme.Color(clrBackground));

   changed = true;

   MessageScroll = NULL;
}

cSkinElchiDisplayMessage::~cSkinElchiDisplayMessage()
{
   DSYSLOG("skinelchi: cSkinElchiDisplayMessage::~cSkinElchiDisplayMessage()")
   changed = true;
   DELETENULL(osd);
}

void cSkinElchiDisplayMessage::SetMessage(eMessageType Type, const char *Text)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayMessage::SetMessage(%d,%s)", (int)Type, Text)
   changed = true;
   tColor msb = Theme.Color(clrMessageStatusBg + 2 * Type);
   int h1 = osd->Height();
   int lh21 = lh / 2 - 1;
   osd->DrawRectangle(0, 0, lh21, h1, clrTransparent);
   osd->DrawEllipse(0, 0, lh21, h1, msb, 7);

   if(MessageScroll) DELETENULL(MessageScroll);
   const cFont *font = cFont::GetFont(fontOsd);
   MessageScroll = new cCharScroll(lhh, 0, osd->Width() - lh, font->Height(), osd,
                             Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), taCenter);
   MessageScroll->SetText(Text, font);
   int wlh2 = osd->Width() - lh / 2;
   osd->DrawRectangle(wlh2 - 1, 0, wlh2, h1, msb);
   int w1 = osd->Width() - 1;
   osd->DrawRectangle(wlh2, 0, w1, h1, clrTransparent);
   osd->DrawEllipse(wlh2, 0, w1, h1, msb, 5);
}

void cSkinElchiDisplayMessage::Flush(void)
{
   if(MessageScroll) changed |= MessageScroll->DrawIfNeeded();

   if (changed) {
      DSYSLOG("skinelchi: cSkinElchiDisplayMessage::Flush()")
      osd->Flush();
      changed = false;
   }
}
