#ifndef __TVGUIDE_CHANNELJUMP_H
#define __TVGUIDE_CHANNELJUMP_H

#include "styledpixmap.h"

// --- cChannelJump  -------------------------------------------------------------

class cChannelJump : public cStyledPixmap {
private:
    int channel;
    cChannelGroups *channelGroups;
    int maxChannels;
    int startTime;
    int timeout;
    cPixmap *pixmapBack;
    cPixmap *pixmapText;
    void SetPixmaps(void);
    void Draw(void);
    cString BuildChannelString(void);
public:
    cChannelJump(cChannelGroups *channelGroups);
    virtual ~cChannelJump(void);
    void Set(int num);
    void DrawText(void);
    bool TimeOut(void);
    int GetChannel(void) { return channel; };
};

#endif //__TVGUIDE_CHANNELJUMP_H