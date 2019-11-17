#include "gridelement.h"
#include "../config.h"

cGridElement::cGridElement(void) {
    current = false;
    indexCurrent = -1;
    viewId = -1;
    plugId = -1;
}

cGridElement::cGridElement(const cGridElement &other) : cViewElement(other) {
    current = false;
    viewId = other.viewId;
    plugId = other.plugId;
    tokenContainer = new skindesignerapi::cTokenContainer(*other.tokenContainer);
    indexCurrent = other.indexCurrent;
    InheritTokenContainerDeep();
}

cGridElement::~cGridElement(void) {
}

void cGridElement::SetTokenContainer(void) {
    skindesignerapi::cTokenContainer *tkGe = plgManager->GetTokenContainerGE(plugId, viewId, id);
    if (!tkGe)
        return;
    tokenContainer = new skindesignerapi::cTokenContainer(*tkGe);
    indexCurrent = tokenContainer->GetNumDefinedIntTokens();
    tokenContainer->DefineIntToken("{current}", indexCurrent);
    InheritTokenContainer();
}

void cGridElement::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
    SetDirty();
}

void cGridElement::SetCurrent(bool current) {
    this->current = current;
    SetDirty();
}

bool cGridElement::Parse(bool forced) {
    if (!dirty)
        return false;
    tokenContainer->AddIntToken(indexCurrent, current);
    return true;
}

int cGridElement::Width(void) {
    return container.Width();
}

int cGridElement::Height(void) {
    return container.Height();
}