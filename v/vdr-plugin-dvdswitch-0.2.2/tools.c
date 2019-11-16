
#include "imagelist.h"
#include "tools.h"
#include "setup.h"

cDirList::cDirList(cImageList &ImageList)
{
  char *buffer = NULL;

  OptExclude("^lost\\+found$"); // lost+found Dir
  OptExclude("^\\."); // hidden Files
  OptExclude("\\.sdel$"); // del Files
  OptExclude("\\.smove$"); // move Files

  cTokenizer *token = new cTokenizer(ImageList.GetDirContains(), "@");
  for(int i = 1; i <= token->Count(); i++)
  {
    if(0 < asprintf(&buffer, "^%s$", token->GetToken(i))) {
      OptExclude(buffer);
      FREENULL(buffer);
    }
  }

  OptSort((eFileList)DVDSwitchSetup.SortMode);
  OptFilterType(tDir);

  delete(token);
}

cFileDelThread::cFileDelThread(const char *file)
{
  File = NULL;
  Ok = false;

  errno = 0;  
  if(!RightCheck(file))
    OSDErrorNumMsg(errno,tr("No rights to delete image"));
  else
  {
    if(file)
    {
      if(0 < asprintf(&File, "%s.sdel", file)) {
        dsyslog("dvdswitch: Prepare delete (rename %s to %s)",file, File);
        if(cFileCMD::Rn(file, File))
          Ok = true;
        else
          OSDErrorNumMsg(errno,tr("Operation failed"));
      }
    }
  }
}

cFileDelThread::~cFileDelThread(void) 
{ 
  free(File); 
}

bool cFileDelThread::RightCheck(const char *value)
{
  bool ret = false;
  if(value)
  {
    cFileInfo *info = new cFileInfo(value);
    ret = info->isWriteable();
    DELETENULL(info);
  }
  return ret;
}

void cFileDelThread::Action(void)
{
  if(File) {
    dsyslog("dvdswitch: Execute remove %s",File);
    errno = 0;
    if(!cFileCMD::Del(File)) {
      char* err = get_strerror(errno);
      esyslog("dvdswitch: could not remove failed %s :%s", File, err ? err : "");
      if(err) free(err);
    }
  }
  delete(this);
};

cFileMoveThread::cFileMoveThread(const char *file, char *dest)
{
  FileName = NULL;
  File = NULL;
  Dest = NULL;
  Ok = false;
  
  errno = 0;
  if(!RightCheck(file) || !RightCheck(dest))
    OSDErrorNumMsg(errno,tr("No rights to move image"));
  else
  {
    if(file)
    {
      cFileInfo *info = new cFileInfo(file);
      FileName = strdup(info->FileName());
      DELETENULL(info);
      if(0 < asprintf(&File, "%s.smove", file)) {
        dsyslog("dvdswitch: Prepare move %s to %s",file, File);
        if(cFileCMD::Rn(file, File))
          Ok = true;
        else
          OSDErrorNumMsg(errno,tr("Operation failed"));
      }
    }
    Dest = dest ? strdup(dest) : NULL;
  }
}

cFileMoveThread::~cFileMoveThread(void)
{
  free(FileName);
  free(File);
  free(Dest);
}

bool cFileMoveThread::RightCheck(const char *value)
{
  bool ret = false;
  if(value)
  {
    cFileInfo *info = new cFileInfo(value);
    ret = info->isWriteable();
    DELETENULL(info);
  }
  return ret;
}

void cFileMoveThread::Action(void)
{
  if(FileName && File && Dest)
  {
    char *buffer = NULL;
    if(0 < asprintf(&buffer, "%s/%s", Dest, FileName)) {
      dsyslog("dvdswitch: Execute move %s to %s",File, buffer);
      errno = 0;
      if(!cFileCMD::Rn(File, buffer)) {
        char* err = get_strerror(errno);
        esyslog("dvdswitch: could not move file %s to %s :%s", File, buffer, err ? err : "");
        if(err) free(err);
      }
      free(buffer);
    }
  }
  delete(this);
}


