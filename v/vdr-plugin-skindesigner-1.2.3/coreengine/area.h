#ifndef __TEMPLATEAREA_H
#define __TEMPLATEAREA_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <list>

#include "osdwrapper.h"
#include "definitions.h"
#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"
#include "attributes.h"
#include "functions.h"
#include "animation.h"

class cArea;
/******************************************************************
* cAreaNode
******************************************************************/
class cAreaNode : public cListObject {
protected:
    cGlobals *globals;
    cRect container;
    bool isTab;
    bool activeTab;
public:
    cAreaNode(void);
    virtual ~cAreaNode(void);
    virtual void SetGlobals(cGlobals *globals) {};
    virtual void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) {};
    virtual void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {};
    void SetContainer(int x, int y, int width, int height);
    virtual void SetAttributes(vector<stringpair> &attributes) {};
    virtual void SetX(int x) {};
    virtual void SetY(int y) {};
    virtual void SetWidth(int width) {};
    virtual void SetHeight(int height) {};
    void SetTab(void) { isTab = true; };
    bool IsTab(void) { return isTab; };
    void SetActiveTab(bool active) { activeTab = active; };
    bool ActiveTab(void) { return activeTab; };
    virtual int GetWidth(void) { return 0; };
    virtual void Cache(void) {};
    virtual void Close(void) {};
    virtual void StopBlinkers(void) {};
    virtual void Clear(bool forceClearBackground = false) {};
    virtual void ClearWithoutIndicators(void) {};
    virtual void Hide(void) {};
    virtual void Show(void) {};
    virtual void Render(void) {};
    virtual bool Execute(void) { return true; };
    virtual void SetTransparency(int transparency, bool absolute = false) {};
    virtual void SetIndicatorTransparency(int transparency) {};
    virtual void SetViewPort(cRect &vp) {};
    virtual void SetPosition(cPoint &pos, cPoint &ref) {};
    virtual void SetIndicatorPosition(cPoint &pos) {};
    virtual cRect CoveringArea(void) { return cRect::Null; };
    virtual bool Scrolling(void) { return false; };
    virtual bool ScrollingStarted(void) { return false; };
    virtual cArea *ScrollingArea(void) { return NULL; };
    virtual cFunction *GetFunction(const char *name) { return NULL; };
    virtual const char *Name(void) { return NULL; };
    virtual bool BackgroundArea(void) { return false; };
    virtual void Debug(bool full = false) {};
};

class cAreaContainer;
/******************************************************************
* cArea
******************************************************************/
class cArea : public cAreaNode, public cScrollable, public cBlinkable {
private:
    cSdOsd *sdOsd;
    bool init;
    bool isBackgroundArea;
    cPixmap *pix;
    cAreaAttribs *attribs;
    cAreaContainer *areaContainer;
    cList<cFunction> functions;
    bool scrolling;
    bool isScrolling;
    bool scrollingStarted;
    cFunction *scrollFunc;
    list<cBlinker*> blinkers;
    bool blinking;
    void InitFunctions(void);
    void CreatePixmap(cRect drawPort = cRect::Null);
    void SetScrollFunc(void);
    void StartBlinkers(void);
public:
    cArea(void);
    cArea(const cArea &other);
    virtual ~cArea(void);
    void SetOsd(cSdOsd *osd) { sdOsd = osd; };
    void SetGlobals(cGlobals *globals);
    void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer);
    void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    void SetAttributes(vector<stringpair> &attributes);
    void SetScrolling(void) { scrolling = true; };
    void SetAreaContainer(cAreaContainer *ac) { areaContainer = ac; };
    bool ValidFunction(const char *func);
    cFunction *AddFunction(const char *name, vector<stringpair> attribs, cFuncLoop *loopFunc = NULL);
    cFunction *GetFunction(const char *name);
    void SetX(int x);
    void SetY(int y);
    void SetWidth(int width);
    void SetHeight(int height);
    void Cache(void);
    int GetWidth(void) { return attribs->Width(); };
    void Close(void);
    void Clear(bool forceClearBackground = false);
    void ClearWithoutIndicators(void);
    void Hide(void);
    void Show(void);
    void Render(void);
    bool Execute(void);
    bool IsIndicatorArea(void) { return attribs->IndicatorArea(); };
    void SetTransparency(int transparency, bool absolute = false);
    void SetIndicatorTransparency(int transparency);
    void SetIndicatorPosition(cPoint &pos);
    cRect CoveringArea(void);
    //Scrollable
    bool Scrolling(void);
    void SetScrollingStarted(void) { scrollingStarted = true; };
    bool ScrollingStarted(void) { return scrollingStarted; };
    int ScrollWidth(void);
    int ScrollHeight(void);
    int ScrollDelay(void);
    eScrollMode ScrollMode(void);
    eScrollSpeed ScrollSpeed(void);
    eOrientation ScrollOrientation(void);
    cArea *ScrollingArea(void) { return this; };
    void StartScrolling(void);
    void StopScrolling(void);
    cRect ViewPort(void);
    void SetDrawPort(cPoint &point);
    void SetViewPort(cRect &vp);
    void SetPosition(cPoint &pos, cPoint &ref);
    cRect DrawPort(void);
    int ScrollStep(void) { return attribs->ScrollStep(); };
    //Blinkable
    int BlinkFreq(int func);
    void DoBlink(int func, bool on);
    void StopBlinkers(void);
    //Common
    const char *Name(void) { return attribs->Name(); };
    bool BackgroundArea(void) { return attribs->BackgroundArea(); };
    void Debug(bool full = false);
};

/******************************************************************
* cAreaContainer
******************************************************************/
class cAreaContainer : public cAreaNode {
private:
    cAreaContainerAttribs *attribs;
    cList<cArea> areas;
public:
    cAreaContainer(void);
    cAreaContainer(const cAreaContainer &other);
    virtual ~cAreaContainer(void);
    void SetGlobals(cGlobals *globals);
    void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer);
    void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    void SetAttributes(vector<stringpair> &attributes);
    void AddArea(cArea *area);
    cFunction *GetFunction(const char *name);
    void SetX(int x);
    void SetY(int y);
    void SetWidth(int width);
    void SetHeight(int height);
    void Cache(void);
    void Close(void);
    void Clear(bool forceClearBackground = false);
    void ClearWithoutIndicators(void);
    void Hide(void);
    void Show(void);
    void Render(void);
    bool Execute(void);
    void SetTransparency(int transparency, bool absolute = false);
    void SetIndicatorTransparency(int transparency);
    void SetIndicatorPosition(cPoint &pos);
    void SetViewPort(cRect &vp);
    void SetPosition(cPoint &pos, cPoint &ref);
    cRect CoveringArea(void);
    bool Scrolling(void);
    cArea *ScrollingArea(void);
    void Debug(bool full = false);
};

#endif //__TEMPLATEAREA_H