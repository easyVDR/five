#ifndef __VIEWDISPLAYVOLUME_H
#define __VIEWDISPLAYVOLUME_H

#include "view.h"

class cViewVolume : public cView {
private:
    cVeVolume *veVolume;
    void SetViewElements(void);
    void SetViewElementObjects(void);
    void ClearVariables(void);
public:
    cViewVolume(void);
    virtual ~cViewVolume(void);
    void SetVolume(int current, int total, bool mute);
    void Flush(void);
};

#endif //__VIEWDISPLAYVOLUME_H