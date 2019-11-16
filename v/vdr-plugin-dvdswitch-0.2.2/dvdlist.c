#include "dvdlist.h"

// --- cDVDList ------------------------------------------------------------------

bool cDVDList::Create(const char *dir, const char *exts, const char *dirs, eFileList smode, bool sub)
{
  Clear();
  FREENULL(DVDExts);
  FREENULL(DVDDirs);

  DVDExts = exts ? strdup(exts) : NULL;
  DVDDirs = dirs ? strdup(dirs) : NULL;

  if(!DVDExts && !DVDDirs)
    return false;

  return Load(dir, smode, sub);
}

bool cDVDList::Load(const char *dir, eFileList smode, bool sub)
{
  bool ret = false;
  int i = 0;

  cFileInfo *fInfo = NULL;
  cFileList *fList = new cFileList();

  fList->OptExclude("^lost\\+found$"); // lost+found Dir
  fList->OptExclude("^\\."); // hidden Files
  fList->OptExclude("\\.sdel$"); // del Files
  fList->OptExclude("\\.smove$"); // move Files

  if(DVDExts)
  {
    cTokenizer *token = new cTokenizer(DVDExts, "@");
    char *extexp = NULL;
    for(i = 1; i <= token->Count(); i++)
    {
      if(0 < asprintf(&extexp, "%s$", token->GetToken(i))) {
        fList->OptInclude(extexp, tFile);
        FREENULL(extexp);
      }
    }
    delete(token);
  }

  if(DVDDirs)
    fList->OptInclude(".*", tDir);

  fList->OptSort(smode, true);

  ret = fList->Load(dir, sub);

  cFileListItem *fItem = fList->First();
  while(fItem)
  {
    fInfo = new cFileInfo(fItem->Value());
    if((fInfo->Type() == tFile) ||
       (fInfo->Type() == tDir && fList->DirIsIn(fItem, DVDDirs)))
      Add(new cDVDListItem(fItem->Value()));
    DELETENULL(fInfo);
    fItem = fList->Next(fItem);
  }

  delete(fList);
  delete(fInfo);

  return ret;
}
