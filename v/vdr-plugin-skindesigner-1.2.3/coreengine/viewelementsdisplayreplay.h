#ifndef __VIEWELEMENTSDR_H
#define __VIEWELEMENTSDR_H

#include "viewelement.h"
#include "../extensions/scrapmanager.h"

/******************************************************************
* cVeDrRecTitle
******************************************************************/
class cVeDrRecTitle : public cViewElement {
private:
    const cRecording *recording;
    char *title;
public:
    cVeDrRecTitle(void);
    virtual ~cVeDrRecTitle(void);
    void SetTokenContainer(void);
    void Set(const cRecording *recording);
    void Set(const char *title);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrRecInfo
******************************************************************/
class cVeDrRecInfo : public cViewElement {
private:
    const cRecording *recording;
public:
    cVeDrRecInfo(void);
    virtual ~cVeDrRecInfo(void);
    void SetTokenContainer(void);
    void Set(const cRecording *recording);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrCurrentTime
******************************************************************/
class cVeDrCurrentTime : public cViewElement {
private:
    bool changed;
    char *current;
public:
    cVeDrCurrentTime(void);
    virtual ~cVeDrCurrentTime(void);
    void SetTokenContainer(void);
    void Set(const char *current);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrTotalTime
******************************************************************/
class cVeDrTotalTime : public cViewElement {
private:
    bool changed;
    char *total;
    bool timeshiftActive;
    char *timeshiftDuration;
public:
    cVeDrTotalTime(void);
    virtual ~cVeDrTotalTime(void);
    void SetTokenContainer(void);
    void Set(const char *total, bool timeshiftActive, const char *timeshiftDuration);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrEndTime
******************************************************************/
class cVeDrEndTime : public cViewElement {
private:
    cString end;
    bool changed;
public:
    cVeDrEndTime(void);
    virtual ~cVeDrEndTime(void);
    void SetTokenContainer(void);
    void Set(cString end);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrProgressBar
******************************************************************/
class cVeDrProgressBar : public cViewElement {
private:
    int current;
    int total;
    bool timeshiftActive;
    int timeshiftTotal;
    bool changed;
public:
    cVeDrProgressBar(void);
    virtual ~cVeDrProgressBar(void);
    void SetTokenContainer(void);
    void Set(int current, int total, bool timeshiftActive, int timeshiftTotal);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrCutMarks
******************************************************************/
class cVeDrCutMarks : public cViewElement {
private:
    int cutmarksIndex;
    const cMarks *marks;
    int current;
    int total;
    bool timeshiftActive;
    int timeshiftTotal;
    int numMarksLast;
    int *lastMarks;
    int markActive;
    bool MarksChanged(void);
    void RememberMarks(void);
public:
    cVeDrCutMarks(void);
    virtual ~cVeDrCutMarks(void);
    void SetTokenContainer(void);
    void Set(const cMarks *marks, int current, int total, bool timeshiftActive, int timeshiftTotal);
    void Reset(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrControlIcons
******************************************************************/
class cVeDrControlIcons : public cViewElement {
private:
    bool play;
    bool forward;
    int speed;
    bool changed;
public:
    cVeDrControlIcons(void);
    virtual ~cVeDrControlIcons(void);
    void SetTokenContainer(void);
    void Set(bool play, bool forward, int speed);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrProgressModeonly
******************************************************************/
class cVeDrProgressModeonly : public cViewElement {
private:
    double fps;
    int current;
    int total;
    bool changed;
public:
    cVeDrProgressModeonly(void);
    virtual ~cVeDrProgressModeonly(void);
    void SetTokenContainer(void);
    void Set(double fps, int current, int total);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrJump
******************************************************************/
class cVeDrJump : public cViewElement {
private:
    char *jump;
    bool changed;
public:
    cVeDrJump(void);
    virtual ~cVeDrJump(void);
    void SetTokenContainer(void);
    void Set(const char *jump);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDrOnPause
******************************************************************/
class cVeDrOnPause : public cViewElement, public cScrapManager {
private:
    bool started;
    int actorsIndex;
    char *recfilename;
public:
    cVeDrOnPause(void);
    virtual ~cVeDrOnPause(void);
    void Close(void);
    int Delay(void) { return attribs->Delay() * 1000; };
    void SetTokenContainer(void);
    void Set(const char *recfilename);
    bool Parse(bool forced = false);
    bool Started(void) { return started; };
    void ResetSleep(void);
};

/******************************************************************
* cVeDrScraperContent
******************************************************************/
class cVeDrScraperContent : public cViewElement, public cScrapManager {
private:
    const cRecording *recording;
public:
    cVeDrScraperContent(void);
    virtual ~cVeDrScraperContent(void);
    void SetTokenContainer(void);
    void Set(const cRecording *recording);
    bool Parse(bool forced = false);
};

#endif //__VIEWELEMENTSDR_H