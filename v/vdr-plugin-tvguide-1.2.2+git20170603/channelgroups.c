#include <vector>
#include "channelgroups.h"

cChannelGroups::cChannelGroups(void) {
}

cChannelGroups::~cChannelGroups(void) {
}

void cChannelGroups::ReadChannelGroups(void) {
    bool setStart = false;
    int lastChannelNumber = 0;
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cChannel *first = Channels->First();
#else
    const cChannel *first = Channels.First();
#endif
    if (!first->GroupSep()) {
        channelGroups.push_back(cChannelGroup(tr("Main Program")));
        setStart = true;
    }    
#if VDRVERSNUM >= 20301
    for (const cChannel *channel = Channels->First(); channel; channel = Channels->Next(channel)) {
#else
    for (const cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
#endif
        if (setStart && (channelGroups.size() > 0)) {
            channelGroups[channelGroups.size()-1].SetChannelStart(channel->Number());
            setStart = false;
        }
        if (channel->GroupSep()) {
            if (channelGroups.size() > 0) {
                channelGroups[channelGroups.size()-1].SetChannelStop(lastChannelNumber);
            }
            channelGroups.push_back(cChannelGroup(channel->Name()));
            setStart = true;
        } else {
            lastChannelNumber = channel->Number();
        }
    }
    if (channelGroups.size() > 0) {
        channelGroups[channelGroups.size()-1].SetChannelStop(lastChannelNumber);
        if ((tvguideConfig.hideLastGroup)&&(channelGroups.size() > 1)) {
            channelGroups.pop_back();
        }
    }
}

int cChannelGroups::GetGroup(const cChannel *channel) {
    int channelNumber = channel->Number();
    int numGroups = channelGroups.size();
    if (numGroups) {
        for (int i=0; i<numGroups; i++) {
            if ((channelGroups[i].StartChannel() <= channelNumber) && ((channelGroups[i].StopChannel() >= channelNumber))) {
                return i;
            }
        }
    }
    return -1;
}

const char* cChannelGroups::GetPrev(int group) {
    if (group > 0) {
        return channelGroups[group-1].GetName();
    }
    return "";
}

const char* cChannelGroups::GetNext(int group) {
    if (group > -1) {
        if ((group+1) < channelGroups.size())
            return channelGroups[group+1].GetName();
    }
    return "";
}

int cChannelGroups::GetPrevGroupChannelNumber(const cChannel *channel) {
    int currentGroup = GetGroup(channel);
    if (currentGroup == -1)
        return 0;
    if (currentGroup > 0) {
        return channelGroups[currentGroup-1].StartChannel();
    }
    return 0;
}

int cChannelGroups::GetNextGroupChannelNumber(const cChannel *channel) {
    int currentGroup = GetGroup(channel);
    if (currentGroup == -1)
        return 0;
    if ((currentGroup+1) < channelGroups.size()) {
        return channelGroups[currentGroup+1].StartChannel();
    }
    return 0;
}

bool cChannelGroups::IsInLastGroup(const cChannel *channel) {
    if (!tvguideConfig.hideLastGroup)
        return false;
    if (channelGroups.size() > 0) {
        if (channel->Number() > channelGroups[channelGroups.size()-1].StopChannel()) {
            return true;
        }
    }
    return false;
}

void cChannelGroups::DrawChannelGroups(const cChannel *start, const cChannel *stop) {
    groupGrids.Clear();
    int group = GetGroup(start);
    int groupLast = group;
    int line = 0;
    int lineStart = 0;
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    for (const cChannel *channel = Channels->Next(start); channel; channel = Channels->Next(channel)) {
#else
    for (const cChannel *channel = Channels.Next(start); channel; channel = Channels.Next(channel)) {
#endif
        if (channel->GroupSep())
            continue;
        group = GetGroup(channel);
        if (group != groupLast) {
            CreateGroupGrid(channelGroups[groupLast].GetName(), group, lineStart, line);
            lineStart = line + 1;
        }
        line++;
        groupLast = group;
        if (channel == stop) {
            CreateGroupGrid(channelGroups[groupLast].GetName(), group+1, lineStart, line);
            break;
        }
    }
    
}

void cChannelGroups::CreateGroupGrid(const char *name, int number, int start, int end) {
    cChannelGroupGrid *groupGrid = new cChannelGroupGrid(name);
    groupGrid->SetColor(number%2);
    groupGrid->SetBackground();
    groupGrid->SetGeometry(start, end);
    groupGrid->Draw();
    groupGrids.Add(groupGrid);
}

int cChannelGroups::GetLastValidChannel(void) {
    if (channelGroups.size() > 0)
        return channelGroups[channelGroups.size()-1].StopChannel();
    return 0;
}

void cChannelGroups::DumpGroups(void) {
    for (std::vector<cChannelGroup>::iterator group = channelGroups.begin(); group!=channelGroups.end(); ++group) {
        group->Dump();
    }
}
