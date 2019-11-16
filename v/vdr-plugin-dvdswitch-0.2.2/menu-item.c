
#include "imagelist.h"
#include "menu.h"
#include "menu-item.h"
#include "setup.h"

// --- cMainMenuItem -----------------------------------------------------

cMainMenuItem::cMainMenuItem(eMainMenuItem itype, cImageList &ImageList, const char *file)
{
  iType = itype;
  File = file ? strdup(file) : NULL;

  OSD = cMainMenu::CreateOSDName(iType, ImageList, File);

  SetText(OSD, true);
  if(iType == iCat)
    SetSelectable(false);
}

cMainMenuItem::~ cMainMenuItem()
{
  free(File);
  free(OSD);
}

