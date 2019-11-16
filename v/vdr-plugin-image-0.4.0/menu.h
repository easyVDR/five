/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold   <vdr07 at deltab.de>
 * (C) 2003 Kai Tobias Burwieck <kai@burwieck.net>
 *
 * based on MP3/MPlayer plugin to VDR (C++)
 * (C) 2001,2002 Stefan Huelswitt <huels@iname.com>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef ___MENU_H
#define ___MENU_H

#include "image.h"
#include "data.h"
// ----------------------------------------------------------------

class cMenuBrowse:public cOsdMenu {
  private:
    eOSState Select(bool isred);
    eOSState Parent(void);
    bool LoadDir(const char *dir);
  protected:
    static cDirItem *lastselect;
    //
    cDirList *list;
    cFileSource *source;
    bool dirselectable;
    char *currentdir, *parent;
//
    bool NewDir(const char *dir);
    void SetSource(cFileSource * Source);
    cDirItem *CurrentItem(void);
    virtual void SetButtons(void);
  public:
     cMenuBrowse(cFileSource * Source, bool Dirselect, const char *title);
    ~cMenuBrowse();
    virtual eOSState ProcessKey(eKeys Key);
    static cDirItem *GetSelected(void) {return lastselect;}
};

// ----------------------------------------------------------------

class cMenuSource:public cOsdMenu {
  private:
    static cFileSource *selected;
    cFileSource *current;
    //
    eOSState Mount(void);
    eOSState Unmount(void);
    eOSState Eject(void);
    eOSState Select(void);
    bool DoMount(cFileSource * src);
    bool CheckMount(void);
  public:
     cMenuSource(cFileSources * Sources, const char *title);
    virtual eOSState ProcessKey(eKeys Key);
    static cFileSource *GetSelected(void) {	return selected;}
};

#endif				//___MENU_H
