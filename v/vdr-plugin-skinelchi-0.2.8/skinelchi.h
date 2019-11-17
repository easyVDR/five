/*
 * skinelchi.h: 'Elchi' skin for the Video Disk Recorder
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINELCHI_H
#define __SKINELCHI_H

#include <vdr/skins.h>
#include "setup.h"

extern const char *OSDSKIN;
extern int SKINVERSION;

class cSkinElchi : public cSkin {
public:
   cSkinElchi(void);
   virtual const char *Description(void);
   virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
   virtual cSkinDisplayMenu *DisplayMenu(void);
   virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
   virtual cSkinDisplayVolume *DisplayVolume(void);
   virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
   virtual cSkinDisplayMessage *DisplayMessage(void);
};

#endif //__SKINELCHI_H
