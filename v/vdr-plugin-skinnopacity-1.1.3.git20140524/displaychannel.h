#ifndef __NOPACITY_DISPLAYCHANNEL_H
#define __NOPACITY_DISPLAYCHANNEL_H

#include <vdr/thread.h>
#include "displaychannelview.h"

class cNopacityDisplayChannel : public cSkinDisplayChannel, cThread {
private:
    cNopacityDisplayChannelView *channelView;
    bool doOutput;
    int FrameTime;
    int FadeTime;
    bool initial;
    bool groupSep;
    bool channelChange;
    time_t lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
    int lastScreenWidth;
    int currentLast;
    bool showSignal;
    const cEvent *present;
    virtual void Action(void);
    void SetProgressBar(const cEvent *present);
public:
    cNopacityDisplayChannel(cImageCache *imgCache, bool WithInfo);
    virtual ~cNopacityDisplayChannel();
    virtual void SetChannel(const cChannel *Channel, int Number);
    virtual void SetEvents(const cEvent *Present, const cEvent *Following);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};
#endif //__NOPACITY_DISPLAYCHANNEL_H