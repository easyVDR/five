#include "imagelist.h"

//cImageList ImageList;

cImageList::cImageList(void)
{
  Setup = false;
}

cImageList::~ cImageList(void)
{
}

void cImageList::Init(void)
{
  if(!Setup)
  {
    Add(new cImageListItem(tr("Directory with 'VIDEO_TS' Folder"),
                           tr("Directory"),
                           tDir,
                           "video_ts",
                           true));
    Add(new cImageListItem(tr("File with '.iso' Extension"),
                           tr("Iso-Image"),
                           tFile,
                           ".iso",
                           true));
  }
}

void cImageList::AddItem(const char *item)
{
  if (!isempty(item))
  {
    cTokenizer *token = new cTokenizer(item, "|");
    if(token->Count() == 5)
      Add(new cImageListItem(token->GetToken(1),
                             token->GetToken(2),
                             (eFileInfo)atoi(token->GetToken(3)),
                             token->GetToken(4),
                             atoi(token->GetToken(5))));
    delete(token);
  }
}

void cImageList::AddSetup(const char *value)
{
  Setup = true;
  Clear();

  if(!isempty(value))
  {
    cTokenizer *token = new cTokenizer(value, "@");
    for(int i = 1; i <= token->Count(); i++)
      AddItem(token->GetToken(i));
    DELETENULL(token);
  }
}

char *cImageList::GetSetupString(void)
{
  SString = NULL;
  cImageListItem *item = First();

  while(item)
  {
    SString += item->SaveString();
    SString += "@";
    item = Next(item);
  }

  return &SString;
}

char *cImageList::GetExtensions(void)
{
  Ext = NULL;
  cImageListItem *item = First();

  while(item)
  {
    if(item->GetFType() == tFile)
    {
      Ext += item->GetValue();
      Ext += "@";
    }
    item = Next(item);
  }

  return stripspace(&Ext);
}

char *cImageList::GetHideExtensions(void)
{
  HideExt = NULL;
  cImageListItem *item = First();

  while(item)
  {
    if(item->GetFType() == tFile && item->IsHide())
    {
      HideExt += item->GetValue();
      HideExt += "@";
    }
    item = Next(item);
  }

  return stripspace(&HideExt);
}

char *cImageList::GetDirContains(void)
{
  DirIn = NULL;
  cImageListItem *item = First();

  while(item)
  {
    if(item->GetFType() == tDir)
    {
      DirIn += item->GetValue();
      DirIn += "@";
    }
    item = Next(item);
  }

  return stripspace(&DirIn);
}

char *cImageList::GetShortName(const char *file)
{
  Short = NULL;
  cImageListItem *item = First();
  cFileInfo *info = new cFileInfo(file);

  switch(info->Type())
  {
    case tDir:
      while(item)
      {
        if(item->GetFType() == tDir &&
           cFileList::DirIsIn(file, item->GetValue()))
          break;
        item = Next(item);
      }
      if(item)
        Short = item->GetSName();
      break;
    case tFile:
      while(item)
      {
        if(item->GetFType() == tFile &&
           !strcasecmp(item->GetValue(), info->Extension()))
          break;
        item = Next(item);
      }
      if(item)
        Short = item->GetSName();
      break;
    default:
      break;
  }

  return &Short;
}

bool cImageList::IsHide(const char *ext)
{
  if(!ext)
    return false;

  cImageListItem *item = First();
  while(item)
  {
    if(item->GetFType() == tFile &&
       item->IsHide() &&
       !strcasecmp(ext, item->GetValue()))
      return true;
    item = Next(item);
  }
  return false;
}
