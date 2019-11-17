#pragma once

#include "baserender.h"

class cFlatDisplayTracks : public cFlatBaseRender, public cSkinDisplayTracks {
    private:
        cPixmap *tracksPixmap;

        int itemHeight;
        int maxItemWidth;
        int currentIndex;

        void SetItem(const char *Text, int Index, bool Current);
    public:
        cFlatDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
        virtual ~cFlatDisplayTracks();
        virtual void SetTrack(int Index, const char * const *Tracks);
        virtual void SetAudioChannel(int AudioChannel);
        virtual void Flush(void);
};
