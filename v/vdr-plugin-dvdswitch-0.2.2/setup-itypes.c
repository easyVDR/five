#include <vdr/plugin.h>
#include "setup-itypes.h"
#include "setup.h"

cMenuSetupDSITypes::cMenuSetupDSITypes(cImageList &imagelist, bool select, int* retindex, char *buffer)
  : cOsdMenu(tr("Type of images"))
  , ImageList(imagelist)
{
  cSetupLine *item = Setup.First();
  Select = select;
  RetIndex = retindex;
  Buffer = buffer;
 
  while(item)
  {
    if(item->Plugin() &&
       !strcasecmp(item->Plugin(), "dvdswitch") &&
       !strcasecmp(item->Name(), "ImageTypes"))
      break;
    item = Setup.Next(item);
  }
  
  SetupLine = item;

  Set();
}


void cMenuSetupDSITypes::Set(void)
{
  int current = Current();

  Clear();

  cImageListItem *item = ImageList.First();
  
  while(item)
  {
    Add(new cOsdItem(item->GetLName()));
    item = ImageList.Next(item);
  }

  SetCurrent(Get(current));

  Display();
  SetHelp();
}

void cMenuSetupDSITypes::SetHelp(void)
{
  cOsdItem *item = Get(Current());
  if(item)
  {
    if(Select)
      cOsdMenu::SetHelp(NULL, tr("New"), NULL , tr("Select"));
    else
      cOsdMenu::SetHelp(tr("Edit"), tr("New"), tr("Delete") , NULL);
  }
  else
    cOsdMenu::SetHelp(NULL, tr("New"), NULL , NULL);
}

eOSState cMenuSetupDSITypes::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  cImageListItem *item = NULL;

  if (!HasSubMenu())
  {
    switch(Key)
    {
      case kRed:
        state = AddSubMenu(new cMenuSetupDSITypesItem(ImageList, Current()));
        break;
      case kGreen:
        state = AddSubMenu(new cMenuSetupDSITypesItem(ImageList, ImageList.Count()));
        break;
      case kYellow:
        if(Interface->Confirm(tr("Really delete entry?")))
        {
          item = ImageList.Get(Current());
          ImageList.Del(item);
          Set();
          Display();
        }
        break;
      case kBlue:
        item = ImageList.Get(Current());
        if(Select && item)
        {
          strn0cpy(Buffer, item->GetLName(), memberof(Buffer));
          *RetIndex = item->Index();
          return osBack;
        }
        break;
      case kBack:
      case kOk:
        item = ImageList.Get(Current());
        if(Select && item)
        {
          strn0cpy(Buffer, item->GetLName(), memberof(Buffer));
          *RetIndex = item->Index();
          return osBack;
        }
        if(state == osContinue)
        {
          Set();
          Display();
        }
        else
        {
          if(SetupLine)
          {
            Setup.Del(SetupLine);
          }
          if(ImageList.GetSetupString())
            SetupLine = new cSetupLine("ImageTypes", ImageList.GetSetupString(), "dvdswitch");
          else
            SetupLine = new cSetupLine("ImageTypes", "", "dvdswitch");
          Setup.Add(SetupLine);
          return osBack;
        }
        break;
      default:
        break;
    }
  }

  return state;
}

// --- cMenuSetupDSITypesItem -----------------------------------

cMenuSetupDSITypesItem::cMenuSetupDSITypesItem(cImageList &imagelist, int itemindex)
  : cOsdMenu(tr("New"), 22)
  , ImageList(imagelist)
{
  if(itemindex < ImageList.Count())
  {
    SetTitle(tr("Edit"));
    Item = ImageList.Get(itemindex);
    strn0cpy(LongName, Item->GetLName(), memberof(LongName));
    strn0cpy(ShortName, Item->GetSName(), memberof(ShortName));
    FileType = (int)Item->GetFType() - 1;
    strn0cpy(Extension, Item->GetValue(), memberof(Extension));
    HideExtension = Item->IsHide();
  } else
  {
    Item = NULL;
    strcpy(LongName, "\0");
    strcpy(ShortName, "\0");
    FileType = 1;
    strcpy(Extension, "\0");
    HideExtension = 1;
  }

  FileTypes[0] = tr("Directory");
  FileTypes[1] = tr("File");

  Set();
}

void cMenuSetupDSITypesItem::Set(void)
{
  int current = Current();

  Clear();

  Add(new cMenuEditStrItem(tr("Description"),
      LongName,
      memberof(LongName)));
  Add(new cMenuEditStrItem(tr("Type title"),
      ShortName,
      memberof(ShortName)));
  Add(new cMenuEditStraItem(tr("Image type"), &FileType, memberof(FileTypes), FileTypes));
  Add(new cMenuEditStrItem(!FileType ? tr("Directory contains") : tr("File extension"),
      Extension,
      memberof(Extension)));
  if(FileType)
    Add(new cMenuEditBoolItem(tr("Hide extension"), &HideExtension));

  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupDSITypesItem::ProcessKey(eKeys Key)
{
  int tmpFileType = FileType;
  
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(tmpFileType != FileType)
  {
    Set();
    Display();
  }

  if(state == osUnknown)
  {
    switch(Key)
    {
      case kOk:
        if(isempty(LongName))
        {
          OsdMsg(mtWarning,tr("'Description' must not empty!"));
          return osContinue;
        }
        if(isempty(ShortName))
        {
          OsdMsg(mtWarning,tr("'Type title' must not empty!"));
          return osContinue;
        }
        if(isempty(Extension))
        {
          if(!FileType)
            OsdMsg(mtWarning,tr("'Directory contains' must not empty!"));
          else
            OsdMsg(mtWarning,tr("'File extension' must not empty!"));
          return osContinue;
        }
        if(!Item)
          ImageList.Add(new cImageListItem(LongName, ShortName, (eFileInfo)(FileType + 1), Extension, HideExtension));
        else
          Item->Edit(LongName, ShortName, (eFileInfo)(FileType + 1), Extension, HideExtension);
        state = osBack;
        break;
      default:
        break;
    }
  }

  return state;
}
