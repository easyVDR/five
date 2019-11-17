#ifndef __VIEW_H
#define __VIEW_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "osdwrapper.h"
#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"
#include "viewelementplugin.h"
#include "viewelementscommon.h"
#include "viewelementsdisplaychannel.h"
#include "viewelementsdisplaymenu.h"
#include "viewelementsdisplayreplay.h"
#include "viewelementsdisplaytracks.h"
#include "viewlist.h"
class cView;
#include "xmlparser.h"
#include "animation.h"

using namespace std;

class cView : public cFadable, public cShiftable {
private:
    void DoScaleTv(const cRect *frame);
    void SetClearOnDisplay(int ve, const char *clearOnDisplay);
protected:
    cSdOsd sdOsd;
    static cAnimator *animator;
    cViewAttribs *attribs;
    cRect container;
    bool init;
    eViewType viewId;
    cGlobals *globals;
    char *viewName;
    int numViewElements;
    cViewElement **viewElements;
    cViewElement **viewElementsHorizontal;
    map<string,int> viewElementNames;
    bool shifting;
    cRect tvFrame;
    cRect *currentTvFrame;
    cRect *newTvFrame;
    bool menuInit;
    int ViewElementId(const char *name);
    virtual void ClearVariables(void);
    virtual void SetViewElementObjects(void) { };
    virtual void StartAnimation(void);
    virtual void WakeViewElements(void);
    cPoint ShiftStart(cRect &shiftbox);
    virtual cRect CoveredArea(void);
    void ScaleTv(void);
    void UnScaleTv(void);
public:
    cView(void);
    virtual ~cView(void);
    //Loading and Caching functionality
    bool ReadFromXML(void);
    virtual void SetGlobals(cGlobals *globals);    
    const char *GetViewName(void) { return viewName; };
    void SetContainer(int x, int y, int width, int height);
    void SetAttributes(vector<stringpair> &attributes);
    void AddViewElement(const char *sViewElement, cViewElement *viewElement);
    bool ValidViewElement(const char *viewElement);
    virtual void AddViewList(cViewList *viewList) { };
    bool ValidViewList(const char *viewList);
    virtual void PreCache(void);
    cRect *GetTvFrame(void) { return &tvFrame; };
    virtual const cFont *GetTextAreaFont(void) { return NULL; };
    virtual int GetTextAreaWidth(void) { return 0; };
    virtual int GetListWidth(void) { return 0; };
    static void AddAnimation(cAnimation *animation, bool startAnimation = true);
    static void RemoveAnimation(cAnimation *animation);
    //View API
    virtual bool Init(void);
    void Clear(int ve, bool forceClearBackground = false);
    void SetDirty(int ve);
    void Render(int ve, bool force = false);
    void Hide(int ve);
    void Show(int ve);
    virtual void Close(void);
    virtual void Flush(void);
    virtual void Debug(void);
    void SetViewelementsAnimOut(void);
    //Fadable
    int Delay(void) { return 0; };
    int FadeTime(void);
    virtual void SetTransparency(int transparency, bool force = false);
    //Shiftable
    int ShiftTime(void);
    int ShiftMode(void);
    void ShiftPositions(cPoint *start, cPoint *end);
    virtual void SetPosition(cPoint &position, cPoint &reference, bool force = false);
    void SetStartShifting(void) { shifting = true; };
    void SetEndShifting(void) { shifting = false; };
};

#endif //__VIEW_H
