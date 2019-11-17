#include "viewdisplayreplay.h"

/************************************************************************************
* cViewReplay
************************************************************************************/

cViewReplay::cViewReplay(void) {
    veCustomTokens = NULL;
    veEndTime = NULL;
    veMessage = NULL;
    veScraperContent = NULL;
    veRecTitle = NULL;
    veRecInfo = NULL;
    veCurrentTime = NULL;
    veTotalTime = NULL;
    veProgressbar = NULL;
    veCutMarks = NULL;
    veProgressModeOnly = NULL;
    veControlIcons = NULL;
    veControlIconsModeOnly = NULL;
    veJump = NULL;
    veOnPause = NULL;
    veOnPauseModeOnly = NULL;
    ClearVariables();
    viewId = eViewType::DisplayReplay;
    viewName = strdup("displayreplay");
    numViewElements = (int)eVeDisplayReplay::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    SetViewElements();
}

cViewReplay::~cViewReplay() {
}

void cViewReplay::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayReplay::background));
    viewElementNames.insert(pair<string, int>("backgroundmodeonly", (int)eVeDisplayReplay::backgroundmodeonly));
    viewElementNames.insert(pair<string, int>("datetime", (int)eVeDisplayReplay::datetime));
    viewElementNames.insert(pair<string, int>("time", (int)eVeDisplayReplay::time));
    viewElementNames.insert(pair<string, int>("scrapercontent", (int)eVeDisplayReplay::scrapercontent));
    viewElementNames.insert(pair<string, int>("currentweather", (int)eVeDisplayReplay::currentweather));
    viewElementNames.insert(pair<string, int>("rectitle", (int)eVeDisplayReplay::rectitle));
    viewElementNames.insert(pair<string, int>("recinfo", (int)eVeDisplayReplay::recinfo));
    viewElementNames.insert(pair<string, int>("currenttime", (int)eVeDisplayReplay::currenttime));
    viewElementNames.insert(pair<string, int>("endtime", (int)eVeDisplayReplay::endtime));
    viewElementNames.insert(pair<string, int>("totaltime", (int)eVeDisplayReplay::totaltime));
    viewElementNames.insert(pair<string, int>("progressbar", (int)eVeDisplayReplay::progressbar));
    viewElementNames.insert(pair<string, int>("cutmarks", (int)eVeDisplayReplay::cutmarks));
    viewElementNames.insert(pair<string, int>("cutmarks", (int)eVeDisplayReplay::cutmarks));
    viewElementNames.insert(pair<string, int>("controlicons", (int)eVeDisplayReplay::controlicons));
    viewElementNames.insert(pair<string, int>("controliconsmodeonly", (int)eVeDisplayReplay::controliconsmodeonly));
    viewElementNames.insert(pair<string, int>("progressmodeonly", (int)eVeDisplayReplay::progressmodeonly));
    viewElementNames.insert(pair<string, int>("jump", (int)eVeDisplayReplay::jump));
    viewElementNames.insert(pair<string, int>("message", (int)eVeDisplayReplay::message));
    viewElementNames.insert(pair<string, int>("onpause", (int)eVeDisplayReplay::onpause));
    viewElementNames.insert(pair<string, int>("onpausemodeonly", (int)eVeDisplayReplay::onpausemodeonly));
    viewElementNames.insert(pair<string, int>("customtokens", (int)eVeDisplayReplay::customtokens));
}

void cViewReplay::SetViewElementObjects(void) {
    for (int i = 0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        if (dynamic_cast<cVeMessage*>(viewElements[i])) 
        {
            veMessage = dynamic_cast<cVeMessage*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrScraperContent*>(viewElements[i])) 
        {
            veScraperContent = dynamic_cast<cVeDrScraperContent*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeCustomTokens*>(viewElements[i])) 
        {
            veCustomTokens = dynamic_cast<cVeCustomTokens*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrRecTitle*>(viewElements[i])) 
        {
            veRecTitle = dynamic_cast<cVeDrRecTitle*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrRecInfo*>(viewElements[i])) 
        {
            veRecInfo = dynamic_cast<cVeDrRecInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrCurrentTime*>(viewElements[i])) 
        {
            veCurrentTime = dynamic_cast<cVeDrCurrentTime*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrTotalTime*>(viewElements[i])) 
        {
            veTotalTime = dynamic_cast<cVeDrTotalTime*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrEndTime*>(viewElements[i])) 
        {
            veEndTime = dynamic_cast<cVeDrEndTime*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrProgressBar*>(viewElements[i])) 
        {
            veProgressbar = dynamic_cast<cVeDrProgressBar*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrCutMarks*>(viewElements[i])) 
        {
            veCutMarks = dynamic_cast<cVeDrCutMarks*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrProgressModeonly*>(viewElements[i])) 
        {
            veProgressModeOnly = dynamic_cast<cVeDrProgressModeonly*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrControlIcons*>(viewElements[i]) && (i == (int)eVeDisplayReplay::controlicons)) 
        {
            veControlIcons = dynamic_cast<cVeDrControlIcons*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrControlIcons*>(viewElements[i]) && i == (int)eVeDisplayReplay::controliconsmodeonly) 
        {
            veControlIconsModeOnly = dynamic_cast<cVeDrControlIcons*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrJump*>(viewElements[i])) 
        {
            veJump = dynamic_cast<cVeDrJump*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDrOnPause*>(viewElements[i]) && i == (int)eVeDisplayReplay::onpause)
        {
            veOnPause = dynamic_cast<cVeDrOnPause*>(viewElements[i]);
            veOnPause->SetDetached();
            veOnPause->UnsetWaitOnWakeup();
        }
        else if (dynamic_cast<cVeDrOnPause*>(viewElements[i])  && i == (int)eVeDisplayReplay::onpausemodeonly) 
        {
            veOnPauseModeOnly = dynamic_cast<cVeDrOnPause*>(viewElements[i]);
            veOnPauseModeOnly->SetDetached();
            veOnPauseModeOnly->UnsetWaitOnWakeup();
        }
    }
}

void cViewReplay::PreCache(void) {
    cView::PreCache();
    SetViewelementsAnimOut();
}

void cViewReplay::ClearVariables(void) {
    cView::ClearVariables();
    modeOnly = false;
    lastFlush = 0;
    lastFlushModeOnly = 0;
    message = false;
    reclength = -1;
    timeShiftActive = false;
    timeShiftFramesTotal = -1;
    timeShiftLength = -1;
    timeShiftDuration = "";
    if (veCustomTokens)
        veCustomTokens->Reset();
    if (veEndTime)
        veEndTime->Set(cString(""));
    if (veCutMarks)
        veCutMarks->Reset();
}

void cViewReplay::SetTimeShift(int framesTotal, int timeShiftLength) {
    timeShiftActive = true;
    timeShiftFramesTotal = framesTotal;
    this->timeShiftLength = timeShiftLength;
    int mins = (timeShiftLength / 60) % 60;
    int hours = (timeShiftLength / 3600) % 24;
    timeShiftDuration = cString::sprintf("%d:%02d", hours, mins);
}

void cViewReplay::SetRecording(const cRecording *recording) {
    if (veRecTitle) {
        veRecTitle->Set(recording);
    }
    if (veRecInfo) {
        veRecInfo->Set(recording);
    }
    if (veScraperContent) {
        veScraperContent->Set(recording);
    }
}

void cViewReplay::SetTitle(const char *title) {
    if (veRecTitle) {
        veRecTitle->Set(title);
    }    
}

void cViewReplay::SetCurrent(const char *current) {
    if (veCurrentTime)
        veCurrentTime->Set(current);
    Render((int)eVeDisplayReplay::currenttime);
}

void cViewReplay::SetTotal(const char *total) {
    if (veTotalTime)
        veTotalTime->Set(total, timeShiftActive, *timeShiftDuration);
    Render((int)eVeDisplayReplay::totaltime);
}

void cViewReplay::SetEndTime(int current, int total) {
    if (!veEndTime)
        return;
    int totalLength = total;
    int recordingLength = reclength;
    if (timeShiftActive && timeShiftFramesTotal > 0) {
        totalLength = timeShiftFramesTotal;
        recordingLength = timeShiftLength;
    }
    double rest = (double)(totalLength - current) / (double)totalLength;
    time_t end = time(0) + rest*recordingLength;
    veEndTime->Set(TimeString(end));
    Render((int)eVeDisplayReplay::endtime);
}

void cViewReplay::SetProgressbar(int current, int total) {
    if (veProgressbar)
        veProgressbar->Set(current, total, timeShiftActive, timeShiftFramesTotal);
    Render((int)eVeDisplayReplay::progressbar);
}

void cViewReplay::SetMarks(const cMarks *marks, int current, int total) {
    if (veCutMarks)
        veCutMarks->Set(marks, current, total, timeShiftActive, timeShiftFramesTotal);
    Render((int)eVeDisplayReplay::cutmarks);
}

void cViewReplay::SetControlIcons(bool play, bool forward, int speed) {
    if (!modeOnly) {
        if (veControlIcons)
            veControlIcons->Set(play, forward, speed);
        Render((int)eVeDisplayReplay::controlicons);
    } else {
        if (veControlIconsModeOnly)
            veControlIconsModeOnly->Set(play, forward, speed);
        Render((int)eVeDisplayReplay::controliconsmodeonly);
    }
    
}

void cViewReplay::SetJump(const char *jump) {
    if (veJump) {
        if (!jump)
            veJump->Clear();
        else
            veJump->Set(jump);
    }
    Render((int)eVeDisplayReplay::jump);
}

void cViewReplay::SetMessage(eMessageType type, const char *text) {
    if (veMessage) {
        if (text)
            veMessage->Set(type, text);
        else
            veMessage->Clear();
    }
    Render((int)eVeDisplayReplay::message);
}

void cViewReplay::StartOnPause(const char *recfilename) {
    cVeDrOnPause *onPause = (!modeOnly) ? veOnPause : veOnPauseModeOnly;
    if (!onPause)
        return;
    onPause->Set(recfilename);
    onPause->Parse(true);
}

void cViewReplay::ClearOnPause(void) {
    cVeDrOnPause *onPause = (!modeOnly) ? veOnPause : veOnPauseModeOnly;
    if (!onPause)
        return;
    onPause->Close();
}

void cViewReplay::DelayOnPause(void) {
    if (!veOnPause)
        return;
    if (!veOnPause->Started())
        return;
    veOnPause->ResetSleep();
}

void cViewReplay::Flush(void) {
    if (init) {
        if (!modeOnly) {
            Render((int)eVeDisplayReplay::background);
            Render((int)eVeDisplayReplay::rectitle);
            Render((int)eVeDisplayReplay::recinfo);
            Render((int)eVeDisplayReplay::scrapercontent);
            Render((int)eVeDisplayReplay::currentweather);
            Render((int)eVeDisplayReplay::customtokens);
        } else {
            Render((int)eVeDisplayReplay::backgroundmodeonly);            
        } 
    }

    time_t now = time(0);
    if (!modeOnly && (now != lastFlush)) {
        Render((int)eVeDisplayReplay::datetime);
        Render((int)eVeDisplayReplay::time);
        Render((int)eVeDisplayChannel::customtokens);
        lastFlush = now;
    }

    if (modeOnly) {
        SetProgressModeOnly();
    }

    cView::Flush();
}

void cViewReplay::SetProgressModeOnly(void) {
    if (!veProgressModeOnly)
        return;
    time_t now = time(0);
    if (now == lastFlushModeOnly) {
        return;
    } 
    lastFlushModeOnly = now;

    cControl *control = cControl::Control();
    if (!control)
        return;
    double fps = control->FramesPerSecond();
    int current = 0;
    int total = 0;
    if (!control->GetIndex(current, total))
        return;
    veProgressModeOnly->Set(fps, current, total);
    if (veProgressModeOnly->Parse())
        veProgressModeOnly->Render();
}
