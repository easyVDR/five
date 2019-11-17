#ifndef __TVGUIDE_TIMER_H
#define __TVGUIDE_TIMER_H

#include <vdr/tools.h>

// --- cMyTime ------------------------------------------------------------- 

class cMyTime {
    private:
        time_t t;
        time_t tStart;
        time_t tEnd;
    public:
        cMyTime(){};
        virtual ~cMyTime(void);
        static cString printTime(time_t displayTime);
        void Now();
        time_t GetNow() { return t; };
        void AddStep(int step);
        bool DelStep(int step);
        void SetTime(time_t newTime);
        time_t Get() {return t;};
        time_t GetStart() {return tStart;};
        time_t GetEnd() {return tEnd;};
        cString GetCurrentTime();
        cString GetDate();
        cString GetWeekday();
        time_t getPrevPrimetime(time_t current);
        time_t getNextPrimetime(time_t current);
        bool tooFarInPast(time_t current);
        int GetTimelineOffset();
        time_t GetRounded();
        bool NowVisible(void);
        void debug();
};

// --- cTimeInterval ------------------------------------------------------------- 

class cTimeInterval {
    private:
        time_t start;
        time_t stop;
    public:
        cTimeInterval(time_t start, time_t stop);
        virtual ~cTimeInterval(void);
        time_t Start(void) { return start; };
        time_t Stop(void) { return stop; };
        cTimeInterval *Intersect(cTimeInterval *interval);
        cTimeInterval *Union(cTimeInterval *interval);
};

#endif //__TVGUIDE_TIMER_H