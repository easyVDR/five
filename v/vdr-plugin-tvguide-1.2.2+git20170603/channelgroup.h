#ifndef __TVGUIDE_CHANNELGROUP_H
#define __TVGUIDE_CHANNELGROUP_H

#include <vdr/tools.h>
#include "styledpixmap.h"


// --- cChannelGroup  -------------------------------------------------------------

class cChannelGroup {
private:
    int channelStart;
    int channelStop;
    const char *name;
public:
    cChannelGroup(const char *name);
    virtual ~cChannelGroup(void);
    void SetChannelStart(int start) { channelStart = start; };
    int StartChannel(void) { return channelStart; };
    void SetChannelStop(int stop) { channelStop = stop; };
    int StopChannel(void) { return channelStop; };
    const char* GetName(void) { return name; };
    void Dump(void);
};

// --- cChannelGroupGrid  -------------------------------------------------------------

class cChannelGroupGrid : public cListObject, public cStyledPixmap {
private:
    const char *name;
    bool isColor1;
    void DrawHorizontal(tColor colorText, tColor colorTextBack);
    void DrawVertical(tColor colorText, tColor colorTextBack);
public:
    cChannelGroupGrid(const char *name);
    virtual ~cChannelGroupGrid(void);
    void SetColor(bool color) {isColor1 = color;};
    void SetBackground(void);
    void SetGeometry(int start, int end);
    void Draw(void);
};


#endif //__TVGUIDE_CHANNELGROUP_H