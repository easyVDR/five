#ifndef __COVER_H
#define __COVER_H

#include <vdr/osdbase.h>
//#include "config.h"

class cMP3Cover : public cOsdMenu {
private:
 char artist[128];
 char album[128];
 char basedir[255];
 char filenoext[255];
 
 cOsdItem *Item(const char *text);
 eOSState Searching(void);
 void SetHelp(void);
 void ShowDisplay(void);
 void ClearValues(void);
public:
 cMP3Cover(const char *Artist, const char *Album, const char *Basedir, const char *FileNoExt);
 virtual ~cMP3Cover();
 virtual eOSState ProcessKey(eKeys Key);
};

#endif // __COVER_H
