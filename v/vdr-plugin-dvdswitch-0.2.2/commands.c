#include <unistd.h>
#include <mntent.h>
#include <vdr/tools.h>
#include "commands.h"
#include "imagelist.h"
#include "setup.h"
#include "tools.h"
#include "menu-item.h"
#include "dvdplugin.h"
#include "setup-itypes.h"
#include "volname.h"
// --- cCMD ----------------------------------------------------------

eOSState cCMD::Play(const cMainMenuItem *item)
{
  if(item)
    isyslog("dvdswitch: play %s, %i", item->FileName(), item->Type());
  else
    esyslog("dvdswitch: play - none item defined");

  if(!item || item->Type() == iDevice)
    cDVDPlugin::Start();
  else if (item && item->Type() == iDVD)
    cDVDPlugin::Start(item->FileName());

  return osEnd;
}

eOSState cCMD::Eject(bool close)
{
  char *cmd = NULL;
  if(0 < asprintf(&cmd, "eject %s %s", close ? "-t" : "", DVDSwitchSetup.DVDLinkOrg)) {
    isyslog("dvdswitch: eject media: %i - %s", close, cmd);
    int rc = system(cmd);
    dsyslog("dvdswitch: result of eject media: %i", rc);
    free(cmd);
  }
  return osContinue;
}

// --- cCMDMenu -------------------------------------------------------------

cCMDMenu::cCMDMenu(cImageList &imagelist, const cMainMenuItem *item, cMainMenu *osdobject)
  : cOsdMenu(tr("Commands"))
  , ImageList(imagelist)
{
  iItem = item;
  OsdObject = osdobject;
  cOsdItem *mItem = NULL;

  Add(new cOsdItem(tr("Directory Management")));
  Add(new cOsdItem(tr("DVD-Tray open")));
  Add(new cOsdItem(tr("DVD-Tray close")));
  Add(new cOsdItem(tr("Play")));
  if(!iItem || (iItem && iItem->Type() != iDVD && iItem->Type() != iDevice))
  {
    mItem = Last();
    mItem->SetSelectable(false);
  }
  Add(new cOsdItem(tr("Image rename")));
  if(!iItem || (iItem && iItem->Type() != iDVD))
  {
    mItem = Last();
    mItem->SetSelectable(false);
  }
  Add(new cOsdItem(tr("Image move")));
  if(!iItem || (iItem && iItem->Type() != iDVD))
  {
    mItem = Last();
    mItem->SetSelectable(false);
  }
  Add(new cOsdItem(tr("Image delete")));
  if(!iItem || (iItem && iItem->Type() != iDVD))
  {
    mItem = Last();
    mItem->SetSelectable(false);
  }
  Add(new cOsdItem(tr("Image burn")));
  if(!iItem || (iItem && iItem->Type() != iDVD))
  {
    mItem = Last();
    mItem->SetSelectable(false);
  }
  Add(new cOsdItem(tr("Image to create from DVD")));
}

eOSState cCMDMenu::ProcessKey(eKeys Key)
{
  eOSState ret = osUnknown;

  if(!HasSubMenu())
  {
    cCMDImage *CMDImage = NULL;
    cOsdItem *item = NULL;
    eCMDs cmd = cmdNone;
    switch(Key)
    {
      case kOk:
        item = Get(Current());
        if(!strcasecmp(item->Text(),tr("Directory Management")))
          cmd = cmdDirManage;
        if(!strcasecmp(item->Text(),tr("DVD-Tray open")))
          cmd = cmdDVDopen;
        if(!strcasecmp(item->Text(),tr("DVD-Tray close")))
          cmd = cmdDVDclose;
        if(!strcasecmp(item->Text(),tr("Play")))
          cmd = cmdImgPlay;
        if(!strcasecmp(item->Text(),tr("Image rename")))
          cmd = cmdImgRename;
        if(!strcasecmp(item->Text(),tr("Image move")))
          cmd = cmdImgMove;
        if(!strcasecmp(item->Text(),tr("Image delete")))
          cmd = cmdImgDelete;
        if(!strcasecmp(item->Text(),tr("Image burn")))
          cmd = cmdImgBurn;
        if(!strcasecmp(item->Text(),tr("Image to create from DVD")))
          cmd = cmdImgRead;
        switch(cmd)
        {
          case cmdDirManage:
            return AddSubMenu(new cCMDDir(ImageList, OsdObject));
            break;
          case cmdDVDopen:
            return cCMD::Eject(false);
            break;
          case cmdDVDclose:
            return cCMD::Eject(true);
            break;
          case cmdImgPlay:
            return cCMD::Play(iItem);
            break;
          case cmdImgRename:
            OsdObject->SetState(mmsImgRename);
            item = OsdObject->Get(OsdObject->Current());
            item->ProcessKey(kRight);
            return osBack;
            break;
          case cmdImgMove:
            return AddSubMenu(new cCMDMove(ImageList, iItem->FileName(), OsdObject, false));
            break;
          case cmdImgDelete:
            CMDImage = new cCMDImage(OsdObject);
            ret = CMDImage->Delete(iItem->FileName());
            DELETENULL(CMDImage);
            return ret;
            break;
          case cmdImgBurn:
            CMDImage = new cCMDImage(OsdObject);
            ret = CMDImage->Burn(iItem->FileName());
            DELETENULL(CMDImage);
            return ret;
            break;
          case cmdImgRead:
            return AddSubMenu(new cCMDImageRead(ImageList));
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }

  return cOsdMenu::ProcessKey(Key);
}

// --- cCMDDir ------------------------------------------------------------

cCMDDir::cCMDDir(cImageList &imagelist, cMainMenu *osdobject, bool select, char *lastdir)
  : cOsdMenu(tr("Directory Management"))
  , ImageList(imagelist)
{
  ImageDir(DVDSwitchSetup.ImageDir);
  CurrentDir(DVDSwitchSetup.ImageDir);
  ParentDir(DVDSwitchSetup.ImageDir);

  State = csNone;
  OsdObject = osdobject;
  Select = select;
  LastDir = lastdir;
  SetDir();

  if(Select)
    SetTitle(tr("Select Directory"));

  Build(DVDSwitchSetup.ImageDir);
  cMainMenuItem *mItem = (cMainMenuItem*)Get(Current());
  if(mItem)
    LastSelDir(mItem->FileName());
}

void cCMDDir::SetHelp(void)
{
  cMainMenuItem *item = NULL;
  switch(State)
  {
    case csNone:
      item = (cMainMenuItem*)Get(Current());
      if(Count() && item && item->Type() == iDir)
        if(Select)
          cOsdMenu::SetHelp(NULL, tr("New"), NULL , tr("Select"));
        else
          cOsdMenu::SetHelp(tr("Rename"), tr("New"), tr("Delete"), tr("Move"));
      else
        cOsdMenu::SetHelp(NULL, tr("New"), NULL , NULL);
      break;
    default:
      cOsdMenu::SetHelp(NULL, NULL, NULL , NULL);
      break;
  }
}

void cCMDDir::Build(const char *dir)
{
  if(!dir)
    dir = CurrentDir();

  Clear();
  if(State == csNone)
  {
    SetCols(0);
    SetTitle(tr("Edit directories"));
  }

  cDirHandling *DirHand = new cDirHandling(this, this);
  SetCurrent(Get(DirHand->Build(dir, true, ImageList)));
  delete(DirHand);

  Display();
  SetHelp();
}

eOSState cCMDDir::ProcessKey(eKeys Key)
{
  cMainMenuItem *mItem = NULL;
  cDirHandling *DirHand = NULL;
  cFileInfo *info = NULL;

  if(!HasSubMenu())
  {
    switch(State)
    {
      case csNone:
        mItem = (cMainMenuItem*)Get(Current());
        switch(Key & ~k_Repeat)
        {
          case kUp:
          case kDown:
            cOsdMenu::ProcessKey(Key);
            SetHelp();
            mItem = (cMainMenuItem*)Get(Current());
            if(mItem)
              LastSelDir(mItem->FileName());
            return osContinue;
            break;
          case kOk:
            if(mItem) {
                info = new cFileInfo(mItem->FileName());
                if(!info->isExecutable() || !info->isReadable())
                {
                  esyslog("dvdswitch: Can't access to directory!");
                  DELETENULL(info);
                  OsdMsg(mtError,tr("No rights to access this directory!"));
                  return osContinue;
                  break;
                }
                DELETENULL(info);
                DirHand = new cDirHandling(this, this);
                DirHand->ProcessKey(mItem);
                delete(DirHand);
                Build();
            }
            break;
          case kRed:
            if(mItem->Type() == iDir)
              return Edit(mItem);
            break;
          case kGreen:
            return New();
            break;
          case kYellow:
            if(mItem->Type() == iDir && Interface->Confirm(tr("Really delete?")))
            {
              dsyslog("dvdswitch: Confirm delete directory");
              if(cFileCMD::DirIsEmpty(mItem->FileName()) ||
                 (!cFileCMD::DirIsEmpty(mItem->FileName()) && Interface->Confirm(tr("Directory not empty! However delete?"))))
              {
                cFileDelThread *del = new cFileDelThread(mItem->FileName());
                if(del->OK())
                {
                  dsyslog("dvdswitch: Start delete directory");
                  del->Start();
                  Build();
                  OsdObject->SetState(mmsReInit);
                }
                else
                  DELETENULL(del);
              }
              return osContinue;
            }
            break;
          case kBlue:
            if(mItem->Type() == iDir)
            {
              if(Select)
              {
                dsyslog("dvdswitch: Directory selected: %s", mItem->FileName());
                info = new cFileInfo(mItem->FileName());
                if(!info->isExecutable() || !info->isReadable())
                {
                  esyslog("dvdswitch: Can't access to directory!");
                  OsdMsg(mtError,tr("No rights to access this directory!"));
                }
                else
                {
                  const char *seldir = mItem->FileName();
                  for (unsigned int i = 0; i < strlen(DVDSwitchSetup.ImageDir); i++)
                    seldir++;
                  if(seldir[0] == '/')
                    seldir++;
                  if(LastDir)
                      strn0cpy(LastDir, seldir, memberof(Dir));
                  cRemote::Put(kBack);
                }
                DELETENULL(info);
                return osContinue;
              }
              else
                return AddSubMenu(new cCMDMove(ImageList, mItem->FileName(), OsdObject));
            }
            break;
          default:
            break;
        }
        break;
      case csDirNew:
        return New(Key);
        break;
      case csDirEdit:
        return Edit(Key);
        break;
      default:
        break;
    }
  }

  eOSState ret = cOsdMenu::ProcessKey(Key);

  if(!HasSubMenu() && Key == kBlue && ret == osContinue)
  {
    if((cMainMenuItem*)Get(Current() + 1))
      mItem = (cMainMenuItem*)Get(Current() + 1);
    else
      mItem = (cMainMenuItem*)Get(Current() - 1);
    if(mItem)
      LastSelDir(mItem->FileName());
    Build();
  }

  return ret;
}

eOSState cCMDDir::New(void)
{
  cFileInfo *info = new cFileInfo(CurrentDir());
  if(!info->isWriteable())
  {
    esyslog("dvdswitch: Can't create directory, missing rights for %s", CurrentDir());
    OsdMsg(mtError,tr("Missing rights to create directory!"));
  }
  else
  {
    State = csDirNew;
    SetCols(5);
    SetTitle(CurrentDir());
    SetDir();

    cMainMenuItem *mItem = (cMainMenuItem*)First();
    Ins(new cMenuEditStrItem(tr("New"), Dir, memberof(Dir)),
        true,
        mItem);
    while(mItem)
    {
      mItem->SetSelectable(false);
      mItem = (cMainMenuItem*)Next(mItem);
    }
    Display();
    cOsdMenu::ProcessKey(kRight);
  }

  return osContinue;
}

eOSState cCMDDir::New(eKeys Key)
{
  switch(Key)
  {
    case kOk:
	
	cOsdMenu::ProcessKey(kOk);
	
      if(!isempty(Dir))
      {
        char *buffer = NULL;
        if(0 < asprintf(&buffer, "%s/%s", CurrentDir(), stripspace(Dir))) {
          dsyslog("dvdswitch: Create new directory: %s", buffer);
          cFileInfo *info = new cFileInfo(buffer);
          if(info->isExists())
          {
            isyslog("dvdswitch: Directory still exists");
            OsdMsg(mtWarning,tr("Directory exists"));
            FREENULL(buffer);
            DELETENULL(info);
            return osContinue;
          }
          if(cFileCMD::Mkdir(buffer))
          {
            dsyslog("dvdswitch: Directory successfully created");
            LastSelDir(buffer);
            if(!Select)
              OsdObject->SetState(mmsReInit);
          }
          FREENULL(buffer);
          DELETENULL(info);
        }
      }
    case kBack:
      State = csNone;
      Build();
      return osContinue;
      break;
    default:
      break;
  }

  return cOsdMenu::ProcessKey(Key);
}


eOSState cCMDDir::Edit(cMainMenuItem *mItem)
{
  cFileInfo *info = new cFileInfo(mItem->FileName());
  if(!info->isWriteable())
  {
    DELETENULL(info);
    info = new cFileInfo(CurrentDir());
    if(!info->isWriteable())
    {
      esyslog("dvdswitch: Missing proper rights to rename");
      OsdMsg(mtError,tr("Missing rights to rename!"));
      DELETENULL(info);
      return osContinue;
    }
  }
  DELETENULL(info);
  info = new cFileInfo(mItem->FileName());
  SetDir(info->FileName());
  DELETENULL(info);
  State = csDirEdit;
  SetCols(11);
  SetTitle(CurrentDir());

  cMainMenuItem *dmItem = NULL;
  mItem = (cMainMenuItem*)First();
  while(mItem)
  {
    if(!strcasecmp(mItem->FileName(), LastSelDir()))
    {
      dsyslog("dvdswitch: Directory: Edit: Item found: %s", mItem->FileName());
      Ins(new cMenuEditStrItem(tr("Rename"), Dir, memberof(Dir)),
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
  cOsdMenu::ProcessKey(kRight);

  return osContinue;
}

eOSState cCMDDir::Edit(eKeys Key)
{
  switch(Key)
  {
    case kOk:
	
	cOsdMenu::ProcessKey(kOk);
	
      if(!isempty(Dir))
      {
        char *buffer = NULL;
        if(0 < asprintf(&buffer, "%s/%s", CurrentDir(), stripspace(Dir))) {
          dsyslog("dvdswitch: Directory: Edit: OK: %s", buffer);
          cFileInfo *info = new cFileInfo(buffer);
          if(info->isExists())
          {
            isyslog("dvdswitch: Directory: Edit: exists");
            OsdMsg(mtWarning,tr("Directory exists"));
            FREENULL(buffer);
            DELETENULL(info);
            return osUnknown;
          }
          if(cFileCMD::Rn(LastSelDir(), buffer))
          {
            dsyslog("dvdswitch: Directory: Edit: Rename OK");
            LastSelDir(buffer);
            OsdObject->SetState(mmsReInit);
          }
          FREENULL(buffer);
          DELETENULL(info);
        }
      }
    case kBack:
      State = csNone;
      Build();
      return osContinue;
      break;
    default:
      break;
  }

  return cOsdMenu::ProcessKey(Key);
}

// --- cCMDMove -------------------------------------------------------------

cCMDMove::cCMDMove(cImageList &imagelist, const char *file, cMainMenu *osdobject, bool dir, bool direct)
  : cOsdMenu(tr("Move"))
  , ImageList(imagelist)
{
  File = file ? strdup(file) : NULL;
  OsdObject = osdobject;
  Dir = dir;
  Direct = direct;

  ImageDir(DVDSwitchSetup.ImageDir);
  CurrentDir(DVDSwitchSetup.ImageDir);
  ParentDir(DVDSwitchSetup.ImageDir);


  Build(DVDSwitchSetup.ImageDir);
}

void cCMDMove::SetHelp(void)
{
  cOsdMenu::SetHelp(NULL, NULL, NULL , tr("Insert"));
}

void cCMDMove::Build(const char *dir)
{
  if(!dir)
    dir = CurrentDir();

  Clear();

  cDirHandling *DirHand = new cDirHandling(this, this);
  SetCurrent(Get(DirHand->Build(dir, true, ImageList)));
  delete(DirHand);

  if(Count())
  {
    cMainMenuItem *item = (cMainMenuItem*)First();
    while(item)
    {
      if(!strcasecmp(item->FileName(), File))
      {
        Del(item->Index());
        break;
      }
      item = (cMainMenuItem*)Next(item);
    }
  }

  Display();
  SetHelp();
}

eOSState cCMDMove::ProcessKey(eKeys Key)
{
  cMainMenuItem *mItem = (cMainMenuItem*)Get(Current());
  cDirHandling *DirHand = NULL;
  cFileMoveThread *move = NULL;

  switch(Key & ~k_Repeat)
  {
    case kUp:
    case kDown:
      cOsdMenu::ProcessKey(Key);
      SetHelp();
      mItem = (cMainMenuItem*)Get(Current());
      if(mItem)
        LastSelDir(mItem->FileName());
      return osContinue;
      break;
    case kOk:
      DirHand = new cDirHandling(this, this);
      DirHand->ProcessKey(mItem);
      DELETENULL(DirHand);
      Build();
      break;
    case kBlue:
      dsyslog("dvdswitch: Directory: Move to: %s", CurrentDir());
      move = new cFileMoveThread(File, CurrentDir());
      if(move->OK())
      {
        dsyslog("dvdswitch: Directory: Move successful");
        move->Start();
        cCondWait::SleepMs(1 * 500);
        OsdObject->SetState(mmsReInit);
      }
      else
        DELETENULL(move);
      if(!Dir && !Direct)
        cRemote::Put(kBack);
      return osBack;
      break;
    default:
      break;
  }

  return cOsdMenu::ProcessKey(Key);
}

// --- cCMDImage ------------------------------------------------------------

cCMDImage::cCMDImage(cMainMenu *osdobject)
{
  File = NULL;
  strcpy(NewFile, "\0");
  OsdObject = osdobject;
}

cCMDImage::~cCMDImage(void)
{
  if(File)
    free(File);
}

char* cCMDImage::Rename(const char *file)
{
  if(file)
  {
    FREENULL(File);
    File = strdup(file);
  }
  
  return File;
}

eOSState cCMDImage::Delete(const char *file)
{
  dsyslog("dvdswitch: Remove dvd image '%s'", file);
  if(file)
  {
    if(Interface->Confirm(tr("Really delete?")))
    {
      cFileDelThread *del = new cFileDelThread(file);
      if(del->OK())
      {
        dsyslog("dvdswitch: Confirm deletion of dvd image");
        del->Start();
        OsdObject->SetState(mmsReInitCur);
      }
      else
        DELETENULL(del);
    }
  }

  return osBack;
}

eOSState cCMDImage::Burn(const char *file)
{
  dsyslog("dvdswitch: Write dvd image '%s'", file);

  cFileInfo *info = new cFileInfo(DVDSwitchSetup.DVDWriteScript);

  if(!info->isExists())
  {
    esyslog("dvdswitch: Missing script to write dvd");
    DELETENULL(info);
    OsdMsg(mtError,tr("Specified Writescript not exist!"));
    return osContinue;
  }
  if(!info->isExecutable())
  {
    esyslog("dvdswitch: Can't execute script to write dvd");
    DELETENULL(info);
    OsdMsg(mtError,tr("Can't execute Writescript!"));
    return osContinue;
  }
  
  DELETENULL(info);

  info = new cFileInfo(file);

  if(Interface->Confirm(tr("Burn now?")))
  {
    dsyslog("dvdswitch: Start Burn-Thread");
    cCMDImageBurnThread *burn = new cCMDImageBurnThread(file, info->Type());
    burn->Start();
  }

  return osContinue;
}

// --- cCmdDVDRead ------------------------------------------------------------

cCMDImageRead::cCMDImageRead(cImageList &imagelist)
  : cOsdMenu(tr("Read DVD"), 14)
  , ImageList(imagelist)
  , m_szItemTypes(NULL)
{
  dsyslog("dvdswitch: Read DVD Image");

  cFileInfo info(DVDSwitchSetup.DVDReadScript);

  if(!info.isExists())
  {
    esyslog("dvdswitch: Missing script to read dvd");
    OsdMsg(mtError,tr("Specified Readscript not exist!"));
    cRemote::Put(kBack);
  }
  else if(!info.isExecutable())
  {
    esyslog("dvdswitch: Can't execute script to write dvd");
    OsdMsg(mtError,tr("Can't execute Readscript!"));
    cRemote::Put(kBack);
  }
  else
  {
    dsyslog("dvdswitch: query name of volume %s",DVDSwitchSetup.DVDLinkOrg);
    int err = volname(DVDSwitchSetup.DVDLinkOrg, File,sizeof(File));
    if(0 == err) {
      // Capitalization - lowercase name, except first character
      bool s;
      unsigned int n;
      for (n = 0,s = true; n < sizeof(File) && File[n] != '\0'; ++n) {
        if (isspace(File[n]) || '_' == File[n]) {
          File[n] = ' ';
          s = true;
        } else if(s) { s = false; }
               else  { File[n] = tolower(File[n]); }
      }

    } else {
      esyslog("dvdswitch: Can't query name of volume! %d", err);
      OSDErrorNumMsg(err, tr("Can't query name of volume!"));
      strcpy(File, "\0");
    }
    strcpy(Dir, "\0");
    strcpy(ImgTypeTxt, "\0");
    ImgType = 0;

    const char ** m_szItemTypes = new const char *[ImageList.Count()];
    cImageListItem *item = ImageList.First();
    unsigned int n = 0;
    while(item)
    {
      m_szItemTypes[n++] = item->GetLName();
      item = ImageList.Next(item);
    }

    Add(new cMenuEditStrItem(tr("Name"), File, memberof(File)));
    Add(new cMenuEditStrItem(tr("Directory"), Dir, memberof(Dir)));
    Add(new cMenuEditStraItem(tr("Type of image"), &ImgType, n, m_szItemTypes));

    SetHelp();
    Display();
  }
}

cCMDImageRead::~cCMDImageRead(void)
{
  dsyslog("dvdswitch: Read DVD Image stopped");
  if(m_szItemTypes)
    delete m_szItemTypes;
}

void cCMDImageRead::SetHelp(void)
{
  switch(Current())
  {
    case 1:
      cOsdMenu::SetHelp(tr("Read in"), NULL, NULL, tr("Select"));
      break;
    default:
      cOsdMenu::SetHelp(tr("Read in"), NULL, NULL, NULL);
      break;
  }
}

eOSState cCMDImageRead::ProcessKey(eKeys Key)
{
  if(!HasSubMenu())
  {
    switch(Key & ~k_Repeat)
    {
      case kUp:
      case kDown:
        cOsdMenu::ProcessKey(Key);
        SetHelp();
        return osContinue;
      case kRight:
        if(Current() == 1) {
            return osContinue;
        }
        break;
      case kRed:
        if(isempty(File))
        {
          OsdMsg(mtError,tr("Invalid file name for DVD-Image"));
          return osContinue;
        }
        if(ImgType < 0 || ImgType >= ImageList.Count())
        {
          OsdMsg(mtError,tr("No type of image selected"));
          return osContinue;
        }
        if(isempty(Dir))
        {
          if(!Interface->Confirm(tr("No directory selected. Use Standard?")))
            return osContinue;
        }
        if(Interface->Confirm(tr("Now read?")))
        {
          char *buffer = NULL;
          if(isempty(Dir))
            buffer = strdup(DVDSwitchSetup.ImageDir);
          else
          {
            if(DVDSwitchSetup.ImageDir[strlen(DVDSwitchSetup.ImageDir)-1] == '/') {
              if(0 >= asprintf(&buffer, "%s%s", DVDSwitchSetup.ImageDir, Dir)) {
                return osBack;
              }
            }
            else {
              if(0 >= asprintf(&buffer, "%s/%s", DVDSwitchSetup.ImageDir, Dir)) {
                return osBack;
              }
            }
          }
          if(buffer[strlen(buffer)-1] == '/')
            buffer[strlen(buffer)-1] = '\0';
          dsyslog("dvdswitch: Thread to read image are started");
          cCMDImageReadThread *read = new cCMDImageReadThread(File, buffer, ImgType, ImageList);
          FREENULL(buffer);
          read->Start();
          return osBack;
          }
        break;
      case kBlue:
        if(Current() == 1) {
            return AddSubMenu(new cCMDDir(ImageList, NULL, true, &Dir[0]));
        }
        break;
      default:
        break;
    }
  }

  return cOsdMenu::ProcessKey(Key);

}

cCMDImageReadThread::cCMDImageReadThread(char *file, char *dir, int imgtype, cImageList &ImageList)
{
  dsyslog("ReadThread created");
  File = NULL;
  Dir = NULL;
  FileType = tNone;

  cImageListItem *item = ImageList.Get(imgtype);
  if(imgtype >= 0 && file && dir)
  {
    if(item->GetFType() == tFile) {
      if(0 >= asprintf(&File, "%s%s", file, item->GetValue()))
        return;
    }
    else
      File = strdup(file);
    Dir = strdup(dir);
    FileType = item->GetFType();
  }
}

cCMDImageReadThread::~cCMDImageReadThread(void)
{
  free(File);
  free(Dir);
}

void cCMDImageReadThread::Action(void)
{
  dsyslog("ReadThread started");
  if(File && Dir && FileType != tNone)
  {
    dsyslog("ReadThread executed");
    char *cmd = NULL;
    char *mountpoint = NULL;

    char buffer[MaxFileName];
    if(realpath(DVDSwitchSetup.DVDLinkOrg, buffer)) {

	    FILE *f = setmntent("/etc/fstab", "r");
      if(f) {
	      struct mntent *m;
	      while ((m = getmntent(f))) {
		      //dsyslog("%s %s %s", m->mnt_fsname, m->mnt_dir, m->mnt_type);
          if(   m && m->mnt_fsname && m->mnt_dir &&
                (0 == strcmp(m->mnt_fsname, DVDSwitchSetup.DVDLinkOrg)
              || 0 == strcmp(m->mnt_fsname, buffer))) {
                  mountpoint = strdup(m->mnt_dir);
                  break;
           }

	      }
	      endmntent(f);
      }
      if(0 < asprintf(&cmd,
                "'%s' '%s' '%s' '%s' '%s' '%s'",
                DVDSwitchSetup.DVDReadScript,
                Dir,
                File,
                buffer,
                mountpoint ? mountpoint : "",
                (FileType == tFile) ? "IMAGE" : "DIR")) {
        dsyslog("ReadThread call: %s", cmd);
        int rc = system(cmd);
        dsyslog("ReadThread return value: %i", rc);
        FREENULL(cmd);
      }
      FREENULL(mountpoint);
    }
  }
  delete(this);
}


/******************************************************************************/

cCMDImageBurnThread::cCMDImageBurnThread(const char *file, eFileInfo type)
{
  dsyslog("BurnThread created");
  File = NULL;
  FileType = tNone;

  if(file && type != tNone)
  {
    File = strdup(file);
    FileType = type;
  }
}
cCMDImageBurnThread::~cCMDImageBurnThread(void) 
{ 
  free(File); 
}

void cCMDImageBurnThread::Action(void)
{
  dsyslog("BurnThread started");
  if(File && FileType != tNone)
  {
    dsyslog("BurnThread executed");
    char *cmd;
    if(0 < asprintf(&cmd,
              "'%s' '%s' '%s'",
              DVDSwitchSetup.DVDWriteScript,
              File,
              FileType == tFile ? "IMAGE" : "DIR")) {
      dsyslog("BurnThread call: %s", cmd);
      int rc = system(cmd);
      dsyslog("BurnThread return value: %i", rc);
      FREENULL(cmd);
    }
  }
  delete(this);
};

