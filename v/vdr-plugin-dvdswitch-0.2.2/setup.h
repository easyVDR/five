#ifndef __SETUP_DVDSWITCH_H
#define __SETUP_DVDSWITCH_H

#include <vdr/menuitems.h>

#define memberof(x) (sizeof(x)/sizeof(*x))

class cDVDSwitchSetup
{
  public:
    int HideMenuEntry;
    char MenuName[50];
    bool CustomMenuName;
    char ImageDir[MaxFileName];
    bool ImageDirPerParam;
    int ViewFreeDiskSpace;
    
    int DisplayMode;
    int CategorieType;
    int HideEmptyDirs;
    int SortMode;
    int DisplayDVDDevice;

    int HideTypeCol;
    int CountTypCol;
    int HideImgSizeCol;
    char CatLineChar;
    char *CatLineChars;
    int CharCountBeforeCat;
    int SpacesBeforeAfterCat;
    char SubCatCutter[7];
    char ChangeCharsOSDName[20];

    int JumpCatByKey;

    bool DVDPluginExist;
    char DVDLink[MaxFileName];
    char DVDLinkOrg[MaxFileName];
    char DVDReadScript[MaxFileName];
    char DVDWriteScript[MaxFileName];

    const char *CommandsShortName[11];
    int k1;
    int k2;
    int k3;
    int k4;
    int k5;
    int k6;
    int k7;
    int k8;
    int k9;
    int k0;
    int kRed;
    int kGreen;
    int kYellow;
    int kBlue;
    int kOk;
    
    cDVDSwitchSetup(void);
    ~cDVDSwitchSetup(void);
    void Init(void);
    bool SetupParse(const char *Name, const char *Value, cImageList &ImageList);

    void SetConfDir(const char *value)
    {
      strncpy(DVDLink, value, memberof(DVDLink));
      strncat(DVDLink, "/dvdlink", memberof(DVDLink));
    }

    void SetDVDDevice(const char *value) { strncpy(DVDLinkOrg, value, memberof(DVDLinkOrg)); }

    bool ProcessArgs(int argc, char *argv[]);
};

class cMenuSetupDVDSwitch : public cMenuSetupPage
{
  private:
    cDVDSwitchSetup data;
    bool ViewGeneric;
    int ViewGenericPos;
    bool ViewDisplay;
    int ViewDisplayPos;
    bool ViewSpecialDisplay;
    int ViewSpecialDisplayPos;
    bool ViewNav;
    int ViewNavPos;
    bool ViewKey;
    int ViewKeyPos;
    cImageList &ImageList;
  protected:
    void Set(void);
    virtual void Store(void);
    virtual eOSState ProcessKey(eKeys Key);
  public:
    cMenuSetupDVDSwitch(cImageList &ImageList);
};

class cMenuEditCatItem : public cOsdItem
{
  private:
    char *Name;
  public:
    cMenuEditCatItem(const char *name, bool view = false);
    ~cMenuEditCatItem(void);
};

extern cDVDSwitchSetup DVDSwitchSetup;

#endif // __SETUP_DVDSWITCH_H
