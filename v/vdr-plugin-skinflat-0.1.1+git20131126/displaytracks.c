#include "displaytracks.h"

cFlatDisplayTracks::cFlatDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {

    CreateFullOsd();
    TopBarCreate();

    itemHeight = fontHeight + 2;
    currentIndex = -1;
    maxItemWidth = font->Width(Title) + marginItem*2;
    for (int i = 0; i < NumTracks; i++)
        maxItemWidth = max(maxItemWidth, font->Width(Tracks[i]) + marginItem*2);

    int itemsHeight = NumTracks * itemHeight;
    int left = osdWidth - maxItemWidth;
    left /= 2;
    TopBarSetTitle(Title);

    tracksPixmap = osd->CreatePixmap(1, cRect(left, osdHeight - itemsHeight - marginItem, maxItemWidth, itemsHeight));
    tracksPixmap->Fill(clrTransparent);

    for (int i = 0; i < NumTracks; i++)
        SetItem(Tracks[i], i, false);
}

cFlatDisplayTracks::~cFlatDisplayTracks() {
    osd->DestroyPixmap(tracksPixmap);
}

void cFlatDisplayTracks::SetItem(const char *Text, int Index, bool Current) {
    int y = Index * itemHeight;
    tColor ColorFg, ColorBg;
    if (Current) {
        ColorFg = Theme.Color(clrTrackItemCurrentFont);
        ColorBg = Theme.Color(clrTrackItemCurrentBg);
        currentIndex = Index;
    }
    else {
        ColorFg = Theme.Color(clrTrackItemFont);
        ColorBg = Theme.Color(clrTrackItemBg);
    }
    tracksPixmap->DrawText(cPoint(0, y), Text, ColorFg, ColorBg, font, maxItemWidth, itemHeight - 2, taCenter);
}

void cFlatDisplayTracks::SetTrack(int Index, const char * const *Tracks) {
    if (currentIndex >= 0)
        SetItem(Tracks[currentIndex], currentIndex, false);
    SetItem(Tracks[Index], Index, true);
}

void cFlatDisplayTracks::SetAudioChannel(int AudioChannel) {
    return;
}

void cFlatDisplayTracks::Flush(void) {
    TopBarUpdate();
    osd->Flush();
}
