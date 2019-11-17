#include "viewdisplayvolume.h"

/************************************************************************************
* cViewVolume
************************************************************************************/

cViewVolume::cViewVolume(void) {
    viewId = eViewType::DisplayVolume;
    viewName = strdup("displayvolume");
    numViewElements = (int)eVeDisplayVolume::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    SetViewElements();
    ClearVariables();
    veVolume = NULL;
}

cViewVolume::~cViewVolume() {
}

void cViewVolume::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayVolume::background));
    viewElementNames.insert(pair<string, int>("volume", (int)eVeDisplayVolume::volume));
}

void cViewVolume::SetViewElementObjects(void) {
    if (!viewElements[(int)eVeDisplayVolume::volume])
        return;
    veVolume = dynamic_cast<cVeVolume*>(viewElements[(int)eVeDisplayVolume::volume]);
}

void cViewVolume::ClearVariables(void) {
    init = true;
}

void cViewVolume::SetVolume(int current, int total, bool mute) {
    if (veVolume)
        veVolume->Set(current, total, mute);
}

void cViewVolume::Flush(void) {
    if (init) {
        Render((int)eVeDisplayVolume::background);
    }
    Render((int)eVeDisplayVolume::volume);
    cView::Flush();
}


