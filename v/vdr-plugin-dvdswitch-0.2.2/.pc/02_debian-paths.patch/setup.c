#include <getopt.h>
#include "imagelist.h"
#include "setup.h"
#include "menu.h"
#include "dvdplugin.h"
#include "setup-itypes.h"

// --- cDVDSwitchSetup ------------------------------------------------

cDVDSwitchSetup DVDSwitchSetup;

cDVDSwitchSetup::cDVDSwitchSetup(void)
{
  HideMenuEntry = 0;
  strncpy(MenuName, "DVDSwitch", memberof(MenuName));
  CustomMenuName = false;
  strncpy(ImageDir, "/video/dvd", memberof(ImageDir));
  ImageDirPerParam = false;
  ViewFreeDiskSpace = 1;

  DisplayMode = 0;
  CategorieType = 1;
  HideEmptyDirs = 1;
  SortMode = 1;
  DisplayDVDDevice = 0;

  HideTypeCol = 0;
  CountTypCol = 35;
  HideImgSizeCol = 0;
  CatLineChars = strdup("-_.:#+*!%<>"); 
  CatLineChar = CatLineChars[0];
  CharCountBeforeCat = 3;
  SpacesBeforeAfterCat = 1;
  strncpy(SubCatCutter, ">",memberof(SubCatCutter));
  strncpy(ChangeCharsOSDName, "_ +&",memberof(ChangeCharsOSDName));

  JumpCatByKey = 0;

  k1 = 2;
  k2 = 3;
  k3 = 0;
  k4 = 0;
  k5 = 0;
  k6 = 0;
  k7 = 0;
  k8 = 5;
  k9 = 6;
  k0 = 1;
  kRed = 10;
  kGreen = 8;
  kYellow = 7;
  kBlue = 9;
  kOk = 4;

  DVDPluginExist = false;
  strncpy(DVDReadScript, "/usr/local/bin/dvdswitch_readdvd.sh",memberof(DVDReadScript));
  strncpy(DVDWriteScript, "/usr/local/bin/dvdswitch_writedvd.sh",memberof(DVDWriteScript));
}

cDVDSwitchSetup::~ cDVDSwitchSetup(void)
{
}

bool cDVDSwitchSetup::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
    { "readscript",   required_argument, NULL, 'r' },
    { "writescript",  required_argument, NULL, 'w' },
    { "imagedir",     required_argument, NULL, 'i' },
    { NULL }
  };

  int c = 0;
  optind = 1; //default for getopt

  while((c = getopt_long(argc, argv, "r:w:i:", long_options, NULL)) != -1)
  {
    switch(c)
    {
      case 'r':
        strn0cpy(DVDReadScript, optarg, memberof(DVDReadScript));
        break;
      case 'w':
        strn0cpy(DVDWriteScript, optarg, memberof(DVDWriteScript));
        break;
      case 'i':
        strn0cpy(ImageDir, optarg, memberof(ImageDir));
        ImageDirPerParam = true;
        break;
      default:
        esyslog("DVDSwitch: unknown parameter: %c", c);
        break;
    }
  }
  
  return true;
}

void cDVDSwitchSetup::Init(void)
{
  if (!CustomMenuName)
    strncpy(MenuName, tr(MenuName),memberof(MenuName));

  CommandsShortName[0] = NULL;
  CommandsShortName[1] = tr("Dir.Mgmt.");
  CommandsShortName[2] = tr("Open");
  CommandsShortName[3] = tr("Close");
  CommandsShortName[4] = tr("Play");
  CommandsShortName[5] = tr("Rename");
  CommandsShortName[6] = tr("Move");
  CommandsShortName[7] = tr("Delete");
  CommandsShortName[8] = tr("Burn");
  CommandsShortName[9] = tr("Create");
  CommandsShortName[10] = tr("Commands");
}

bool cDVDSwitchSetup::SetupParse(const char *Name, const char *Value, cImageList &ImageList)
{
  if (!strcasecmp(Name, "HideMenuEntry")) HideMenuEntry = atoi(Value);
  if (!strcasecmp(Name, "MenuName"))
  {
    strn0cpy(MenuName, Value, memberof(MenuName));
    CustomMenuName = true;
  }
  if (!strcasecmp(Name, "ImageDir") && !ImageDirPerParam) strn0cpy(ImageDir, Value, memberof(ImageDir));
  if (!strcasecmp(Name, "ImageTypes")) ImageList.AddSetup(Value);
  if (!strcasecmp(Name, "ViewFreeDiskSpace")) ViewFreeDiskSpace = atoi(Value);

  if (!strcasecmp(Name, "DisplayMode")) DisplayMode = atoi(Value);
  if (!strcasecmp(Name, "CategorieType")) CategorieType = atoi(Value);
  if (!strcasecmp(Name, "HideEmptyDirs")) HideEmptyDirs = atoi(Value);
  if (!strcasecmp(Name, "SortMode")) SortMode = atoi(Value);
  if (!strcasecmp(Name, "DisplayDVDDevice")) DisplayDVDDevice = atoi(Value);


  if (!strcasecmp(Name, "HideTypeCol")) HideTypeCol = atoi(Value);
  if (!strcasecmp(Name, "CountTypCol")) CountTypCol = atoi(Value);
  if (!strcasecmp(Name, "HideImgSizeCol")) HideImgSizeCol = atoi(Value);

  if (!strcasecmp(Name, "CatLineChar"))
  {
    CatLineChar = atoi(Value);
    if(!CatLineChar || !strchr(CatLineChars, CatLineChar))
      CatLineChar = CatLineChars[0];
  }
  if (!strcasecmp(Name, "CharCountBeforeCat")) CharCountBeforeCat = atoi(Value);
  if (!strcasecmp(Name, "SpacesBeforeAfterCat")) SpacesBeforeAfterCat = atoi(Value);
  if (!strcasecmp(Name, "SubCatCutter")) strn0cpy(SubCatCutter, Value, memberof(SubCatCutter));
  if (!strcasecmp(Name, "ChangeCharsOSDName")) strn0cpy(ChangeCharsOSDName, Value, memberof(ChangeCharsOSDName));

  if (!strcasecmp(Name, "JumpCatByKey")) JumpCatByKey = atoi(Value);

  if (!strcasecmp(Name, "k1"))      k1 = atoi(Value);
  if (!strcasecmp(Name, "k2"))      k2 = atoi(Value);
  if (!strcasecmp(Name, "k3"))      k3 = atoi(Value);
  if (!strcasecmp(Name, "k4"))      k4 = atoi(Value);
  if (!strcasecmp(Name, "k5"))      k5 = atoi(Value);
  if (!strcasecmp(Name, "k6"))      k6 = atoi(Value);
  if (!strcasecmp(Name, "k7"))      k7 = atoi(Value);
  if (!strcasecmp(Name, "k8"))      k8 = atoi(Value);
  if (!strcasecmp(Name, "k9"))      k9 = atoi(Value);
  if (!strcasecmp(Name, "k0"))      k0 = atoi(Value);
  if (!strcasecmp(Name, "kRed"))    kRed = atoi(Value);
  if (!strcasecmp(Name, "kGreen"))  kGreen = atoi(Value);
  if (!strcasecmp(Name, "kYellow")) kYellow = atoi(Value);
  if (!strcasecmp(Name, "kBlue"))   kBlue = atoi(Value);
  if (!strcasecmp(Name, "kOk"))     kOk = atoi(Value);

  return true;
}

// --- cMenuSetupDVDSwitch -------------------------------------------

cMenuSetupDVDSwitch::cMenuSetupDVDSwitch(cImageList &imagelist)
    : ImageList(imagelist)
{
  data = DVDSwitchSetup;

  ViewGeneric = false;
  ViewGenericPos = -1;
  ViewDisplay = false;
  ViewDisplayPos = -1;
  ViewSpecialDisplay = false;
  ViewSpecialDisplayPos = -1;
  ViewNav = false;
  ViewNavPos = -1;
  ViewKey = false;
  ViewKeyPos = -1;

  SetCols(2,30);
  Set();
}

void cMenuSetupDVDSwitch::Set(void)
{
  int current = Current();
  cOsdItem *item = NULL;

  Clear();

  Add(new cMenuEditCatItem(tr("Generic settings"), ViewGeneric));
  ViewGenericPos = Count() - 1;
  if(ViewGeneric)
  {
    Add(new cMenuEditBoolItem(tr("\tHide mainmenu entry"), &data.HideMenuEntry));
    if (!data.HideMenuEntry)
      Add(new cMenuEditStrItem(tr("\t  Name"),
          data.MenuName,
          memberof(data.MenuName)));
    if(!data.ImageDirPerParam)
      Add(new cMenuEditStrItem(tr("\tImage Dir."),
          data.ImageDir,
          memberof(data.ImageDir)));
    Add(new cOsdItem(tr("\tType of images")));
    Add(new cMenuEditBoolItem(tr("\tView free disk space"), &data.ViewFreeDiskSpace));
    Add(new cOsdItem("------------------------------------------------------------------------------------------"));
    item = Last();
    item->SetSelectable(false);
  }

  Add(new cMenuEditCatItem(tr("Display settings"), ViewDisplay));
  ViewDisplayPos = Count() - 1;
  if(ViewDisplay)
  {
    static const char *DModes[3] = {
        tr("All Images"),
        tr("All with Cat."),
        tr("Directories")
    };

    static const char *CTypes[3] = {
        tr("Image type"),
        tr("Directories"),
        tr("File type")
    };

    static const char *SModes[3] = {
        tr("none"),
        tr("ascending"),
        tr("descending")
    };

    Add(new cMenuEditStraItem(tr("\tDisplay mode"), &data.DisplayMode, memberof(DModes), DModes));

    if(data.DisplayMode == 1)
      Add(new cMenuEditStraItem(tr("\t  Category Type"), &data.CategorieType, memberof(CTypes), CTypes));
    if(data.DisplayMode > 0)
      Add(new cMenuEditBoolItem(tr("\t  invisible empty Dir's/Cat."), &data.HideEmptyDirs));
    Add(new cMenuEditStraItem(tr("\tAlphabetic Sort"), &data.SortMode, memberof(SModes), SModes));
    Add(new cMenuEditBoolItem(tr("\tvisible DVD-Drive"), &data.DisplayDVDDevice));
    Add(new cOsdItem("------------------------------------------------------------------------------------------"));
    item = Last();
    item->SetSelectable(false);
  }

  Add(new cMenuEditCatItem(tr("Special display settings"), ViewSpecialDisplay));
  ViewSpecialDisplayPos = Count() - 1;
  if(ViewSpecialDisplay)
  {
    Add(new cMenuEditBoolItem(tr("\tvisible Image-Type Col"), &data.HideTypeCol));
    if(data.HideTypeCol)
      Add(new cMenuEditIntItem(tr("\t  Width for Image-Col"), &data.CountTypCol, 10, 50));
    Add(new cMenuEditBoolItem(tr("\tvisible Image-Size Col"), &data.HideImgSizeCol));
    Add(new cMenuEditStrItem(tr("\tchange Character"),
        data.ChangeCharsOSDName,
        memberof(data.ChangeCharsOSDName)));
    if(data.DisplayMode == 1)
    {
      //if(data.CategorieType == 0)
      //  Add(new cMenuEditBoolItem(tr("\tISO Images first"), &data.ISOFirst));
      Add(new cMenuEditChrItem(tr("\tchar for Cat.Line"),
          &data.CatLineChar,
          data.CatLineChars));
      Add(new cMenuEditIntItem(tr("\t  Count before Cat.Name"), &data.CharCountBeforeCat, 0, 30));
      Add(new cMenuEditBoolItem(tr("\tSpaces before/after Cat.Name"), &data.SpacesBeforeAfterCat));
      if(data.CategorieType == 1)
        Add(new cMenuEditStrItem(tr("\tSeparator for Subcat"),
            data.SubCatCutter,
            memberof(data.SubCatCutter),
            "-_.:#+*!%<>"));
    }
    Add(new cOsdItem("------------------------------------------------------------------------------------------"));
    item = Last();
    item->SetSelectable(false);
  }

  if(data.DisplayMode == 1)
  {
    Add(new cMenuEditCatItem(tr("Navigation"), ViewNav));
    ViewNavPos = Count() - 1;
    if(ViewNav)
    {
      Add(new cMenuEditBoolItem(tr("\tCat.Jump by Left/Right"), &data.JumpCatByKey));
      Add(new cOsdItem("------------------------------------------------------------------------------------------"));
      item = Last();
      item->SetSelectable(false);
    }
  }

  Add(new cMenuEditCatItem(tr("Key allocation"), ViewKey));
  ViewKeyPos = Count() - 1;
  if(ViewKey)
  {
    static const char *Commands[memberof(DVDSwitchSetup.CommandsShortName)] = {
        tr("None function"),
        tr("Dir. Management"),
        tr("Tray open"),
        tr("Tray close"),
        tr("Play"),
        tr("Image rename"),
        tr("Image move"),
        tr("Image delete"),
        tr("Image burn"),
        tr("Image create"),
        tr("Commands")
    };

    Add(new cMenuEditStraItem(tr("\tKey 1"), &data.k1, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 2"), &data.k2, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 3"), &data.k3, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 4"), &data.k4, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 5"), &data.k5, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 6"), &data.k6, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 7"), &data.k7, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 8"), &data.k8, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 9"), &data.k9, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey 0"), &data.k0, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey Red"), &data.kRed, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey Green"), &data.kGreen, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey Yellow"), &data.kYellow, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey Blue"), &data.kBlue, memberof(Commands), Commands));
    Add(new cMenuEditStraItem(tr("\tKey OK"), &data.kOk, memberof(Commands), Commands));
  }

  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupDVDSwitch::ProcessKey(eKeys Key)
{
  eOSState state = osContinue;

  if(!HasSubMenu())
  {
    const char *ItemText = Get(Current())->Text();
    
    switch(Key & ~k_Repeat)
    {
      case kRight:
      case kLeft:
        if(Current() == ViewGenericPos        ||
           Current() == ViewDisplayPos        ||
           Current() == ViewSpecialDisplayPos ||
           Current() == ViewNavPos            ||
           Current() == ViewKeyPos)
        {
          if(Current() == ViewGenericPos)
          {
            if((Key == kRight && !ViewGeneric) ||
               (Key == kLeft  && ViewGeneric))
              ViewGeneric = !ViewGeneric;
          }
          else if(Current() == ViewDisplayPos)
          {
            if((Key == kRight && !ViewDisplay) ||
               (Key == kLeft  && ViewDisplay))
              ViewDisplay = !ViewDisplay;
          }
          else if(Current() == ViewSpecialDisplayPos)
          {
            if((Key == kRight && !ViewSpecialDisplay) ||
               (Key == kLeft  && ViewSpecialDisplay))
              ViewSpecialDisplay = !ViewSpecialDisplay;
          }
          else if(Current() == ViewNavPos)
          {
            if((Key == kRight && !ViewNav) ||
               (Key == kLeft  && ViewNav))
              ViewNav = !ViewNav;
          }
          else if(Current() == ViewKeyPos)
          {
            if((Key == kRight && !ViewKey) ||
                (Key == kLeft  && ViewKey))
              ViewKey = !ViewKey;
          }
          Set();
          Display();
          return osContinue;
        }
        else
        {
          int tmpHideMenuEntry = data.HideMenuEntry;
          int tmpDisplayMode = data.DisplayMode;
          int tmpCategorieType = data.CategorieType;
          int tmpHideTypeCol = data.HideTypeCol;

          eOSState state = cMenuSetupPage::ProcessKey(Key);

          if (tmpHideMenuEntry != data.HideMenuEntry ||
              tmpDisplayMode   != data.DisplayMode   ||
              tmpCategorieType != data.CategorieType ||
              tmpHideTypeCol   != data.HideTypeCol)
          {
            Set();
            Display();
          }

          return state;
        }
        break;
      case kUp:
      case kDown:
        state = cMenuSetupPage::ProcessKey(Key);
        ItemText = Get(Current())->Text();
        
        if(strstr(ItemText, tr("\tType of images")) == ItemText)
          cOsdMenu::SetHelp(NULL, NULL, NULL, tr("to adjust"));
        else
          cOsdMenu::SetHelp(NULL, NULL, NULL, NULL);
        break;
      case kBlue:
        if(strstr(ItemText, tr("\tType of images")) == ItemText)
          return AddSubMenu(new cMenuSetupDSITypes(ImageList));
        break;
      default:
        state = cMenuSetupPage::ProcessKey(Key);
        break;
    }
  }
  else
    state = cMenuSetupPage::ProcessKey(Key);

  return state;
}

void cMenuSetupDVDSwitch::Store(void)
{
  dsyslog("dvdswitch: store setup");

  DVDSwitchSetup = data;

  SetupStore("HideMenuEntry",           DVDSwitchSetup.HideMenuEntry);
  SetupStore("MenuName",                DVDSwitchSetup.MenuName);
  if(!DVDSwitchSetup.ImageDirPerParam)
    SetupStore("ImageDir",              DVDSwitchSetup.ImageDir);
  SetupStore("ViewFreeDiskSpace",       DVDSwitchSetup.ViewFreeDiskSpace);

  SetupStore("DisplayMode",             DVDSwitchSetup.DisplayMode);
  SetupStore("CategorieType",           DVDSwitchSetup.CategorieType);
  SetupStore("HideEmptyDirs",           DVDSwitchSetup.HideEmptyDirs);
  SetupStore("SortMode",                DVDSwitchSetup.SortMode);
  SetupStore("DisplayDVDDevice",        DVDSwitchSetup.DisplayDVDDevice);

  SetupStore("HideTypeCol",             DVDSwitchSetup.HideTypeCol);
  SetupStore("CountTypCol",             DVDSwitchSetup.CountTypCol);
  SetupStore("HideImgSizeCol",          DVDSwitchSetup.HideImgSizeCol);
  SetupStore("CatLineChar",             DVDSwitchSetup.CatLineChar);
  SetupStore("CharCountBeforeCat",      DVDSwitchSetup.CharCountBeforeCat);
  SetupStore("SpacesBeforeAfterCat",    DVDSwitchSetup.SpacesBeforeAfterCat);
  SetupStore("SubCatCutter",            DVDSwitchSetup.SubCatCutter);
  SetupStore("ChangeCharsOSDName",      DVDSwitchSetup.ChangeCharsOSDName);

  SetupStore("JumpCatByKey",            DVDSwitchSetup.JumpCatByKey);

  SetupStore("k1",                      DVDSwitchSetup.k1);
  SetupStore("k2",                      DVDSwitchSetup.k2);
  SetupStore("k3",                      DVDSwitchSetup.k3);
  SetupStore("k4",                      DVDSwitchSetup.k4);
  SetupStore("k5",                      DVDSwitchSetup.k5);
  SetupStore("k6",                      DVDSwitchSetup.k6);
  SetupStore("k7",                      DVDSwitchSetup.k7);
  SetupStore("k8",                      DVDSwitchSetup.k8);
  SetupStore("k9",                      DVDSwitchSetup.k9);
  SetupStore("k0",                      DVDSwitchSetup.k0);
  SetupStore("kRed",                    DVDSwitchSetup.kRed);
  SetupStore("kGreen",                  DVDSwitchSetup.kGreen);
  SetupStore("kYellow",                 DVDSwitchSetup.kYellow);
  SetupStore("kBlue",                   DVDSwitchSetup.kBlue);
  SetupStore("kOk",                     DVDSwitchSetup.kOk);
}

// --- cMenuEditCatItem ------------------------------------------------------

cMenuEditCatItem::cMenuEditCatItem(const char *name, bool view)
{
  Name = (char*) malloc(120);
  if(view)
    strncpy(Name, "-\t", 119);
  else
    strncpy(Name, "+\t", 119);
  strncat(Name, name, 119);

  SetText(Name);
}

cMenuEditCatItem::~cMenuEditCatItem(void)
{
  free(Name);
}
