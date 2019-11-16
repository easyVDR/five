/**
 * based on setup.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_SETUP_H
#define VDR_BLOCK_SETUP_H

#include <vdr/menuitems.h>

#include "event.h"
#include "config.h"

class cMenuSetupBlock : public cMenuSetupPage {
private:
  cEventsBlock mEventsData;  
  cSetupBlock  mSetupData;
  const char *DetectionMethods[2];
  const char *FuzzyFallsback[2];

protected:
  virtual void Store(void);
  virtual eOSState ProcessKey(eKeys Key);

  void Set(void);
  void SetHelpKeys(void);

  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState SetListProperty(void);

public:
  cMenuSetupBlock();
};

class cMenuSetupEditBlock : public cOsdMenu {
private:
  cEventBlock *mEvent;
  cEventBlock  mData;

protected:
  eOSState ProcessKey(eKeys Key);

public:
  cMenuSetupEditBlock(cEventBlock *Event);
};

#endif // VDR_BLOCK_SETUP_H
