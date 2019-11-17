#pragma once

#include "baserender.h"

class cFlatDisplayReplay : public cFlatBaseRender, public cSkinDisplayReplay {
    private:
        cString current, total;
    
        int labelHeight;
        cPixmap *labelPixmap;
        cPixmap *labelJump;
        cPixmap *iconsPixmap;
    
        void UpdateInfo(void);
    public:
        cFlatDisplayReplay(bool ModeOnly);
        virtual ~cFlatDisplayReplay();
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
