#ifndef __MP3ID3_H
#define __MP3ID3_H

#include <vdr/osdbase.h>
#include "data-mp3.h"

class cMP3SearchID3Info : public cOsdMenu {
private:
  cOsdItem *Item(const char *name, const char *text);
  cOsdItem *Item(const char *name, const char *format, const float num);
  void Build(cSongInfo *info, const char *name);
public:
  cMP3SearchID3Info(cSong *song);
  virtual ~cMP3SearchID3Info();
  cMP3SearchID3Info(cSongInfo *si, const char *name);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif // __MP3ID3_H
