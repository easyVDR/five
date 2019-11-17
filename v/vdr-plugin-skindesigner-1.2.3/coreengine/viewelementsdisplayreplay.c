#include "viewelementsdisplayreplay.h"
#include "../config.h"
#include "../extensions/helpers.h"
#include "../services/scraper2vdr.h"

/******************************************************************
* cVeDrRecTitle
******************************************************************/
cVeDrRecTitle::cVeDrRecTitle(void) {
    recording = NULL;
    title = NULL;
}

cVeDrRecTitle::~cVeDrRecTitle(void) {
    free(title);
}

void cVeDrRecTitle::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{rectitle}", (int)eDRRecTitleST::rectitle);
    tokenContainer->DefineStringToken("{recsubtitle}", (int)eDRRecTitleST::recsubtitle);
    tokenContainer->DefineStringToken("{recdate}", (int)eDRRecTitleST::recdate);
    tokenContainer->DefineStringToken("{rectime}", (int)eDRRecTitleST::rectime);
    InheritTokenContainer();
}

void cVeDrRecTitle::Set(const cRecording *recording) {
    this->recording = recording;
}

void cVeDrRecTitle::Set(const char *title) {
    if (!title)
        return;
    free(this->title);
    this->title = strdup(title);
}

bool cVeDrRecTitle::Parse(bool force) {
    if (!cViewElement::Parse(force))
        return false;
    if (!recording && !title)
        return false;

    tokenContainer->Clear();
    if (recording) {
        const char *recName = NULL;
        const cRecordingInfo *recInfo = recording->Info();
        if (recInfo)
            recName = recInfo->Title();
        if (!recName)
            recName = recording->Name();
        tokenContainer->AddStringToken((int)eDRRecTitleST::rectitle, recName);
        tokenContainer->AddStringToken((int)eDRRecTitleST::recsubtitle, recInfo ? recInfo->ShortText() : "");
        tokenContainer->AddStringToken((int)eDRRecTitleST::recdate, *ShortDateString(recording->Start()));
        tokenContainer->AddStringToken((int)eDRRecTitleST::rectime, *TimeString(recording->Start()));        
    } else if (title) {
        tokenContainer->AddStringToken((int)eDRRecTitleST::rectitle, title);        
    }

    SetDirty();
    return true;
}

/******************************************************************
* cVeDrRecInfo
******************************************************************/
cVeDrRecInfo::cVeDrRecInfo(void) {
    recording = NULL;
}

cVeDrRecInfo::~cVeDrRecInfo(void) {
}

void cVeDrRecInfo::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{resolution}", (int)eDRRecInfoST::resolution);
    tokenContainer->DefineStringToken("{aspect}", (int)eDRRecInfoST::aspect);
    tokenContainer->DefineIntToken("{screenwidth}", (int)eDRRecInfoIT::screenwidth);
    tokenContainer->DefineIntToken("{screenheight}", (int)eDRRecInfoIT::screenheight);
    tokenContainer->DefineIntToken("{isHD}", (int)eDRRecInfoIT::isHD);
    tokenContainer->DefineIntToken("{isWideScreen}", (int)eDRRecInfoIT::isWideScreen);
    InheritTokenContainer();
}

void cVeDrRecInfo::Set(const cRecording *recording) {
    this->recording = recording;
}

bool cVeDrRecInfo::Parse(bool force) {
    if (!cViewElement::Parse(force))
        return false;
    if (!recording)
        return false;
    
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;
    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
    bool isHD = false;
    string resName = GetScreenResolutionString(screenWidth, screenHeight, &isHD);
    bool isWideScreen = false;
    string aspectName = GetScreenAspectString(aspect, &isWideScreen);

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDRRecInfoIT::screenwidth, screenWidth);
    tokenContainer->AddIntToken((int)eDRRecInfoIT::screenheight, screenHeight);
    tokenContainer->AddIntToken((int)eDRRecInfoIT::isHD, isHD);
    tokenContainer->AddIntToken((int)eDRRecInfoIT::isWideScreen, isWideScreen);
    tokenContainer->AddStringToken((int)eDRRecInfoST::resolution, resName.c_str());
    tokenContainer->AddStringToken((int)eDRRecInfoST::aspect, aspectName.c_str());

    SetDirty();
    return true;
}

/******************************************************************
* cVeDrCurrentTime
******************************************************************/
cVeDrCurrentTime::cVeDrCurrentTime(void) {
    changed = true;
    current = NULL;
}

cVeDrCurrentTime::~cVeDrCurrentTime(void) {
    free(current);
}

void cVeDrCurrentTime::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{reccurrent}", (int)eDRCurrentTimeST::reccurrent);
    InheritTokenContainer();
}

void cVeDrCurrentTime::Set(const char *current) {
    if (!current)
        return;
    free(this->current);
    this->current = strdup(current);
    changed = true;
}

bool cVeDrCurrentTime::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDRCurrentTimeST::reccurrent, current);
    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDrTotalTime
******************************************************************/
cVeDrTotalTime::cVeDrTotalTime(void) {
    changed = true;
    total = NULL;
    timeshiftActive = false;
    timeshiftDuration = NULL;
}

cVeDrTotalTime::~cVeDrTotalTime(void) {
    free(total);
    free(timeshiftDuration);
}

void cVeDrTotalTime::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{rectotal}", (int)eDRTotalTimeST::rectotal);
    tokenContainer->DefineStringToken("{timeshifttotal}", (int)eDRTotalTimeST::timeshifttotal);
    tokenContainer->DefineIntToken("{timeshift}", (int)eDRTotalTimeIT::timeshift);
    InheritTokenContainer();
}

void cVeDrTotalTime::Set(const char *total, bool timeshiftActive, const char *timeshiftDuration) {
    if (!total)
        return;
    free(this->total);
    this->total = strdup(total);
    this->timeshiftActive = timeshiftActive;
    free(this->timeshiftDuration);
    this->timeshiftDuration = NULL;
    if (timeshiftDuration)
        this->timeshiftDuration = strdup(timeshiftDuration);
    changed = true;
}

bool cVeDrTotalTime::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDRTotalTimeST::rectotal, total);
    tokenContainer->AddStringToken((int)eDRTotalTimeST::timeshifttotal, timeshiftDuration);
    tokenContainer->AddIntToken((int)eDRTotalTimeIT::timeshift, timeshiftActive);
    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDrEndTime
******************************************************************/
cVeDrEndTime::cVeDrEndTime(void) {
    end = "";
}

cVeDrEndTime::~cVeDrEndTime(void) {
}

void cVeDrEndTime::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{recend}", (int)eDREndTimeST::recend);
    InheritTokenContainer();
}

void cVeDrEndTime::Set(cString end) {
    if (strcmp(*this->end, *end)) {
        this->end = end;
        changed = true;
    }
}

bool cVeDrEndTime::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDREndTimeST::recend, *end);
    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDrProgressBar
******************************************************************/
cVeDrProgressBar::cVeDrProgressBar(void) {
    current = -1;
    total = -1;
    timeshiftActive = false;
    timeshiftTotal = -1;
    changed = true;
}

cVeDrProgressBar::~cVeDrProgressBar(void) {
}

void cVeDrProgressBar::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{current}", (int)eDRProgressbarIT::current);
    tokenContainer->DefineIntToken("{total}", (int)eDRProgressbarIT::total);
    tokenContainer->DefineIntToken("{timeshift}", (int)eDRProgressbarIT::timeshift);
    tokenContainer->DefineIntToken("{timeshifttotal}", (int)eDRProgressbarIT::timeshifttotal);
    InheritTokenContainer();
}

void cVeDrProgressBar::Set(int current, int total, bool timeshiftActive, int timeshiftTotal) {
    if (this->current == current)
        return;
    this->current = current;
    this->total = total;
    this->timeshiftActive = timeshiftActive;
    this->timeshiftTotal = timeshiftTotal;
    changed = true;
}

bool cVeDrProgressBar::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDRProgressbarIT::current, current);
    tokenContainer->AddIntToken((int)eDRProgressbarIT::total, total);
    tokenContainer->AddIntToken((int)eDRProgressbarIT::timeshift, timeshiftActive);
    tokenContainer->AddIntToken((int)eDRProgressbarIT::timeshifttotal, timeshiftTotal);

    SetDirty();
    return true;
}

/******************************************************************
* cVeDrCutMarks
******************************************************************/
cVeDrCutMarks::cVeDrCutMarks(void) {
    cutmarksIndex = -1;
    lastMarks = NULL;
    Reset();
}

cVeDrCutMarks::~cVeDrCutMarks(void) {
    delete[] lastMarks;
}

void cVeDrCutMarks::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{timeshift}", (int)eDRCutmarksIT::timeshift);
    tokenContainer->DefineLoopToken("{marks[position]}", (int)eDRCutmarksLT::position);
    tokenContainer->DefineLoopToken("{marks[endposition]}", (int)eDRCutmarksLT::endposition);
    tokenContainer->DefineLoopToken("{marks[total]}", (int)eDRCutmarksLT::total);
    tokenContainer->DefineLoopToken("{marks[timeshifttotal]}", (int)eDRCutmarksLT::timeshifttotal);
    tokenContainer->DefineLoopToken("{marks[active]}", (int)eDRCutmarksLT::active);
    tokenContainer->DefineLoopToken("{marks[startmark]}", (int)eDRCutmarksLT::startmark);
    cutmarksIndex = tokenContainer->LoopIndex("marks");
    InheritTokenContainer();
}

void cVeDrCutMarks::Set(const cMarks *marks, int current, int total, bool timeshiftActive, int timeshiftTotal) {
    this->marks = marks;
    this->current = current;
    this->total = total;
    this->timeshiftActive = timeshiftActive;
    this->timeshiftTotal = timeshiftTotal;
}

void cVeDrCutMarks::Reset(void) {
    marks = NULL;
    current = -1;
    total = -1;
    numMarksLast = 0;
    delete[] lastMarks;
    lastMarks = NULL;
    markActive = -1;
    timeshiftActive = false;
    timeshiftTotal = -1;
}

bool cVeDrCutMarks::Parse(bool force) {
    if (!cViewElement::Parse(force))
        return false;
    if (!marks || !MarksChanged()) {
        return false;
    }
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDRCutmarksIT::timeshift, timeshiftActive);
    int numMarks = marks->Count();
    vector<int> cutmarksInfo;
    cutmarksInfo.push_back(numMarks);
    tokenContainer->CreateLoopTokenContainer(&cutmarksInfo);
    cString tot = cString::sprintf("%d", total);
    cString timeshifttot = cString::sprintf("%d",timeshiftTotal);
    int i = 0;
    bool isStartMark = true;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::position, *cString::sprintf("%d", m->Position()));
        tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::total, *tot);
        if (timeshiftActive) {
            tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::timeshifttotal, *timeshifttot);
        }
        tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::startmark, isStartMark ? "1" : "0");
        tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::active, (m->Position() == current) ? "1" : "0");
        const cMark *m2 = marks->Next(m);
        if (m2) { 
            tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::endposition, *cString::sprintf("%d", m2->Position()));
        } else {
            tokenContainer->AddLoopToken(cutmarksIndex, i, (int)eDRCutmarksLT::endposition, *tot);
        }
        i++;
        isStartMark = !isStartMark;
    }
    SetDirty();
    return true;
}

bool cVeDrCutMarks::MarksChanged(void) {
    bool redraw = false;
    //if mark was active, we redraw always
    if (markActive >= 0) {
        markActive = -1;
        redraw = true;
    }
    //check if current position in recording hits mark exactly
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        if (m->Position() == current) {
            markActive = current;
            redraw = true;
            break;
        }
    }
    if (redraw)
        return true;
    //if number of marks has changed, redraw
    int numMarks = marks->Count();
    if (numMarks != numMarksLast) {
        RememberMarks();
        return true;
    }
    if (!lastMarks)
        return false;
    //if position has changed, redraw
    int i=0;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        if (m->Position() != lastMarks[i]) {
            RememberMarks();
            return true;
        }
        i++;
    }
    return false;
}

void cVeDrCutMarks::RememberMarks(void) {
    if (!marks)
        return;
    numMarksLast = marks->Count();
    if (numMarksLast < 1)
        return;
    delete[] lastMarks;
    lastMarks = new int[numMarksLast];
    int i=0;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        lastMarks[i] = m->Position();
        i++;
    }
}

/******************************************************************
* cVeDrControlIcons
******************************************************************/
cVeDrControlIcons::cVeDrControlIcons(void) {
    play = false;
    forward = false;
    speed = 0;
    changed = true;
}

cVeDrControlIcons::~cVeDrControlIcons(void) {
}

void cVeDrControlIcons::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{play}", (int)eDRControlIconsIT::play);
    tokenContainer->DefineIntToken("{pause}", (int)eDRControlIconsIT::pause);
    tokenContainer->DefineIntToken("{forward}", (int)eDRControlIconsIT::forward);
    tokenContainer->DefineIntToken("{slowforward}", (int)eDRControlIconsIT::slowforward);
    tokenContainer->DefineIntToken("{forward1x}", (int)eDRControlIconsIT::forward1x);
    tokenContainer->DefineIntToken("{forward2x}", (int)eDRControlIconsIT::forward2x);
    tokenContainer->DefineIntToken("{forward3x}", (int)eDRControlIconsIT::forward3x);
    tokenContainer->DefineIntToken("{slowforward1x}", (int)eDRControlIconsIT::slowforward1x);
    tokenContainer->DefineIntToken("{slowforward2x}", (int)eDRControlIconsIT::slowforward2x);
    tokenContainer->DefineIntToken("{slowforward3x}", (int)eDRControlIconsIT::slowforward3x);
    tokenContainer->DefineIntToken("{rewind}", (int)eDRControlIconsIT::rewind);
    tokenContainer->DefineIntToken("{slowrewind}", (int)eDRControlIconsIT::slowrewind);
    tokenContainer->DefineIntToken("{rewind1x}", (int)eDRControlIconsIT::rewind1x);
    tokenContainer->DefineIntToken("{rewind2x}", (int)eDRControlIconsIT::rewind2x);
    tokenContainer->DefineIntToken("{rewind3x}", (int)eDRControlIconsIT::rewind3x);
    tokenContainer->DefineIntToken("{slowrewind1x}", (int)eDRControlIconsIT::slowrewind1x);
    tokenContainer->DefineIntToken("{slowrewind2x}", (int)eDRControlIconsIT::slowrewind2x);
    tokenContainer->DefineIntToken("{slowrewind3x}", (int)eDRControlIconsIT::slowrewind3x);
    InheritTokenContainer();
}

void cVeDrControlIcons::Set(bool play, bool forward, int speed) {
    this->play = play;
    this->forward = forward;
    this->speed = speed;
    changed = true;
}

bool cVeDrControlIcons::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;

    bool isPlay = false;
    bool isPause = false;
    bool isFF = false;
    bool isSlowFF = false;
    bool isFF1x = false;
    bool isFF2x = false;
    bool isFF3x = false;
    bool isSlowFF1x = false;
    bool isSlowFF2x = false;
    bool isSlowFF3x = false;
    bool isRew = false;
    bool isSlowRew = false;
    bool isRew1x = false;
    bool isRew2x = false;
    bool isRew3x = false;
    bool isSlowRew1x = false;
    bool isSlowRew2x = false;
    bool isSlowRew3x = false;

    if (speed == -1) {
        if (play) {
            isPlay = true;
        } else {
            isPause = true;
        }
    } else if (forward) {
        if (speed == 1) {
            if (play)
                isFF1x = true;
            else
                isSlowFF1x = true;
        } else if (speed == 2) {
            if (play)
                isFF2x = true;
            else
                isSlowFF2x = true;
        } else if (speed == 3) {
            if (play)
                isFF3x = true;
            else
                isSlowFF3x = true;
        } else {
            if (play)
                isFF = true;
            else
                isSlowFF = true;
        }
    } else {
        if (speed == 1) {
            if (play)
                isRew1x = true;
            else
                isSlowRew1x = true;
        } else if (speed == 2) {
            if (play)
                isRew2x = true;
            else
                isSlowRew2x = true;
        } else if (speed == 3) {
            if (play)
                isRew3x = true;
            else
                isSlowRew3x = true;
        } else {
            if (play)
                isRew = true;
            else
                isSlowRew = true;
        }
    }
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDRControlIconsIT::play, isPlay);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::pause, isPause);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::forward, isFF);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowforward, isSlowFF);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::forward1x, isFF1x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::forward2x, isFF2x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::forward3x, isFF3x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowforward1x, isSlowFF1x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowforward2x, isSlowFF2x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowforward3x, isSlowFF3x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::rewind, isRew);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowrewind, isSlowRew);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::rewind1x, isRew1x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::rewind2x, isRew2x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::rewind3x, isRew3x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowrewind1x, isSlowRew1x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowrewind2x, isSlowRew2x);
    tokenContainer->AddIntToken((int)eDRControlIconsIT::slowrewind3x, isSlowRew3x);
    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDrProgressModeonly
******************************************************************/
cVeDrProgressModeonly::cVeDrProgressModeonly(void) {
    fps = 0.0f;
    current = 0;
    total = 0;
    changed = true;
}

cVeDrProgressModeonly::~cVeDrProgressModeonly(void) {
}

void cVeDrProgressModeonly::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{current}", (int)eDRProgressModeonlyIT::current);
    tokenContainer->DefineIntToken("{total}", (int)eDRProgressModeonlyIT::total);
    tokenContainer->DefineStringToken("{timecurrent}", (int)eDRProgressModeonlyST::timecurrent);
    tokenContainer->DefineStringToken("{timetotal}", (int)eDRProgressModeonlyST::timetotal);
    InheritTokenContainer();
}

void cVeDrProgressModeonly::Set(double fps, int current, int total) {
    if (this->current == current)
        return;
    this->fps = fps;
    this->current = current;
    this->total = total;    
    changed = true;
}

bool cVeDrProgressModeonly::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;

    string cur = GetTimeString((double)current / fps);
    string tot = GetTimeString((double)total / fps);

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDRProgressModeonlyIT::current, current);
    tokenContainer->AddIntToken((int)eDRProgressModeonlyIT::total, total);
    tokenContainer->AddStringToken((int)eDRProgressModeonlyST::timecurrent, cur.c_str());
    tokenContainer->AddStringToken((int)eDRProgressModeonlyST::timetotal, tot.c_str());

    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDrJump
******************************************************************/
cVeDrJump::cVeDrJump(void) {
    jump = NULL;
    changed = false;
}

cVeDrJump::~cVeDrJump(void) {
    free(jump);
}

void cVeDrJump::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{jump}", (int)eDRJumpST::jump);
    InheritTokenContainer();
}

void cVeDrJump::Set(const char *jump) {
    if (!jump)
        return;
    free(this->jump);
    this->jump = strdup(jump);
    changed = true;
}

bool cVeDrJump::Parse(bool force) {
    if (!cViewElement::Parse(force) || !changed)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDRJumpST::jump, jump);
    changed = false;
    SetDirty();
    return true;
}

/******************************************************************
* cVeDrOnPause
******************************************************************/
cVeDrOnPause::cVeDrOnPause(void) {
    started = false;
    actorsIndex = -1;
    recfilename = NULL;
}

cVeDrOnPause::~cVeDrOnPause(void) {
    free(recfilename);
}

void cVeDrOnPause::Close(void) {
    started = false;
    cViewElement::Close();
}

void cVeDrOnPause::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eDROnpauseST::name);
    tokenContainer->DefineStringToken("{shorttext}", (int)eDROnpauseST::shorttext);
    tokenContainer->DefineStringToken("{description}", (int)eDROnpauseST::description);
    tokenContainer->DefineStringToken("{date}", (int)eDROnpauseST::date);
    tokenContainer->DefineStringToken("{time}", (int)eDROnpauseST::time);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eDROnpauseST::durationminutes);
    tokenContainer->DefineStringToken("{durationeventminutes}", (int)eDROnpauseST::durationeventminutes);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eDROnpauseIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eDROnpauseIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eDROnpauseIT::year);
    tokenContainer->DefineIntToken("{duration}", (int)eDROnpauseIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eDROnpauseIT::durationhours);
    tokenContainer->DefineIntToken("{durationevent}", (int)eDROnpauseIT::durationevent);
    tokenContainer->DefineIntToken("{durationeventhours}", (int)eDROnpauseIT::durationeventhours);
    tokenContainer->DefineStringToken("{movietitle}", (int)eScraperST::movietitle);
    tokenContainer->DefineStringToken("{movieoriginalTitle}", (int)eScraperST::movieoriginalTitle);
    tokenContainer->DefineStringToken("{movietagline}", (int)eScraperST::movietagline);
    tokenContainer->DefineStringToken("{movieoverview}", (int)eScraperST::movieoverview);
    tokenContainer->DefineStringToken("{moviegenres}", (int)eScraperST::moviegenres);
    tokenContainer->DefineStringToken("{moviehomepage}", (int)eScraperST::moviehomepage);
    tokenContainer->DefineStringToken("{moviereleasedate}", (int)eScraperST::moviereleasedate);
    tokenContainer->DefineStringToken("{moviepopularity}", (int)eScraperST::moviepopularity);
    tokenContainer->DefineStringToken("{movievoteaverage}", (int)eScraperST::movievoteaverage);
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperST::posterpath);
    tokenContainer->DefineStringToken("{fanartpath}", (int)eScraperST::fanartpath);
    tokenContainer->DefineStringToken("{moviecollectionName}", (int)eScraperST::moviecollectionName);
    tokenContainer->DefineStringToken("{collectionposterpath}", (int)eScraperST::collectionposterpath);
    tokenContainer->DefineStringToken("{collectionfanartpath}", (int)eScraperST::collectionfanartpath);
    tokenContainer->DefineStringToken("{seriesname}", (int)eScraperST::seriesname);
    tokenContainer->DefineStringToken("{seriesoverview}", (int)eScraperST::seriesoverview);
    tokenContainer->DefineStringToken("{seriesfirstaired}", (int)eScraperST::seriesfirstaired);
    tokenContainer->DefineStringToken("{seriesnetwork}", (int)eScraperST::seriesnetwork);
    tokenContainer->DefineStringToken("{seriesgenre}", (int)eScraperST::seriesgenre);
    tokenContainer->DefineStringToken("{seriesrating}", (int)eScraperST::seriesrating);
    tokenContainer->DefineStringToken("{seriesstatus}", (int)eScraperST::seriesstatus);
    tokenContainer->DefineStringToken("{episodetitle}", (int)eScraperST::episodetitle);
    tokenContainer->DefineStringToken("{episodefirstaired}", (int)eScraperST::episodefirstaired);
    tokenContainer->DefineStringToken("{episodegueststars}", (int)eScraperST::episodegueststars);
    tokenContainer->DefineStringToken("{episodeoverview}", (int)eScraperST::episodeoverview);
    tokenContainer->DefineStringToken("{episoderating}", (int)eScraperST::episoderating);
    tokenContainer->DefineStringToken("{episodeimagepath}", (int)eScraperST::episodeimagepath);
    tokenContainer->DefineStringToken("{seasonposterpath}", (int)eScraperST::seasonposterpath);
    tokenContainer->DefineStringToken("{seriesposter1path}", (int)eScraperST::seriesposter1path);
    tokenContainer->DefineStringToken("{seriesposter2path}", (int)eScraperST::seriesposter2path);
    tokenContainer->DefineStringToken("{seriesposter3path}", (int)eScraperST::seriesposter3path);
    tokenContainer->DefineStringToken("{seriesfanart1path}", (int)eScraperST::seriesfanart1path);
    tokenContainer->DefineStringToken("{seriesfanart2path}", (int)eScraperST::seriesfanart2path);
    tokenContainer->DefineStringToken("{seriesfanart3path}", (int)eScraperST::seriesfanart3path);
    tokenContainer->DefineStringToken("{seriesbanner1path}", (int)eScraperST::seriesbanner1path);
    tokenContainer->DefineStringToken("{seriesbanner2path}", (int)eScraperST::seriesbanner2path);
    tokenContainer->DefineStringToken("{seriesbanner3path}", (int)eScraperST::seriesbanner3path);
    tokenContainer->DefineIntToken("{ismovie}", (int)eScraperIT::ismovie);
    tokenContainer->DefineIntToken("{moviebudget}", (int)eScraperIT::moviebudget);
    tokenContainer->DefineIntToken("{movierevenue}", (int)eScraperIT::movierevenue);
    tokenContainer->DefineIntToken("{movieadult}", (int)eScraperIT::movieadult);
    tokenContainer->DefineIntToken("{movieruntime}", (int)eScraperIT::movieruntime);
    tokenContainer->DefineIntToken("{isseries}", (int)eScraperIT::isseries);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperIT::posterheight);
    tokenContainer->DefineIntToken("{fanartwidth}", (int)eScraperIT::fanartwidth);
    tokenContainer->DefineIntToken("{fanartheight}", (int)eScraperIT::fanartheight);
    tokenContainer->DefineIntToken("{movieiscollection}", (int)eScraperIT::movieiscollection);
    tokenContainer->DefineIntToken("{collectionposterwidth}", (int)eScraperIT::collectionposterwidth);
    tokenContainer->DefineIntToken("{collectionposterheight}", (int)eScraperIT::collectionposterheight);
    tokenContainer->DefineIntToken("{collectionfanartwidth}", (int)eScraperIT::collectionfanartwidth);
    tokenContainer->DefineIntToken("{collectionfanartheight}", (int)eScraperIT::collectionfanartheight);
    tokenContainer->DefineIntToken("{epgpicavailable}", (int)eScraperIT::epgpicavailable);
    tokenContainer->DefineIntToken("{episodenumber}", (int)eScraperIT::episodenumber);
    tokenContainer->DefineIntToken("{episodeseason}", (int)eScraperIT::episodeseason);
    tokenContainer->DefineIntToken("{episodeimagewidth}", (int)eScraperIT::episodeimagewidth);
    tokenContainer->DefineIntToken("{episodeimageheight}", (int)eScraperIT::episodeimageheight);
    tokenContainer->DefineIntToken("{seasonposterwidth}", (int)eScraperIT::seasonposterwidth);
    tokenContainer->DefineIntToken("{seasonposterheight}", (int)eScraperIT::seasonposterheight);
    tokenContainer->DefineIntToken("{seriesposter1width}", (int)eScraperIT::seriesposter1width);
    tokenContainer->DefineIntToken("{seriesposter1height}", (int)eScraperIT::seriesposter1height);
    tokenContainer->DefineIntToken("{seriesposter2width}", (int)eScraperIT::seriesposter2width);
    tokenContainer->DefineIntToken("{seriesposter2height}", (int)eScraperIT::seriesposter2height);
    tokenContainer->DefineIntToken("{seriesposter3width}", (int)eScraperIT::seriesposter3width);
    tokenContainer->DefineIntToken("{seriesposter3height}", (int)eScraperIT::seriesposter3height);
    tokenContainer->DefineIntToken("{seriesfanart1width}", (int)eScraperIT::seriesfanart1width);
    tokenContainer->DefineIntToken("{seriesfanart1height}", (int)eScraperIT::seriesfanart1height);
    tokenContainer->DefineIntToken("{seriesfanart2width}", (int)eScraperIT::seriesfanart2width);
    tokenContainer->DefineIntToken("{seriesfanart2height}", (int)eScraperIT::seriesfanart2height);
    tokenContainer->DefineIntToken("{seriesfanart3width}", (int)eScraperIT::seriesfanart3width);
    tokenContainer->DefineIntToken("{seriesfanart3height}", (int)eScraperIT::seriesfanart3height);
    tokenContainer->DefineIntToken("{seriesbanner1width}", (int)eScraperIT::seriesbanner1width);
    tokenContainer->DefineIntToken("{seriesbanner1height}", (int)eScraperIT::seriesbanner1height);
    tokenContainer->DefineIntToken("{seriesbanner2width}", (int)eScraperIT::seriesbanner2width);
    tokenContainer->DefineIntToken("{seriesbanner2height}", (int)eScraperIT::seriesbanner2height);
    tokenContainer->DefineIntToken("{seriesbanner3width}", (int)eScraperIT::seriesbanner3width);
    tokenContainer->DefineIntToken("{seriesbanner3height}", (int)eScraperIT::seriesbanner3height);
    tokenContainer->DefineLoopToken("{actors[name]}", (int)eScraperLT::name);
    tokenContainer->DefineLoopToken("{actors[role]}", (int)eScraperLT::role);
    tokenContainer->DefineLoopToken("{actors[thumb]}", (int)eScraperLT::thumb);
    tokenContainer->DefineLoopToken("{actors[thumbwidth]}", (int)eScraperLT::thumbwidth);
    tokenContainer->DefineLoopToken("{actors[thumbheight]}", (int)eScraperLT::thumbheight);
    actorsIndex = tokenContainer->LoopIndex("actors");
    InheritTokenContainer();
}

void cVeDrOnPause::Set(const char *recfilename) {
    if (!recfilename)
        return;
    free(this->recfilename);
    this->recfilename = strdup(recfilename);
    started = true;
}

bool cVeDrOnPause::Parse(bool force) {
    if (!cViewElement::Parse(force))
        return false;
    if (!recfilename)
        return false;
    const cRecording *recording = new cRecording(recfilename);
    if (!recording)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDROnpauseST::name, recording->Name());
    const cRecordingInfo *info = recording->Info();
    if (info) {
        tokenContainer->AddStringToken((int)eDROnpauseST::shorttext, info->ShortText());
        tokenContainer->AddStringToken((int)eDROnpauseST::description, info->Description());
        const cEvent *event = info->GetEvent();
        if (event) {
            string recDate = *(event->GetDateString());
            string recTime = *(event->GetTimeString());
            if (recDate.find("1970") != string::npos) {
                time_t start = recording->Start();
                recDate = *DateString(start);
                recTime = *TimeString(start);
            }
            tokenContainer->AddStringToken((int)eDROnpauseST::date, recDate.c_str());
            tokenContainer->AddStringToken((int)eDROnpauseST::time, recTime.c_str());
            time_t startTime = event->StartTime();
            struct tm * sStartTime = localtime(&startTime);
            tokenContainer->AddIntToken((int)eDROnpauseIT::year, sStartTime->tm_year + 1900);
            tokenContainer->AddIntToken((int)eDROnpauseIT::daynumeric, sStartTime->tm_mday);
            tokenContainer->AddIntToken((int)eDROnpauseIT::month, sStartTime->tm_mon+1);
            int duration = event->Duration() / 60;
            int recDuration = recording->LengthInSeconds();
            recDuration = (recDuration>0)?(recDuration / 60):0;
            tokenContainer->AddIntToken((int)eDROnpauseIT::duration, recDuration);
            tokenContainer->AddIntToken((int)eDROnpauseIT::durationhours, recDuration / 60);
            tokenContainer->AddStringToken((int)eDROnpauseST::durationminutes, *cString::sprintf("%.2d", recDuration%60));
            tokenContainer->AddIntToken((int)eDROnpauseIT::durationevent, duration);
            tokenContainer->AddIntToken((int)eDROnpauseIT::durationeventhours, duration / 60);
            tokenContainer->AddStringToken((int)eDROnpauseST::durationeventminutes, *cString::sprintf("%.2d", duration%60));
        }
    }
    
    vector<int> loopInfo;
    bool scrapInfoAvailable = LoadFullScrapInfo(NULL, recording);
    int numActors = NumActors();
    loopInfo.push_back(numActors);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
    if (scrapInfoAvailable) {
        SetFullScrapInfo(tokenContainer, actorsIndex);
    }

    SetDirty();
    delete recording;
    return true;
}

void cVeDrOnPause::ResetSleep(void) {
    if (!detacher)
        return;
    detacher->ResetSleep();
}

/******************************************************************
* cVeDrScraperContent
******************************************************************/
cVeDrScraperContent::cVeDrScraperContent(void) {
    recording = NULL;
}

cVeDrScraperContent::~cVeDrScraperContent(void) {
}

void cVeDrScraperContent::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperPosterBannerST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eScraperPosterBannerST::bannerpath);
    tokenContainer->DefineStringToken("{mediapath}", (int)eScraperPosterBannerST::mediapath);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperPosterBannerIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperPosterBannerIT::posterheight);
    tokenContainer->DefineIntToken("{hasposter}", (int)eScraperPosterBannerIT::hasposter);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eScraperPosterBannerIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eScraperPosterBannerIT::bannerheight);
    tokenContainer->DefineIntToken("{hasbanner}", (int)eScraperPosterBannerIT::hasbanner);
    tokenContainer->DefineIntToken("{mediawidth}", (int)eScraperPosterBannerIT::mediawidth);
    tokenContainer->DefineIntToken("{mediaheight}", (int)eScraperPosterBannerIT::mediaheight);
    tokenContainer->DefineIntToken("{isbanner}", (int)eScraperPosterBannerIT::isbanner);
    InheritTokenContainer();
}

void cVeDrScraperContent::Set(const cRecording *recording) {
    this->recording = recording;
}

bool cVeDrScraperContent::Parse(bool force) {
    if (!cViewElement::Parse(force))
        return false;
    if (!recording)
        return false;
    SetPosterBanner(tokenContainer, NULL, recording);
    SetDirty();
    return true;
}
