#include "../config.h"
#include "animation.h"
#include <math.h>

/******************************************************************
* cDetacher
******************************************************************/
cDetacher::cDetacher(cDetachable *detachable, bool wait, bool animation) : cThread("detacher thread") {
    this->detachable = detachable;
    waitOnWakeup = wait;
    keepSleeping = false;
    doAnimation = animation;
}

cDetacher::~cDetacher(void) {
    sleepWait.Signal();
    Cancel(2);
}

void cDetacher::WakeUp(void) {
    sleepWait.Signal();
}

void cDetacher::ResetSleep(void) {
    keepSleeping = true;
    sleepWait.Signal();
}

void cDetacher::Stop(bool deletePixmaps) { 
    sleepWait.Signal();
    Cancel(2);
}

void cDetacher::Action(void) {
    if (!detachable) {
        return;
    }
    if (waitOnWakeup) {
        Wait();
        int delay = 50 + detachable->Delay();
        Sleep(delay);
    } else {
        int delay = detachable->Delay();
        if (delay > 0)
            Sleep(delay);
    }

    detachable->ParseDetached();
    detachable->RenderDetached();
    if (!doAnimation)
        detachable->Flush();
    if (!Running()) return;
    if (doAnimation) {
        detachable->StartAnimation();        
    }
}

void cDetacher::Sleep(int duration) {
    if (duration <= 0)
        return;
    do {
        keepSleeping = false;
        sleepWait.Wait(duration);
    } while (keepSleeping);
}

void cDetacher::Wait(void) {
    //wait has to be waked up from outside
    sleepWait.Wait(0);
}

/******************************************************************
* cAnimaton
******************************************************************/
cAnimation::cAnimation(void) {
    started = 0;
    finished = false;
    persistent = false;
    frametime = 1000 / config.FPS;
}

cAnimation::~cAnimation(void) {
}

/******************************************************************
* cScroller
******************************************************************/
cScroller::cScroller(cScrollable *scrollable) { 
    this->scrollable = scrollable;
    paused = true;
    pauseTime = 0;
    scrollingStarted = false;
    secondDelay = false;
    delScrollPix = true;
    Init();
}

cScroller::~cScroller(void) {
}

void cScroller::Init(void) {
    delay = scrollable->ScrollDelay();
    orientation = scrollable->ScrollOrientation();
    if (orientation == eOrientation::horizontal) {
        scrollLength = scrollable->ScrollWidth();
    } else if (orientation == eOrientation::vertical) {
        scrollLength = scrollable->ScrollHeight();
    }
    eScrollMode mode = scrollable->ScrollMode();
    carriageReturn = (mode == eScrollMode::carriagereturn) ? true : false;
    drawPortX = 0.0f;
    drawPortY = 0.0f;
    eScrollSpeed speed = scrollable->ScrollSpeed();
    if (speed == eScrollSpeed::slow)
        scrollDelta = 0.5f;
    else if (speed == eScrollSpeed::fast)
        scrollDelta = 2.0f;
    else
        scrollDelta = 1.0f;
}

void cScroller::Reactivate(void) {}

void cScroller::SetInitial(void) {
    scrollable->SetScrollingStarted();
}

bool cScroller::Pause(void) {
    if (!paused)
        return false;
    if ((pauseTime + frametime) > delay) {
        paused = false;
        pauseTime = 0;
        return false;
    }
    pauseTime += frametime;
    return true;
}

bool cScroller::Overflow(void) {
    if (orientation == eOrientation::horizontal) {
        if (!carriageReturn && (drawPortX >= 1)) {
            drawPortX = 0;
            scrollDelta *= -1;
            paused = true;
            return true;
        }
        if (carriageReturn && (drawPortX >= 0) && secondDelay) {
            cPoint drawPortPoint(drawPortX,0);
            scrollable->SetDrawPort(drawPortPoint);
            drawPortX = -1;
            paused = true;
            secondDelay = false;
            return true;
        }
        if (abs((int)drawPortX) < scrollLength)
            return false;
        if (carriageReturn) {
            drawPortX = 0;
            secondDelay = true;
        } else {
            scrollDelta *= -1;
            drawPortX -= scrollDelta;
        }
    } else if (orientation == eOrientation::vertical) {
        if ((drawPortY >= 0) && secondDelay) {
            cPoint drawPortPoint(0, drawPortY);
            scrollable->SetDrawPort(drawPortPoint);
            drawPortY = -1;
            paused = true;
            secondDelay = false;
            return true;
        }
        if (abs((int)drawPortY) < scrollLength)
            return false;
        secondDelay = true;
        drawPortY = 0;
    }
    paused = true;
    return true;
}

void cScroller::SetFinished(void) {
    finished = true;
    if (delScrollPix) {
        scrollable->StopScrolling();
    }
}

bool cScroller::Tick(void) {
    if (finished) {
        return false;
    }

    if (Pause())
        return true;

    if (!scrollingStarted) {
        scrollable->StartScrolling();
        scrollingStarted = true;
    }

    if (Overflow())
        return true;

    cPoint drawPortPoint(0,0);
    if (orientation == eOrientation::horizontal) {
        drawPortX -= scrollDelta;
        drawPortPoint.SetX(drawPortX);
    } else if (orientation == eOrientation::vertical) {
        drawPortY -= scrollDelta;
        drawPortPoint.SetY(drawPortY);
    }
    scrollable->SetDrawPort(drawPortPoint);
    return true;
};

/******************************************************************
* cFader
******************************************************************/
cFader::cFader(cFadable *fadable) { 
    this->fadable = fadable;
    fadein = true;
    fadetime = fadable->FadeTime();
    step = 100.0f / ((double)fadetime / (double)frametime);
    transparency = 100;
    hideWhenFinished = false;
}

cFader::~cFader(void) {
}

void cFader::Reactivate(void) {
    started = 0;
    finished = false;
    fadein = false;
}

void cFader::SetInitial(void) {
    fadable->SetTransparency(transparency);
}

void cFader::SetFadeOut(void) { 
    fadein = false;
    transparency = 0; 
}

void cFader::SetFinished(void) {
    finished = true;
    if (hideWhenFinished)
        fadable->SetTransparency(100);
}

bool cFader::Tick(void) {
    if (finished) {
        if (fadein)
            fadable->SetTransparency(0);
        else
            fadable->SetTransparency(100);
        return false;
    }
    if (!started) {
        started = cTimeMs::Now();
    }

    if ((int)(cTimeMs::Now() - started) > fadetime) {
        if (fadein)
            fadable->SetTransparency(0);
        else
            fadable->SetTransparency(100);
        finished = true;
        return false;
    }
    fadable->SetTransparency(transparency);
    if (fadein) {
        transparency -= step;
    } else {
        transparency += step;
    }
    return true;
};

/******************************************************************
* cShifter
******************************************************************/
cShifter::cShifter(cShiftable *shiftable) { 
    this->shiftable = shiftable;
    step = 0;
    shiftin = true;
    shifttime = 0;
    x = 0.0f;
    y = 0.0f;
    stepXLinear = 0.0f;
    stepYLinear = 0.0f;
    stepsFast = 0;
    stepXFast = 0.0f;
    stepXSlow = 0.0f;
    stepYFast = 0.0f;
    stepYSlow = 0.0f;
    Init();
}

cShifter::~cShifter(void) {
}

void cShifter::Init(void) {
    shifttime = shiftable->ShiftTime();
    mode = (eShiftMode)shiftable->ShiftMode();
    shiftable->ShiftPositions(&start, &end);
    int steps = (double)shifttime / (double)frametime;
    if (steps <= 0) steps = 1;
    float percentFast = 33.3f;
    float distanceFast = 85.0f;
    stepsFast = (float)steps * percentFast / 100.0f;
    if (start.X() == end.X()) {
        stepYLinear = (float)(end.Y() - start.Y()) / (float)steps;
        stepYFast = (float)(end.Y() - start.Y()) * distanceFast / 100.0f / (float)stepsFast;
        stepYSlow = (float)(end.Y() - start.Y()) * (100.0f - distanceFast) / 100.0f / (float)(steps-stepsFast);
    } else if (start.Y() == end.Y()) {
        stepXLinear = (float)(end.X() - start.X()) / (float)steps;
        stepXFast = (float)(end.X() - start.X()) * distanceFast / 100.0f / (float)stepsFast;
        stepXSlow = (float)(end.X() - start.X()) * (100.0f - distanceFast) / 100.0f / (float)(steps-stepsFast);            
    } else {
        stepXLinear = (float)(end.X() - start.X()) / (float)steps;
        stepXFast = (float)(end.X() - start.X()) * distanceFast / 100.0f / (float)stepsFast;
        stepXSlow = (float)(end.X() - start.X()) * (100.0f - distanceFast) / 100.0f / (float)(steps-stepsFast);            
        stepYLinear = (float)(end.Y() - start.Y()) / (float)steps;
        stepYFast = (float)(end.Y() - start.Y()) * distanceFast / 100.0f / (float)stepsFast;
        stepYSlow = (float)(end.Y() - start.Y()) * (100.0f - distanceFast) / 100.0f / (float)(steps-stepsFast);
    }
    if (shiftin) {
        x = start.X();
        y = start.Y();
    } else {
        x = end.X();
        y = end.Y();
    }
}

void cShifter::Reactivate(void) {
    started = 0;
    finished = false;
    shiftin = false;
    step = 0;
    Init();
}

void cShifter::SetInitial(void) {
    cPoint pos(x, y);
    shiftable->SetPosition(pos, end);
}

void cShifter::NextPosition(void) {
    if (mode == eShiftMode::linear) {
        if (shiftin) {
            x += stepXLinear;
            y += stepYLinear;
        } else {
            x -= stepXLinear;
            y -= stepYLinear;
        }
    } else if (mode == eShiftMode::slowedDown) {
        if (shiftin) {
            if (step <= stepsFast) {
                x += stepXFast;
                y += stepYFast;
            } else {
                x += stepXSlow;
                y += stepYSlow;
            }
        } else {
            if (step <= stepsFast) {
                x -= stepXFast;
                y -= stepYFast;
            } else {
                x -= stepXSlow;
                y -= stepYSlow;
            }
        }
    }
}

bool cShifter::Tick(void) {
    if (finished)
        return false;
    if (!started) {
        started = cTimeMs::Now();
    }
    if ((int)(cTimeMs::Now() - started) > shifttime) {
        if (shiftin)
            shiftable->SetPosition(end, end);
        else
            shiftable->SetPosition(start, end);
        finished = true;
        return false;
    }
    cPoint pos(x, y);
    shiftable->SetPosition(pos, end);
    step++;
    NextPosition();
    return true;
};

/******************************************************************
* cListShifter
******************************************************************/
cListShifter::cListShifter(cListShiftable *shiftable) { 
    this->shiftable = shiftable;
    shifttime = shiftable->ListShiftTime();
    distance = shiftable->ShiftDistance();
    orientation = shiftable->ShiftOrientation();
    int steps = (double)shifttime / (double)frametime;
    if (steps <= 0) steps = 1;
    step = distance / steps;
    shiftin = true;
    fromtop = true;
}

cListShifter::~cListShifter(void) {
}

void cListShifter::Reactivate(void) {}

void cListShifter::SetInitial(void) {
    if (shiftin) {
        if (orientation == eOrientation::horizontal) {
            if (fromtop) {
                pos.SetX(-1 * distance);
                pos.SetY(0);
            } else {
                pos.SetX(distance);
                pos.SetY(0);
            }
        } else {
            if (fromtop) {
                pos.SetX(0);
                pos.SetY(-1 * distance);
            } else {
                pos.SetX(0);
                pos.SetY(distance);
            }
        }
    }
    shiftable->SetIndicatorPosition(pos);
}

void cListShifter::NextPosition(void) {
    int x = pos.X();
    int y = pos.Y();
    if (orientation == eOrientation::horizontal) {
        if (fromtop) {
            pos.SetX(x+step);
        } else {
            pos.SetX(x-step);
        }
    } else {
        if (fromtop) {
            pos.SetY(y+step);
        } else {
            pos.SetY(y-step);
        }
    }
}

void cListShifter::EndPosition(void) {
    if (shiftin) {
        pos.SetX(0);
        pos.SetY(0);
    } else {
        if (orientation == eOrientation::horizontal) {
            pos.SetX(distance);
        } else {
            pos.SetY(distance);                
        }
    }
    shiftable->SetIndicatorPosition(pos);
}

bool cListShifter::Tick(void) {
    if (finished) {
        EndPosition();
        return false;
    }

    if (!started) {
        started = cTimeMs::Now();
    }

    if ((int)(cTimeMs::Now() - started) > shifttime) {
        EndPosition();
        finished = true;
        return false;
    }
    shiftable->SetIndicatorPosition(pos);
    NextPosition();
    return true;
};

/******************************************************************
* cBlinker
******************************************************************/
cBlinker::cBlinker(cBlinkable *blinkable, int blinkFunc) {
    this->blinkable = blinkable;
    this->blinkFunc = blinkFunc;
    freq = blinkable->BlinkFreq(blinkFunc);
    blinkOn = false;
    paused = true;
    pauseTime = 0;
}

cBlinker::~cBlinker(void) {
}

void cBlinker::Reactivate(void) {}
void cBlinker::SetInitial(void) {}

bool cBlinker::Pause(void) {
    if (!paused)
        return false;
    if ((pauseTime + frametime) > freq) {
        paused = false;
        pauseTime = 0;
        return false;
    }
    pauseTime += frametime;
    return true;
}

bool cBlinker::Tick(void) {
    if (finished)
        return false;
    if (Pause())
        return true;
    blinkable->DoBlink(blinkFunc, blinkOn);
    blinkOn = !blinkOn;
    paused = true;
    pauseTime = 0;
    return true;
};

/******************************************************************
* cAnimator
******************************************************************/
cAnimator::cAnimator(cSdOsd *osd) : cThread("animator thread") {
    this->osd = osd;
    timeneeded = 0;
    timeslice = 1000 / config.FPS;
}

cAnimator::~cAnimator(void) {
    Stop();
}

void cAnimator::Sleep(uint64_t start) {
    timeneeded = cTimeMs::Now() - start;
    int sleepTime = (timeslice - timeneeded) > 0 ? timeslice - timeneeded : 0;
    if (sleepTime)
        sleepWait.Wait(sleepTime);
}

void cAnimator::DoTick(bool &animActive) {
    animLock.Lock();
    for (cAnimation *animation = animations.First(); animation; animation = animations.Next(animation)) {
        if (Running()) {
            bool currentAnimActive = animation->Tick();
            animActive = animActive || currentAnimActive;
        }
    }
    animLock.Unlock();
}

/*****************************************************************************************
* Cleanup Anims
* removes finished anims
* remembers persistent anims 
*****************************************************************************************/
void cAnimator::CleanupAnims(void) {
    bool found;
    animLock.Lock();
    do {
        found = false;
        for (cAnimation *animation = animations.First(); animation; animation = animations.Next(animation)) {
            if (!animation->Finished())
                continue;
            if (animation->Persistent()) {
                animations.Del(animation, false);
                animationsPersistent.Add(animation);
            } else {
                animations.Del(animation);
            }
            found = true;
            break;
        }
    } while (found);
    animLock.Unlock();
}

/*****************************************************************************************
* Main Loop
*****************************************************************************************/
void cAnimator::Action(void) {
    while(Running()) {
        bool animActive = false;
        uint64_t start = cTimeMs::Now();
        DoTick(animActive);                 if (!Running()) break;
        osd->Flush();                       if (!Running()) break;
        CleanupAnims();                     if (!Running()) break;
        if (!animActive) {
            pauseWait.Wait();
        } else {
            Sleep(start);            
        }
    }
}

/*****************************************************************************************
* Add Animation
* if startAnim is set to true, main loop gets waked up
*****************************************************************************************/
void cAnimator::AddAnimation(cAnimation *animation, bool startAnim) {
    animation->SetInitial();
    animLock.Lock();
    animations.Ins(animation);
    animLock.Unlock();
    if (startAnim)
        pauseWait.Signal();
}

/*****************************************************************************************
* Remove Animation
* animation will be set to finished and removed later by Cleanup()
*****************************************************************************************/
void cAnimator::RemoveAnimation(cAnimation *remove) {
    animLock.Lock();
    for (cAnimation *animation = animations.First(); animation; animation = animations.Next(animation)) {
        if (animation == remove) {
            animation->SetFinished();
            break;
        }
    }
    animLock.Unlock();
}

/*****************************************************************************************
* Finish Main Loop
*****************************************************************************************/
void cAnimator::Stop(void) {
    if (!Running())
        return;
    Cancel(-1);
    pauseWait.Signal();
    sleepWait.Signal();
    Cancel(2);
}

/*****************************************************************************************
* shift or fade out persistent animations
*****************************************************************************************/
void cAnimator::Finish(void) {
    bool animActive = true;
    bool reactivate = true;
    while(animActive) {
        animActive = false;
        uint64_t start = cTimeMs::Now();
        animLock.Lock();
        for (cAnimation *animation = animationsPersistent.First(); animation; animation = animationsPersistent.Next(animation)) {
            if (reactivate)
                animation->Reactivate();
            bool currentAnimActive = animation->Tick();
            animActive = animActive || currentAnimActive;
        }
        animLock.Unlock();
        reactivate = false;
        osd->Flush();
        if (!animActive)
            break;
        Sleep(start);
    }
}
