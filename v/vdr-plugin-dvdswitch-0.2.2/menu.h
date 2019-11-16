#ifndef __MENU_DVDSWITCH_H
#define __MENU_DVDSWITCH_H

#include <vdr/osdbase.h>
#include "menu-item.h"
#include "dvdlist.h"

enum eMainMenuState
{
  mmsNone,
  mmsReInit,
  mmsReInitCur,
  mmsImgRename,
};

class cCMDImage;

class cMainMenu : public cOsdMenu
{
  private:
    int FirstSelectable;
    eMainMenuState State;
    cCMDImage *CMDImg;
    cImageList &ImageList;

    void Init(void);
    void SetMenuTitle(void);
    void Build(const char *dir);
    void BuildDisp0(const char *dir);
    void BuildDisp1(const char *dir);
    void BuildDisp2(const char *dir);
    eOSState MenuMove(eKeys Key);
    eOSState SelectItem(void);
    eOSState Commands(eKeys Key);
  public:
    cMainMenu(cImageList &ImageList);
    ~cMainMenu(void);
    void SetHelp(void);
    virtual eOSState ProcessKey(eKeys Key);

    static char *CreateOSDName(eMainMenuItem itype, cImageList &ImageList, const char *file = NULL);
    void SetState(eMainMenuState state);
    eMainMenuState GetState(void) { return State; };
};

class cDirHandlingOpt
{
  char *IDir;
  char *CDir;
  char *PDir;
  char *LSDir;
  char *LSItemName;
  eMainMenuItem LSItemType;

  public:
    cDirHandlingOpt(void)
    {
      IDir = NULL;
      CDir = NULL;
      PDir = NULL;
      LSDir = NULL;
      LSItemName = NULL;
      LSItemType = iCat;
    }
    ~cDirHandlingOpt(void)
    {
      free(IDir);
      free(CDir);
      free(PDir);
      free(LSDir);
      free(LSItemName);
    }
    char *ImageDir(const char *dir = NULL)
    {
      if(dir)
      {
        FREENULL(IDir);
        IDir = strdup(dir);
      }
      return IDir;
    }
    char *CurrentDir(const char *dir = NULL)
    {
      if(dir)
      {
        FREENULL(CDir);
        CDir = strdup(dir);
      }
      return CDir;
    }
    char *ParentDir(const char *dir = NULL)
    {
      if(dir)
      {
        FREENULL(PDir);
        PDir = strdup(dir);
      }
      return PDir;
    }
    char *LastSelDir(const char *dir = NULL)
    {
      if(dir)
      {
        FREENULL(LSDir);
        LSDir = strdup(dir);
      }
      return LSDir;
    }
    bool isParent(const char *dir)
    {
      if(dir && PDir && strcasecmp(dir, PDir))
        return true;
      return false;
    }
    bool isLastSel(const char *dir)
    {
      if(dir && LSDir && !strcasecmp(dir, LSDir))
        return true;
      return false;
    }
    void setLastSelectItemName(const char *filename = NULL)
    {
      FREENULL(LSItemName);
      if(filename)
        LSItemName = strdup(filename);
    };
    char *getLastSelectItemName(void) { return LSItemName; }
    eMainMenuItem LastSelectItemType(eMainMenuItem type = iCat)
    {
      if(type != iCat)
        LSItemType = type;
      return LSItemType;
    }
};

class cDirHandling
{
  private:
    cOsdMenu *OsdObject;
    cDirHandlingOpt *DirObject;
  public:
    cDirHandling(cOsdMenu *osdobject, cDirHandlingOpt *dirobject);

    int Build(const char *dir, bool emptydirs, cImageList &ImageList);
    void ProcessKey(cMainMenuItem *mItem);
};

extern cDirHandlingOpt MainMenuOptions;

#endif // __MENU_DVDSWITCH_H
