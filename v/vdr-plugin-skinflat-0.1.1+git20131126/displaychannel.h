#pragma once

#include "baserender.h"

class cFlatDisplayChannel : public cFlatBaseRender, public cSkinDisplayChannel {
    private:
        const cEvent *present;

        cString channelName;

        cPixmap *chanInfoTopPixmap;
        cPixmap *chanInfoBottomPixmap;
        cPixmap *chanLogoPixmap;
    
        int heightBottom;
    public:
        cFlatDisplayChannel(bool WithInfo);
        virtual ~cFlatDisplayChannel();
        virtual void SetChannel(const cChannel *Channel, int Number);
        virtual void SetEvents(const cEvent *Present, const cEvent *Following);
        virtual void SetMessage(eMessageType Type, const char *Text);
        virtual void Flush(void);
};
