#ifndef __TVGUIDE_GRID_H
#define __TVGUIDE_GRID_H

#include <vdr/tools.h>
#include "styledpixmap.h"

class cChannelColumn;

// --- cEpgGrid  -------------------------------------------------------------

class cGrid : public cListObject, public cStyledPixmap {
protected:
    cTextWrapper *text;
    int viewportHeight;
    int borderWidth;
    void setBackground();
    bool isColor1;
    bool active;
    bool dirty;
    bool hasTimer;
    bool hasSwitchTimer;
    bool intersects(cGrid *neighbor);
    virtual time_t Duration(void) {};
    virtual void drawText(void) {};
    bool dummy;
public:
    cGrid(cChannelColumn *c);
    virtual ~cGrid(void);
    cChannelColumn *column;
    virtual void SetViewportHeight() {};
    virtual void PositionPixmap() {};
    virtual void setText(void) {};
    void Draw();
    void SetDirty() {dirty = true;};
    void SetActive() {dirty = true; active = true;};
    void SetInActive() {dirty = true; active = false;};
    void SetColor(bool color) {isColor1 = color;};
    bool IsColor1() {return isColor1;};
    bool isFirst(void);
    virtual const cEvent *GetEvent() {};
    bool Match(time_t t);
    virtual time_t StartTime() {};
    virtual time_t EndTime() {};
    virtual void SetStartTime(time_t start) {};
    virtual void SetEndTime(time_t end) {};
    int calcOverlap(cGrid *neighbor);
    virtual void SetTimer() {};
    virtual void SetSwitchTimer() {};
    virtual cString getText(void) { return cString("");};
    virtual cString getTimeString(void) { return cString("");};
    bool Active(void) { return active; };
    bool HasTimer() {return hasTimer;};
    bool HasSwitchTimer() {return hasSwitchTimer;};
    bool isDummy() { return dummy; };
    virtual void debug() {};
};

#endif //__TVGUIDE_GRID_H