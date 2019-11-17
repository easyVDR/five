#include "displaytracks.h"

cSDDisplayTracks::cSDDisplayTracks(cViewTracks *tracksView, const char *Title, int NumTracks, const char * const *Tracks) {
    view = tracksView;
    ok = view->Init();
    if (!ok)
        esyslog("skindesigner: Error initiating displaytracks view - aborting");
    view->SetTitle(Title);
    view->SetNumtracks(NumTracks);
    view->SetTracks(Tracks);
}

cSDDisplayTracks::~cSDDisplayTracks() {
    view->Close();
}

void cSDDisplayTracks::SetTrack(int Index, const char * const *Tracks) {
    if (ok)
        view->SetCurrentTrack(Index);
}

void cSDDisplayTracks::SetAudioChannel(int AudioChannel) {
    if (ok)
        view->SetAudiochannel(AudioChannel);
}

void cSDDisplayTracks::Flush(void) {
    if (!ok)
        return;
    view->Flush();
}
