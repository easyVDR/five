#include "viewelementplugin.h"
#include "../config.h"

cVePlugin::cVePlugin(void) {
    plugId = -1;
    viewId = -1;
}

cVePlugin::~cVePlugin(void) {
}

void cVePlugin::Close(void) {
    cViewElement::Close();
}

void cVePlugin::SetTokenContainer(void) {
    skindesignerapi::cTokenContainer *tkVe = plgManager->GetTokenContainerVE(plugId, viewId, id);
    if (!tkVe)
        return;
    tokenContainer = new skindesignerapi::cTokenContainer(*tkVe);
    InheritTokenContainer();
}

void cVePlugin::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
    SetDirty();
}

bool cVePlugin::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;
    return true;
}
