#ifndef __VIEWELEMENTPLUGIN_H
#define __VIEWELEMENTPLUGIN_H

#include "viewelement.h"

class cVePlugin : public cViewElement {
private:
    int plugId;
    int viewId;
public:
    cVePlugin(void);
    virtual ~cVePlugin(void);
    void SetPluginId(int plugId) { this->plugId = plugId; };
    void SetViewId(int viewId) { this->viewId = viewId; };
    void Close(void);
    void SetTokenContainer(void);
    void Set(skindesignerapi::cTokenContainer *tk);
    bool Parse(bool forced = false);
    const char *Name(void) { return attribs->Name(); };
};

#endif //__VIEWELEMENTPLUGIN_H
