#ifndef __VIEWELEMENT_H
#define __VIEWELEMENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <list>
#include <vdr/tools.h>
#include "osdwrapper.h"
#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"
#include "area.h"
#include "animation.h"

/******************************************************************
* cViewElement
******************************************************************/
class cViewElement : public cDetachable, public cFadable, public cShiftable {
protected:
    cSdOsd *sdOsd;
    int id;
    bool init;
    bool drawn;
    bool dirty;
    bool blocked;
    bool detached;
    bool doAnimOut;
    bool doStartAnim;
    bool waitOnWakeup;
    bool scrollingStarted;
    bool startAnimation;
    bool restartAnimation;
    cGlobals *globals;
    cRect container;
    cViewElementAttribs *attribs;
    vector<int> clearOnDisplay;
    bool clearAll;
    cList<cAreaNode> areaNodes;
    skindesignerapi::cTokenContainer *tokenContainer;
    list<cScroller*> scrollers;
    cDetacher *detacher;
    cShifter *shifter;
    cFader *fader;
    void InheritTokenContainer(void);
    void InheritTokenContainerDeep(void);
    virtual bool DoScroll(void) { return true; };
    cPoint ShiftStart(cRect &shiftbox);
    void StopAnimation(void);
public:
    cViewElement(void);
    cViewElement(const cViewElement &other);
    virtual ~cViewElement(void);
    void SetOsd(cSdOsd *osd) { sdOsd = osd; };
    static cViewElement *CreateViewElement(const char *name, const char *viewname);
    void SetId(int id) { this->id = id; };
    void SetGlobals(cGlobals *globals);
    virtual void SetTokenContainer(void);
    void SetDetached(void) { detached = true; };
    void SetAnimOut(void) { doAnimOut = true; };
    void UnsetStartAnim(void) { doStartAnim = false; };
    void UnsetWaitOnWakeup(void) { waitOnWakeup = false; };
    bool Detached(void);
    void SetContainer(int x, int y, int width, int height);
    void SetAttributes(vector<stringpair> &attributes);
    void AddArea(cAreaNode *area);
    void SetAreaX(int x);
    void SetAreaY(int y);
    void SetAreaWidth(int width);
    void SetAreaHeight(int height);
    void Cache(void);
    virtual void Close(void);
    void StopBlinking(void);
    virtual void Clear(bool forceClearBackground = false);
    void Hide(void);
    void Show(void);
    void WakeUp(void);
    bool Execute(void);
    void SetDirty(void) { dirty = true; };
    bool Dirty(void) { return dirty; };
    void SetPosition(int newX, int newY, int newWidth, int newHeight);
    virtual void Render(void);
    void StopScrolling(bool deletePixmaps = true);
    eOrientation Orientation(void) { return attribs->Orientation(); };
    virtual int Delay(void) { return attribs->Delay(); };
    const char *ClearOnDisplay(void) { return attribs->ClearOnDisplay(); };
    void SetClearOnDisplay(vector<int> clearOnDisplay) { this->clearOnDisplay = clearOnDisplay; };
    vector<int> GetClearOnDisplay(void) { return clearOnDisplay; };
    void SetClearAll(void) { clearAll = true; };
    bool DoClearAll(void) { return clearAll; };
    void ParseDetached(void);
    void RenderDetached(void);
    bool Shifting(void);
    bool Fading(void);
    int FadeTime(void);
    int ShiftTime(void);
    int ShiftMode(void);
    void ShiftPositions(cPoint *start, cPoint *end);
    void StartAnimation(void);
    void SetRestartAnimation(void) { restartAnimation = true; };
    virtual void SetTransparency(int transparency, bool force = false);
    virtual void SetPosition(cPoint &position, cPoint &reference, bool force = false);
    void SetStartShifting(void) { };
    void SetEndShifting(void) { };
    cRect CoveredArea(void);
    void Flush(void);
    virtual bool Parse(bool forced = false);
    cFunction *GetFunction(const char *name);
    virtual void Debug(bool full = false);
};

#endif //__VIEWELEMENT_H