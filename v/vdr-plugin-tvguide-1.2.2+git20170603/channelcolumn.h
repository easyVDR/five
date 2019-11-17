#ifndef __TVGUIDE_CHANNELCOLUMN_H
#define __TVGUIDE_CHANNELCOLUMN_H

#include <vdr/tools.h>
#include "grid.h"
#include "epggrid.h"
#include "headergrid.h"
#include "switchtimer.h"

class cGrid;
class cEpgGrid;
class cHeaderGrid;

// --- cChannelColumn  -------------------------------------------------------------

class cChannelColumn : public cListObject, public cStyledPixmap {
private:
    cMyTime *myTime;
    int num;
    const cChannel *channel;
    cHeaderGrid *header;
    cList<cGrid> grids;
#if VDRVERSNUM >= 20301
#else
    cSchedulesLock *schedulesLock;
#endif
    const cSchedules *schedules;
    bool hasTimer;
    bool hasSwitchTimer;
    cGrid *addEpgGrid(const cEvent *event, cGrid *firstGrid, bool color);
    cGrid *addDummyGrid(time_t start, time_t end, cGrid *firstGrid, bool color);
public:
    cChannelColumn(int num, const cChannel *channel, cMyTime *myTime);
    virtual ~cChannelColumn(void);
    void createHeader();
    void drawHeader();
    bool readGrids();
    void drawGrids();
    int getX();
    int getY();
    int Start() { return myTime->GetStart(); };
    int Stop()  { return myTime->GetEnd(); };
    const char* Name() { return channel->Name(); };
    const cChannel * getChannel() {return channel;}
    cGrid * getActive();
    cGrid * getNext(cGrid *activeGrid);
    cGrid * getPrev(cGrid *activeGrid);
    cGrid * getNeighbor(cGrid *activeGrid);
    bool isFirst(cGrid *grid);
    void AddNewGridsAtStart();
    void AddNewGridsAtEnd();
    void ClearOutdatedStart();
    void ClearOutdatedEnd();
    int GetNum() {return num;};
    void SetNum(int num) {this->num = num;};
#if VDRVERSNUM >= 20301
    void setTimer();
#else
    void setTimer() {hasTimer = channel->HasTimer();};
#endif
    bool HasTimer() { return hasTimer; };
    void setSwitchTimer() {hasSwitchTimer = SwitchTimers.ChannelInSwitchList(channel);};
    bool HasSwitchTimer() { return hasSwitchTimer; };
    void SetTimers();
    void clearGrids();
    void dumpGrids();
};

#endif //__TVGUIDE_CHANNELCOLUMN_H
