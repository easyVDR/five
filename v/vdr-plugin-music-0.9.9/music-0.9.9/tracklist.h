#ifndef __TRACKLIST_H
#define __TRACKLIST_H

#include <vdr/osdbase.h>
#include "setup-mp3.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "menu.h"



class cTracklistSave : public cOsdMenu {
private:
 const char *newname;
 char data[64];
 virtual void SetHelp();  
eOSState Rename(void);
public:
 cTracklistSave(void);
 virtual eOSState ProcessKey(eKeys Key);
};

class cTrackItem : public cOsdItem {
public:
 enum eTrackSortMode { tsNumber, tsArtist, tsTitle };
private:
  static eTrackSortMode sortMode;
  cSong *song;
  cSongInfo *si;
public:
  cTrackItem(cSong *Song);
  static void SetSortMode(eTrackSortMode SortMode) { sortMode = SortMode; }
  static void IncSortMode(void) { sortMode = eTrackSortMode((sortMode == tsTitle) ? tsNumber : sortMode +1); }
  static eTrackSortMode SortMode(void) { return sortMode; }
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  virtual ~cTrackItem();
  cSong *Song(void) { return song; }
};
  
class cTrackList : public cOsdMenu {
private:
 cList<cSong> *tracklist;

 char title[64];
 
 int number, buttons;
 cTimeMs numberTimer;

 cMP3Player *mp3player;

 virtual void Move(int From, int To);
 int GetTab(int Tab);
 void Refresh(bool all = false);
 void SetHelp();
 void Rebuild(void);
 
protected:
 eOSState Number(eKeys Key); 
 eOSState GotoTrack(void);
 eOSState CopyFile(void);
 eOSState Remove(bool all);
 eOSState SavePlayList(bool burn);
 
public:
 cTrackList(cList<cSong> *Tracklist, cMP3Player *player);
 virtual ~cTrackList();
 virtual eOSState ProcessKey(eKeys Key);
};

#endif // __TRACKLIST_H

