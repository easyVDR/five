#ifndef __TVGUIDE_CHANNELGROUPS_H
#define __TVGUIDE_CHANNELGROUPS_H

#include <vector>
#include <vdr/channels.h>
#include "channelgroup.h"

// --- cChannelGroups  -------------------------------------------------------------

class cChannelGroups {
private:
    std::vector<cChannelGroup> channelGroups;
    cList<cChannelGroupGrid> groupGrids;
public:
    cChannelGroups(void);
    virtual ~cChannelGroups(void);
    void ReadChannelGroups(void);
    const char* GetPrev(int group);
    const char* GetNext(int group);
    int GetGroup(const cChannel *channel);
    int GetPrevGroupChannelNumber(const cChannel *channel);
    int GetNextGroupChannelNumber(const cChannel *channel);
    bool IsInLastGroup(const cChannel *channel);
    void DrawChannelGroups(const cChannel *start, const cChannel *stop);
    void CreateGroupGrid(const char *name, int number, int start, int end);
    int GetLastValidChannel(void);
    void DumpGroups(void);
};

#endif //__TVGUIDE_CHANNELGROUPS_H
