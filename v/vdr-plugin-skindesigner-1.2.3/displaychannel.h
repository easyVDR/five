#ifndef __DISPLAYCHANNEL_H
#define __DISPLAYCHANNEL_H

#include <vdr/skins.h>
#include "config.h"
#include "coreengine/definitions.h"
#include "coreengine/viewdisplaychannel.h"

#ifdef USE_ZAPCOCKPIT
class cSDDisplayChannel : public cSkinDisplayChannelExtended {
#else
class cSDDisplayChannel : public cSkinDisplayChannel {
#endif
private:
    cViewChannel *view;
    bool ok;
public:
    cSDDisplayChannel(cViewChannel *channelView, bool WithInfo);
    virtual ~cSDDisplayChannel();
    virtual void SetChannel(const cChannel *Channel, int Number);
    virtual void SetEvents(const cEvent *Present, const cEvent *Following);
    virtual void SetMessage(eMessageType Type, const char *Text);
#ifdef USE_ZAPCOCKPIT
    virtual void SetViewType(eDisplaychannelView ViewType);
    virtual int MaxItems(void);
    virtual bool KeyRightOpensChannellist(void);
    virtual void SetChannelInfo(const cChannel *Channel);
    virtual void SetChannelList(const cChannel *Channel, int Index, bool Current);
    virtual void SetGroupList(const char *Group, int NumChannels, int Index, bool Current);
    virtual void SetGroupChannelList(const cChannel *Channel, int Index, bool Current);
    virtual void ClearList(void);
    virtual void SetNumChannelHints(int Num);
    virtual void SetChannelHint(const cChannel *Channel);
#endif
    virtual void Flush(void);
};
#endif //__DISPLAYCHANNEL_H
