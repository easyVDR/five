#ifndef __TVGUIDE_TVGUIDEOSD_H
#define __TVGUIDE_TVGUIDEOSD_H

#include "timer.h"
#include "grid.h"
#include "channelcolumn.h"
#include "statusheader.h"
#include "detailview.h"
#include "timeline.h"
#include "channelgroups.h"
#include "footer.h"
#include "recmenumanager.h"
#include "channeljump.h"

// --- cTvGuideOsd -------------------------------------------------------------

class cTvGuideOsd : public cOsdObject {
private:
  cMyTime *myTime;
  cList<cChannelColumn> columns;
  cGrid *activeGrid;
  cStatusHeader *statusHeader;
  cDetailView *detailView;
  cTimeLine *timeLine;
  cChannelGroups *channelGroups;
  cFooter *footer;
  cRecMenuManager *recMenuManager;
  cChannelJump *channelJumper;
  bool detailViewActive;
  void drawOsd();
  void readChannels(const cChannel *channelStart);
  void drawGridsChannelJump(int offset = 0);
  void drawGridsTimeJump();
  void processKeyUp();
  void processKeyDown();
  void processKeyLeft();
  void processKeyRight();
  void processKeyRed();
  void processKeyGreen();
  void processKeyYellow();
  eOSState processKeyBlue(bool *alreadyUnlocked);
  eOSState processKeyOk(bool *alreadyUnlocked);
  void processNumKey(int numKey);
  void TimeJump(int mode);
  void ChannelJump(int num);
  void CheckTimeout(void);
  void setNextActiveGrid(cGrid *next);
  void channelForward();
  void channelBack();
  void timeForward();
  void timeBack();
  void ScrollForward();
  void ScrollBack();
  eOSState ChannelSwitch(bool *alreadyUnlocked);
  void DetailedEPG();
  void SetTimers();
  void dump();
public:
  cTvGuideOsd(void);
  virtual ~cTvGuideOsd(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_TVGUIDEOSD_H
