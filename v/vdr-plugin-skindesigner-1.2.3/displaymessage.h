#ifndef __DISPLAYMESSAGE_H
#define __DISPLAYMESSAGE_H

#include "config.h"
#include "coreengine/viewdisplaymessage.h"

class cSDDisplayMessage : public cSkinDisplayMessage {
private:
    cViewMessage *view;
    bool ok;
public:
    cSDDisplayMessage(cViewMessage *messageView);
    virtual ~cSDDisplayMessage();
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};

#endif //__DISPLAYMESSAGE_H
