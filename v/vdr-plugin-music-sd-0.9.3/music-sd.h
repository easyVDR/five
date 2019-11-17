#ifndef __MUSICHD_H
#define __MUSICHD_H

#include <string.h>

#include <vdr/plugin.h>

#include "mp3control.h"
//#include "version.h"
//#include "i18n.h"
#include "service.h"
#include "config.h"
#include "common.h"
#include "setup-mp3.h"
#include "setup.h"
#include "menubrowse.h"


class cMenuSetupMP3 : public cMenuSetupPage {
private:
  //
  const char *userlevel[3];
  const char *cddb[3], *scan[3], *coversize[4];
  const char *aout[AUDIOOUTMODES];
  int amode, amodes[AUDIOOUTMODES];
protected:
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Store(void);
public:
  cMenuSetupMP3(void);
};


#endif // __MUSICHD_H
