#ifndef __MENUBROWSE_H
#define __MENUBROWSE_H

//#include <vdr/menuitems.h>
#include "data-mp3.h"
#include "menu.h"
//#include "mp3control.h"

//---- cMenuID3Info ------------------------------------------------------------
class cMenuID3Info : public cOsdMenu {
private:
  cOsdItem *Item(const char *name, const char *text);
  cOsdItem *Item(const char *name, const char *format, const float num);
  void Build(cSongInfo *info, const char *name);
public:
  cMenuID3Info(cSong *song);
  cMenuID3Info(cSongInfo *si, const char *name);
  virtual eOSState ProcessKey(eKeys Key);
};


//--- cMenuInstantBrowse -------------------------------------------------------
class cMenuInstantBrowse : public cMenuBrowse {
private:
  const char *selecttext, *alltext;
  bool playnow;
  virtual void SetButtons(void);
  virtual eOSState ID3Info(void);
  virtual eOSState PlayNow(void);
public:
  cMenuInstantBrowse(cFileSource *Source, const char *Selecttext, const char *Alltext);
  ~cMenuInstantBrowse(void);
  virtual eOSState ProcessKey(eKeys Key);
};

// --- cMenuPlayListItem -------------------------------------------------------
class cMenuPlayListItem : public cOsdItem {
  private:
  bool showID3;
  cSong *song;
public:
  cMenuPlayListItem(cSong *Song, bool showid3);
  cSong *Song(void) { return song; }
  virtual void Set(void);
  void Set(bool showid3);
};

// --- cMenuPlayList -----------------------------------------------------------
class cMenuPlayList : public cOsdMenu {
private:
  cPlayList *playlist;
  bool browsing, showid3;
  void Buttons(void);
  void Refresh(bool all = false);
  void Add(void);
  virtual void Move(int From, int To);
  eOSState Remove(void);
  eOSState ShowID3(void);
  eOSState ID3Info(void);
public:
  cMenuPlayList(cPlayList *Playlist);
  virtual eOSState ProcessKey(eKeys Key);
};

// --- cPlaylistRename --------------------------------------------------------
class cPlaylistRename : public cOsdMenu {
private:
  static char *newname;
  const char *oldname;
  char data[64];
public:
  cPlaylistRename(const char *Oldname);
  virtual eOSState ProcessKey(eKeys Key);
  static const char *GetNewname(void) { return newname; }
};


// --- cMenuMP3Item -----------------------------------------------------
class cMenuMP3Item : public cOsdItem {
  private:
  cPlayList *playlist;
  virtual void Set(void);
public:
  cMenuMP3Item(cPlayList *PlayList);
  cPlayList *List(void) { return playlist; }
};

// --- cMenuMP3 --------------------------------------------------------

class cMenuMP3 : public cOsdMenu {
private:
  cPlayLists *lists;
  bool renaming, sourcing, instanting;
  int buttonnum;
  eOSState Play(void);
  eOSState Edit(void);
  eOSState New(void);
  eOSState Convert(void);
  eOSState Delete(void);
  eOSState Rename(bool second);
  eOSState Source(bool second);
  eOSState Instant(bool second);
  void ScanLists(void);
  eOSState SetButtons(int num);
public:
  cMenuMP3(void);
  ~cMenuMP3(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif // __MENUBROWSE_H
