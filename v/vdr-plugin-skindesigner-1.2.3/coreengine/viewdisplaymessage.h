#ifndef __VIEWDISPLAYMESSAGE_H
#define __VIEWDISPLAYMESSAGE_H

#include "view.h"

class cViewMessage : public cView {
private:
    cVeMessage *veMessage;
    void SetViewElements(void);
    void SetViewElementObjects(void);
    void ClearVariables(void);
public:
    cViewMessage(void);
    virtual ~cViewMessage(void);
    void SetMessage(eMessageType type, const char *text);
    void Flush(void);
};

#endif //__VIEWDISPLAYMESSAGE_H