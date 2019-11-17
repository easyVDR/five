#include "displaychannel.h"

cSDDisplayChannel::cSDDisplayChannel(cViewChannel *channelView, bool WithInfo) {
    view = channelView;
    ok = view->Init();
    if (!ok)
        esyslog("skindesigner: Error initiating displaychannel view - aborting");
}

cSDDisplayChannel::~cSDDisplayChannel() {
    view->Close();
}

void cSDDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    if (!ok)
        return;
    view->SetChannel(Channel, Number);
}

void cSDDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
    if (!ok)
        return;
    view->SetEvents(Present, Following);
}

void cSDDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
    if (!ok)
        return;
    view->SetMessage(Type, Text);
}

#ifdef USE_ZAPCOCKPIT

void cSDDisplayChannel::SetViewType(eDisplaychannelView ViewType) {
    if (!ok)
        return;
    view->SetViewType(ViewType);
}

int cSDDisplayChannel::MaxItems(void) {
    if (!ok)
        return 0;
    return view->MaxItems();
}

bool cSDDisplayChannel::KeyRightOpensChannellist(void) {
    if (!ok)
        return true;
    return view->KeyRightOpensChannellist();    
}

void cSDDisplayChannel::SetChannelInfo(const cChannel *Channel) {
    if (!ok)
        return;
    view->SetChannelInfo(Channel);
}

void cSDDisplayChannel::SetChannelList(const cChannel *Channel, int Index, bool Current) {
    if (!ok)
        return;
    view->SetChannelList(Channel, Index, Current);
}

void cSDDisplayChannel::SetGroupList(const char *Group, int NumChannels, int Index, bool Current) {
    if (!ok)
        return;
    view->SetGroupList(Group, NumChannels, Index, Current);
}

void cSDDisplayChannel::SetGroupChannelList(const cChannel *Channel, int Index, bool Current) {
    if (!ok)
        return;
}

void cSDDisplayChannel::ClearList(void) {
    if (!ok)
        return;
    view->ClearList();
}

void cSDDisplayChannel::SetNumChannelHints(int Num) {
    if (!ok)
        return;
    view->SetNumChannelHints(Num);
}

void cSDDisplayChannel::SetChannelHint(const cChannel *Channel) {
    if (!ok)
        return;
    view->SetChannelHint(Channel);
}

#endif //USE_ZAPCOCKPIT

void cSDDisplayChannel::Flush(void) {
    if (!ok)
        return;
    view->Flush();
}
