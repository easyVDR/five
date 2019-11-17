/*
 * skinsoppalusikka.h: The 'soppalusikka' VDR skin
 *
 * $Id: $
 */

#ifndef __SKINSOPPALUSIKKA_SOPPALUSIKKA_H
#define __SKINSOPPALUSIKKA_SOPPALUSIKKA_H

#include <vdr/skins.h>

class cSkinSoppalusikka : public cSkin {
public:
  cSkinSoppalusikka();
  virtual const char *Description(void);
  virtual cSkinDisplayChannel *DisplayChannel(bool withInfoP);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool modeOnlyP);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayTracks *DisplayTracks(const char *titleP, int numTracksP, const char * const *tracksP);
  virtual cSkinDisplayMessage *DisplayMessage(void);
  };

#endif //__SKINSOPPALUSIKKA_SOPPALUSIKKA_H
