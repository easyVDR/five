/*
 * DisplayTracks.h: Audio track display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DISPLAYTRACKS_H
#define __DISPLAYTRACKS_H

#include <vdr/skins.h>
#include "scroll.h"
#include "config.h"

class cSkinElchiDisplayTracks : public cSkinDisplayTracks
{
private:
   cOsd *osd;
   cBitmap *bmAudioLeft, *bmAudioRight, *bmAudioStereo, *bmArrowDown, *bmArrowUp;
   static cBitmap bmAAudioLeft, bmAAudioRight, bmAAudioStereo, bmAArrowDown, bmAArrowUp;
   static cBitmap bmBAudioLeft, bmBAudioRight, bmBAudioStereo, bmBArrowDown, bmBArrowUp;
   static cBitmap bmCAudioLeft, bmCAudioRight, bmCAudioStereo, bmCArrowDown, bmCArrowUp;
   int x0, x1, x2, x3;
   int y0, y1, y2, y3, y4, y5;
   int lh, lhh;
   int currentIndex;
   int index;
   int offset;
   int numTracks;
   bool changed, showMessage;
   void SetItem(const char *Text, int Index, bool Current);
   cCharScroll *MessageScroll;
public:
   cSkinElchiDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
   virtual ~cSkinElchiDisplayTracks();
   virtual void SetTrack(int Index, const char * const *Tracks);
   virtual void SetAudioChannel(int AudioChannel);
   virtual void SetMessage(eMessageType Type, const char *Text);//it not defined in skins.h but is really called
   virtual void Flush(void);
   virtual int MaxItems(void);
   virtual void Clear(void);
};

#endif //__DISPLAYTRACKS_H
