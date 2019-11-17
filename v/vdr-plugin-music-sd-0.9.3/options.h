#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <sys/types.h>
#include <sys/stat.h>

#include <vdr/osdbase.h>

class cMP3TimerShutdown : public cOsdMenu {
private:
 int timertime;
 void SetHelp();
 void ShowDisplay(void);
 eOSState Execute();
public:
 cMP3TimerShutdown(void);
 virtual ~cMP3TimerShutdown();
 virtual eOSState ProcessKey(eKeys Key);
// int TimerTime(void) {return timertime;}
};


class cMP3Skiptime : public cOsdMenu {
private:
 int skiptime;
 void SetHelp();
 void ShowDisplay(void);
 eOSState Execute();
public:
 cMP3Skiptime(void);
 virtual ~cMP3Skiptime();
 virtual eOSState ProcessKey(eKeys Key);
};


class cMP3AddCopyDir : public cOsdMenu {
private:
 char AddDir[256];
 void SetHelp(void);
 void ShowDisplay(void);
 void Save();
public:
 cMP3AddCopyDir(void);
 virtual ~cMP3AddCopyDir();
 virtual eOSState ProcessKey(eKeys Key);
};


class cMP3CopyDir : public cOsdMenu {
private:
 cOsdItem *Item(const char *dir, bool selectable);
 void SetHelp();
 void GetCopyDirs();
 int DeleteCopyDir();
 int mkdir(const char dir, int modus);
 eOSState Execute();
public:
 cMP3CopyDir(void);
 virtual ~cMP3CopyDir();
 virtual eOSState ProcessKey(eKeys Key); 
};


class cMP3Options : public cOsdMenu {
private:
 cOsdItem *Item(const char *text);
 void SetHelp();
 eOSState Execute();
public:
 cMP3Options(void);
 virtual ~cMP3Options();
 virtual eOSState ProcessKey(eKeys Key);
};

#endif // __OPTIONS_H
