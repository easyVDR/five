/*
 * DisplayReplay.c: Replay display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <vdr/menu.h>
#include <vdr/device.h>

//#define DEBUG
//#define DEBUG2
#include "DisplayReplay.h"
#include "setup.h"
#include "common.h"
#include "vdrstatus.h"
#include "videostatus.h"

#include "symbols/576/ffwd.xpm"
#include "symbols/576/ffwd1.xpm"
#include "symbols/576/ffwd2.xpm"
#include "symbols/576/ffwd3.xpm"
#include "symbols/576/ffwd4.xpm"
#include "symbols/576/ffwd5.xpm"
#include "symbols/576/ffwd6.xpm"
#include "symbols/576/ffwd7.xpm"
#include "symbols/576/ffwd8.xpm"
#include "symbols/576/frew.xpm"
#include "symbols/576/frew1.xpm"
#include "symbols/576/frew2.xpm"
#include "symbols/576/frew3.xpm"
#include "symbols/576/frew4.xpm"
#include "symbols/576/frew5.xpm"
#include "symbols/576/frew6.xpm"
#include "symbols/576/frew7.xpm"
#include "symbols/576/frew8.xpm"
#include "symbols/576/sfwd.xpm"
#include "symbols/576/sfwd1.xpm"
#include "symbols/576/sfwd2.xpm"
#include "symbols/576/sfwd3.xpm"
#include "symbols/576/sfwd4.xpm"
#include "symbols/576/sfwd5.xpm"
#include "symbols/576/sfwd6.xpm"
#include "symbols/576/sfwd7.xpm"
#include "symbols/576/sfwd8.xpm"
#include "symbols/576/srew.xpm"
#include "symbols/576/srew1.xpm"
#include "symbols/576/srew2.xpm"
#include "symbols/576/srew3.xpm"
#include "symbols/576/srew4.xpm"
#include "symbols/576/srew5.xpm"
#include "symbols/576/srew6.xpm"
#include "symbols/576/srew7.xpm"
#include "symbols/576/srew8.xpm"
#include "symbols/576/pause.xpm"
#include "symbols/576/play.xpm"
#include "symbols/576/recording.xpm"
#include "symbols/576/mail.xpm"
#include "symbols/576/ar43.xpm"
#include "symbols/576/arL149.xpm"
#include "symbols/576/arL169.xpm"
#include "symbols/576/arLG169.xpm"
#include "symbols/576/ar169.xpm"
#include "symbols/576/ar2211.xpm"
#include "symbols/576/arHD.xpm"

#include "symbols/720/Bffwd.xpm"
#include "symbols/720/Bffwd1.xpm"
#include "symbols/720/Bffwd2.xpm"
#include "symbols/720/Bffwd3.xpm"
#include "symbols/720/Bffwd4.xpm"
#include "symbols/720/Bffwd5.xpm"
#include "symbols/720/Bffwd6.xpm"
#include "symbols/720/Bffwd7.xpm"
#include "symbols/720/Bffwd8.xpm"
#include "symbols/720/Bfrew.xpm"
#include "symbols/720/Bfrew1.xpm"
#include "symbols/720/Bfrew2.xpm"
#include "symbols/720/Bfrew3.xpm"
#include "symbols/720/Bfrew4.xpm"
#include "symbols/720/Bfrew5.xpm"
#include "symbols/720/Bfrew6.xpm"
#include "symbols/720/Bfrew7.xpm"
#include "symbols/720/Bfrew8.xpm"
#include "symbols/720/Bsfwd.xpm"
#include "symbols/720/Bsfwd1.xpm"
#include "symbols/720/Bsfwd2.xpm"
#include "symbols/720/Bsfwd3.xpm"
#include "symbols/720/Bsfwd4.xpm"
#include "symbols/720/Bsfwd5.xpm"
#include "symbols/720/Bsfwd6.xpm"
#include "symbols/720/Bsfwd7.xpm"
#include "symbols/720/Bsfwd8.xpm"
#include "symbols/720/Bsrew.xpm"
#include "symbols/720/Bsrew1.xpm"
#include "symbols/720/Bsrew2.xpm"
#include "symbols/720/Bsrew3.xpm"
#include "symbols/720/Bsrew4.xpm"
#include "symbols/720/Bsrew5.xpm"
#include "symbols/720/Bsrew6.xpm"
#include "symbols/720/Bsrew7.xpm"
#include "symbols/720/Bsrew8.xpm"
#include "symbols/720/Bpause.xpm"
#include "symbols/720/Bplay.xpm"
#include "symbols/720/Brecording.xpm"
#include "symbols/720/Bmail.xpm"
#include "symbols/720/Bar43.xpm"
#include "symbols/720/BarL149.xpm"
#include "symbols/720/BarL169.xpm"
#include "symbols/720/BarLG169.xpm"
#include "symbols/720/Bar169.xpm"
#include "symbols/720/Bar2211.xpm"
#include "symbols/720/BarHD.xpm"

#include "symbols/1080/Cffwd.xpm"
#include "symbols/1080/Cffwd1.xpm"
#include "symbols/1080/Cffwd2.xpm"
#include "symbols/1080/Cffwd3.xpm"
#include "symbols/1080/Cffwd4.xpm"
#include "symbols/1080/Cffwd5.xpm"
#include "symbols/1080/Cffwd6.xpm"
#include "symbols/1080/Cffwd7.xpm"
#include "symbols/1080/Cffwd8.xpm"
#include "symbols/1080/Cfrew.xpm"
#include "symbols/1080/Cfrew1.xpm"
#include "symbols/1080/Cfrew2.xpm"
#include "symbols/1080/Cfrew3.xpm"
#include "symbols/1080/Cfrew4.xpm"
#include "symbols/1080/Cfrew5.xpm"
#include "symbols/1080/Cfrew6.xpm"
#include "symbols/1080/Cfrew7.xpm"
#include "symbols/1080/Cfrew8.xpm"
#include "symbols/1080/Csfwd.xpm"
#include "symbols/1080/Csfwd1.xpm"
#include "symbols/1080/Csfwd2.xpm"
#include "symbols/1080/Csfwd3.xpm"
#include "symbols/1080/Csfwd4.xpm"
#include "symbols/1080/Csfwd5.xpm"
#include "symbols/1080/Csfwd6.xpm"
#include "symbols/1080/Csfwd7.xpm"
#include "symbols/1080/Csfwd8.xpm"
#include "symbols/1080/Csrew.xpm"
#include "symbols/1080/Csrew1.xpm"
#include "symbols/1080/Csrew2.xpm"
#include "symbols/1080/Csrew3.xpm"
#include "symbols/1080/Csrew4.xpm"
#include "symbols/1080/Csrew5.xpm"
#include "symbols/1080/Csrew6.xpm"
#include "symbols/1080/Csrew7.xpm"
#include "symbols/1080/Csrew8.xpm"
#include "symbols/1080/Cpause.xpm"
#include "symbols/1080/Cplay.xpm"
#include "symbols/1080/Crecording.xpm"
#include "symbols/1080/Cmail.xpm"
#include "symbols/1080/Car43.xpm"
#include "symbols/1080/CarL149.xpm"
#include "symbols/1080/CarL169.xpm"
#include "symbols/1080/CarLG169.xpm"
#include "symbols/1080/Car169.xpm"
#include "symbols/1080/Car2211.xpm"
#include "symbols/1080/CarHD.xpm"

cBitmap cSkinElchiDisplayReplay::bmAFfwd(ffwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd1(ffwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd2(ffwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd3(ffwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd4(ffwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd5(ffwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd6(ffwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd7(ffwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFfwd8(ffwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew(frew_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew1(frew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew2(frew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew3(frew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew4(frew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew5(frew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew6(frew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew7(frew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmAFrew8(frew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd(sfwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd1(sfwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd2(sfwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd3(sfwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd4(sfwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd5(sfwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd6(sfwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd7(sfwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmASfwd8(sfwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew(srew_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew1(srew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew2(srew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew3(srew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew4(srew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew5(srew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew6(srew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew7(srew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmASrew8(srew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmAPause(pause_xpm);
cBitmap cSkinElchiDisplayReplay::bmAPlay(play_xpm);
cBitmap cSkinElchiDisplayReplay::bmARecording(recording_xpm);
cBitmap cSkinElchiDisplayReplay::bmAMail(mail_xpm);
cBitmap cSkinElchiDisplayReplay::bmAAR43(ar43_xpm);
cBitmap cSkinElchiDisplayReplay::bmAARL149(arL149_xpm);
cBitmap cSkinElchiDisplayReplay::bmAARL169(arL169_xpm);
cBitmap cSkinElchiDisplayReplay::bmAARLG169(arLG169_xpm);
cBitmap cSkinElchiDisplayReplay::bmAAR169(ar169_xpm);
cBitmap cSkinElchiDisplayReplay::bmAAR2211(ar2211_xpm);
cBitmap cSkinElchiDisplayReplay::bmAARHD(arHD_xpm);

cBitmap cSkinElchiDisplayReplay::bmBFfwd(Bffwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd1(Bffwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd2(Bffwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd3(Bffwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd4(Bffwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd5(Bffwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd6(Bffwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd7(Bffwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFfwd8(Bffwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew(Bfrew_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew1(Bfrew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew2(Bfrew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew3(Bfrew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew4(Bfrew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew5(Bfrew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew6(Bfrew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew7(Bfrew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmBFrew8(Bfrew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd(Bsfwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd1(Bsfwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd2(Bsfwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd3(Bsfwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd4(Bsfwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd5(Bsfwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd6(Bsfwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd7(Bsfwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSfwd8(Bsfwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew(Bsrew_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew1(Bsrew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew2(Bsrew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew3(Bsrew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew4(Bsrew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew5(Bsrew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew6(Bsrew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew7(Bsrew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmBSrew8(Bsrew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmBPause(Bpause_xpm);
cBitmap cSkinElchiDisplayReplay::bmBPlay(Bplay_xpm);
cBitmap cSkinElchiDisplayReplay::bmBRecording(Brecording_xpm);
cBitmap cSkinElchiDisplayReplay::bmBMail(Bmail_xpm);
cBitmap cSkinElchiDisplayReplay::bmBAR43(Bar43_xpm);
cBitmap cSkinElchiDisplayReplay::bmBARL149(BarL149_xpm);
cBitmap cSkinElchiDisplayReplay::bmBARL169(BarL169_xpm);
cBitmap cSkinElchiDisplayReplay::bmBARLG169(BarLG169_xpm);
cBitmap cSkinElchiDisplayReplay::bmBAR169(Bar169_xpm);
cBitmap cSkinElchiDisplayReplay::bmBAR2211(Bar2211_xpm);
cBitmap cSkinElchiDisplayReplay::bmBARHD(BarHD_xpm);

cBitmap cSkinElchiDisplayReplay::bmCFfwd(Cffwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd1(Cffwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd2(Cffwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd3(Cffwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd4(Cffwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd5(Cffwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd6(Cffwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd7(Cffwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFfwd8(Cffwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew(Cfrew_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew1(Cfrew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew2(Cfrew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew3(Cfrew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew4(Cfrew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew5(Cfrew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew6(Cfrew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew7(Cfrew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmCFrew8(Cfrew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd(Csfwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd1(Csfwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd2(Csfwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd3(Csfwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd4(Csfwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd5(Csfwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd6(Csfwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd7(Csfwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSfwd8(Csfwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew(Csrew_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew1(Csrew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew2(Csrew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew3(Csrew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew4(Csrew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew5(Csrew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew6(Csrew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew7(Csrew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmCSrew8(Csrew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmCPause(Cpause_xpm);
cBitmap cSkinElchiDisplayReplay::bmCPlay(Cplay_xpm);
cBitmap cSkinElchiDisplayReplay::bmCRecording(Crecording_xpm);
cBitmap cSkinElchiDisplayReplay::bmCMail(Cmail_xpm);
cBitmap cSkinElchiDisplayReplay::bmCAR43(Car43_xpm);
cBitmap cSkinElchiDisplayReplay::bmCARL149(CarL149_xpm);
cBitmap cSkinElchiDisplayReplay::bmCARL169(CarL169_xpm);
cBitmap cSkinElchiDisplayReplay::bmCARLG169(CarLG169_xpm);
cBitmap cSkinElchiDisplayReplay::bmCAR169(Car169_xpm);
cBitmap cSkinElchiDisplayReplay::bmCAR2211(Car2211_xpm);
cBitmap cSkinElchiDisplayReplay::bmCARHD(CarHD_xpm);

#if VDRVERSNUM < 10700
#define STRVIDEOSIZE " 000x000"
#else
#define STRVIDEOSIZE " 0000x0000"
#endif

extern cSkinElchiStatus *ElchiStatus;

// --- cSkinElchiDisplayReplay ---------------------------------------------
cSkinElchiDisplayReplay::cSkinElchiDisplayReplay(bool ModeOnly)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::cSkinElchiDisplayReplay(%s)", ModeOnly ? "'ModeOnly'" : "'nonModeOnly'")
   //isyslog("skinelchi: cSkinElchiDisplayReplay::cSkinElchiDisplayReplay(%s)", ModeOnly ? "'ModeOnly'" : "'nonModeOnly'");
   if (ElchiConfig.showVideoInfo)
      VideoStatus->Activate(true);

   modeonly = ModeOnly;
   showVolume = false;
   showMessage = false;
   volumechange = ElchiStatus->GetVolumeChange();
   rec = false;
   rectitle = NULL;
   title = NULL;
   oldCurrent = NULL;
   old_ar = ar_unknown;
   old_width = -1;
   old_height = -1;
   const cFont *font = cFont::GetFont(fontOsd);
   const cFont *smallfont = cFont::GetFont(fontSml);
   lh = font->Height();
   lhh = lh / 2;
   x0 = 0;

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

   DSYSLOG("skinelchi: OSDsize Replay %dx%d left=%d/%d top=%d/%d width=%d/%d heigth=%d/%d",
           OSDWidth, OSDHeight, OSDsize.left, Setup.OSDLeft, OSDsize.top, Setup.OSDTop, OSDsize.width, Setup.OSDWidth, OSDsize.height, Setup.OSDHeight)

   x1 = x0 + OSDsize.width;
   x2 = x1 - lhh - smallfont->Width("00:00:00.00 / 00:00:00");

   if (OSDHeight >= 1080) {
      bmFfwd      = &bmCFfwd;
      bmFfwd1     = &bmCFfwd1;
      bmFfwd2     = &bmCFfwd2;
      bmFfwd3     = &bmCFfwd3;
      bmFfwd4     = &bmCFfwd4;
      bmFfwd5     = &bmCFfwd5;
      bmFfwd6     = &bmCFfwd6;
      bmFfwd7     = &bmCFfwd7;
      bmFfwd8     = &bmCFfwd8;
      bmFrew      = &bmCFrew;
      bmFrew1     = &bmCFrew1;
      bmFrew2     = &bmCFrew2;
      bmFrew3     = &bmCFrew3;
      bmFrew4     = &bmCFrew4;
      bmFrew5     = &bmCFrew5;
      bmFrew6     = &bmCFrew6;
      bmFrew7     = &bmCFrew7;
      bmFrew8     = &bmCFrew8;
      bmSfwd      = &bmCSfwd;
      bmSfwd1     = &bmCSfwd1;
      bmSfwd2     = &bmCSfwd2;
      bmSfwd3     = &bmCSfwd3;
      bmSfwd4     = &bmCSfwd4;
      bmSfwd5     = &bmCSfwd5;
      bmSfwd6     = &bmCSfwd6;
      bmSfwd7     = &bmCSfwd7;
      bmSfwd8     = &bmCSfwd8;
      bmSrew      = &bmCSrew;
      bmSrew1     = &bmCSrew1;
      bmSrew2     = &bmCSrew2;
      bmSrew3     = &bmCSrew3;
      bmSrew4     = &bmCSrew4;
      bmSrew5     = &bmCSrew5;
      bmSrew6     = &bmCSrew6;
      bmSrew7     = &bmCSrew7;
      bmSrew8     = &bmCSrew8;
      bmRecording = &bmCRecording;
      bmMail      = &bmCMail;
      bmPause     = &bmCPause;
      bmPlay      = &bmCPlay;
      bmAR43      = &bmCAR43;
      bmARL149    = &bmCARL149;
      bmARL169    = &bmCARL169;
      bmARLG169   = &bmCARLG169;
      bmAR169     = &bmCAR169;
      bmAR2211    = &bmCAR2211;
      bmARHD      = &bmCARHD;
      Gap            = 6;
      SymbolGap      = 6;
   } else if (OSDHeight >= 720) {
      bmFfwd      = &bmBFfwd;
      bmFfwd1     = &bmBFfwd1;
      bmFfwd2     = &bmBFfwd2;
      bmFfwd3     = &bmBFfwd3;
      bmFfwd4     = &bmBFfwd4;
      bmFfwd5     = &bmBFfwd5;
      bmFfwd6     = &bmBFfwd6;
      bmFfwd7     = &bmBFfwd7;
      bmFfwd8     = &bmBFfwd8;
      bmFrew      = &bmBFrew;
      bmFrew1     = &bmBFrew1;
      bmFrew2     = &bmBFrew2;
      bmFrew3     = &bmBFrew3;
      bmFrew4     = &bmBFrew4;
      bmFrew5     = &bmBFrew5;
      bmFrew6     = &bmBFrew6;
      bmFrew7     = &bmBFrew7;
      bmFrew8     = &bmBFrew8;
      bmSfwd      = &bmBSfwd;
      bmSfwd1     = &bmBSfwd1;
      bmSfwd2     = &bmBSfwd2;
      bmSfwd3     = &bmBSfwd3;
      bmSfwd4     = &bmBSfwd4;
      bmSfwd5     = &bmBSfwd5;
      bmSfwd6     = &bmBSfwd6;
      bmSfwd7     = &bmBSfwd7;
      bmSfwd8     = &bmBSfwd8;
      bmSrew      = &bmBSrew;
      bmSrew1     = &bmBSrew1;
      bmSrew2     = &bmBSrew2;
      bmSrew3     = &bmBSrew3;
      bmSrew4     = &bmBSrew4;
      bmSrew5     = &bmBSrew5;
      bmSrew6     = &bmBSrew6;
      bmSrew7     = &bmBSrew7;
      bmSrew8     = &bmBSrew8;
      bmRecording = &bmBRecording;
      bmMail      = &bmBMail;
      bmPause     = &bmBPause;
      bmPlay      = &bmBPlay;
      bmAR43      = &bmBAR43;
      bmARL149    = &bmBARL149;
      bmARL169    = &bmBARL169;
      bmARLG169   = &bmBARLG169;
      bmAR169     = &bmBAR169;
      bmAR2211    = &bmBAR2211;
      bmARHD      = &bmBARHD;
      Gap            = 4;
      SymbolGap      = 4;
   } else {  // <  720 incl. 576
      bmFfwd      = &bmAFfwd;
      bmFfwd1     = &bmAFfwd1;
      bmFfwd2     = &bmAFfwd2;
      bmFfwd3     = &bmAFfwd3;
      bmFfwd4     = &bmAFfwd4;
      bmFfwd5     = &bmAFfwd5;
      bmFfwd6     = &bmAFfwd6;
      bmFfwd7     = &bmAFfwd7;
      bmFfwd8     = &bmAFfwd8;
      bmFrew      = &bmAFrew;
      bmFrew1     = &bmAFrew1;
      bmFrew2     = &bmAFrew2;
      bmFrew3     = &bmAFrew3;
      bmFrew4     = &bmAFrew4;
      bmFrew5     = &bmAFrew5;
      bmFrew6     = &bmAFrew6;
      bmFrew7     = &bmAFrew7;
      bmFrew8     = &bmAFrew8;
      bmSfwd      = &bmASfwd;
      bmSfwd1     = &bmASfwd1;
      bmSfwd2     = &bmASfwd2;
      bmSfwd3     = &bmASfwd3;
      bmSfwd4     = &bmASfwd4;
      bmSfwd5     = &bmASfwd5;
      bmSfwd6     = &bmASfwd6;
      bmSfwd7     = &bmASfwd7;
      bmSfwd8     = &bmASfwd8;
      bmSrew      = &bmASrew;
      bmSrew1     = &bmASrew1;
      bmSrew2     = &bmASrew2;
      bmSrew3     = &bmASrew3;
      bmSrew4     = &bmASrew4;
      bmSrew5     = &bmASrew5;
      bmSrew6     = &bmASrew6;
      bmSrew7     = &bmASrew7;
      bmSrew8     = &bmASrew8;
      bmRecording = &bmARecording;
      bmMail      = &bmAMail;
      bmPause     = &bmAPause;
      bmPlay      = &bmAPlay;
      bmAR43      = &bmAAR43;
      bmARL149    = &bmAARL149;
      bmARL169    = &bmAARL169;
      bmARLG169   = &bmAARLG169;
      bmAR169     = &bmAAR169;
      bmAR2211    = &bmAAR2211;
      bmARHD      = &bmAARHD;
      Gap            = 3;
      SymbolGap      = 3;
   }
   
   xSymbol = x1 - lh + lhh - bmFrew->Width() - SymbolGap - bmSrew->Width() - SymbolGap - bmPlay->Width() - SymbolGap - bmSfwd->Width() - SymbolGap - bmFfwd->Width();
   x3 = xRecording = xSymbol - bmRecording->Width() - SymbolGap;


   if (ElchiConfig.showVideoInfo) {
      xAR = xRecording - bmAR43->Width() - SymbolGap;  // space for AR symbol
      x3 -= bmAR43->Width() + SymbolGap;
   }

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

         if (MailIconState >= 1)
            xMail = x3 - bmMail->Width() - SymbolGap;
         x3 -= bmMail->Width() + SymbolGap;
      }
   }

   x3 -= lhh;

   x4 = x2 - 2 * lh;
   x5 = x3 - 2 * lh;

   y0 = 0;
   y1 = lh;
   y2 = 2 * lh;
   y3 = 3 * lh;

   osd = cOsdProvider::NewOsd(OSDsize.left, OSDsize.top + OSDsize.height - y3);
   tArea Areas[] = { { x0, y0, x1 - 1, y3 - 1, 4 } };
   osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));

   cBitmap *bitmap = osd->GetBitmap(0);
   if (bitmap) {
      bitmap->Reset();
      bitmap->SetColor(0, clrTransparent);
      bitmap->SetColor(1, Theme.Color(clrBackground));
/* used colors:
clrReplayCurrent
clrReplayTitleFg
clrReplayTitleBg
clrReplaySymbolOn
clrReplaySymbolOff
clrReplayProgressSeen
clrReplayProgressRest
clrReplayProgressSelected
clrReplayProgressMark
clrReplayProgressCurrent
clrReplayTotal
clrReplayModeJump
clrMessageStatusBg
clrMessageStatusFg
clrSymbolRecFg
clrSymbolRecBg
clrVolumeBarUpper
clrVolumeBarLower
clrbackground
clrTransparent
:19
*/
   }

   bg = Theme.Color(clrBackground);
   osd->DrawPixel(x3, y3, clrTransparent);
   osd->DrawPixel(x3, y3, bg);
   osd->DrawPixel(x3, y3, Theme.Color(clrReplaySymbolOn));
   osd->DrawPixel(x3, y3, Theme.Color(clrReplaySymbolOff));
   //osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);

   if (ModeOnly) {
      osd->DrawRectangle(x0, y0, x1 - 1, y2 - 1, clrTransparent);
      osd->DrawRectangle(x3, y2, x3 + lhh - 1, y3 - 1, clrTransparent);
      osd->DrawEllipse(x3, y2, x3 + lhh - 1, y3 - 1, bg, 7);
      osd->DrawRectangle(x3 + lhh, y2, x1 - lhh - 1, y3 - 1, bg);
      osd->DrawRectangle(x1 - lhh, y2, x1 - 1, y3 - 1, clrTransparent);
      osd->DrawEllipse(x1 - lhh, y2, x1 - 1, y3 - 1, bg, 5);
   }
   else {
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayCurrent));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayTitleFg));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayTitleBg));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayProgressSeen));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayProgressRest));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayProgressSelected));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayProgressMark));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayProgressCurrent));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayTotal));
      osd->DrawPixel(x3, y3, Theme.Color(clrReplayModeJump));
      osd->DrawPixel(x3, y3, Theme.Color(clrSymbolRecFg));
      osd->DrawPixel(x3, y3, Theme.Color(clrSymbolRecBg));
      osd->DrawPixel(x3, y3, Theme.Color(clrVolumeBarUpper));
      osd->DrawPixel(x3, y3, Theme.Color(clrVolumeBarLower));

      osd->DrawRectangle(x0, y0, x2 - 1, y1 - 1, clrTransparent);
      osd->DrawSlope(x4, y0, x2 - 1, y1 - 1, bg, 0);

      osd->DrawRectangle(x2, y0, x1 - lhh - 1, y1 - 1, bg);

      osd->DrawRectangle(x1 - lhh, y0, x1 - 1, y1 - lhh - 1, clrTransparent);
      osd->DrawEllipse(x1 - lhh, y0, x1 - 1, y1 - lhh - 1, bg, 1);

      osd->DrawRectangle(x0 + lhh, y1, x0 + lh - 1, y2 - 1, clrTransparent);
      osd->DrawEllipse(x0 + lhh, y1, x0 + lh - 1, y2 - 1, bg, 7);
      osd->DrawRectangle(x0 + lh, y1, x1 - lhh - 1, y2 - 1, bg);
      osd->DrawRectangle(x2, y0, x1 - lhh - 1, y1 - 1, bg);

      if (Setup.ShowReplayMode) {
         osd->DrawRectangle(x0, y2, x3 - 1, y3 - 1, clrTransparent);
         osd->DrawSlope(x5, y2, x3 - 1, y3 - 1, bg, 3);
         osd->DrawRectangle(x1 - lhh, y1 - lhh, x1 - 1, y2 + lhh, bg);
         osd->DrawRectangle(x3, y2, x1 - lhh - 1, y3 - 1, bg);
         osd->DrawRectangle(x1 - lhh, y3 - lhh, x1 - 1, y3 - 1, clrTransparent);
         osd->DrawEllipse(x1 - lhh, y3 - lhh, x1 - 1, y3 - 1, bg, 4);
      }
      else {
         osd->DrawRectangle(x1 - lhh, y1 - lhh, x1 - 1, y1 + lhh, bg);
         osd->DrawRectangle(x1 - lhh, y2 - lhh, x1 - 1, y2 - 1, clrTransparent);
         osd->DrawEllipse(x1 - lhh, y2 - lhh, x1 - 1, y2 - 1, bg, 4);
      }

      const char *separator = " / ";
      xTotalWidth = smallfont->Width("00:00:00");
      xTotal = x1 - xTotalWidth - y1 / 2;
      osd->DrawText(xTotal - smallfont->Width(separator), y0, separator,Theme.Color(clrReplayCurrent), Theme.Color(clrBackground), smallfont);
      xCurrentWidth = smallfont->Width("00:00:00.00");
      xCurrent = xTotal - smallfont->Width(separator) - xCurrentWidth;

   }
   TitleScroll = NULL;

   changed = true;
}

cSkinElchiDisplayReplay::~cSkinElchiDisplayReplay()
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::~cSkinElchiDisplayReplay()")

   if (ElchiConfig.showVideoInfo)
      VideoStatus->Activate(false);

   free(oldCurrent);

   DELETENULL(TitleScroll);
   DELETENULL(osd);
   changed = true;
}

void cSkinElchiDisplayReplay::SetTitle(const char *Title)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::SetTitle(%s)", Title)

   rectitle = Title;

   SetScrollTitle(rectitle);
}


void cSkinElchiDisplayReplay::SetScrollTitle(const char *Title)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::SetScrollTitle(%s)", Title)

   const cFont *font = cFont::GetFont(fontOsd);
   const cFont *smallfont = cFont::GetFont(fontSml);

   int w = smallfont->Width(Title);
   if(ElchiConfig.showVideoInfo > 1 && !showVolume)
      w += smallfont->Width(STRVIDEOSIZE);

   w = min(w, x4 - x0);
   int h = font->Height() - smallfont->Height();

   DELETENULL(TitleScroll);
   osd->DrawRectangle(x0, y0, x4 + 2, y1 - h + 2, clrTransparent);
   TitleScroll = new cCharScroll(x0, y0, w, smallfont->Height(), osd,
                   Theme.Color(clrReplayTitleFg), Theme.Color(clrReplayTitleBg));
   TitleScroll->SetText(Title, smallfont);

   osd->DrawRectangle(w, y0 + Gap, w + Gap - 1, y1 - h + Gap - 1, bg);
   osd->DrawRectangle(x0 + Gap, y1 - h, w - 1, y1 - h + Gap - 1, bg);
   changed = true;
}

void cSkinElchiDisplayReplay::SetMode(bool Play, bool Forward, int Speed)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::SetMode(%s,%s,Speed=%d)", Play ? "'Play'" : "'nonPlay'", Forward ? "'Forward'" : "'nonForward'", Speed)

   if (Setup.ShowReplayMode) {
      tColor clr_on  = Theme.Color(clrReplaySymbolOn);
      tColor clr_off = Theme.Color(clrReplaySymbolOff);

      int x = xSymbol;
      int y = y3 - y2;
      if (Speed < -1)
         Speed = -1;
      if (Speed > 9)
         Speed = 9;

      if (MailIconState > 0) {
         osd->DrawBitmap(xMail, y2 + (y - bmMail->Height()) / 2, *bmMail, (MailIconState > 1) ? clr_on : clr_off, bg);
      }

      rec = cRecordControls::Active();
      osd->DrawBitmap(xRecording, y2 + (y - bmRecording->Height()) / 2, *bmRecording, rec ? Theme.Color(clrSymbolRecFg) : clr_off, rec ? Theme.Color(clrSymbolRecBg) : bg);

      cBitmap *bm = NULL;
      if ((Speed > -1) && Play && !Forward) {
         switch (Speed) {
            case 0:
            case 1: bm = bmFrew; break;
            case 2: bm = bmFrew1; break;
            case 3: bm = bmFrew2; break;
            case 4: bm = bmFrew3; break;
            case 5: bm = bmFrew4; break;
            case 6: bm = bmFrew5; break;
            case 7: bm = bmFrew6; break;
            case 8: bm = bmFrew7; break;
            case 9: bm = bmFrew8; break;
         }
         osd->DrawBitmap(x, y2 + (y - bm->Height()) / 2, *bm, clr_on, bg);
      }
      else
         osd->DrawBitmap(x, y2 + (y - bmFrew->Height()) / 2, *bmFrew, clr_off, bg);
      x += bmFrew->Width() + SymbolGap;

      if ((Speed > -1) && !Play && !Forward) {
         switch (Speed) {
            case 0:
            case 1: bm = bmSrew; break;
            case 2: bm = bmSrew1; break;
            case 3: bm = bmSrew2; break;
            case 4: bm = bmSrew3; break;
            case 5: bm = bmSrew4; break;
            case 6: bm = bmSrew5; break;
            case 7: bm = bmSrew6; break;
            case 8: bm = bmSrew7; break;
            case 9: bm = bmSrew8; break;
         }
         osd->DrawBitmap(x, y2 + (y - bm->Height()) / 2, *bm, clr_on, bg);
      }
      else
         osd->DrawBitmap(x, y2 + (y - bmSrew->Height()) / 2, *bmSrew, clr_off, bg);
      x += bmSrew->Width() + SymbolGap;

      if (Speed == -1)
         osd->DrawBitmap(x, y2 + (y - bmPlay->Height()) / 2, Play ? *bmPlay : *bmPause, clr_on, bg);
      else
         osd->DrawBitmap(x, y2 + (y - bmPlay->Height()) / 2, *bmPlay, clr_off, bg);
      x += bmPlay->Width() + SymbolGap;

      if ((Speed > -1) && !Play && Forward) {
         switch (Speed) {
            case 0: 
            case 1: bm = bmSfwd; break;
            case 2: bm = bmSfwd1; break;
            case 3: bm = bmSfwd2; break;
            case 4: bm = bmSfwd3; break;
            case 5: bm = bmSfwd4; break;
            case 6: bm = bmSfwd5; break;
            case 7: bm = bmSfwd6; break;
            case 8: bm = bmSfwd7; break;
            case 9: bm = bmSfwd8; break;
         }
         osd->DrawBitmap(x, y2 + (y - bm->Height()) / 2, *bm, clr_on, bg);
      }
      else
         osd->DrawBitmap(x, y2 + (y - bmSfwd->Height()) / 2, *bmSfwd, clr_off, bg);
      x += bmSfwd->Width() + SymbolGap;

      if ((Speed > -1) && Play && Forward) {
         switch (Speed) {
            case 0: 
            case 1: bm = bmFfwd; break;
            case 2: bm = bmFfwd1; break;
            case 3: bm = bmFfwd2; break;
            case 4: bm = bmFfwd3; break;
            case 5: bm = bmFfwd4; break;
            case 6: bm = bmFfwd5; break;
            case 7: bm = bmFfwd6; break;
            case 8: bm = bmFfwd7; break;
            case 9: bm = bmFfwd8; break;
         }
         osd->DrawBitmap(x, y2 + (y - bm->Height()) / 2, *bm, clr_on, bg);
      }
      else
         osd->DrawBitmap(x, y2 + (y - bmFfwd->Height()) / 2, *bmFfwd, clr_off, bg);
      changed = true;
   }
   //isyslog("skinelchi: cSkinElchiDisplayReplay::SetMode end");
}

void cSkinElchiDisplayReplay::SetProgress(int Current, int Total)
{
   DSYSLOG2("skinelchi: cSkinElchiDisplayReplay::SetProgress(%d,%d)", Current, Total)
   if (!showVolume) {
      cProgressBar pb(x1 - x0 - y1 - y1 / 2, y2 - y1 - 8, Current, Total, marks, Theme.Color(clrReplayProgressSeen), Theme.Color(clrReplayProgressRest), Theme.Color(clrReplayProgressSelected), Theme.Color(clrReplayProgressMark), Theme.Color(clrReplayProgressCurrent));
      osd->DrawBitmap(x0 + y1, y1 + 4, pb);
      changed = true;
   }
}

void cSkinElchiDisplayReplay::SetCurrent(const char *Current)
{
   if (!oldCurrent || strcmp(oldCurrent, Current)) {
      DSYSLOG2("skinelchi: cSkinElchiDisplayReplay::SetCurrent(%s)", Current)
      int oldLength = oldCurrent ? strlen(oldCurrent) : 0;
      int Length = strlen(Current);
      free(oldCurrent);
      oldCurrent = strdup(Current);

      if (oldLength > Length)
         osd->DrawRectangle(xCurrent, y0, xCurrent + xCurrentWidth - xTotalWidth - 1, y1 - 1, Theme.Color(clrBackground));
  
      osd->DrawText(Length > 8 ? xCurrent : xCurrent + xCurrentWidth - xTotalWidth, y0, Current, Theme.Color(clrReplayCurrent), Theme.Color(clrBackground), cFont::GetFont(fontSml), Length > 8 ? xCurrentWidth : xTotalWidth, 0, taLeft);

      changed = true;
   }
   else
      DSYSLOG2("skinelchi: skipped cSkinElchiDisplayReplay::SetCurrent(%s)", Current)
}

void cSkinElchiDisplayReplay::SetTotal(const char *Total)
{
   DSYSLOG2("skinelchi: cSkinElchiDisplayReplay::SetTotal(%s)", Total)

   osd->DrawText(xTotal, y0, Total, Theme.Color(clrReplayTotal), Theme.Color(clrBackground), cFont::GetFont(fontSml), xTotalWidth, 0 ,taLeft);
   changed = true;
}

void cSkinElchiDisplayReplay::SetJump(const char *Jump)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::SetJump(%s)", Jump)
   osd->DrawRectangle(x0, y2, x5 - 1, y3 - 1, clrTransparent);
   if (Jump) {
      const cFont *font = cFont::GetFont(fontOsd);
      int w = font->Width(Jump);
      osd->DrawText(x5 - w, y2, Jump, Theme.Color(clrReplayModeJump), Theme.Color(clrBackground), font, w);
   }
   changed = true;
}

void cSkinElchiDisplayReplay::SetMessage(eMessageType Type, const char *Text)
{
   DSYSLOG("skinelchi: cSkinElchiDisplayReplay::SetMessage(%d,%s)", (int)Type, Text)
   if (Text) {
      if (showVolume) {  // destroy Volume Display first
         showVolume = false;
         osd->RestoreRegion();
         if (modeonly) {
            osd->DrawRectangle(x5, y2, x3 + lhh - 1, y3 - 1, clrTransparent);
            osd->DrawEllipse(x3, y2, x3 + lhh - 1, y3 - 1, bg, 7);
            osd->DrawRectangle(x1 - lhh, y2, x1 - 1, y3 - 1, clrTransparent);
            osd->DrawEllipse(x1 - lhh, y2, x1 - 1, y3 - 1, bg, 5);
         }
         else {
            SetScrollTitle(rectitle);
            old_width = -1;
         }
      }

      tColor msbg = Theme.Color(clrMessageStatusBg + 2 * Type);
      int y = y3 - 1;
      if (!showMessage) {
         osd->SaveRegion(x0, y2, x1 - 1, y);
         showMessage = true;
      }
      osd->DrawEllipse(x0, y2, lhh - 1, y, msbg, 7);
      osd->DrawText(x0 + lhh, y2, Text, Theme.Color(clrMessageStatusFg + 2 * Type), msbg, cFont::GetFont(fontOsd), x1 - x0 - 2 * lhh, y3 - y2, taCenter);
      osd->DrawEllipse(x1 - lhh, y2, x1 - 1, y, msbg, 5);
      changed = true;
   }
   else {
      if (showMessage) {
         osd->RestoreRegion();
         showMessage = false;
         changed = true;
      }
   }
}

void cSkinElchiDisplayReplay::Flush(void)
{
   DSYSLOG2("skinelchi: cSkinElchiDisplayReplay::Flush()")

   // update volume if changed
   if (!showMessage && (ElchiStatus->GetVolumeChange() != volumechange)) {
      volumechange = ElchiStatus->GetVolumeChange();
      int x = x0 + y1;
      int w = x1 - y1 / 2;
      int y = y1 + 4;
      int h = y2 - 5;
      if (!showVolume) {
         showVolume = true;
         if (modeonly) {
            osd->SaveRegion(x0, y0, x1 - 1, y2 - 1);
            osd->DrawRectangle(x1 - lhh, y1, x1 - 1, y2 - lhh - 1, clrTransparent);
            osd->DrawEllipse(x1 - lhh, y1, x1 - 1, y2 - lhh - 1, bg, 1);
            osd->DrawRectangle(x0 + lhh, y1, x0 + lh - 1, y2 - 1, clrTransparent);
            osd->DrawEllipse(x0 + lhh, y1, x0 + lh - 1, y2 - 1, bg, 7);
            osd->DrawRectangle(x0 + lh, y1, x1 - lhh - 1, y2 - 1, bg);
            osd->DrawRectangle(x0, y2, x3 - 1, y3 - 1, clrTransparent);
            osd->DrawSlope(x5, y2, x3 - 1, y3 - 1, bg, 3);
            osd->DrawRectangle(x3, y2, x3 + lhh - 1, y3 - 1, bg);
            osd->DrawRectangle(x1 - lhh, y2 - lhh, x1 - 1, y2 + lhh, bg);
         }
         else
            osd->SaveRegion(x, y, w, h);

         //isyslog("skinelchi:Replay (%s:%d)",__FILE__,__LINE__);
         SetScrollTitle(trVDR("Volume "));
      }
      int p = (w - x) * ElchiStatus->GetVolume() / 255;
      int xp = x + p;
      osd->DrawRectangle(x, y, xp - 1, h, Theme.Color(clrVolumeBarUpper));
      osd->DrawRectangle(xp, y, w - 1, h, Theme.Color(clrVolumeBarLower));
      changed = true;
      DSYSLOG2("skinelchi: Replay Flush() (%s:%d)",__FILE__,__LINE__)
      volumeTimer.Set(1000);
   }
   else {
      if (showVolume && volumeTimer.TimedOut()) {
         showVolume = false;
         osd->RestoreRegion();
         if (modeonly) {
            osd->DrawRectangle(x5, y2, x3 + lhh - 1, y3 - 1, clrTransparent);
            osd->DrawEllipse(x3, y2, x3 + lhh - 1, y3 - 1, bg, 7);
            osd->DrawRectangle(x1 - lhh, y2, x1 - 1, y3 - 1, clrTransparent);
            osd->DrawEllipse(x1 - lhh, y2, x1 - 1, y3 - 1, bg, 5);
         }
         else {
            //DSYSLOG2("skinelchi: Replay Flush() (%s:%d)",__FILE__,__LINE__)
            SetScrollTitle(rectitle);
            old_width = -1;
         }
         changed = true;
         DSYSLOG2("skinelchi: Replay Flush() (%s:%d)",__FILE__,__LINE__)
      }
   }

   // show video format info (size in title and AR-bitmap)
   if (ElchiConfig.showVideoInfo) {
      cVideoInfo videoinfo;

      if (VideoStatus->GetVideoInfo(&videoinfo) && Setup.ShowReplayMode && !showMessage && old_ar != videoinfo.aspectratio) {
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
            osd->DrawBitmap(xAR, y2 + (y3 - y2 - bmp->Height()) / 2, *bmp, Theme.Color(clrReplaySymbolOn), Theme.Color(clrBackground));
         else
            osd->DrawRectangle(xAR, y2 + (y3 - y2 - bmARHD->Height()) / 2, bmARHD->Width() - 1, bmARHD->Height() - 1, Theme.Color(clrBackground));

         changed = true;
         DSYSLOG2("skinelchi: Replay Flush() (%s:%d)",__FILE__,__LINE__)
         old_ar = videoinfo.aspectratio;
      }

      if (ElchiConfig.showVideoInfo == 2 && !modeonly && !showVolume) {
         if ((old_width != videoinfo.width) || (old_height != videoinfo.height)) {
            if (videoinfo.width && videoinfo.height) {
               title = cString::sprintf("%s %dx%d", (const char *)rectitle, videoinfo.width, videoinfo.height);
            }
            else {
               title = rectitle;
            }
            //isyslog("skinelchi:Replay %dx%d (%s:%d)", videoinfo.width, videoinfo.height, __FILE__,__LINE__);
            TitleScroll->SetText(title, cFont::GetFont(fontSml));
            old_width = videoinfo.width;
            old_height = videoinfo.height;
            changed = true;
            DSYSLOG2("skinelchi: Replay Flush() (%s:%d)",__FILE__,__LINE__)
         }
      }
   }  //end videoinfo

   if (TitleScroll) changed |= TitleScroll->DrawIfNeeded();

   // update "rec" symbol
   if (Setup.ShowReplayMode && !showMessage) {
      bool rectemp = cRecordControls::Active();
      if (rec != rectemp) {
         rec = rectemp;
         osd->DrawBitmap(xRecording, y2 + (y3 - y2 - bmRecording->Height()) / 2, *bmRecording, rec ? Theme.Color(clrSymbolRecFg) : Theme.Color(clrReplaySymbolOff), rec ? Theme.Color(clrSymbolRecBg) : bg);
         changed = true;
         DSYSLOG2("skinelchi: Replay Flush() (%s:%d)",__FILE__,__LINE__)
      }
   }

   if (changed) {
      DSYSLOG2("skinelchi: cSkinElchiDisplayReplay::Flush()ing!")
      osd->Flush();
      changed = false;
   }
   //isyslog("skinelchi: cSkinElchiDisplayReplay::Flush() end");
}
