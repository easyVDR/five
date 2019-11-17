#include "displayvolume.h"

#include "config.h"

cSDDisplayVolume::cSDDisplayVolume(cViewVolume *volumeView) {
    view = volumeView;
    ok = view->Init();
    if (!ok)
        esyslog("skindesigner: Error initiating displayvolume view - aborting");
}

cSDDisplayVolume::~cSDDisplayVolume() {
    view->Close();
}

void cSDDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    if (!ok)
        return;
    view->SetVolume(Current, Total, Mute);
}

void cSDDisplayVolume::Flush(void) {
    if (!ok)
        return;
    view->Flush();
}
