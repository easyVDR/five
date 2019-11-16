#include <vdr/plugin.h>
#include "imagelist.h"
#include "menu.h"
#include "tools.h"
#include "setup.h"
#include "dvdplugin.h"
#include "commands.h"

cDirHandlingOpt MainMenuOptions;

// --- cMainMenu ------------------------------------------------------------------------

cMainMenu::cMainMenu(cImageList &imagelist)
  : cOsdMenu(DVDSwitchSetup.MenuName, DVDSwitchSetup.CountTypCol, 10)
  , ImageList(imagelist)
{
  FirstSelectable = -1;
  CMDImg = NULL;
  State = mmsNone;

  if(!MainMenuOptions.ImageDir())
    Init();

  Build(MainMenuOptions.CurrentDir());
}

cMainMenu::~cMainMenu(void)
{
  if(CMDImg)
    delete(CMDImg);
}

void cMainMenu::Init(void)
{
  MainMenuOptions.ImageDir(DVDSwitchSetup.ImageDir);
  MainMenuOptions.CurrentDir(DVDSwitchSetup.ImageDir);
  MainMenuOptions.ParentDir(DVDSwitchSetup.ImageDir);
}

void cMainMenu::SetMenuTitle(void)
{
  char *title = NULL;

  if(DVDSwitchSetup.ViewFreeDiskSpace)
  {
    int mByte = FreeDiskSpaceMB(DVDSwitchSetup.ImageDir);
    double gByte = (double)((int)((double)mByte / 1024 * 10)) / 10;
    if(0 >= asprintf(&title,
             "%s - %4.1f GB (DVD5: %i / DVD9: %i)",
             DVDSwitchSetup.MenuName,
             gByte,
             (int)(gByte / 4.7),
             (int)(gByte / 8.5)))
      return;
  } else
    title = DVDSwitchSetup.MenuName ? strdup(DVDSwitchSetup.MenuName) : NULL;

  SetTitle(title);
  free(title);
}

void cMainMenu::Build(const char *dir)
{
  SetMenuTitle();
  if(!DVDSwitchSetup.HideImgSizeCol)
    SetCols(DVDSwitchSetup.CountTypCol, 10);
  else
    SetCols(5, DVDSwitchSetup.CountTypCol-5, 10);

  FirstSelectable = -1;

  Clear();

  switch(DVDSwitchSetup.DisplayMode)
  {
    case 0:
      BuildDisp0(dir);
      break;
    case 1:
      BuildDisp1(dir);
      break;
    case 2:
      BuildDisp2(dir);
      break;
  }

  if(FirstSelectable >= 0)
  {
    cMainMenuItem *mmItem = (cMainMenuItem*)Get(FirstSelectable);
    SetCurrent(mmItem);
    MainMenuOptions.setLastSelectItemName(mmItem->FileName());
    MainMenuOptions.LastSelectItemType(mmItem->Type());
  }

  Display();
  SetHelp();
}

void cMainMenu::BuildDisp0(const char *dir)
{
  cDVDList *DVDList = new cDVDList;
  cMainMenuItem *mItem = NULL;

  if(DVDSwitchSetup.DisplayDVDDevice)
  {
    Add(new cMainMenuItem(iDevice,ImageList));
    if(!MainMenuOptions.getLastSelectItemName() && MainMenuOptions.LastSelectItemType() == iDevice)
      FirstSelectable = 0;
  }

  if(DVDList->Create(dir,
                     ImageList.GetExtensions(),
                     ImageList.GetDirContains(),
                     (eFileList)DVDSwitchSetup.SortMode,
                     true))
  {
    cDVDListItem *item = DVDList->First();
    while(item)
    {
      Add(new cMainMenuItem(iDVD, ImageList, item->FileName()));
      if(MainMenuOptions.getLastSelectItemName() &&
         !strcasecmp(item->FileName(), MainMenuOptions.getLastSelectItemName()))
      {
        mItem = (cMainMenuItem*)Last();
        FirstSelectable = mItem->Index();
        mItem = NULL;
      }
      item = DVDList->Next(item);
    }
  }

  if(Count() && FirstSelectable < 0)
    FirstSelectable = 0;

  delete(DVDList);
}

void cMainMenu::BuildDisp1(const char *dir)
{
  cImageListItem *iItem = NULL;
  cDVDList *DVDList = new cDVDList;
  cDVDListItem *dItem = NULL;
  cDirList *DirList = NULL;
  cFileListItem *fItem = NULL;
  cMainMenuItem *mItem = NULL;

  switch(DVDSwitchSetup.CategorieType)
  {
    case 0: // Image-Type
      if(DVDSwitchSetup.DisplayDVDDevice)
      {
        Add(new cMainMenuItem(iCat, ImageList, dir));
        Add(new cMainMenuItem(iDevice, ImageList));
        if(!MainMenuOptions.getLastSelectItemName() && MainMenuOptions.LastSelectItemType() == iDevice)
          FirstSelectable = 1;
      }
      iItem = ImageList.First();
      while(iItem)
      {
        if(DVDList->Create(dir,
                           iItem->GetFType() == tFile ? iItem->GetValue() : NULL,
                           iItem->GetFType() == tDir ? iItem->GetValue() : NULL,
                           (eFileList)DVDSwitchSetup.SortMode,
                           true) &&
           (DVDList->Count() || !DVDSwitchSetup.HideEmptyDirs))
        {
          Add(new cMainMenuItem(iCat, ImageList, iItem->GetSName()));
          dItem = DVDList->First();
          while(dItem)
          {
            Add(new cMainMenuItem(iDVD, ImageList, dItem->FileName()));
            if(MainMenuOptions.getLastSelectItemName() &&
               !strcasecmp(dItem->FileName(), MainMenuOptions.getLastSelectItemName()))
            {
              mItem = (cMainMenuItem*)Last();
              FirstSelectable = mItem->Index();
              mItem = NULL;
            }
            dItem = DVDList->Next(dItem);
          }
        }
        iItem = ImageList.Next(iItem);
      }
      break;
    case 1: // Directories
      if(DVDList->Create(dir,
                         ImageList.GetExtensions(),
                         ImageList.GetDirContains(),
                         (eFileList)DVDSwitchSetup.SortMode,
                         false) &&
         (DVDList->Count() || !DVDSwitchSetup.HideEmptyDirs || DVDSwitchSetup.DisplayDVDDevice))
      {
        Add(new cMainMenuItem(iCat, ImageList, dir));
        if(DVDSwitchSetup.DisplayDVDDevice)
        {
          Add(new cMainMenuItem(iDevice, ImageList));
          if(!MainMenuOptions.getLastSelectItemName() && MainMenuOptions.LastSelectItemType() == iDevice)
            FirstSelectable = 1;
        }
        dItem = DVDList->First();
        while(dItem)
        {
          Add(new cMainMenuItem(iDVD, ImageList, dItem->FileName()));
          if(MainMenuOptions.getLastSelectItemName() &&
             !strcasecmp(dItem->FileName(), MainMenuOptions.getLastSelectItemName()))
          {
            mItem = (cMainMenuItem*)Last();
            FirstSelectable = mItem->Index();
            mItem = NULL;
          }
          dItem = DVDList->Next(dItem);
        }
      }
      DirList = new cDirList(ImageList);
      if(DirList->Load(dir, true))
      {
        fItem = DirList->First();
        while(fItem)
        {
          if(!DirList->DirIsIn(fItem, ImageList.GetDirContains()) &&
             DVDList->Create(fItem->Value(),
                             ImageList.GetExtensions(),
                             ImageList.GetDirContains(),
                             (eFileList)DVDSwitchSetup.SortMode,
                             false) &&
             (DVDList->Count() || !DVDSwitchSetup.HideEmptyDirs))
          {
            Add(new cMainMenuItem(iCat, ImageList, fItem->Value()));
            dItem = DVDList->First();
            while(dItem)
            {
              Add(new cMainMenuItem(iDVD, ImageList, dItem->FileName()));
              if(MainMenuOptions.getLastSelectItemName() &&
                 !strcasecmp(dItem->FileName(), MainMenuOptions.getLastSelectItemName()))
              {
                mItem = (cMainMenuItem*)Last();
                FirstSelectable = mItem->Index();
                mItem = NULL;
              }
              dItem = DVDList->Next(dItem);
            }
          }
          fItem = DirList->Next(fItem);
        }
      }
      DELETENULL(DirList);
      break;
    case 2: // FileType
      if(DVDSwitchSetup.DisplayDVDDevice)
      {
        Add(new cMainMenuItem(iCat, ImageList, dir));
        Add(new cMainMenuItem(iDevice, ImageList));
        if(!MainMenuOptions.getLastSelectItemName() && MainMenuOptions.LastSelectItemType() == iDevice)
          FirstSelectable = 1;
      }
      if(DVDList->Create(dir,
         ImageList.GetExtensions(),
         NULL,
         (eFileList)DVDSwitchSetup.SortMode,
         true) &&
         (DVDList->Count() || !DVDSwitchSetup.HideEmptyDirs))
      {
        Add(new cMainMenuItem(iCat, ImageList, tr("Image-File")));
        dItem = DVDList->First();
        while(dItem)
        {
          Add(new cMainMenuItem(iDVD, ImageList, dItem->FileName()));
          if(MainMenuOptions.getLastSelectItemName() &&
             !strcasecmp(dItem->FileName(), MainMenuOptions.getLastSelectItemName()))
          {
            mItem = (cMainMenuItem*)Last();
            FirstSelectable = mItem->Index();
            mItem = NULL;
          }
          dItem = DVDList->Next(dItem);
        }
      }
      if(DVDList->Create(dir,
         NULL,
         ImageList.GetDirContains(),
         (eFileList)DVDSwitchSetup.SortMode,
         true) &&
         (DVDList->Count() || !DVDSwitchSetup.HideEmptyDirs))
      {
        Add(new cMainMenuItem(iCat, ImageList, tr("Image-Directory")));
        dItem = DVDList->First();
        while(dItem)
        {
          Add(new cMainMenuItem(iDVD, ImageList, dItem->FileName()));
          if(MainMenuOptions.getLastSelectItemName() &&
             !strcasecmp(dItem->FileName(), MainMenuOptions.getLastSelectItemName()))
          {
            mItem = (cMainMenuItem*)Last();
            FirstSelectable = mItem->Index();
            mItem = NULL;
          }
          dItem = DVDList->Next(dItem);
        }
      }
      break;
    default:
      break;
  }

  mItem = (cMainMenuItem*)First();
  if(mItem && FirstSelectable < 0)
  {
    while(mItem && !mItem->Selectable())
      mItem = (cMainMenuItem*)Next(mItem);
    if(mItem)
      FirstSelectable = mItem->Index();
  }

  delete(DVDList);
  delete(DirList);
}

void cMainMenu::BuildDisp2(const char *dir)
{
  cMainMenuItem *mItem = NULL;
  cDVDList *DVDList = new cDVDList;
  cDVDListItem *dItem = NULL;

  cDirHandling *DirHand = new cDirHandling(this, &MainMenuOptions);
  FirstSelectable = DirHand->Build(dir, !DVDSwitchSetup.HideEmptyDirs, ImageList);
  delete(DirHand);

  if(DVDSwitchSetup.DisplayDVDDevice && !strcasecmp(dir, MainMenuOptions.ImageDir()))
  {
    Add(new cMainMenuItem(iDevice, ImageList));
    if(!MainMenuOptions.getLastSelectItemName() && MainMenuOptions.LastSelectItemType() == iDevice)
    {
      mItem = (cMainMenuItem*)Last();
      FirstSelectable = mItem->Index();
    }
  }
  
  if(DVDList->Create(dir,
     ImageList.GetExtensions(),
     ImageList.GetDirContains(),
     (eFileList)DVDSwitchSetup.SortMode,
     false))
  {
    dItem = DVDList->First();
    while(dItem)
    {
      Add(new cMainMenuItem(iDVD, ImageList, dItem->FileName()));
      if(MainMenuOptions.getLastSelectItemName() &&
         !strcasecmp(dItem->FileName(), MainMenuOptions.getLastSelectItemName()))
      {
        mItem = (cMainMenuItem*)Last();
        FirstSelectable = mItem->Index();
        mItem = NULL;
      }
      dItem = DVDList->Next(dItem);
    }
  }

  delete(DVDList);

  mItem = (cMainMenuItem*)First();
  if(mItem && FirstSelectable < 0)
    FirstSelectable = 0;
}

void cMainMenu::SetHelp(void)
{
  cMainMenuItem *mItem = (cMainMenuItem*)Get(Current());

  const char *kRed = NULL, *kGreen = NULL, *kYellow = NULL, *kBlue = NULL;

  switch((eCMDs)DVDSwitchSetup.kRed)
  {
    case cmdImgPlay:
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
      {
         kRed = tr("to change");
         break;
      }
      if(mItem && !DVDSwitchSetup.DVDPluginExist)
        break;
    case cmdImgRename:
    case cmdImgMove:
    case cmdImgDelete:
    case cmdImgBurn:
      if(!mItem)
        break;
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
        break;
      if(mItem && (eCMDs)DVDSwitchSetup.kRed != cmdImgPlay && mItem->Type() != iDVD)
        break;
    case cmdDirManage:
    case cmdDVDopen:
    case cmdDVDclose:
    case cmdImgRead:
    case cmdCommands:
      kRed = DVDSwitchSetup.CommandsShortName[DVDSwitchSetup.kRed];
      break;
    default:
      break;
  }

  switch((eCMDs)DVDSwitchSetup.kGreen)
  {
    case cmdImgPlay:
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
      {
        kGreen = tr("to change");
        break;
      }
      if(mItem && !DVDSwitchSetup.DVDPluginExist)
        break;
    case cmdImgRename:
    case cmdImgMove:
    case cmdImgDelete:
    case cmdImgBurn:
      if(!mItem)
        break;
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
        break;
      if(mItem && (eCMDs)DVDSwitchSetup.kGreen != cmdImgPlay && mItem->Type() != iDVD)
        break;
    case cmdDirManage:
    case cmdDVDopen:
    case cmdDVDclose:
    case cmdImgRead:
    case cmdCommands:
      kGreen = DVDSwitchSetup.CommandsShortName[DVDSwitchSetup.kGreen];
      break;
    default:
      break;
  }

  switch((eCMDs)DVDSwitchSetup.kYellow)
  {
    case cmdImgPlay:
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
      {
        kYellow = tr("to change");
        break;
      }
      if(mItem && !DVDSwitchSetup.DVDPluginExist)
        break;
    case cmdImgRename:
    case cmdImgMove:
    case cmdImgDelete:
    case cmdImgBurn:
      if(!mItem)
        break;
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
        break;
      if(mItem && (eCMDs)DVDSwitchSetup.kYellow != cmdImgPlay && mItem->Type() != iDVD)
        break;
    case cmdDirManage:
    case cmdDVDopen:
    case cmdDVDclose:
    case cmdImgRead:
    case cmdCommands:
      kYellow = DVDSwitchSetup.CommandsShortName[DVDSwitchSetup.kYellow];
      break;
    default:
      break;
  }

  switch((eCMDs)DVDSwitchSetup.kBlue)
  {
    case cmdImgPlay:
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
      {
        kBlue = tr("to change");
        break;
      }
      if(mItem && !DVDSwitchSetup.DVDPluginExist)
        break;
    case cmdImgRename:
    case cmdImgMove:
    case cmdImgDelete:
    case cmdImgBurn:
      if(!mItem)
        break;
      if(mItem && (mItem->Type() == iParent || mItem->Type() == iDir))
        break;
      if(mItem && (eCMDs)DVDSwitchSetup.kBlue != cmdImgPlay && mItem->Type() != iDVD)
        break;
    case cmdDirManage:
    case cmdDVDopen:
    case cmdDVDclose:
    case cmdImgRead:
    case cmdCommands:
      kBlue = DVDSwitchSetup.CommandsShortName[DVDSwitchSetup.kBlue];
      break;
    default:
      break;
  }

  cOsdMenu::SetHelp(kRed, kGreen, kYellow, kBlue);
}

eOSState cMainMenu::ProcessKey(eKeys Key)
{
  if(!HasSubMenu())
  {
    switch(State)
    {
      case mmsImgRename:
        switch(Key)
        {
          case kBack:
            SetState(mmsNone);
            DVDSwitchSetup.HideTypeCol = CMDImg->tmpHideTypeCol;
            DELETENULL(CMDImg);
            Build(MainMenuOptions.CurrentDir());
            Display();
            return osContinue;
            break;
          case kOk: {
              eOSState nState = cOsdMenu::ProcessKey(Key);

              SetState(mmsNone);
              DVDSwitchSetup.HideTypeCol = CMDImg->tmpHideTypeCol;
              if(!isempty(CMDImg->NewFile))
              {
                int iRet = 0;
                char *buffer = NULL;
                char *buffer2 = NULL;
                cFileInfo *info = new cFileInfo(CMDImg->Rename());
                if(ImageList.IsHide(info->Extension()))
                {
                  buffer2 = strdup(info->Extension());
                  iRet = asprintf(&buffer, "%s/%s%s", info->Path(), stripspace(CMDImg->NewFile), buffer2);
                }
                else
                  iRet = asprintf(&buffer, "%s/%s", info->Path(), stripspace(CMDImg->NewFile));
                DELETENULL(info);
                if(iRet > 0) {
                  info = new cFileInfo(buffer);
                  if(!info->isExists())
                  {
                    if(cFileCMD::Rn(CMDImg->Rename(), buffer))
                      MainMenuOptions.setLastSelectItemName(buffer);
                  }
                  else
                    OsdMsg(mtWarning,tr("File exists in Directory"));
                  FREENULL(buffer);
                  DELETENULL(info);
                }
                FREENULL(buffer2);
              }
              DELETENULL(CMDImg);
              Build(MainMenuOptions.CurrentDir());
              Display();
              return nState;
            }
          default:
            break;
        }
        break;
      default:
        switch(Key & ~k_Repeat)
        {
          case kUp:
          case kDown:
          case kRight:
          case kLeft:
            return MenuMove(Key);
            break;
          case k1:
          case k2:
          case k3:
          case k4:
          case k5:
          case k6:
          case k7:
          case k8:
          case k9:
          case k0:
          case kRed:
          case kGreen:
          case kYellow:
          case kBlue:
          case kOk:
            return Commands(Key);
            break;
          default:
            break;
        }
    }
  }

  return cOsdMenu::ProcessKey(Key);
}

eOSState cMainMenu::MenuMove(eKeys Key)
{
  eOSState state = osContinue;
  cMainMenuItem *mItem = NULL;
  int step = 0;

  if(DVDSwitchSetup.DisplayMode == 1)
  {
    switch(Key & ~k_Repeat)
    {
      case kDown:
      case kUp:
        state = cOsdMenu::ProcessKey(Key);
        if(Count() &&
           Current() == FirstSelectable)
        {
          SetCurrent(Get(0));
          Display();
          SetCurrent(Get(FirstSelectable));
          Display();
        }
        break;
      case kLeft:
        if(Count() &&
          DVDSwitchSetup.JumpCatByKey)
        {
          mItem = (cMainMenuItem*)Get(Current());
          step = 0;
          while(step < 4)
          {
            while(mItem && step == 0)
            {
              if(mItem->Type() == iCat)
              {
                step++;
                break;
              }
              mItem = (cMainMenuItem*)Prev(mItem);
            }
            while(mItem && step == 1)
            {
              if(mItem->Type() != iCat)
              {
                step++;
                break;
              }
              mItem = (cMainMenuItem*)Prev(mItem);
            }
            while(mItem && step == 2)
            {
              if(mItem->Type() == iCat)
              {
                step++;
                break;
              }
              mItem = (cMainMenuItem*)Prev(mItem);
            }
#if VDRVERSNUM >= 10327
            if(!mItem && Setup.MenuScrollWrap)
            {
              mItem = (cMainMenuItem*)Last();
              if(mItem->Type() != iCat)
                step = 2;
              else
                step = 1;
            }
            else
#endif
              step = 4;
          }
          if(mItem)
          {
            SetCurrent(Next(mItem));
            Display();
          }
        }
        else
          state = cOsdMenu::ProcessKey(Key);
        if(Count() &&
           Current() == FirstSelectable)
        {
          SetCurrent(Get(0));
          Display();
          SetCurrent(Get(FirstSelectable));
          Display();
        }
        break;
      case kRight:
        if(Count() &&
           DVDSwitchSetup.JumpCatByKey)
        {
          mItem = (cMainMenuItem*)Get(Current());
          step = 0;
          while(step < 3)
          {
            while(mItem && step == 0)
            {
              if(mItem->Type() == iCat)
              {
                step++;
                break;
              }
              mItem = (cMainMenuItem*)Next(mItem);
            }
            while(mItem && step == 1)
            {
              if(mItem->Type() != iCat)
              {
                step++;
                break;
              }
              mItem = (cMainMenuItem*)Next(mItem);
            }
#if VDRVERSNUM >= 10327
            if(!mItem && Setup.MenuScrollWrap)
            {
              mItem = (cMainMenuItem*)First();
              step = 1;
            }
            else
#endif
              step = 3;
          }
          if(mItem)
          {
            SetCurrent(mItem);
            Display();
          }
        }
        else
          state = cOsdMenu::ProcessKey(Key);
        if(Count() &&
           Current() == FirstSelectable)
          {
            SetCurrent(Get(0));
            Display();
            SetCurrent(Get(FirstSelectable));
            Display();
          }
        break;
      default:
        state = cOsdMenu::ProcessKey(Key);
        break;
    }
  }
  else
    state = cOsdMenu::ProcessKey(Key);
  SetHelp();

  mItem = (cMainMenuItem*)Get(Current());
  if(mItem) {
    MainMenuOptions.setLastSelectItemName(mItem->FileName());
    MainMenuOptions.LastSelectItemType(mItem->Type());
  }
  return state;
}


eOSState cMainMenu::SelectItem(void)
{
  cMainMenuItem *mItem = (cMainMenuItem*)Get(Current());
  cFileInfo *info = NULL;
  cDirHandling *DirHand = NULL;

  if (mItem)
  {
    switch(mItem->Type())
    {
      case iParent:
      case iDir:
        info = new cFileInfo(mItem->FileName());
        if(!info->isExecutable() || !info->isReadable())
        {
          DELETENULL(info);
          OsdMsg(mtError,tr("No rights to access this directory!"));
          return osContinue;
          break;
        }
        DELETENULL(info);
        DirHand = new cDirHandling(this, &MainMenuOptions);
        DirHand->ProcessKey(mItem);
        delete(DirHand);
        Build(MainMenuOptions.CurrentDir());
        break;
      case iDVD:
      case iDevice:
        return cCMD::Play(mItem);
        break;
      default:
        break;
    }
  }

  return osContinue;
}

eOSState cMainMenu::Commands(eKeys Key)
{
  const cMainMenuItem *mItem = (const cMainMenuItem*)Get(Current());
  eCMDs cmd = cmdNone;
  cCMDImage *CMDImage = NULL;

  if(!HasSubMenu())
  {
    switch(Key)
    {
      case k1:
        cmd = (eCMDs)DVDSwitchSetup.k1;
        break;
      case k2:
        cmd = (eCMDs)DVDSwitchSetup.k2;
        break;
      case k3:
        cmd = (eCMDs)DVDSwitchSetup.k3;
        break;
      case k4:
        cmd = (eCMDs)DVDSwitchSetup.k4;
        break;
      case k5:
        cmd = (eCMDs)DVDSwitchSetup.k5;
        break;
      case k6:
        cmd = (eCMDs)DVDSwitchSetup.k6;
        break;
      case k7:
        cmd = (eCMDs)DVDSwitchSetup.k7;
        break;
      case k8:
        cmd = (eCMDs)DVDSwitchSetup.k8;
        break;
      case k9:
        cmd = (eCMDs)DVDSwitchSetup.k9;
        break;
      case k0:
        cmd = (eCMDs)DVDSwitchSetup.k0;
        break;
      case kRed:
        cmd = (eCMDs)DVDSwitchSetup.kRed;
        break;
      case kGreen:
        cmd = (eCMDs)DVDSwitchSetup.kGreen;
        break;
      case kYellow:
        cmd = (eCMDs)DVDSwitchSetup.kYellow;
        break;
      case kBlue:
        cmd = (eCMDs)DVDSwitchSetup.kBlue;
        break;
      case kOk:
        cmd = (eCMDs)DVDSwitchSetup.kOk;
        break;
      default:
        break;
    }
    switch(cmd)
    {
      case cmdDirManage:
        return AddSubMenu(new cCMDDir(ImageList, this));
        break;
      case cmdDVDopen:
        return cCMD::Eject(false);
        break;
      case cmdDVDclose:
        return cCMD::Eject(true);
        break;
      case cmdImgPlay:
        return SelectItem();
        break;
      case cmdImgRename:
        if(mItem && mItem->Type() == iDVD)
        {
          SetState(mmsImgRename);
          cOsdMenu::ProcessKey(kRight);
          return osContinue;
        }
        break;
      case cmdImgMove:
        if(mItem) {
          return AddSubMenu(new cCMDMove(ImageList, mItem->FileName(), this, false, true));
        }
        break;
      case cmdImgDelete:
        if(mItem && mItem->Type() == iDVD)
        {
          CMDImage = new cCMDImage(this);
          CMDImage->Delete(mItem->FileName());
          DELETENULL(CMDImage);
          return osContinue;
        }
        break;
      case cmdImgBurn:
        if(mItem && mItem->Type() == iDVD)
        {
          CMDImage = new cCMDImage(this);
          CMDImage->Burn(mItem->FileName());
          DELETENULL(CMDImage);
          return osContinue;
        }
        break;
      case cmdImgRead:
        return AddSubMenu(new cCMDImageRead(ImageList));
        break;
      case cmdCommands:
        return AddSubMenu(new cCMDMenu(ImageList, mItem, this));
      default:
        break;
    }
  }

  return cOsdMenu::ProcessKey(Key);
}

char *cMainMenu::CreateOSDName(eMainMenuItem itype, cImageList &ImageList, const char *file)
{
  cStringValue buffer;
  cStringValue tmpOSD;
  cFileInfo *info = NULL;
  char *reg = NULL;
  int i = 0;
  int len = 0;

  switch(itype)
  {
    case iCat:
      if(0 >= asprintf(&reg, "^%s", MainMenuOptions.ImageDir()))
        break;
      if(!RegIMatch(file, reg))
        buffer = file;
      else
      {
        const char *p = &file[strlen(MainMenuOptions.ImageDir())];
        if(p[0] == '/')
          p++;
        cTokenizer *token = new cTokenizer(p, "/");
        for (i = 1; i <= token->Count(); i++)
        {
          char * s = strdup(token->GetToken(i));
          ChangeChars(s, DVDSwitchSetup.ChangeCharsOSDName);
          buffer += s;
          if(i != token->Count())
            buffer += DVDSwitchSetup.SubCatCutter;
          free(s);
        }
        DELETENULL(token);
      }
      FREENULL(reg);
      for (i = 0; i < DVDSwitchSetup.CharCountBeforeCat; i++)
        tmpOSD += &DVDSwitchSetup.CatLineChar;
      if(DVDSwitchSetup.SpacesBeforeAfterCat)
        tmpOSD += " ";
      if(isempty(&buffer))
        tmpOSD += tr("without category");
      else
        tmpOSD += &buffer;
      if(DVDSwitchSetup.SpacesBeforeAfterCat)
        tmpOSD += " ";
      len = tmpOSD.len();
      for (i = 0; i < (119 - len); i++)
        tmpOSD += &DVDSwitchSetup.CatLineChar;
      break;
    case iParent:
      buffer = "..";
    case iDir:
      if(!&buffer && file)
      {
        info = new cFileInfo(file);
        buffer = info->FileName();
      }
      tmpOSD = "[";
      tmpOSD += &buffer;
      tmpOSD += "]";
      break;
    case iDVD:
      info = new cFileInfo(file);
      if(ImageList.IsHide(info->Extension()))
        buffer = info->FileNameWithoutExt();
      else
        buffer = info->FileName();
      ChangeChars(&buffer, DVDSwitchSetup.ChangeCharsOSDName);

      if(DVDSwitchSetup.HideImgSizeCol)
      {
        char *size = NULL;
        if(0 < asprintf(&size, "(%03.1f)\t", info->SizeGByte(1))) {
          tmpOSD += size;
          free(size);
        }
      }
      tmpOSD += &buffer;
      
      //tmpOSD = &buffer;
      if(DVDSwitchSetup.HideTypeCol)
      {
        tmpOSD += "\t";
        tmpOSD += ImageList.GetShortName(file);
      }
      break;
    case iDevice:
      tmpOSD = "--> (";
      tmpOSD += tr("DVD-Drive");
      tmpOSD += ") <--";
      break;
    default:
      break;
  }

  delete(info);
  free(reg);

  return strdup(&tmpOSD);
}

void cMainMenu::SetState(eMainMenuState state)
{
  State = state;
  cMainMenuItem *mItem = NULL;
  cMainMenuItem *dmItem = NULL;
  cFileInfo *info = NULL;

  switch(State)
  {
    case mmsReInit:
      SetState(mmsNone);
      Init();
      Build(MainMenuOptions.CurrentDir());
      break;
    case mmsReInitCur:
      SetState(mmsNone);
      Build(MainMenuOptions.CurrentDir());
      break;
    case mmsImgRename: {
      DELETENULL(CMDImg);
      mItem = (cMainMenuItem*)Get(Current());
      if(mItem->Type() != iDVD)
      {
        SetState(mmsNone);
        break;
      }
      info = new cFileInfo(mItem->FileName());
      if(!info->isWriteable())
      {
        DELETENULL(info);
        info = new cFileInfo(MainMenuOptions.CurrentDir());
        if(!info->isWriteable())
        {
          OsdMsg(mtError,tr("Missing rights to rename!"));
          SetState(mmsNone);
          DELETENULL(info);
          break;
        }
      }
      DELETENULL(info);
      CMDImg = new cCMDImage();
      CMDImg->Rename(mItem->FileName());
      CMDImg->tmpHideTypeCol = DVDSwitchSetup.HideTypeCol;
      if(DVDSwitchSetup.HideTypeCol)
      {
        DVDSwitchSetup.HideTypeCol = 0;
        Build(MainMenuOptions.CurrentDir());
      }
      const char* szRename = tr("Rename");
      SetCols(strlen(szRename) + 3);
      mItem = (cMainMenuItem*)First();
      while(mItem)
      {
        if(mItem->Index() == Current())
        {
          info = new cFileInfo(mItem->FileName());
          strn0cpy(CMDImg->NewFile, 
            ImageList.IsHide(info->Extension()) ? info->FileNameWithoutExt() : info->FileName(), 
            memberof(CMDImg->NewFile));
          DELETENULL(info);
          Ins(new cMenuEditStrItem(szRename, CMDImg->NewFile, memberof(CMDImg->NewFile)),
              true,
              mItem);
          dmItem = mItem;
        }
        mItem->SetSelectable(false);
        mItem = (cMainMenuItem*)Next(mItem);
      }
      if(dmItem)
        Del(dmItem->Index());
      Display();
      break;
      }
    default:
      break;
  }
}

// --- cDirHandling -----------------------------------------

cDirHandling::cDirHandling(cOsdMenu *osdobject, cDirHandlingOpt *dirobject)
{
  OsdObject = osdobject;
  DirObject = dirobject;
}

int cDirHandling::Build(const char *dir, bool emptydirs, cImageList &ImageList)
{
  int ret = -1;
  cMainMenuItem *mItem = NULL;

  cDirList *DirList = new cDirList(ImageList);
  if(DirList->Load(dir, false))
  {
    if(DirObject->isParent(dir))
      OsdObject->Add(new cMainMenuItem(iParent, ImageList, DirObject->ParentDir()));

    cFileListItem *fItem = DirList->First();
    while(fItem)
    {
      if(!DirList->DirIsIn(fItem, ImageList.GetDirContains()) &&
          (!DirList->DirIsEmpty(fItem) || emptydirs))
      {
        OsdObject->Add(new cMainMenuItem(iDir, ImageList, fItem->Value()));
        mItem = (cMainMenuItem*)OsdObject->Last();
        if(mItem &&
           DirObject->getLastSelectItemName() &&
           !strcasecmp(mItem->FileName(), DirObject->getLastSelectItemName()))
          ret = mItem->Index();
        if(mItem && DirObject->isLastSel(mItem->FileName()))
          ret = mItem->Index();
      }
      fItem = DirList->Next(fItem);
    }
  }

  return ret;
}

void cDirHandling::ProcessKey(cMainMenuItem *mItem)
{
  cFileInfo *info = NULL;

  if (mItem)
  {
    switch(mItem->Type())
    {
      case iParent:
        DirObject->LastSelDir(DirObject->CurrentDir());
        DirObject->CurrentDir(DirObject->ParentDir());
        if(strcasecmp(DirObject->ImageDir(), DirObject->ParentDir()))
        {
          info = new cFileInfo(DirObject->ParentDir());
          DirObject->ParentDir(info->Path());
          DELETENULL(info);
        }
        break;
      case iDir:
        DirObject->ParentDir(DirObject->CurrentDir());
        DirObject->CurrentDir(mItem->FileName());
        break;
      default:
        break;
    }
  }
}
