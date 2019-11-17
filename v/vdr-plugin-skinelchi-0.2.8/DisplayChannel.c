/*
 * DisplayChannel.c: Channel display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <sstream>
#include <string>
#include <time.h>

#include <vdr/device.h>
#include <vdr/menu.h>
#include <vdr/osd.h>

//#define DEBUG
//#define DEBUG2
#include "common.h"
#include "DisplayChannel.h"
#include "vdrstatus.h"
// für VdrStatus->VolumeChange, RecordingsNumbers, AudioTracks, AudioChannel

#ifdef SKINELCHI_HAVE_IMAGEMAGICK
#include "bitmap.h"
#endif

#include "symbols/576/audio.xpm"
#include "symbols/576/dolbydigital.xpm"
#include "symbols/576/encrypted.xpm"
#include "symbols/576/radio.xpm"
#include "symbols/576/recording.xpm"
#include "symbols/576/teletext.xpm"
#include "symbols/576/vps.xpm"
#include "symbols/576/mail.xpm"
#include "symbols/576/ar43.xpm"
#include "symbols/576/arL149.xpm"
#include "symbols/576/arL169.xpm"
#include "symbols/576/arLG169.xpm"
#include "symbols/576/ar169.xpm"
#include "symbols/576/ar2211.xpm"
#include "symbols/576/arHD.xpm"

#include "symbols/720/Baudio.xpm"
#include "symbols/720/Bdolbydigital.xpm"
#include "symbols/720/Bencrypted.xpm"
#include "symbols/720/Bradio.xpm"
#include "symbols/720/Brecording.xpm"
#include "symbols/720/Bteletext.xpm"
#include "symbols/720/Bvps.xpm"
#include "symbols/720/Bmail.xpm"
#include "symbols/720/Bar43.xpm"
#include "symbols/720/BarL149.xpm"
#include "symbols/720/BarL169.xpm"
#include "symbols/720/BarLG169.xpm"
#include "symbols/720/Bar169.xpm"
#include "symbols/720/Bar2211.xpm"
#include "symbols/720/BarHD.xpm"

#include "symbols/1080/Caudio.xpm"
#include "symbols/1080/Cdolbydigital.xpm"
#include "symbols/1080/Cencrypted.xpm"
#include "symbols/1080/Cradio.xpm"
#include "symbols/1080/Crecording.xpm"
#include "symbols/1080/Cteletext.xpm"
#include "symbols/1080/Cvps.xpm"
#include "symbols/1080/Cmail.xpm"
#include "symbols/1080/Car43.xpm"
#include "symbols/1080/CarL149.xpm"
#include "symbols/1080/CarL169.xpm"
#include "symbols/1080/CarLG169.xpm"
#include "symbols/1080/Car169.xpm"
#include "symbols/1080/Car2211.xpm"
#include "symbols/1080/CarHD.xpm"

cBitmap cSkinElchiDisplayChannel::bmAVps(vps_xpm);
cBitmap cSkinElchiDisplayChannel::bmATeletext(teletext_xpm);
cBitmap cSkinElchiDisplayChannel::bmARadio(radio_xpm);
cBitmap cSkinElchiDisplayChannel::bmAAudio(audio_xpm);
cBitmap cSkinElchiDisplayChannel::bmADolbyDigital(dolbydigital_xpm);
cBitmap cSkinElchiDisplayChannel::bmAEncrypted(encrypted_xpm);
cBitmap cSkinElchiDisplayChannel::bmARecording(recording_xpm);
cBitmap cSkinElchiDisplayChannel::bmAMail(mail_xpm);
cBitmap cSkinElchiDisplayChannel::bmAAR43(ar43_xpm);
cBitmap cSkinElchiDisplayChannel::bmAARL149(arL149_xpm);
cBitmap cSkinElchiDisplayChannel::bmAARL169(arL169_xpm);
cBitmap cSkinElchiDisplayChannel::bmAARLG169(arLG169_xpm);
cBitmap cSkinElchiDisplayChannel::bmAAR169(ar169_xpm);
cBitmap cSkinElchiDisplayChannel::bmAAR2211(ar2211_xpm);
cBitmap cSkinElchiDisplayChannel::bmAARHD(arHD_xpm);

cBitmap cSkinElchiDisplayChannel::bmBVps(Bvps_xpm);
cBitmap cSkinElchiDisplayChannel::bmBTeletext(Bteletext_xpm);
cBitmap cSkinElchiDisplayChannel::bmBRadio(Bradio_xpm);
cBitmap cSkinElchiDisplayChannel::bmBAudio(Baudio_xpm);
cBitmap cSkinElchiDisplayChannel::bmBDolbyDigital(Bdolbydigital_xpm);
cBitmap cSkinElchiDisplayChannel::bmBEncrypted(Bencrypted_xpm);
cBitmap cSkinElchiDisplayChannel::bmBRecording(Brecording_xpm);
cBitmap cSkinElchiDisplayChannel::bmBMail(Bmail_xpm);
cBitmap cSkinElchiDisplayChannel::bmBAR43(Bar43_xpm);
cBitmap cSkinElchiDisplayChannel::bmBARL149(BarL149_xpm);
cBitmap cSkinElchiDisplayChannel::bmBARL169(BarL169_xpm);
cBitmap cSkinElchiDisplayChannel::bmBARLG169(BarLG169_xpm);
cBitmap cSkinElchiDisplayChannel::bmBAR169(Bar169_xpm);
cBitmap cSkinElchiDisplayChannel::bmBAR2211(Bar2211_xpm);
cBitmap cSkinElchiDisplayChannel::bmBARHD(BarHD_xpm);

cBitmap cSkinElchiDisplayChannel::bmCVps(Cvps_xpm);
cBitmap cSkinElchiDisplayChannel::bmCTeletext(Cteletext_xpm);
cBitmap cSkinElchiDisplayChannel::bmCRadio(Cradio_xpm);
cBitmap cSkinElchiDisplayChannel::bmCAudio(Caudio_xpm);
cBitmap cSkinElchiDisplayChannel::bmCDolbyDigital(Cdolbydigital_xpm);
cBitmap cSkinElchiDisplayChannel::bmCEncrypted(Cencrypted_xpm);
cBitmap cSkinElchiDisplayChannel::bmCRecording(Crecording_xpm);
cBitmap cSkinElchiDisplayChannel::bmCMail(Cmail_xpm);
cBitmap cSkinElchiDisplayChannel::bmCAR43(Car43_xpm);
cBitmap cSkinElchiDisplayChannel::bmCARL149(CarL149_xpm);
cBitmap cSkinElchiDisplayChannel::bmCARL169(CarL169_xpm);
cBitmap cSkinElchiDisplayChannel::bmCARLG169(CarLG169_xpm);
cBitmap cSkinElchiDisplayChannel::bmCAR169(Car169_xpm);
cBitmap cSkinElchiDisplayChannel::bmCAR2211(Car2211_xpm);
cBitmap cSkinElchiDisplayChannel::bmCARHD(CarHD_xpm);

#if VDRVERSNUM < 10700
#define STRVIDEOSIZE " 000x000"
#else
#define STRVIDEOSIZE " 0000x0000"
#endif

extern cSkinElchiStatus *ElchiStatus;


// --- cSkinElchiDisplayChannel --------------------------------------------

cSkinElchiDisplayChannel::cSkinElchiDisplayChannel(bool WithInfo)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayChannel::cSkinElchiDisplayChannel(%s)", WithInfo ? "'WithInfo'" : "'WithoutInfo'")

   if (ElchiConfig.showVideoInfo)
      VideoStatus->Activate(true);
   video = true;
   AudioChanged = false;
   old_ar = ar_unknown;
   old_width = -1;
   old_height = -1;
   showMessage = changed = false;
   showVolume = false;
   volumechange = ElchiStatus->GetVolumeChange();
   recordingchange = -1;
   date = NULL;
   rec = false;
   withInfo = WithInfo;
   const cFont *smallfont = cFont::GetFont(fontSml);
   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();
   slh = smallfont->Height();
   lhh = lh / 2;
   lineOffset = (lh - slh) / 2;
   bg = Theme.Color(clrBackground);
   tOSDsize OSDsize;

   OSDsize.left   = cOsd::OsdLeft();
   OSDsize.top    = cOsd::OsdTop();
   OSDsize.width  = cOsd::OsdWidth();
   OSDsize.height = cOsd::OsdHeight();
   
   OSDHeight = 576;
   int OSDWidth = 720;
   double OSDAspect = 4.0/3.0;
#if VDRVERSNUM >= 10708   
   cDevice::PrimaryDevice()->GetOsdSize(OSDWidth, OSDHeight, OSDAspect);
#endif

   DSYSLOG("skinelchi: OSDsize Channel %dx%d left=%d/%d top=%d/%d width=%d/%d heigth=%d/%d",
           OSDWidth, OSDHeight, OSDsize.left, Setup.OSDLeft, OSDsize.top, Setup.OSDTop, OSDsize.width, Setup.OSDWidth, OSDsize.height, Setup.OSDHeight);

   if (OSDHeight >= 1080) {
      logox          = 160;
      logoy          = 120;
      bmVps          = &bmCVps;
      bmTeletext     = &bmCTeletext;
      bmRadio        = &bmCRadio;
      bmAudio        = &bmCAudio;
      bmDolbyDigital = &bmCDolbyDigital;
      bmEncrypted    = &bmCEncrypted;
      bmRecording    = &bmCRecording;
      bmMail         = &bmCMail;
      bmAR43         = &bmCAR43;
      bmARL149       = &bmCARL149;
      bmARL169       = &bmCARL169;
      bmARLG169      = &bmCARLG169;
      bmAR169        = &bmCAR169;
      bmAR2211       = &bmCAR2211;
      bmARHD         = &bmCARHD;
      Gap            = 9;
      SymbolGap      = 6;
      ScrollWidth    = 9;
   } else if (OSDHeight >= 720) {
      logox          = 104;
      logoy          = 78;
      bmVps          = &bmBVps;
      bmTeletext     = &bmBTeletext;
      bmRadio        = &bmBRadio;
      bmAudio        = &bmBAudio;
      bmDolbyDigital = &bmBDolbyDigital;
      bmEncrypted    = &bmBEncrypted;
      bmRecording    = &bmBRecording;
      bmMail         = &bmBMail;
      bmAR43         = &bmBAR43;
      bmARL149       = &bmBARL149;
      bmARL169       = &bmBARL169;
      bmARLG169      = &bmBARLG169;
      bmAR169        = &bmBAR169;
      bmAR2211       = &bmBAR2211;
      bmARHD         = &bmBARHD;
      Gap            = 6;
      SymbolGap      = 4;
      ScrollWidth    = 6;
   } else {  // <  720 incl. 576
      logox          = 64;
      logoy          = 48;
      bmVps          = &bmAVps;
      bmTeletext     = &bmATeletext;
      bmRadio        = &bmARadio;
      bmAudio        = &bmAAudio;
      bmDolbyDigital = &bmADolbyDigital;
      bmEncrypted    = &bmAEncrypted;
      bmRecording    = &bmARecording;
      bmMail         = &bmAMail;
      bmAR43         = &bmAAR43;
      bmARL149       = &bmAARL149;
      bmARL169       = &bmAARL169;
      bmARLG169      = &bmAARLG169;
      bmAR169        = &bmAAR169;
      bmAR2211       = &bmAAR2211;
      bmARHD         = &bmAARHD;
      Gap            = 5;
      SymbolGap      = 3;
      ScrollWidth    = 5;
   }

   if (!ElchiConfig.showLogo)
      logox = logoy = 0;
   
   x0 = 0;
   x1 = ElchiConfig.showLogo ? 16 : 0;
   x2 = x1 + lh - lhh;
   x3 = logox + lh - lhh;
   x4 = x2 + font->Width("00:00");
   x5 = x4 + lhh + Gap;
   x6 = x3 + font->Width("00000-") + smallfont->Width("_000 00.00 00:00_");
   x7 = x6 + lh - lhh;
   x11 = OSDsize.width;
   x8 = x11 - lh;
   x10 = x8 + lhh;
   x9 = x10 - lh - bmRecording->Width() - SymbolGap - bmEncrypted->Width() - SymbolGap - bmDolbyDigital->Width() - SymbolGap - bmAudio->Width() - SymbolGap - bmTeletext->Width() - SymbolGap - bmVps->Width() - SymbolGap;

   MailIconState = 0;
   if (ElchiConfig.showMailIcon > 0) {
      cPlugin *MailBoxPlugin = cPluginManager::GetPlugin(PLUGIN_MAILBOX);
      bool fHasNewMail = false;
      if (MailBoxPlugin && MailBoxPlugin->Service(MailBox_HasNewMail_v1_0_NAME, &fHasNewMail)) {
         if (fHasNewMail)
            MailIconState = 2; // icon: new mail
         else
            if (ElchiConfig.showMailIcon >= 2)
               MailIconState = 1; // icon: no new mail

         if (MailIconState > 0)
            x9 -= bmMail->Width() + SymbolGap;
      }
   }

   if (ElchiConfig.showVideoInfo) // space for AR symbol
      x9 -= bmAR43->Width() + SymbolGap;

   y0 = 0;
   y1 = (ElchiConfig.showLogo &&  logoy >= lh) ?  (logoy - lh)/2 : 0;
   y3 = y1 + lh;
   y2 = y3 - lhh;
   y4 = y3 + 3;
   y6 = y4 + lh;
   y6a = y6 + (withInfo && ((ElchiConfig.showRecInfo == 2) ||
              (ElchiConfig.showRecInfo == 1 && ElchiStatus->recordinglist.Count()))? lh : 0);
   y5 = y6 - lhh;
   y9 = y6a + (withInfo ? lh * 4 : 0);
   y7 = y9 - lh;
   y8 = y9 - lhh;

   osd = cOsdProvider::NewOsd(OSDsize.left, OSDsize.top + (Setup.ChannelInfoPos ? 0 : OSDsize.height - y9));
   if (withInfo) {
      tArea Areas[] = { { 0, 0, x11 - 1, y9 - 1, 8 } };
      if ((ElchiConfig.tryChannelDisplay8Bpp || OSDHeight > 576 ) && (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)) {
         //isyslog("skinelchi: withinfo Area 1");
         osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      }
      else {
         if (ElchiConfig.showLogo) {
            //isyslog("skinelchi: withinfo Area 4");
            tArea Areas[] = {{ logox, y1, x11 - 1, y6 - 1, 4 },
                             { x1, logoy, logox - 1, y6 - 1, 4 },
                             { x0, y0, logox - 1, logoy - 1, 4 },
                             { x1, y6, x11 - 1, y9 - 1, 4 },};
            osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
            //isyslog("skinelchi:DisplayChannels SetArea: %d", osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea)));
         }
         else {
            //isyslog("skinelchi: withinfo Area 2");
            tArea Areas[] = { { x0, y0, x11 - 1, y6 - 1, 4 },
                              { x0, y6, x11 - 1, y9 - 1, 4 }, };
            osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
         }
      }

      DrawBackground();

      // scrolling texts
      PresentTitleScroll = new cCharScroll(x5 + 2, y6a, x10 - x5 - 2, font->Height(), osd,
                           Theme.Color(clrChannelEpgTitleFg), Theme.Color(clrChannelEpgTitleBg));

      PresentShortScroll = new cCharScroll(x5 + 2, y6a + lh, x10 - x5 - 2, smallfont->Height(), osd,
                           Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTitleBg));

      FollowingTitleScroll = new cCharScroll(x5 + 2, y6a + 2*lh, x10 - x5 - 2, font->Height(), osd,
                           Theme.Color(clrChannelEpgTitleFg), Theme.Color(clrChannelEpgTitleBg));

      FollowingShortScroll = new cCharScroll(x5 + 2, y6a + 3*lh, x10 - x5 - 2, smallfont->Height(), osd,
                           Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTitleBg));

      RecordingScroll = new cCharScroll(x1 + 4, y6, x10 - x1 - 4, smallfont->Height(), osd,
                           Theme.Color(clrChannelSymbolOn), bg);
   } // with Info
   else
   { // without Info
      tArea Areas[] = { { 0, 0, x11 - 1, y6 - 1, 8 } };
      if ((ElchiConfig.tryChannelDisplay8Bpp || OSDHeight > 576 ) && (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)) {
         //isyslog("skinelchi: withoutinfo Area 1 8Bit");
         osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      }
      else {
         if (ElchiConfig.showLogo) {
            //isyslog("skinelchi: withoutinfo Area 3");
            tArea Areas[] ={{ logox, y1, x11 - 1, y6 - 1, 4 },
                            { x1, logoy, logox - 1, y6 - 1, 4 },
                            { x0, y0, logox - 1, logoy - 1, 8/*4*/},};
            osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
            //isyslog("skinelchi:DisplayChannels SetArea: %d", osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea)));
         }
         else {
            //isyslog("skinelchi: withoutinfo Area 1a 4Bit");
            tArea Areas[] = { { x0, y0, x11 - 1, y6 - 1, 4 } };
            osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
         }
      }

      DrawBackground();
      PresentTitleScroll = NULL;
      PresentShortScroll = NULL;
      FollowingTitleScroll = NULL;
      FollowingShortScroll = NULL;
      RecordingScroll = NULL;
   } // end with/without Info

   AudioScroll = new cCharScroll(x7, y1 + lineOffset, x10 - x7, smallfont->Height(), osd,
                         Theme.Color(clrChannelSymbolOn), bg, taRight|taTop);

   int DestWidth = x9 - x3;
   if (ElchiConfig.showVideoInfo > 1)
      DestWidth -= smallfont->Width(STRVIDEOSIZE);

   ChannelNameScroll = new cCharScroll(x3, y4 + lineOffset, DestWidth, smallfont->Height(), osd,
                          Theme.Color(clrChannelNameFg), Theme.Color(clrChannelNameBg));

   //isyslog("skinelchi: cSkinElchiDisplayChannel::cSkinElchiDisplayChannel(%s) end", WithInfo ? "'WithInfo'" : "'WithoutInfo'");
}


cSkinElchiDisplayChannel::~cSkinElchiDisplayChannel()
{
   DSYSLOG("skinelchi: cSkinElchiDisplayChannel::~cSkinElchiDisplayChannel()")
   showMessage = changed = false;

   if (ElchiConfig.showVideoInfo)
      VideoStatus->Activate(false);

   free(date);

   DELETENULL(PresentTitleScroll);
   DELETENULL(PresentShortScroll);
   DELETENULL(FollowingTitleScroll);
   DELETENULL(FollowingShortScroll);
   DELETENULL(RecordingScroll);
   DELETENULL(AudioScroll);
   DELETENULL(ChannelNameScroll);
   audiostring = NULL; // avoid double free or corruption

   DELETENULL(osd);
}

void cSkinElchiDisplayChannel::DrawBackground()
{
   //clear all
   osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);

   osd->DrawPixel(0, 0, Theme.Color(clrBackground));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelNameFg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelNameBg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelDateFg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelDateBg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelEpgTimeFg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelEpgTimeBg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelEpgTitleFg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelEpgTitleBg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelEpgShortText));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelSymbolOn));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelSymbolOff));
   osd->DrawPixel(0, 0, Theme.Color(clrSymbolRecFg));
   osd->DrawPixel(0, 0, Theme.Color(clrSymbolRecBg));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelTimebarRest));
   osd->DrawPixel(0, 0, Theme.Color(clrChannelTimebarSeen));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumeBarUpper));
   osd->DrawPixel(0, 0, Theme.Color(clrVolumeBarLower));
   osd->DrawPixel(0, 0, clrTransparent);

   // clear logo (and reset logo palette)
   //osd->DrawRectangle(0, 0, osd->Width(), logoy+1, clrTransparent);

   if(withInfo) {
      // draw backgrounds with info
      if (ElchiConfig.showLogo) {
         osd->DrawRectangle(x1, y1, x10 - 1, y6a - 1, Theme.Color(clrBackground));

         // empty Logo area
         osd->DrawRectangle(x0, y0, logox - 1, logoy - 1, Theme.Color(clrChannelNameBg));

         // Date
         osd->DrawRectangle(logox + 2, y1, x6 - 1, y3 - 1, Theme.Color(clrChannelDateBg));

         // Channel
         osd->DrawRectangle(x1, logoy + 2, logox - 1 + 2, y6 - 1, Theme.Color(clrChannelNameBg));
         osd->DrawRectangle(logox + 2, y4, x9 - 1, y6 - 1, Theme.Color(clrChannelNameBg));
      }
      else {
         osd->DrawRectangle(x2, y1, x10 - 1, y6a - 1, Theme.Color(clrBackground));
         osd->DrawRectangle(x1, y2, x2 - 1,  y6a - 1, Theme.Color(clrBackground));

         // Date
         osd->DrawRectangle(x1, y2, x2 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
         osd->DrawEllipse  (x1, y1, x2 - 1, y2 - 1, Theme.Color(clrChannelDateBg), 2);

         osd->DrawRectangle(x2, y1, x6 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
         //osd->DrawRectangle(x6, y1, x10 - 1, y3 - 1, Theme.Color(clrBackground));

         // Channel
         osd->DrawRectangle(x1, y4, x9 - 1, y6 - 1, Theme.Color(clrChannelNameBg));
      }

      //both
      //Channel & Date
      osd->DrawEllipse  (x10, y1, x11 - 1, y2 - 1, Theme.Color(clrBackground), 1);
      osd->DrawRectangle(x10, y2, x11 - 1, y6a - 1, Theme.Color(clrBackground));

      osd->DrawEllipse  (x6, y1, x7 - 1, y3 - 1, Theme.Color(clrChannelDateBg), 5);
      osd->DrawEllipse  (x9, y4, x9 + lhh - 1, y6 - 1, Theme.Color(clrChannelNameBg), 5);

      // EPG Time
      osd->DrawRectangle(x1, y6a, x4 - 1, y8 - 1, Theme.Color(clrChannelEpgTimeBg));
      osd->DrawEllipse  (x1, y8, x2 - 1, y9 - 1, Theme.Color(clrChannelEpgTimeBg), 3);
      osd->DrawRectangle(x2, y8, x4 - 1, y9 - 1, Theme.Color(clrChannelEpgTimeBg));

      // EPG Event
      osd->DrawRectangle(x4, y6a, x11 - 1, y8 - 1, Theme.Color(clrChannelEpgTitleBg));
      osd->DrawRectangle(x4, y8, x10 - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg));
      osd->DrawEllipse  (x10, y8, x11 - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg), 4);
   }
   else {  //withoutinfo
      // draw backgrounds without info
      if (ElchiConfig.showLogo) {
         osd->DrawRectangle(x2, y1, x10 - 1, y6 - 1, Theme.Color(clrBackground));
         osd->DrawRectangle(x1, y2, x2 - 1,  y5 - 1, Theme.Color(clrBackground));

         // empty Logo area
         osd->DrawRectangle(x0, y0, logox - 1, logoy - 1, Theme.Color(clrChannelNameBg));

         // Date
         osd->DrawRectangle(logox + 2, y1, x6 - 1, y3 - 1, Theme.Color(clrChannelDateBg));

         // Channel
         osd->DrawRectangle(logox + 2, y4, x9 - 1, y6 - 1, Theme.Color(clrChannelNameBg));
         osd->DrawRectangle(x1, logoy + 2, logox - 1 + 2, y5 - 1, Theme.Color(clrChannelNameBg));
         osd->DrawRectangle(x2, y5, logox - 1 + 2, y6 - 1, Theme.Color(clrChannelNameBg));
         osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 3);
      }
      else {
         osd->DrawRectangle(x2, y1, x10 - 1, y6 - 1, Theme.Color(clrBackground));
         osd->DrawRectangle(x1, y2, x2 - 1,  y5 - 1, Theme.Color(clrBackground));

         // Date
         osd->DrawRectangle(x1, y2, x2 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
         osd->DrawEllipse  (x1, y1, x2 - 1, y2 - 1, Theme.Color(clrChannelDateBg), 2);
         osd->DrawRectangle(x2, y1, x6 - 1, y3 - 1, Theme.Color(clrChannelDateBg));

         // Channel
         osd->DrawRectangle(x2, y4, x9 - 1, y6 - 1, Theme.Color(clrChannelNameBg));
         osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, Theme.Color(clrChannelNameBg));
         osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 3);
      }
      //both
      //Channel & Date
      osd->DrawEllipse  (x10, y1, x11 - 1, y2 - 1, Theme.Color(clrBackground), 1);
      osd->DrawRectangle(x10, y2, x11 - 1, y5 - 1, Theme.Color(clrBackground));
      osd->DrawEllipse  (x10, y5, x11 - 1, y6 - 1, Theme.Color(clrBackground), 4);

      osd->DrawEllipse  (x6, y1, x7 - 1, y3 - 1, Theme.Color(clrChannelDateBg), 5);
      osd->DrawEllipse  (x9, y4, x9 + lhh - 1, y6 - 1, Theme.Color(clrChannelNameBg), 5);
   }

   recordingchange = -1; // force redraw of recordingscroll
   free(date); // force date redraw
   date = NULL;
}


cString cSkinElchiDisplayChannel::CheckLogoFile(const cChannel *Channel, const char *path)
{
   if (!Channel || !Channel->Name())
      return NULL;

   cString filename;
   cString ChannelName = strreplace(strdup(Channel->Name()), '/', '~');
   
   DSYSLOG("skinelchi: CheckLogoFile ChannelName: %s", *ChannelName)

   if (ElchiConfig.LogoSearchNameFirst) {
      filename = cString::sprintf("%s/%s.%s", path, strreplace(strdup(ChannelName), ' ', '_'), "xpm"); ///TODO mem leak ?
      DSYSLOG("skinelchi: CheckLogoFile filename: %s", *filename)
      if (access(filename, F_OK) == 0) // the file exists
         return filename;
      else {
         filename = cString::sprintf("%s/%s.%s", path, *ChannelName, "xpm");
         DSYSLOG("skinelchi: CheckLogoFile filename: %s", *filename)
         if (access(filename, F_OK) == 0) // the file exists
            return filename;
         else
         {
            filename = cString::sprintf("%s/%s.%s", path, *Channel->GetChannelID().ToString(), "xpm");
            DSYSLOG("skinelchi: CheckLogoFile filename: %s", *filename)
            if (access(filename, F_OK) == 0) // the file exists
               return filename;
         }
      }
   }
   else {
      filename = cString::sprintf("%s/%s.%s", path, *Channel->GetChannelID().ToString(), "xpm");
      DSYSLOG("skinelchi: CheckLogoFile filename: %s", *filename)
      if (access(filename, F_OK) == 0) // the file exists
         return filename;
      else {
         filename = cString::sprintf("%s/%s.%s", path, strreplace(strdup(ChannelName), ' ', '_'), "xpm"); ///TODO mem leak ?
         DSYSLOG("skinelchi: CheckLogoFile filename: %s", *filename)
         if (access(filename, F_OK) == 0) // the file exists
            return filename;
         else {
            filename = cString::sprintf("%s/%s.%s", path, *ChannelName, "xpm");
            DSYSLOG("skinelchi: CheckLogoFile filename: %s", *filename)
            if (access(filename, F_OK) == 0) // the file exists
               return filename;
         }
      }
   }
   return NULL;
}


void cSkinElchiDisplayChannel::SetChannel(const cChannel *Channel, int ChannelNumber)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayChannel::SetChannel(%d,%d)", Channel ? Channel->Number():0, ChannelNumber)

   changed = true;

   int x = x10;
   if (!Channel) {
      // if no channel object is available (e.g., while entering new channel number), update at least Channelnumber
      if (ChannelNumber)
         snprintf(Channelnumber, sizeof(Channelnumber), "%d-", ChannelNumber);
      else
         Channelnumber[0] = 0;
      osd->DrawText(x3, y1, Channelnumber, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), cFont::GetFont(fontOsd), cFont::GetFont(fontOsd)->Width("00000-"));
   }
   else
   {
      if (Channel->GroupSep()) {
         snprintf(Channelnumber, sizeof(Channelnumber), " ");
         //isyslog("skinelchi: Groupsep: %s", Channel->Name());
         AudioScroll->SetText(NULL);
         video = false;
      }
      else  // Channel is not a GroupSep()
      {
         audiostring = NULL;
         AudioChanged = true;

         // clear and redraw complete display
         DrawBackground();

         AudioScroll->SetText(NULL); // updated a while later by Flush (to let signal settle)
         old_ar = ar_unknown;
         old_width = -1;
         old_height = -1;

         //  draw symbols: rec, encrypted, DD, Audio, Teletext
         x -= bmRecording->Width();
         osd->DrawBitmap(x, y4 + (y6 - y4 - bmRecording->Height()) / 2, *bmRecording, Theme.Color(rec ? clrSymbolRecFg : clrChannelSymbolOff), rec ? Theme.Color(clrSymbolRecBg) : bg);
         x -= bmEncrypted->Width() + SymbolGap;
         osd->DrawBitmap(x, y4 + (y6 - y4 - bmEncrypted->Height()) / 2, *bmEncrypted, Theme.Color(Channel->Ca() ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
         x -= bmDolbyDigital->Width() + SymbolGap;
         osd->DrawBitmap(x, y4 + (y6 - y4 - bmDolbyDigital->Height()) / 2, *bmDolbyDigital, Theme.Color(Channel->Dpid(0) ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
         x -= bmAudio->Width() + SymbolGap;
         osd->DrawBitmap(x, y4 + (y6 - y4 - bmAudio->Height()) / 2, *bmAudio, Theme.Color(Channel->Apid(1) ? clrChannelSymbolOn : clrChannelSymbolOff), bg);

         if (Channel->Vpid()) { // Channel has Video
            x -= bmTeletext->Width() + SymbolGap;
            osd->DrawBitmap(x, y4 + (y6 - y4 - bmTeletext->Height()) / 2, *bmTeletext, Theme.Color(Channel->Tpid() ? clrChannelSymbolOn : clrChannelSymbolOff), bg);

            video = true;
         }
         else // Channel has no Video
         {
            video = false;
            if (Channel->Apid(0)) {
               if (Channel->Tpid()) {
                  x -= bmTeletext->Width() + SymbolGap;
                  osd->DrawBitmap(x, y4 + (y6 - y4 - bmTeletext->Height()) / 2, *bmTeletext, Theme.Color(clrChannelSymbolOn), bg);
               }
               x -= bmRadio->Width() + SymbolGap;
               osd->DrawBitmap(x-2*(bmRadio->Width() + SymbolGap), y4 + (y6 - y4 - bmRadio->Height()) / 2, *bmRadio, Theme.Color(clrChannelSymbolOn), bg);
            }
         }

         snprintf(Channelnumber, sizeof(Channelnumber), "%d%s", ChannelNumber ? ChannelNumber : Channel->Number(), ChannelNumber ? "-" : "");

         setchannel_x = x;
         x -= bmVps->Width() + SymbolGap;
         if (ElchiConfig.showVideoInfo) {
            x -= bmAR43->Width() + SymbolGap;
         }

         if (MailIconState > 0) {
            x -= bmMail->Width() + SymbolGap;
            osd->DrawBitmap(x, y4 + (y6 - y4 - bmMail->Height()) / 2, *bmMail, Theme.Color((MailIconState > 1) ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
         }
      } // end not GroupSep()

      ChannelNameScroll->SetText(Channel->Name(), cFont::GetFont(fontSml));

      osd->DrawText(x3, y1, Channelnumber, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), cFont::GetFont(fontOsd), cFont::GetFont(fontOsd)->Width("00000-"));

      // load Logo
      if (ElchiConfig.showLogo)
      {
         if (!Channel || !Channel->Name())
            // no channel or channel without name: draw empty rectangle
            osd->DrawRectangle(0, 0, logox - 1, logoy - 1, Theme.Color(clrChannelDateBg));
         else
         {
            cString filename = NULL;
            if (!osd->GetBitmap(1) && ElchiConfig.useHQlogo && OSDHeight <= 576)
               filename = CheckLogoFile(Channel, cString::sprintf("%s/hqlogos", ElchiConfig.GetLogoBaseDir()));
            
            if (!(const char *)filename)
               filename = CheckLogoFile(Channel, cString::sprintf("%s/logos/%d", ElchiConfig.GetLogoBaseDir(), (OSDHeight >= 1080 ? 1080 : (OSDHeight >= 720) ? 720 : 576)));
            
            if (ElchiConfig.LogoMessages) {
               if (NULL != (const char *)filename)
                  isyslog("skinelchi: loading logo \"%s\"", *filename);
               else
                  esyslog("skinelchi: no logo found for \"%s\" (%s)", strreplace(strdup(Channel->Name()), '/', '~'), *Channel->GetChannelID().ToString());
            }
            cBitmap bmp(logox, logoy, 8);
            if (!(const char *)filename || !bmp.LoadXpm(filename))
               osd->DrawRectangle(0, 0, logox - 1, logoy - 1, Theme.Color(clrChannelDateBg));
            else {
               if ((bmp.Width() != logox) || (bmp.Height() != logoy))
                  esyslog("skinelchi: logo %s has wrong size (%dx%d instead of %dx%d)", *filename, bmp.Width(), bmp.Height(), logox, logoy);
               else {
                  //int colors;
                  //bmp.Colors(colors);
                  //isyslog("skinelchi SetChannel: bmp %s has %d colors, (max=%d)", Channel->Name(), colors, 1<<bmp.Bpp());
                  osd->DrawBitmap(0, 0, bmp);
               }
            }
         }
      }
   }
}

void cSkinElchiDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following)
{
   DSYSLOG2("skinelchi: SetEvents(%d,%d)", Present ? Present->EventID():0, Following ? Following->EventID():0)
   if (withInfo) {
      int offsetPresent = 0;
      int offsetFollowing = 0;
      if (!Present || !Present->StartTime()) {
         osd->DrawRectangle(x4 + Gap, y6a, x4 + Gap + ScrollWidth - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg));
         changed = true;
      }
      else {
         time_t t = time(NULL);
         int seen = max(0, min(y9 - y6a - 1, int((y9 - y6a) * double(t - Present->StartTime()) / Present->Duration())));
         osd->DrawRectangle(x4 + Gap, y6a + seen, x4 + Gap + ScrollWidth - 1, y9 - 1, Theme.Color(clrChannelTimebarRest));
         osd->DrawRectangle(x4 + Gap, y6a, x4 + Gap + ScrollWidth - 1, y6a + seen - 1, Theme.Color(clrChannelTimebarSeen));
         changed = true;
      }

      if (Present) {
         ///isyslog("SetEvents: present, hastimer=%d, IsRunning=%d", Present->HasTimer(),Present->IsRunning());
#if VDRVERSNUM < 10733
         int TimerMatch = tmNone;
#else
         eTimerMatch TimerMatch = tmNone;
#endif
         const cTimer * Timer = Timers.GetMatch(Present, &TimerMatch);
         if (Timer && Timer->Recording()) {
            ///isyslog("SetEvents: present has Timer");
            offsetPresent = bmRecording->Width() + Gap;
            if (Present->IsRunning())
               osd->DrawBitmap(x5 + 2, y6a + (lh - bmRecording->Height())/2, *bmRecording,
                               Theme.Color(clrSymbolRecFg), Theme.Color(clrSymbolRecBg));
            else
               osd->DrawBitmap(x5 + 2, y6a + (lh - bmRecording->Height())/2, *bmRecording,
                               Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTitleBg));
         }
      }

      osd->DrawText(x1 + 4, y6a, Present ? Present->GetTimeString() : (cString)NULL, Theme.Color(clrChannelEpgTimeFg), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontOsd), x4 -x1 -5);
      PresentTitleScroll->SetText(Present ? Present->Title() : NULL, cFont::GetFont(fontOsd), offsetPresent);
      PresentShortScroll->SetText(Present ? Present->ShortText() : NULL, cFont::GetFont(fontSml));

      if (Following && Following->HasTimer()) {
         offsetFollowing = bmRecording->Width() + Gap;
         osd->DrawBitmap(x5 + 2, y6a + 2*lh + (lh - bmRecording->Height())/2, *bmRecording,
                         Theme.Color(clrChannelEpgTitleBg), Theme.Color(clrChannelEpgShortText));
                         ///Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTitleBg));
      }
      osd->DrawText(x1 + 4, y6a + 2 * lh, Following ? Following->GetTimeString() : (cString)NULL, Theme.Color(clrChannelEpgTimeFg), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontOsd), x4 -x1 -5);
      FollowingTitleScroll->SetText(Following ? Following->Title() : NULL, cFont::GetFont(fontOsd), offsetFollowing);
      FollowingShortScroll->SetText(Following ? Following->ShortText() : NULL, cFont::GetFont(fontSml));
   }

   if (Present && Present->Vps()) {
      int x = setchannel_x - bmVps->Width() - SymbolGap;
      osd->DrawBitmap(x, y4 + (y6 - y4 - bmVps->Height()) / 2, *bmVps, Theme.Color(Present->Vps() - Present->StartTime() ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
      changed = true;
   }
}

void cSkinElchiDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayChannel::SetMessage(%d,%s)", (int)Type, Text)

   if (Text) { // draw msg
      if (showVolume) { // restore volume first
         osd->RestoreRegion();
         showVolume = false;
      }

      tColor msgfg = Theme.Color(clrMessageStatusFg + 2 * Type);
      tColor msgbg = Theme.Color(clrMessageStatusBg + 2 * Type);
      int startx = ElchiConfig.showLogo ? x3 : x2;

      if (!showMessage) {
         osd->SaveRegion(ElchiConfig.showLogo ? logox + 2 : x0, y4, x11 - 1, y6 - 1);
         showMessage = true;

         if (withInfo) {
            if (ElchiConfig.showLogo) {
               osd->DrawRectangle(x1, logoy+2, logox + 1, y6 - 1, msgbg);
               osd->DrawRectangle(logox + 2, y4, startx - 1, y6 - 1, msgbg);
               osd->DrawRectangle(x10, y4, x11 - 1, y6 - 1, msgbg);
            }
            else
               osd->DrawRectangle(x1, y4, startx - 1, y6 - 1, msgbg);
               osd->DrawRectangle(x10, y4, x11 - 1, y6 - 1, msgbg);
         }
         else { //withoutinfo
            if (ElchiConfig.showLogo) {
               osd->DrawRectangle(logox + 2, y4, startx - 1, y6 - 1, msgbg);
               osd->DrawRectangle(x1, logoy + 2, logox + 1, y5 - 1, msgbg);
               osd->DrawRectangle(x2, y5, logox + 1, y6 - 1, msgbg);
               osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, msgbg, 3);

            }
            else {
               osd->DrawRectangle(x2, y4, startx - 1, y6 - 1, msgbg);
               osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, msgbg);
               osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, msgbg, 3);
            }
            osd->DrawRectangle(x10, y4, x11 - 1, y5 - 1, msgbg);
            osd->DrawEllipse  (x10, y5, x11 - 1, y6 - 1, msgbg, 4);
         }
      }

      osd->DrawText(startx, y4, Text, msgfg, msgbg, cFont::GetFont(fontOsd), x10 - startx, y6 - y4, taCenter);
      changed = true;
   }
   else { // remove msg and restore previous content
      if (showMessage) {
         if (ElchiConfig.showLogo) {
            if (withInfo)
               osd->DrawRectangle(x1, logoy+2, logox + 1, y6 - 1, Theme.Color(clrChannelNameBg));
            else { //withoutinfo
               osd->DrawRectangle(x1, y4, logox + 1, y5 - 1, Theme.Color(clrChannelNameBg));
               osd->DrawRectangle(x2, y5, logox + 1, y6 - 1, Theme.Color(clrChannelNameBg));
               osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 3);
            }
         }
         osd->RestoreRegion();
         changed = true;
         showMessage = false;
      }
   }
}


void cSkinElchiDisplayChannel::Flush(void)
{
   DSYSLOG2("skinelchi: cSkinElchiDisplayChannel::Flush() is called")

   int x = setchannel_x;
   x -= bmVps->Width() + SymbolGap;
   const cFont *smallfont = cFont::GetFont(fontSml);
   const cFont *font = cFont::GetFont(fontOsd);

   if (!showMessage && !showVolume) {

      // ----------------------- video format --------------------------------
      //  draw AR symbol

      if (ElchiConfig.showVideoInfo) {
         x -= bmAR43->Width() + SymbolGap;

         if (video) { // Channel has video and is allowed to display

            cVideoInfo videoinfo;
            VideoStatus->GetVideoInfo(&videoinfo);
            if (old_ar != videoinfo.aspectratio ) {
               cBitmap *bmp = NULL;
               switch (videoinfo.aspectratio) {
                  case arHD:
                        bmp = bmARHD;
                        break;
                  case ar4_3:
                        bmp = bmAR43;
                        break;
                  case arL14_9:
                        bmp = bmARL149;
                        break;
                  case arL16_9:
                        bmp = bmARL169;
                        break;
                  case arLG16_9:
                        bmp = bmARL169;
                        break;
                  case ar16_9:
                        bmp = bmAR169;
                        break;
                  case ar221_1:
                        bmp = bmAR2211;
                        break;
                  default:
                        break;
               }

               if (bmp) 
                  osd->DrawBitmap(x, y4 + (y6 - y4 - bmp->Height()) / 2, *bmp, Theme.Color(clrChannelSymbolOn), bg);
               else
                  osd->DrawRectangle(x, y4, x + bmAR43->Width() - 1, y6 - 1, bg);

               changed = true;
               old_ar = videoinfo.aspectratio;
            }

            if (2 == ElchiConfig.showVideoInfo) {
               //isyslog("skinelchi: cSkinElchiDisplayChannel::Flush format=%d/%d x %d/%d (%s,%d)", old_width, videoinfo.width, old_height, videoinfo.height, __FILE__, __LINE__);
               if ((old_width != videoinfo.width) || (old_height != videoinfo.height)) {
                  char videoformatstring[11];
                  if ((videoinfo.width != 0) && (videoinfo.height != 0))
                     snprintf(videoformatstring, sizeof(videoformatstring), " %dx%d", videoinfo.width, videoinfo.height);
                  else
                     videoformatstring[0]=0;

                  //isyslog("skinelchi: cSkinElchiDisplayChannel::Flush format=%s (%s,%d)", videoformatstring, __FILE__, __LINE__);
                  osd->DrawText(x9 - smallfont->Width(STRVIDEOSIZE), y4 + lineOffset, videoformatstring, Theme.Color(clrChannelNameFg), Theme.Color(clrChannelNameBg),
                                 smallfont, smallfont->Width(STRVIDEOSIZE), smallfont->Height(STRVIDEOSIZE), taRight);
                  changed = true;

                  old_width = videoinfo.width;
                  old_height = videoinfo.height;
               }
            }
         }
      }

      // ----------------------- audio --------------------------------
      // build audio string (excluding Recording infos)
      cDevice *Device = cDevice::PrimaryDevice();
      cString newaudiostring;

      int numaudiotracks = Device->NumAudioTracks();
      //isyslog("skinelchi-audio: numtracks=%d", numaudiotracks);
      if (!numaudiotracks)
         newaudiostring = trVDR("No audio available!");
      else {
         const tTrackId *Track = Device->GetTrack(Device->GetCurrentAudioTrack());
         if (!Track)
         {  // kein AudioTrack, dann auch keine Anzeige
            newaudiostring = NULL;
         }
         else
         {  // AudioTrack vorh., numaudiotracks => 1
            // Get AudioChannels, Track->description and Track->Language
            int audiochannel = Device->GetAudioChannel();
            //isyslog("skinelchi-audio : %s/%d (%s) %d", Track?Track->description:"?", numaudiotracks, Track?Track->language:"?", Device->GetAudioChannel());
            if (isempty(Track->description))
               if (audiochannel > 0)
                  if (numaudiotracks > 1)
                     newaudiostring = cString::sprintf("%s /%d (%s)", Track->language, numaudiotracks, audiochannel == 1?tr("left channel"): tr("right channel"));
                  else
                     newaudiostring = cString::sprintf("%s (%s)", Track->language, audiochannel == 1?tr("left channel"): tr("right channel"));
               else
                  if (numaudiotracks > 1)
                     newaudiostring = cString::sprintf("%s /%d", Track->language, numaudiotracks);
                  else
                     newaudiostring = cString::sprintf("%s", Track->language);
            else
               if (audiochannel > 0)
                  if (numaudiotracks > 1)
                     newaudiostring = cString::sprintf("%s /%d (%s, %s)", Track?Track->description:"?", numaudiotracks, Track?Track->language:"?", audiochannel == 1?tr("left channel"): tr("right channel"));
                  else
                     newaudiostring = cString::sprintf("%s (%s, %s)", Track?Track->description:"?", Track?Track->language:"?", audiochannel == 1?tr("left channel"): tr("right channel"));
               else
                  if (numaudiotracks > 1)
                     newaudiostring = cString::sprintf("%s /%d (%s)", Track?Track->description:"?", numaudiotracks, Track?Track->language:"?");
                  else
                     newaudiostring = cString::sprintf("%s (%s)", Track?Track->description:"?", Track?Track->language:"?");
         }
      }

      //isyslog("skinelchi-audio0: new=%s old=%s", newaudiostring?newaudiostring:"0", audiostring?audiostring:"0");
      if ( (NULL == (const char*)newaudiostring) || (NULL == (const char*)audiostring) || strcmp(audiostring, (const char*)newaudiostring)) {
         AudioScroll->SetText(newaudiostring, cFont::GetFont(fontSml));
         audiostring = newaudiostring;
         //isyslog("skinelchi-audiob: %s", newaudiostring);
      } else {
         newaudiostring = NULL;
      }

      // ----------------------- recording --------------------------------
      if ((y6 != y6a) && (recordingchange != ElchiStatus->GetRecordingChange())) {
         recordingchange = ElchiStatus->GetRecordingChange();
         if (ElchiStatus->recordinglist.Count() == 0) {
            RecordingScroll->SetText(NULL);
         }
         else {
            cString recordingstring;
            recordingstring = cString::sprintf("%s: ", ElchiStatus->recordinglist.Count() == 1 ? trVDR("Recording") : trVDR("Recordings"));

            cString tmp;
            cString tmp2;
            cString tmp3;
            int lastCardIndex = ElchiStatus->recordinglist.First()->GetDevice()->CardIndex();

            for (cRecordingEntry *entry = ElchiStatus->recordinglist.First(); entry; entry = ElchiStatus->recordinglist.Next(entry)) {
               if (lastCardIndex != entry->GetDevice()->CardIndex()) {
                  tmp3 = cString::sprintf("%s DVB%d (%s)", (const char *)recordingstring, lastCardIndex, (const char *)tmp);
                  recordingstring = tmp3;
                  tmp = NULL;
               }
               tmp2 = cString::sprintf("%s%s%s", (const char *)tmp?(const char *)tmp:"", (const char *)tmp?", ":"", entry->GetName());
               tmp = tmp2;

               lastCardIndex = entry->GetDevice()->CardIndex();
            }
            tmp3 = cString::sprintf("%s DVB%d (%s)", (const char *)recordingstring, lastCardIndex, (const char *)tmp);
            recordingstring = tmp3;

            RecordingScroll->SetText(recordingstring, cFont::GetFont(fontSml));
            //isyslog("skinelchi: recordingstring=\"%s\"", (const char *)recordingstring);
         }
      } //end recording
   } // end !showMessage && !showVolume

   if (withInfo) changed |= RecordingScroll->DrawIfNeeded();

   // draw recording symbol
   bool rectemp = cRecordControls::Active();
   if (!showVolume && !showMessage && rec != rectemp) {
      rec = rectemp;
      osd->DrawBitmap(x10 - bmRecording->Width(), y4 + (y6 - y4 - bmRecording->Height()) / 2, *bmRecording, Theme.Color(rec ? clrSymbolRecFg : clrChannelSymbolOff), rec ? Theme.Color(clrSymbolRecBg) : bg);
      changed = true;
   }

   // ----------------------- volume display --------------------------------
   if (ElchiStatus->GetVolumeChange() != volumechange) {
      // volume has changed
      volumechange = ElchiStatus->GetVolumeChange();
      int y = y4 + 4;
      int h = y6 - 5;
      int startx = ElchiConfig.showLogo ? x3 : x2;
      if (!showVolume) {
         // if not already shown draw it
         // save region of channel name and symbols (2nd line)
         osd->SaveRegion(ElchiConfig.showLogo ? logox + 2 : x0, y4, x11 - 1, y6 - 1);
         osd->DrawText(startx, y1, trVDR("Volume "), Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), font, x6 - 1 - startx);

         if (withInfo) {
            if (ElchiConfig.showLogo) {
               osd->DrawRectangle(x1, logoy+2, logox + 1, y6 - 1, bg);
               osd->DrawRectangle(logox + 2, y4, x10 - 1, y6 - 1, bg);
            }
            else
               osd->DrawRectangle(x1, y4, x10 - 1, y6 - 1, bg);
         }
         else { //withoutinfo
            if (ElchiConfig.showLogo) {
               osd->DrawRectangle(logox + 2, y4, x10 - 1, y6 - 1, bg);
               osd->DrawRectangle(x1, logoy + 2, logox + 1, y5 - 1, bg);
               osd->DrawRectangle(x2, y5, logox + 1, y6 - 1, bg);
               osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, bg, 3);
            }
            else {
               osd->DrawRectangle(x2, y4, x10 - 1, y6 - 1, bg);
               osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, bg);
               osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, bg, 3);
            }
         }
         showVolume = true;
      }

      // draw volume bar and set timeout
      int p = (x10 - startx) * ElchiStatus->GetVolume() / 255;
      int xp = startx + p;
      osd->DrawRectangle(startx, y, xp - 1, h, Theme.Color(clrVolumeBarUpper));
      osd->DrawRectangle(xp, y, x10 - 1, h, Theme.Color(clrVolumeBarLower));
      changed = true;
      volumeTimer.Set(1000);
   }
   else { // volume has not changed
      if (showVolume) {
         if(volumeTimer.TimedOut()) {
            // redraw Channelnumber/name
            osd->DrawText(x3, y1, Channelnumber, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), font, font->Width("00000-"));
            // update date/time
            free(date);
            date = strdup(DayDateTime());
            osd->DrawText(x3 + font->Width("00000-"), y1 + lineOffset, date, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), smallfont, x6 - 1 - x3 - font->Width("00000-"), slh, taTop | taRight);

            if (ElchiConfig.showLogo) {
               if (withInfo)
                  osd->DrawRectangle(x1, ElchiConfig.showLogo ? logoy+2 : y4, logox + 1, y6 - 1, Theme.Color(clrChannelNameBg));
               else { //withoutinfo
                  osd->DrawRectangle(x1, ElchiConfig.showLogo ? logoy+2 : y4, logox + 1, y5 - 1, Theme.Color(clrChannelNameBg));
                  osd->DrawRectangle(x2, y5, logox + 1, y6 - 1, Theme.Color(clrChannelNameBg));
                  osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 3);
                  }
            }
            osd->RestoreRegion();
            showVolume = false;
            changed = true;
         }
      }
      else {  // !showVolume
         // update date and time
         cString newdate = DayDateTime();
         if (!date || strcmp(date, newdate)) {
            DSYSLOG2("skinelchi: show DayDateTime in DisplayChannel::Flush()")
            free(date);
            date = strdup(newdate);
            //osd->DrawText(x6 - smallfont->Width(date), y1 + lineOffset, date, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), smallfont);
            osd->DrawText(x3 + font->Width("00000-"), y1 + lineOffset, date, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), smallfont, x6 - 1 - x3 - font->Width("00000-"), slh, taTop | taRight);
            changed = true;
         }
         else
            DSYSLOG2("skinelchi: skip show DayDateTime in DisplayChannel::Flush()")

         if (!showMessage)  // !showvolume & !showMessage
            changed |= ChannelNameScroll->DrawIfNeeded();
      }
   } // volume stuff end

   // ----------------------- scrolling --------------------------------
   if (withInfo) {
      if (PresentTitleScroll) changed |= PresentTitleScroll->DrawIfNeeded();
      if (PresentShortScroll) changed |= PresentShortScroll->DrawIfNeeded();
      if (FollowingTitleScroll) changed |= FollowingTitleScroll->DrawIfNeeded();
      if (FollowingShortScroll) changed |= FollowingShortScroll->DrawIfNeeded();
   }

   if (changed) {
      DSYSLOG2("skinelchi: DisplayChannel::Flush()ing, changed")
      osd->Flush();
      changed = false;
   }
}
