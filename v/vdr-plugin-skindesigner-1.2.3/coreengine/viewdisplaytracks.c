#include "viewdisplaytracks.h"
#include "../config.h"

/************************************************************************************
* cViewTracks
************************************************************************************/

cViewTracks::cViewTracks(void) {
    ClearVariables();
    viewId = eViewType::DisplayTracks;
    viewName = strdup("displayaudiotracks");
    numViewElements = (int)eVeDisplayTracks::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    viewList = NULL;
    veBackground = NULL;
    veHeader = NULL;
    SetViewElements();
}

cViewTracks::~cViewTracks() {
}

void cViewTracks::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayTracks::background));
    viewElementNames.insert(pair<string, int>("header", (int)eVeDisplayTracks::header));
}

void cViewTracks::SetViewElementObjects(void) {
    if (viewElements[(int)eVeDisplayTracks::background])
        veBackground = dynamic_cast<cVeDtBackground*>(viewElements[(int)eVeDisplayTracks::background]);
    if (viewElements[(int)eVeDisplayTracks::header])
        veHeader = dynamic_cast<cVeDtHeader*>(viewElements[(int)eVeDisplayTracks::header]);
}

void cViewTracks::ClearVariables(void) {
    init = true;
    change = true;
}

void cViewTracks::Close(void) {
    animator->Stop();
    animator->Finish();
    delete animator;
    animator = NULL;
    for (int i=0; i < numViewElements; i++) {
        if (viewElements[i]) {
            viewElements[i]->Close();
        }
    }
    if (viewList) {
        viewList->Close();
    }
    ClearVariables();
    sdOsd.DeleteOsd();
}

void cViewTracks::AddViewList(cViewList *viewList) {
    this->viewList = dynamic_cast<cViewListAudioTracks*>(viewList);
}

void cViewTracks::PreCache(void) {
    cView::PreCache();
    if (viewList) {
        viewList->SetContainer(0, 0, attribs->Width(), attribs->Height());
        viewList->SetGlobals(globals);
        viewList->PreCache();
    }
}

void cViewTracks::SetTitle(const char *title) {
    if (veHeader)
        veHeader->SetTitle(title);
    change = true;
}

void cViewTracks::SetNumtracks(int numTracks) {
    if (veBackground)
        veBackground->Set(numTracks);
    if (veHeader)
        veHeader->SetNumtracks(numTracks);
    if (viewList)
        viewList->SetNumtracks(numTracks);
}

void cViewTracks::SetAudiochannel(int audioChannel) {
    if (veHeader)
        veHeader->SetAudiochannel(audioChannel);
    change = true;
}

void cViewTracks::SetTracks(const char * const *tracks) {
    if (viewList)
        viewList->SetTracks(tracks);
    change = true;
}

void cViewTracks::SetCurrentTrack(int index) {
    if (viewList)
        viewList->SetCurrentTrack(index);    
    change = true;
}

void cViewTracks::Flush(void) {
    if (init) {
        Render((int)eVeDisplayTracks::background);
        if (viewList) {
            viewList->Draw();
            viewList->StartAnimation(true);
        }
    }
    if (change) {
        Render((int)eVeDisplayTracks::header);
        if (viewList && !init)
            viewList->Draw();
        change = false;
    }
    cView::Flush();
}
