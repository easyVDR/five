#include <vdr/tools.h>
#include "dvdlist-item.h"
#include "helpers.h"

class cDVDList : public cList<cDVDListItem>
{
  private:
    char *DVDExts;
    char *DVDDirs;

    bool Load(const char *dir, eFileList smode, bool sub);
  public:
    cDVDList(void)
    {
      DVDExts = NULL;
      DVDDirs = NULL;
    }
    ~cDVDList(void)
    {
      free(DVDExts);
      free(DVDDirs);
    }
    bool Create(const char *dir, const char *exts, const char *dirs, eFileList smode = sNone, bool sub = false);
    
};

