#ifndef __SEARCH_H
#define __SEARCH_H

#include <string>

#include <vdr/osdbase.h>
#include "setup-mp3.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "menu.h"

class cFileSources;
class cFileSource;
class cFileObj;
class cFileObjItem;

class cMP3SearchBrowseItem : public cOsdItem {
private:
 cFileObj *item;
 virtual void SetID3(void);
 int idx;
 int total;
 int bitrate;
 const char *artist;
 const char *title;
public:
 cMP3SearchBrowseItem(cFileObj *Item, int index, const char *Artist, const char *Title, const char *Bitrate, const char *Total);
 cFileObj *Item(void) {return item;}
 void NewName(void);
};


class cMP3SearchResult : public cOsdMenu {
private:
 virtual eOSState ID3Info(void);
 virtual eOSState AddOne(void);
 virtual eOSState AddAll(void);
 void SetHelp();
 int GetTab(int Tab);
protected:
 static cFileObj *selected;
 cFileObj *newsong;
 cFileSource *source;
 cFileObj *CurrentItem(void);
public:
 cMP3SearchResult(const char *szTitle);
 virtual ~cMP3SearchResult();
 virtual eOSState ProcessKey(eKeys Key);
 int LoadResult();
 static cFileObj *GetSelected(void) { return selected; }
};


class cMP3SearchRatingFrom : public cOsdMenu {
private:
 static char *ratingfrom;
 void LoadItems(void);
 void GetValue(void);
 void SetHelp(void);
 cOsdItem *Item(const char *text);
public:
 cMP3SearchRatingFrom(void);
 virtual ~cMP3SearchRatingFrom();
 virtual eOSState ProcessKey(eKeys Key);
 static const char *RatingFrom(void) { return ratingfrom; }
};


class cMP3SearchRatingTo : public cOsdMenu {
private:
 static char *ratingto;
 void LoadItems(void);
 void GetValue(void);
 void SetHelp(void);
 cOsdItem *Item(const char *text);
public:
 cMP3SearchRatingTo(void);
 virtual ~cMP3SearchRatingTo();
 virtual eOSState ProcessKey(eKeys Key);
 static const char *RatingTo(void) { return ratingto; }
};


class cMP3SearchGenre : public cOsdMenu {
private:
 static char *genretext;
 void LoadItems(void);
 void GetValue(void);
 void SetHelp(void);
public:
 cMP3SearchGenre(void);
 virtual ~cMP3SearchGenre();
 virtual eOSState ProcessKey(eKeys Key);
 static const char *GenreText(void) { return genretext; }
};


class cMP3Search : public cOsdMenu {
private:
// cMP3SearchGenre *SearchGenre;
// cMP3SearchRatingFrom *NewRatingFrom;
// cMP3SearchRatingTo *NewRatingTo;
 char Artist[64];
 char Title[64];
 char Genre[64];
 char FullFile[64];
 char searchresult[64];
 int YearFrom;
 int YearTo;
 char RatingFrom[64];
 char RatingTo[64];
 int Exist;
 bool updateGenre;
 bool updateRatingFrom;
 bool updateRatingTo;
 cOsdItem *Item(const char *text);
 eOSState Searching(void);
 void SetHelp(void);
 void ShowDisplay(void);
 void GetRatingSearchText();
 void UpdateMenu(int Which);
 void PreviousSearch(void);
 void NewValues(void);
 void ClearValues(void);
 void SetGenre(void);
 void SetRatingFrom(void);
 void SetRatingTo(void);
 void SaveSearch(void);
 void UpdateHelp(void);
public:
 cMP3Search(const char *Name);
 virtual ~cMP3Search();
 virtual eOSState ProcessKey(eKeys Key);
};

#endif // __SEARCH_H
