#include "viewelementsdisplaytracks.h"

/******************************************************************
* cVeDtBackground
******************************************************************/
cVeDtBackground::cVeDtBackground(void) {
    numTracks = 0;
}

cVeDtBackground::~cVeDtBackground(void) {
}

void cVeDtBackground::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{numtracks}", (int)eDTBackgroundIT::numtracks);
    InheritTokenContainer();
}

void cVeDtBackground::Set(int numTracks) {
    this->numTracks = numTracks;
}

bool cVeDtBackground::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDTBackgroundIT::numtracks, numTracks);
    SetDirty();
    return true;
}

/******************************************************************
* cVeDtHeader
******************************************************************/
cVeDtHeader::cVeDtHeader(void) {
    title = NULL;
    audioChannel = 0;
    numTracks = 0;
    changed = true;
}

cVeDtHeader::~cVeDtHeader(void) {
    free(title);
}

void cVeDtHeader::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{numtracks}", (int)eDTHeaderIT::numtracks);
    tokenContainer->DefineIntToken("{isstereo}", (int)eDTHeaderIT::isstereo);
    tokenContainer->DefineIntToken("{isac3}", (int)eDTHeaderIT::isac3);
    tokenContainer->DefineStringToken("{title}", (int)eDTHeaderST::title);
    InheritTokenContainer();
}

void cVeDtHeader::SetTitle(const char *title) {
    if (!title)
        return;
    free(this->title);
    this->title = strdup(title);
    changed = true;
}

void cVeDtHeader::SetNumtracks(int numTracks) {    
    this->numTracks = numTracks;
    changed = true;
}

void cVeDtHeader::SetAudiochannel(int audioChannel) {
    this->audioChannel = audioChannel;
    changed = true;
}

bool cVeDtHeader::Parse(bool forced) {
    if (!cViewElement::Parse(forced) || !changed)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDTHeaderIT::numtracks, numTracks);
    tokenContainer->AddIntToken((int)eDTHeaderIT::isstereo, (audioChannel < 0) ? false : true);
    tokenContainer->AddIntToken((int)eDTHeaderIT::isac3, (audioChannel < 0) ? true : false);
    tokenContainer->AddStringToken((int)eDTHeaderST::title, title);
    SetDirty();
    return true;
}