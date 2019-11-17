#ifndef __DISPLAYVOLUME_H
#define __DISPLAYVOLUME_H

#include "config.h"
#include "coreengine/viewdisplayvolume.h"

class cSDDisplayVolume : public cSkinDisplayVolume {
private:
    cViewVolume *view;
    bool ok;
public:
    cSDDisplayVolume(cViewVolume *volumeView);
    virtual ~cSDDisplayVolume();
    virtual void SetVolume(int Current, int Total, bool Mute);
    virtual void Flush(void);
};

#endif //__DISPLAYVOLUME_H
