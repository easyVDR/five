#ifndef __VIEWDISPLAYCHANNEL_H
#define __VIEWDISPLAYCHANNEL_H

#include "view.h"
#include "../extensions/globaltimers.h"

enum eDisplayMode {
    dmDefault,
    dmChannelGroups
};

class cViewChannel : public cView {
private:
    cVeMessage                  *veMessage;
    cVeCustomTokens             *veCustomTokens;
    cVeDcChannelInfo            *veChannelInfo;
    cVeDcChannelGroup           *veChannelGroup;
    cVeDcEpgInfo                *veEpgInfo;
    cVeDcProgressBar            *veProgressBar;
    cVeDcStatusInfo             *veStatusInfo;
    cVeDcScraperContent         *veScraperContent;
    cVeDcEcmInfo                *veEcmInfo;
#ifdef USE_ZAPCOCKPIT
    cVeDcChannelHints           *veChannelHints;
    cVeDcChannelDetail          *veChannelDetail;
    cVeDcChannelListDetail      *veChannelListDetail;
    cVeDcGroupChannelListDetail *veGroupChannelListDetail;
    cViewListChannelList    *channelList;
    cViewListGroupList      *groupList;
    cViewListChannelList    *groupChannelList;
    eDisplaychannelView viewType;
    eDisplaychannelView viewTypeLast;
    bool initExtended;
    bool displayList;
    bool initList;
    bool channelHints;
    bool channelInput;
#endif
    bool channelChange;
    eDisplayMode mode;
    bool timersLoaded;
    cGlobalTimers globalTimers;
    void SetViewElements(void);
    void ClearVariables(void);
    void SetViewElementObjects(void);
    void ClearBasic(bool clearBackground);
    void ClearExtended(void);
    void ClearOnDisplay(void);
    void DrawBasic(bool initial);
    void HideBasic(void);
    void ShowBasic(void);
    void DrawExtended(void);
public:
    cViewChannel(void);
    virtual ~cViewChannel(void);
    void SetGlobals(cGlobals *globals);
    void PreCache(void);
    void AddChannelViewList(const char *listName, cViewList *viewList);
    void SetChannel(const cChannel *channel, int number);
    void SetEvents(const cEvent *present, const cEvent *following);
    void SetMessage(eMessageType type, const char *text);
#ifdef USE_ZAPCOCKPIT
    void SetViewType(eDisplaychannelView viewType);
    int MaxItems(void);
    bool KeyRightOpensChannellist(void);
    void SetChannelInfo(const cChannel *channel);
    void SetChannelList(const cChannel *channel, int index, bool current);
    void SetGroupList(const char *group, int numChannels, int index, bool current);
    void ClearList(void);
    void SetNumChannelHints(int num);
    void SetChannelHint(const cChannel *channel);
#endif
    void Close(void);
    void Flush(void);
};

#endif //__VIEWDISPLAYCHANNEL_H