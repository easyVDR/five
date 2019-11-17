#ifndef __TVGUIDE_TIMELINE_H
#define __TVGUIDE_TIMELINE_H

#include "timer.h"
#include "styledpixmap.h"

// --- cTimeLine  -------------------------------------------------------------

class cTimeLine  {
private:
    cMyTime *myTime;
    cStyledPixmap *dateViewer;
    cPixmap *timeline;
    cStyledPixmap *clock;
    cPixmap *timeBase;
    void decorateTile(int posX, int posY, int tileWidth, int tileHeight);
    void drawRoundedCorners(int posX, int posY, int width, int height, int radius);
    cImage *createBackgroundImage(int width, int height, tColor clrBgr, tColor clrBlend);
    void drawCurrentTimeBase(void);
public:
    cTimeLine(cMyTime *myTime);
    virtual ~cTimeLine(void);
    void setTimeline();
    void drawDateViewer();
    void drawTimeline();
    void drawClock();
};

#endif //__TVGUIDE_TIMELINE_H