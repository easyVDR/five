#ifndef __RATING_H
#define __RATING_H

#include <vdr/osdbase.h>
#include "setup-mp3.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "menu.h"

class cFileSources;
class cFileSource;
class cFileObj;
class cFileObjItem;

class cMP3Rating : public cOsdMenu {
private:
 cOsdItem *Item(const char *text);
 cSong *Song;
 char Artist[64];
 char Album[64];
 char Title[64];
 char Genre[64];
 int Year;
 void SetHelp();
 void GetRatingText();
 eOSState SetRating(void);
public:
 cMP3Rating(cSong *current);
 virtual ~cMP3Rating();
 virtual eOSState ProcessKey(eKeys Key);
};
  
#endif // __RATING_H
