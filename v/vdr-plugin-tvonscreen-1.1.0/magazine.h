/*
 * magazine.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef MAGAZINE_H
#define MAGAZINE_H

#include <vdr/plugin.h>
#include <time.h>
#include <vdr/device.h>
#include "gfxtools.h"
#include "config.h"
#include "timer.h"
#include "search.h"

void mzlog(int level, const char *fmt, ...);

typedef enum
{
    NAME1_AREA = 0,
    NAME2_AREA,
    NAME3_AREA,
    DATETIME_AREA,
    TIMELINE_AREA,
    SCHED1_AREA,
    SCHED2_AREA,
    SCHED3_AREA,
    CONTROL_AREA,
    NUMBER_OF_AREAS
} tMagazineArea;

class magazine : public cOsdObject
{
private:
    cPlugin *parent;
    cOsd *osd;

    class tvOcMenuEvent *me;
    class cMenuEditTimer *met;
    class cSearchMenu *mes;

    anyFont *f1,*f2,*f3,*f4,*f5;

    cSchedulesLock _schedulesLock;
    const cSchedule** schedArray;
    int schedArrayNum;
    int currentFirst;

    int evnum;
    class cEvent **ev1;
    class cEvent **ev2;
    class cEvent **ev3;
    int *fullHours;

    enum modes {SHOW,EDIT};
    enum modes curmode;

    unsigned short EDIT_curEvent;
    int EDIT_curChannel;
    int EDIT_curEVI;

    bool timeline_tested;
    bool timeline_found_conflict;

    int YSTART;
    int TimelineWidth;
    int ScheduleWidth;
    int ScheduleHeight;
    int Width;
    int Height;
    double Aspect;

    cEvent **ev4ch(int);
    void searchcEvt();

    void printLogo(const cSchedule *s,int p);
    void printHead(const cSchedule *s,int p);
    void showKeys(void);
    void showDatetime(void);
    void showTimeline(void);
    void showHeads(bool onlyBG=false);
    void showScheds(void);
    void showSched(cEvent **ev, tMagazineArea area);
    void calcSched(const cSchedule *s,cEvent **ev);
    void calcScheds(void);

    void colorworkaround(cBitmap *b);
    void output(void);
    void outputLR(void);

    void gotoUsertime(int u);
    void showHelp(void);

    void autoTimer(const class cEvent *EventInfo);

    void setTransparency(unsigned int *color);
    unsigned int clrSched1;
    unsigned int clrSched2;
    unsigned int clrTimeline1;
    unsigned int clrTimeline2;
    unsigned int clrNames;
    unsigned int clrText;

    const cEvent *GetEventAfter(const cSchedule *s, time_t StartTime, time_t EndTime) const;

    time_t currentFirstTime;
    void setcurrentFirstTime(time_t now);

public:
    static const class cEvent *getNext(const cSchedule *s,const cEvent *e);
    static const class cEvent *getPrev(const cSchedule *s,const cEvent *e);

    magazine(class cPlugin *);
    virtual ~magazine();
    virtual void Show(void);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif
