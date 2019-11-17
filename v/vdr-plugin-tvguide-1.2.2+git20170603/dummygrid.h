#ifndef __TVGUIDE_DUMMYGRID_H
#define __TVGUIDE_DUMMYGRID_H

#include "grid.h"
#include "channelcolumn.h"

// --- cDummyGrid  -------------------------------------------------------------

class cDummyGrid : public cGrid {
private:
    time_t start;
    time_t end;
    cString strText;
    void drawText();
    time_t Duration(void);
public:
    cDummyGrid(cChannelColumn *c, time_t start, time_t end);
    virtual ~cDummyGrid(void);
    void SetViewportHeight();
    void PositionPixmap();
    void setText(void);
    const cEvent *GetEvent() {return NULL;};
    time_t StartTime() { return start; };
    time_t EndTime() { return end; };
    void SetStartTime(time_t start) { this->start = start; };
    void SetEndTime(time_t end) { this->end = end; };
    int calcOverlap(cGrid *neighbor);
    void setTimer() {};
    cString getText(void);
    cString getTimeString(void);
    void debug();
};

#endif //__TVGUIDE_DUMMYGRID_H