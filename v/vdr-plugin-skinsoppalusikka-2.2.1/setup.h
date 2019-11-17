/*
 * setup.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINSOPPALUSIKKA_SETUP_H
#define __SKINSOPPALUSIKKA_SETUP_H

#include <vdr/menuitems.h>

class cSkinSoppalusikkaSetup : public cMenuSetupPage {
private:
  int showAuxInfoM;
  int showLogoM;
  int showVpsM;
  int showDurationM;
  int showSymbolsM;
  int showProgressBarM;
  int cacheSizeM;
  cVector<const char*> helpM;
  void Setup(void);

protected:
  virtual eOSState ProcessKey(eKeys keyP);
  virtual void Store(void);

public:
  cSkinSoppalusikkaSetup();
};

#endif // __SKINSOPPALUSIKKA_SETUP_H
