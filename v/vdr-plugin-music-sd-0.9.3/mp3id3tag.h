#ifndef __MP3ID3TAG_H
#define __MP3ID3TAG_H

#include <vdr/osdbase.h>
#include "data-mp3.h"

class cMP3id3TagYear : public cOsdMenu {
private:
 static char *year;
 void LoadItems(void);
 void SetHelp();
 void GetValue(void);
public:
 cMP3id3TagYear(void);
 virtual ~cMP3id3TagYear();
 virtual eOSState ProcessKey(eKeys Key);
 static const char *Year(void) { return year; }
};


class cMP3id3TagRating : public cOsdMenu {
private:
 void LoadItems(void);
 void SetHelp();
 void GetValue(void);
 cOsdItem *Item(const char *text);
public:
 cMP3id3TagRating(void);
 virtual ~cMP3id3TagRating();
 virtual eOSState ProcessKey(eKeys Key);
};


class cMP3id3TagGenre : public cOsdMenu {
private:
 static char *genre;
 void LoadItems(void);
 void SetHelp();
 void GetValue(void);
public:
 cMP3id3TagGenre(void);
 virtual ~cMP3id3TagGenre();
 virtual eOSState ProcessKey(eKeys Key);
 static const char *Genre(void) { return genre; }
};


class cMP3id3Tag : public cOsdMenu {
private:
 cSong *id3_Song;
 char FullFile[256];
 char Artist[64];
 char Album[64];
 char Title[64];
 char Genre[64];
 int Year;
 int Rating;
 bool updateGenre;
 bool updateYear;
 bool updateRating;

 cOsdItem *Item(const char *text);
 void SetHelp();
 void ShowDisplay(void);
 void UpdateMenu(int Which);
 void GetRealTags(void);
 void RemoveTagsOnly(void);
 void SaveTags(void);
 void SetGenre(void);
 void SetYear(void);
 void SetRating(void);
public:
 cMP3id3Tag(cSong *song);
 virtual ~cMP3id3Tag();
 virtual eOSState ProcessKey(eKeys Key);
};

#endif // __MP3ID3TAG_H
