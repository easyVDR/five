#ifndef __COMMANDS_H
#define __COMMANDS_H

#include <string>
#include <vdr/osdbase.h>

#include "setup-mp3.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "menu.h"
#include "config.h"

//class cMP3Player;
class cFileSources;
class cFileSource;
class cFileObj;
class cFileObjItem;

class cMP3EditPlaylist : public cOsdMenu {
private:
 cMP3Player *mp3player;
 eOSState Execute(void);
 void SetHelp();
public:
 cMP3EditPlaylist(const char *sz_Title);
 virtual eOSState ProcessKey(eKeys Key);
 bool DeleteList(void);
};


class cMP3Commands : public cOsdMenu {
private:
 cCommands commands;
 eOSState Execute(void);
 void SetHelp();
public:
 cMP3Commands(void);
 virtual ~cMP3Commands();
 virtual eOSState ProcessKey(eKeys Key);
 void LoadCommands();
 bool DeleteList(void);
 void StartRecord(void);
 void StopRecord(void);
};

#endif // __COMMANDS_H
