#ifndef __NOPACITY_H
#define __NOPACITY_H

#include <vdr/skins.h>
#include <vdr/videodir.h>
#include "imagecache.h"

class cNopacityDisplayMenu;

class cNopacity : public cSkin {
private:
  cNopacityDisplayMenu *displayMenu;
  cImageCache *imgCache;
  void ReloadCaches(void);
public:
  cNopacity(cImageCache *imgCache);
  virtual const char *Description(void);
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual cSkinDisplayMessage *DisplayMessage(void);
  };

#endif //__NOPACITY_H
