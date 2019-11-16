#ifndef __COMMANDS_DVDSWITCH_H
#define __COMMANDS_DVDSWITCH_H

#include <vdr/plugin.h>
#include "imagelist.h"
#include "menu.h"
#include "setup.h"
#include <stdlib.h>

enum eCMDs
{
  cmdNone,
  cmdDirManage,
  cmdDVDopen,
  cmdDVDclose,
  cmdImgPlay,
  cmdImgRename,
  cmdImgMove,
  cmdImgDelete,
  cmdImgBurn,
  cmdImgRead,
  cmdCommands
};

enum eCMDState
{
  csNone,
  csDirNew,
  csDirEdit,
  csDirMove,
  csDirDel
};


class cCMD
{
  public:
    static eOSState Play(const cMainMenuItem *item = NULL);
    static eOSState Eject(bool close = false);
};


class cCMDMenu : public cOsdMenu
{
  private:
    const cMainMenuItem *iItem;
    cMainMenu *OsdObject;
    cImageList &ImageList;
  public:
    cCMDMenu(cImageList &ImageList, const cMainMenuItem *item, cMainMenu *osdobject);
    virtual eOSState ProcessKey(eKeys Key);
};

class cCMDDir : public cOsdMenu, private cDirHandlingOpt
{
  private:
    eCMDState State;
    char Dir[MaxFileName];
    cMainMenu *OsdObject;
    bool Select;
    char *LastDir;
    cImageList &ImageList;

    void Build(const char *dir = NULL);
    void SetDir(const char *value = NULL)
    {
      if(value)
        strn0cpy(Dir, value, MaxFileName);
      else
        strcpy(Dir, "\0");
    }

    eOSState New(void);
    eOSState New(eKeys Key);
    eOSState Edit(cMainMenuItem *mItem);
    eOSState Edit(eKeys Key);
  public:
    cCMDDir(cImageList &ImageList, cMainMenu *osdobject = NULL, bool select = false, char *lastdir = NULL);

    virtual eOSState ProcessKey(eKeys Key);
    void SetHelp(void);
};

class cCMDMove : public cOsdMenu, private cDirHandlingOpt
{
  private:
    char *File;
    cMainMenu *OsdObject;
    bool Dir;
    bool Direct;
    cImageList &ImageList;

    void Build(const char *dir = NULL);
  public:
    cCMDMove(cImageList &ImageList, const char *file, cMainMenu *osdobject, bool dir = true, bool direct = false);
    ~cCMDMove(void) { free(File); }

    virtual eOSState ProcessKey(eKeys Key);
    void SetHelp(void);
};

class cCMDImage
{
  private:
    char *File;
    cMainMenu *OsdObject;
  public:
    char NewFile[MaxFileName];
    int tmpHideTypeCol;
    //int tmpHideImgSize

    cCMDImage(cMainMenu *osdobject = NULL);
    ~cCMDImage(void);

    char* Rename(const char *file = NULL);
    eOSState Delete(const char *file = NULL);
    eOSState Burn(const char *file);
};

class cCMDImageRead : public cOsdMenu
{
  private:
    char File[MaxFileName];
    char Dir[MaxFileName];
    char ImgTypeTxt[MaxFileName];
    int ImgType;
    cImageList &ImageList;
    const char ** m_szItemTypes;

  public:
    cCMDImageRead(cImageList &ImageList);
    ~cCMDImageRead(void);

    void SetHelp(void);
    virtual eOSState ProcessKey(eKeys Key);
};

class cCMDImageReadThread : public cThread
{
  private:
    char *File;
    char *Dir;
    eFileInfo FileType;
  protected:
    virtual void Action(void);
  public:
    cCMDImageReadThread(char *file, char *dir, int imgtype, cImageList &ImageList);
    virtual ~cCMDImageReadThread(void);
};

class cCMDImageBurnThread : public cThread
{
  private:
    char *File;
    eFileInfo FileType;
  protected:
    virtual void Action(void);
  public:
    cCMDImageBurnThread(const char *file, eFileInfo type);
    virtual ~cCMDImageBurnThread(void);
};

#endif // __COMMANDS_DVDSWITCH_H
