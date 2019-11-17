#ifndef __DISPLAYREPLAY_H
#define __DISPLAYREPLAY_H

#include <vdr/skins.h>
#include "config.h"
#include "coreengine/viewdisplayreplay.h"

class cSDDisplayReplay : public cSkinDisplayReplay {
private:
    bool ok;
    bool init;
    cViewReplay *view;
    void SetTimeShiftValues(const cRecording *recording);
public:
    cSDDisplayReplay(cViewReplay *replayView, bool ModeOnly);
    virtual ~cSDDisplayReplay();
    virtual void SetRecording(const cRecording *Recording);
    virtual void SetTitle(const char *Title);
    virtual void SetMode(bool Play, bool Forward, int Speed);
    virtual void SetProgress(int Current, int Total);
    virtual void SetCurrent(const char *Current);
    virtual void SetTotal(const char *Total);
    virtual void SetJump(const char *Jump);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};

#endif //__DISPLAYREPLAY_H
