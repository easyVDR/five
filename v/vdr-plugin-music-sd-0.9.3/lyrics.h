#ifndef __LYRICS_H
#define __LYRICS_H

#include <vdr/osdbase.h>
#include "data-mp3.h"
#include "menu.h"
#include "player-mp3.h"

class cMP3LoadLocalLyrics : public cOsdMenu {
private:
 char *CheckLocalLyrics(char *fileName);
 void DisplayLocalLyrics();
public:
 cMP3LoadLocalLyrics(void);
 virtual eOSState ProcessKey(eKeys Key); 
};


class cMP3LoadExternLyrics : public cOsdMenu {
private:
 virtual eOSState SaveExternLyrics();
 int RunCommand(const char *cmd);
 char *CheckExternLyrics(const char *fileName);
 void DisplayExternLyrics();
public:
 cMP3LoadExternLyrics(void);
 virtual eOSState ProcessKey(eKeys Key); 
};


class cMP3Lyrics : public cOsdMenu {
private:
 void SetHelp();
 eOSState Execute();
public:
 cMP3Lyrics(void);
 virtual ~cMP3Lyrics();
 virtual eOSState ProcessKey(eKeys Key);
};

#endif // __LYRICS_H
