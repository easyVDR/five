#ifndef __DISPLAYTRACKS_H
#define __DISPLAYTRACKS_H

#include <vdr/skins.h>
#include "config.h"
#include "coreengine/viewdisplaytracks.h"

class cSDDisplayTracks : public cSkinDisplayTracks {
private:
    cViewTracks *view;
    bool ok;
public:
    cSDDisplayTracks(cViewTracks *tracksView, const char *Title, int NumTracks, const char * const *Tracks);
    virtual ~cSDDisplayTracks();
    virtual void SetTrack(int Index, const char * const *Tracks);
    virtual void SetAudioChannel(int AudioChannel);
    virtual void Flush(void);
};


#endif //__DISPLAYTRACKS_H
