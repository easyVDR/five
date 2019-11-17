/*
 * DisplayVolume.h: Volume display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DISPLAYVOLUME_H
#define __DISPLAYVOLUME_H

#include <vdr/skins.h>
#include "scroll.h"
#include "config.h"
#include "common.h"

class cSkinElchiDisplayVolume : public cSkinDisplayVolume
{
private:
   cOsd *osd;
   cBitmap *bmMute, *bmVolume;
   static cBitmap bmAMute, bmAVolume;
   static cBitmap bmBMute, bmBVolume;
   static cBitmap bmCMute, bmCVolume;
   //const cFont *font;
   int lh, lhh;
   int mute, current;
   bool changed, showMessage;
   tOSDsize OSDsize;
   cCharScroll *MessageScroll;
public:
   cSkinElchiDisplayVolume(void);
   virtual ~cSkinElchiDisplayVolume();
   virtual void SetVolume(int Current, int Total, bool Mute);
   virtual void SetMessage(eMessageType Type, const char *Text);//it not defined in skins.h but is really called
   virtual void Flush(void);
};

#endif //__DISPLAYVOLUME_H
