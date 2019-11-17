/*
 * DisplayChannel.h: Channel display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DISPLAYCHANNEL_H
#define __DISPLAYCHANNEL_H

#include <vdr/skins.h>

#include <vdr/device.h>

#include "scroll.h"
#include "config.h"
#include "videostatus.h"


class cSkinElchiDisplayChannel : public cSkinDisplayChannel
{
private:
   cOsd *osd;
   int logox, logoy;
   int x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
   int y0, y1, y2, y3, y4, y5, y6, y6a, y7, y8, y9;
   bool withInfo, rec, showMessage, showVolume, sleeptimermessage;
   int lh, slh, lhh, lineOffset;
   tColor bg;
   char Channelnumber[6];
   cTimeMs volumeTimer;
   int volumechange;
   int recordingchange;
   bool changed;
   char *date;
   cBitmap *bmVps, *bmTeletext, *bmRadio, *bmAudio, *bmDolbyDigital, *bmEncrypted, *bmRecording, *bmMail,
                  *bmAR43, *bmARL149, *bmARL169, *bmARLG169, *bmAR169, *bmAR2211, *bmARHD;
   static cBitmap bmAVps, bmATeletext, bmARadio, bmAAudio, bmADolbyDigital, bmAEncrypted, bmARecording, bmAMail,
                  bmAAR43, bmAARL149, bmAARL169, bmAARLG169, bmAAR169, bmAAR2211, bmAARHD;
   static cBitmap bmBVps, bmBTeletext, bmBRadio, bmBAudio, bmBDolbyDigital, bmBEncrypted, bmBRecording, bmBMail,
                  bmBAR43, bmBARL149, bmBARL169, bmBARLG169, bmBAR169, bmBAR2211, bmBARHD;
   static cBitmap bmCVps, bmCTeletext, bmCRadio, bmCAudio, bmCDolbyDigital, bmCEncrypted, bmCRecording, bmCMail,
                  bmCAR43, bmCARL149, bmCARL169, bmCARLG169, bmCAR169, bmCAR2211, bmCARHD;
   cString audiostring;
   bool AudioChanged;
   bool video;
   int old_width;
   int old_height;
   int OSDHeight;
   eAspectRatio old_ar;
   int setchannel_x;
   cCharScroll *AudioScroll;
   cCharScroll *RecordingScroll;
   cCharScroll *ChannelNameScroll;
   cCharScroll *PresentTitleScroll;
   cCharScroll *PresentShortScroll;
   cCharScroll *FollowingTitleScroll;
   cCharScroll *FollowingShortScroll;
   int MailIconState; // 0 = no icon, 1 = no mail, 2 = new mail
   int Gap;
   int SymbolGap;
   int ScrollWidth;
   void DrawBackground();
   cString CheckLogoFile(const cChannel *Channel, const char *path);
public:
   cSkinElchiDisplayChannel(bool WithInfo);
   virtual ~cSkinElchiDisplayChannel();
   virtual void SetChannel(const cChannel *Channel, int Number);
   virtual void SetEvents(const cEvent *Present, const cEvent *Following);
   virtual void SetMessage(eMessageType Type, const char *Text);
   virtual void Flush(void);
};

#endif //__DISPLAYCHANNEL_H
