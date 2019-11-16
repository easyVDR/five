#ifndef __MENUITEM_DVDSWITCH_H
#define __MENUITEM_DVDSWITCH_H

#include <vdr/osdbase.h>
#include "dvdlist-item.h"

enum eMainMenuItem
{
  iCat,
  iParent,
  iDir,
  iDVD,
  iDevice
};

class cMainMenuItem : public cOsdItem
{
  private:
    eMainMenuItem iType;
    char *File;
    char *OSD;
  public:
    cMainMenuItem(eMainMenuItem itype, cImageList &ImageList, const char *file = NULL);
    ~cMainMenuItem(void);

    eMainMenuItem Type(void) const { return iType; };
    const char* FileName(void) const { return File; };
};

#endif // __MENUITEM_DVDSWITCH_H
