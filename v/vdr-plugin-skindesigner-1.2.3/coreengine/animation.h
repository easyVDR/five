#ifndef __ANIMATION_H
#define __ANIMATION_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "definitions.h"
#include "osdwrapper.h"

/******************************************************************
* Detaching
******************************************************************/
class cDetachable {
protected:
    cDetachable(void) {};
    ~cDetachable(void) {};
public:
    virtual int Delay(void) = 0;
    virtual void StartAnimation(void) = 0;
    virtual void ParseDetached(void) = 0;
    virtual void RenderDetached(void) = 0;
    virtual void Flush(void) = 0;
};

class cDetacher : public cThread, public cListObject {
private:
    cCondWait sleepWait;
    cDetachable *detachable;
    bool waitOnWakeup;
    bool keepSleeping;
    bool doAnimation;
    void Sleep(int duration);
    void Wait(void);
    virtual void Action(void);
public:
    cDetacher(cDetachable *detachable, bool wait, bool animation);
    ~cDetacher(void);
    void WakeUp(void);
    void ResetSleep(void);
    void Stop(bool deletePixmaps);
};

/******************************************************************
* cAnimation
******************************************************************/
class cAnimation : public cListObject {
protected:
    uint64_t started;
    bool finished;
    bool persistent;
    int frametime;
public:
    cAnimation(void);
    virtual ~cAnimation(void);
    virtual void SetInitial(void) = 0;
    virtual void Reactivate(void) = 0;
    virtual bool Tick(void) = 0;
    bool Finished(void) { return finished; };
    virtual void SetFinished(void) { finished = true; };
    void SetPersistent(void) { persistent = true; };
    bool Persistent(void) { return persistent; };
};

/******************************************************************
* Scrolling
******************************************************************/
class cScrollable {
protected:
    cScrollable(void) {};
    ~cScrollable(void) {};
public:
    virtual int ScrollDelay(void) = 0;
    virtual int ScrollWidth(void) = 0;
    virtual int ScrollHeight(void) = 0;
    virtual eScrollMode ScrollMode(void) = 0;
    virtual eScrollSpeed ScrollSpeed(void) = 0;
    virtual eOrientation ScrollOrientation(void) = 0;
    virtual void SetScrollingStarted(void) = 0;
    virtual void StartScrolling(void) = 0;
    virtual void StopScrolling(void) = 0;
    virtual void SetDrawPort(cPoint &point) = 0;
};

class cScroller : public cAnimation {
private:
    cScrollable *scrollable;
    int delay;
    bool paused;
    int pauseTime;
    bool scrollingStarted;
    bool secondDelay;
    eOrientation orientation;
    int scrollLength;
    bool carriageReturn;
    float drawPortX;
    float drawPortY;
    float scrollDelta;
    bool delScrollPix;
    void Init(void);
    bool Pause(void);
    bool Overflow(void);
public:
    cScroller(cScrollable *scrollable);
    ~cScroller(void);
    void SetInitial(void);
    void Reactivate(void);
    void SetFinished(void);
    void UnsetDelScrollPix(void) { delScrollPix = false; };
    bool Tick(void);
};

/******************************************************************
* Fading
******************************************************************/
class cFadable {
protected:
    cFadable(void) {};
    ~cFadable(void) {};
public:
    virtual int Delay(void) = 0;
    virtual int FadeTime(void) = 0;
    virtual void SetTransparency(int transparency, bool force = false) = 0;
};

class cFader : public cAnimation {
private:
    cFadable *fadable;
    bool fadein;
    int fadetime;
    int step;
    int transparency;
    bool hideWhenFinished;
public:
    cFader(cFadable *fadable);
    ~cFader(void);
    void SetInitial(void);
    void Reactivate(void);
    void SetFadeOut(void);
    void SetFinished(void);
    void SetHideWhenFinished(void) { hideWhenFinished = true; };
    bool Tick(void);
};
/******************************************************************
* Shifting
******************************************************************/
class cShiftable {
protected:
    cShiftable(void) {};
    ~cShiftable(void) {};
public:
    virtual int Delay(void) = 0;
    virtual int ShiftTime(void) = 0;
    virtual int ShiftMode(void) = 0;
    virtual void ShiftPositions(cPoint *start, cPoint *end) = 0;
    virtual void SetPosition(cPoint &position, cPoint &reference, bool force = false) = 0;
};

class cListShiftable {
protected:
    cListShiftable(void) {};
    ~cListShiftable(void) {};
public:
    virtual int ListShiftTime(void) = 0;
    virtual int ShiftDistance(void) = 0;
    virtual eOrientation ShiftOrientation(void) = 0;
    virtual void SetIndicatorPosition(cPoint &position) = 0;
};

class cShifter : public cAnimation {
private:
    cShiftable *shiftable;
    bool shiftin;
    cPoint start, end;
    int shifttime;
    eShiftMode mode;
    int step;
    float stepXLinear, stepYLinear;
    int stepsFast;
    float stepXFast, stepXSlow;
    float stepYFast, stepYSlow;
    float x, y;
    void Init(void);
    void NextPosition(void);
public:
    cShifter(cShiftable *shiftable);
    ~cShifter(void);
    void SetInitial(void);
    void Reactivate(void);
    bool Tick(void);
};

class cListShifter : public cAnimation {
private:
    cListShiftable *shiftable;
    bool shiftin;
    bool fromtop;
    int distance;
    eOrientation orientation;
    int shifttime;
    int step;
    cPoint pos;
    void NextPosition(void);
    void EndPosition(void);
public:
    cListShifter(cListShiftable *shiftable);
    ~cListShifter(void);
    void SetInitial(void);
    void Reactivate(void);
    void SetShiftOut(void) { shiftin = false; };
    void SetDirection(bool fromTop) { fromtop = fromTop; };
    bool Tick(void);
};

/******************************************************************
* Blinking
******************************************************************/
class cBlinkable {
protected:
    cBlinkable(void) {};
    ~cBlinkable(void) {};
public:
    virtual int BlinkFreq(int func) = 0;
    virtual void DoBlink(int func, bool on) = 0;
};

class cBlinker : public cAnimation {
private:
    cBlinkable *blinkable;
    int blinkFunc;
    int freq;
    bool blinkOn;
    bool paused;
    int pauseTime;
    bool Pause(void);
public:
    cBlinker(cBlinkable *blinkable, int blinkFunc);
    ~cBlinker(void);
    void SetInitial(void);
    void Reactivate(void);
    bool Tick(void);
};

/******************************************************************
* cAnimator
******************************************************************/
class cAnimator : public cThread {
private:
    cSdOsd *osd;
    cCondWait sleepWait;
    cCondWait pauseWait;
    int timeslice;
    int timeneeded;
    cMutex animLock;
    cList<cAnimation> animations;
    cList<cAnimation> animationsPersistent;
    void Sleep(uint64_t start);
    void DoTick(bool &animActive);
    void CleanupAnims(void);
    virtual void Action(void);
public:
    cAnimator(cSdOsd *osd);
    ~cAnimator(void);
    void AddAnimation(cAnimation *animation, bool startAnim = true);
    void RemoveAnimation(cAnimation *remove);
    void Stop(void);
    void Finish(void);
};

#endif //__ANIMATION_H