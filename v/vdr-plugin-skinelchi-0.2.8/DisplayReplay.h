/*
 * DisplayReplay.h: Replay display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DISPLAYREPLAY_H
#define __DISPLAYREPLAY_H

#include <vdr/skins.h>
#include <vdr/tools.h>
#include "scroll.h"
#include "config.h"
#include "videostatus.h"

class cSkinElchiDisplayReplay : public cSkinDisplayReplay
{
private:
   cOsd *osd;
   cBitmap        *bmFfwd, *bmFfwd1, *bmFfwd2, *bmFfwd3, *bmFfwd4, *bmFfwd5, *bmFfwd6, *bmFfwd7, *bmFfwd8,
                  *bmFrew, *bmFrew1, *bmFrew2, *bmFrew3, *bmFrew4, *bmFrew5, *bmFrew6, *bmFrew7, *bmFrew8,
                  *bmSfwd, *bmSfwd1, *bmSfwd2, *bmSfwd3, *bmSfwd4, *bmSfwd5, *bmSfwd6, *bmSfwd7, *bmSfwd8,
                  *bmSrew, *bmSrew1, *bmSrew2, *bmSrew3, *bmSrew4, *bmSrew5, *bmSrew6, *bmSrew7, *bmSrew8,
                  *bmRecording, *bmMail, *bmPause, *bmPlay,
                  *bmAR43, *bmARL149, *bmARL169, *bmARLG169, *bmAR169, *bmAR2211, *bmARHD;
   static cBitmap bmAFfwd, bmAFfwd1, bmAFfwd2, bmAFfwd3, bmAFfwd4, bmAFfwd5, bmAFfwd6, bmAFfwd7, bmAFfwd8,
                  bmAFrew, bmAFrew1, bmAFrew2, bmAFrew3, bmAFrew4, bmAFrew5, bmAFrew6, bmAFrew7, bmAFrew8,
                  bmASfwd, bmASfwd1, bmASfwd2, bmASfwd3, bmASfwd4, bmASfwd5, bmASfwd6, bmASfwd7, bmASfwd8,
                  bmASrew, bmASrew1, bmASrew2, bmASrew3, bmASrew4, bmASrew5, bmASrew6, bmASrew7, bmASrew8,
                  bmARecording, bmAMail, bmAPause, bmAPlay,
                  bmAAR43, bmAARL149, bmAARL169, bmAARLG169, bmAAR169, bmAAR2211, bmAARHD;
   static cBitmap bmBFfwd, bmBFfwd1, bmBFfwd2, bmBFfwd3, bmBFfwd4, bmBFfwd5, bmBFfwd6, bmBFfwd7, bmBFfwd8,
                  bmBFrew, bmBFrew1, bmBFrew2, bmBFrew3, bmBFrew4, bmBFrew5, bmBFrew6, bmBFrew7, bmBFrew8,
                  bmBSfwd, bmBSfwd1, bmBSfwd2, bmBSfwd3, bmBSfwd4, bmBSfwd5, bmBSfwd6, bmBSfwd7, bmBSfwd8,
                  bmBSrew, bmBSrew1, bmBSrew2, bmBSrew3, bmBSrew4, bmBSrew5, bmBSrew6, bmBSrew7, bmBSrew8,
                  bmBRecording, bmBMail, bmBPause, bmBPlay,
                  bmBAR43, bmBARL149, bmBARL169, bmBARLG169, bmBAR169, bmBAR2211, bmBARHD;
   static cBitmap bmCFfwd, bmCFfwd1, bmCFfwd2, bmCFfwd3, bmCFfwd4, bmCFfwd5, bmCFfwd6, bmCFfwd7, bmCFfwd8,
                  bmCFrew, bmCFrew1, bmCFrew2, bmCFrew3, bmCFrew4, bmCFrew5, bmCFrew6, bmCFrew7, bmCFrew8,
                  bmCSfwd, bmCSfwd1, bmCSfwd2, bmCSfwd3, bmCSfwd4, bmCSfwd5, bmCSfwd6, bmCSfwd7, bmCSfwd8,
                  bmCSrew, bmCSrew1, bmCSrew2, bmCSrew3, bmCSrew4, bmCSrew5, bmCSrew6, bmCSrew7, bmCSrew8,
                  bmCRecording, bmCMail, bmCPause, bmCPlay,
                  bmCAR43, bmCARL149, bmCARL169, bmCARLG169, bmCAR169, bmCAR2211, bmCARHD;
   tColor bg;
//#ifdef VIDEOSTATUS
   char *oldtitlestatus;
//#endif
   int x0, x1, x2, x3, x4, x5;
   int y0, y1, y2, y3;
   int lh, lhh;
   int xAR, xSymbol, xRecording, xMail;
   int xCurrent, xTotal;
   int xCurrentWidth, xTotalWidth;
   bool rec, modeonly, changed, showMessage, showVolume;
   cTimeMs volumeTimer;
   int volumechange;
   char *oldCurrent;
   cString title;
   cString rectitle;
   int old_width;
   int old_height;
   eAspectRatio old_ar;

   cCharScroll *TitleScroll;
   int MailIconState; // 0 = no icon, 1 = no mail, 2 = new mail
   int Gap;
   int SymbolGap;
   void SetScrollTitle(const char *Title);

public:
   cSkinElchiDisplayReplay(bool ModeOnly);
   virtual ~cSkinElchiDisplayReplay();
   virtual void SetTitle(const char *Title);
   virtual void SetMode(bool Play, bool Forward, int Speed);
   virtual void SetProgress(int Current, int Total);
   virtual void SetCurrent(const char *Current);
   virtual void SetTotal(const char *Total);
   virtual void SetJump(const char *Jump);
   virtual void SetMessage(eMessageType Type, const char *Text);
   virtual void Flush(void);
};

#endif //__DISPLAYREPLAY_H
