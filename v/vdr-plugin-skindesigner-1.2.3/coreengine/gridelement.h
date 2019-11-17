#ifndef __GRIDELEMENT_H
#define __GRIDELEMENT_H

#include "viewelement.h"

class cGridElement : public cViewElement {
private:
    int viewId;
    int plugId;
    bool current;
    int indexCurrent;
public:
    cGridElement(void);
    cGridElement(const cGridElement &other);
    virtual ~cGridElement(void);
    void SetPluginId(int plugId) { this->plugId = plugId; };
    void SetViewId(int viewId) { this->viewId = viewId; };
    void SetTokenContainer(void);
    skindesignerapi::cTokenContainer *GetTokenContainer(void) { return tokenContainer; };
    void Set(skindesignerapi::cTokenContainer *tk);
    void SetCurrent(bool current);
    bool Parse(bool forced = true);
    int Width(void);
    int Height(void);
};

#endif //__GRIDELEMENT_H