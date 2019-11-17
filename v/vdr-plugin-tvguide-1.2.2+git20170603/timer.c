#include <time.h>
#include <vdr/tools.h>
#include "config.h"
#include "timer.h"

cMyTime::~cMyTime(void) {
}

cString cMyTime::printTime(time_t displayTime) {
    struct tm *ts;
    ts = localtime(&displayTime);
    cString strTime = cString::sprintf("%d.%d-%d:%d.%d", ts->tm_mday, ts->tm_mon+1, ts->tm_hour, ts->tm_min, ts->tm_sec);
    return strTime;
}


void cMyTime::Now() {   
    t = time(0);
    tStart = t;
    tStart = GetRounded();
    if (tvguideConfig.displayMode == eVertical) {
        tEnd = tStart + (geoManager.osdHeight - geoManager.statusHeaderHeight - geoManager.channelHeaderHeight - geoManager.channelGroupsHeight - geoManager.footerHeight)/geoManager.minutePixel*60;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        tEnd = tStart + (geoManager.osdWidth - geoManager.channelHeaderWidth - geoManager.channelGroupsWidth)/geoManager.minutePixel*60;
    }
}

void cMyTime::AddStep(int step) {
    tStart += step*60;
    tEnd  += step*60;
}

bool cMyTime::DelStep(int step) {
    if ((tStart - step*60)+30*60 < t) {
        return true;
    }
    tStart -= step*60;
    tEnd -= step*60;
    return false;
}

void cMyTime::SetTime(time_t newTime) {
    tStart = newTime;
    if (tvguideConfig.displayMode == eVertical) {
        tEnd = tStart + (geoManager.osdHeight - geoManager.statusHeaderHeight - geoManager.channelHeaderHeight - geoManager.channelGroupsHeight - geoManager.footerHeight)/geoManager.minutePixel*60;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        tEnd = tStart + (geoManager.osdWidth - geoManager.channelHeaderWidth - geoManager.channelGroupsWidth)/geoManager.minutePixel*60;
    }
}

time_t cMyTime::getPrevPrimetime(time_t current) {
    tm *st = localtime(&current);
    if (st->tm_hour < 21) {
        current -= 24 * 60* 60;
        st = localtime(&current);
    }
    st->tm_hour = 20;
    st->tm_min = 0;
    time_t primeTime = mktime(st);
    return primeTime;
}

time_t cMyTime::getNextPrimetime(time_t current){
    tm *st = localtime(&current);
    if (st->tm_hour > 19) {
        current += 24 * 60* 60;
        st = localtime(&current);
    }
    st->tm_hour = 20;
    st->tm_min = 0;
    time_t primeTime = mktime(st);
    return primeTime;
}

bool cMyTime::tooFarInPast(time_t current) {
    if (current < t) {
        return true;
    }
    return false;
}

cString cMyTime::GetCurrentTime() {
    char buf[25];
    t = time(0);
    tm *st = localtime(&t);
    //snprintf(text, sizeof(text), "%d:%02d", st->tm_hour, st->tm_min);
    if (tvguideConfig.timeFormat == e12Hours) {
        strftime(buf, sizeof(buf), "%I:%M %p", st);
    } else if (tvguideConfig.timeFormat == e24Hours)
        strftime(buf, sizeof(buf), "%H:%M", st);
    return buf;
    
}

cString cMyTime::GetDate() {
    char text[6];
    tm *st = localtime(&tStart);
    snprintf(text, sizeof(text), "%d.%d", st->tm_mday, st->tm_mon+1);
    return text;
}

cString cMyTime::GetWeekday() {
    return WeekDayName(tStart);
}

int cMyTime::GetTimelineOffset() {
    tm *st = localtime(&tStart);
    int offset = st->tm_hour*60;
    offset += st->tm_min;
    return offset;
}

time_t cMyTime::GetRounded() {
    tm *rounded = localtime ( &tStart );
    rounded->tm_sec = 0;
    if (rounded->tm_min > 29)
        rounded->tm_min = 30;
    else
        rounded->tm_min = 0;
    return mktime(rounded);
}

bool cMyTime::NowVisible(void) {
    if (t > tStart)
        return true;
    return false;
}


void cMyTime::debug() {
    esyslog("t: %s, tStart: %s, tEnd: %s", *TimeString(t), *TimeString(tStart), *TimeString(tEnd));
}

// --- cTimeInterval ------------------------------------------------------------- 

cTimeInterval::cTimeInterval(time_t start, time_t stop) {
    this->start = start;
    this->stop = stop;
}

cTimeInterval::~cTimeInterval(void) {
}

cTimeInterval *cTimeInterval::Intersect(cTimeInterval *interval) {
    time_t startIntersect, stopIntersect;
    
    if ((stop <= interval->Start()) || (interval->Stop() <= start)) {
        return NULL;
    }
    
    if (start <= interval->Start()) {
        startIntersect = interval->Start();
    } else {
        startIntersect = start;
    }
    if (stop <= interval->Stop()) {
        stopIntersect = stop;
    } else {
        stopIntersect = interval->Stop();
    }
    return new cTimeInterval(startIntersect, stopIntersect);
}

cTimeInterval *cTimeInterval::Union(cTimeInterval *interval) {
    time_t startUnion, stopUnion;
    
    if (start <= interval->Start()) {
        startUnion = start;
    } else {
        startUnion = interval->Start();
    }
    if (stop <= interval->Stop()) {
        stopUnion = interval->Stop();
    } else {
        stopUnion = stop;
    }
    return new cTimeInterval(startUnion, stopUnion);
}