#include "services/remotetimers.h"
#include "tools.h"
#include "recmenumanager.h"
#include "recmenus.h"

// --- cRecMenuMain  ---------------------------------------------------------
cRecMenuMain::cRecMenuMain(bool epgSearchAvailable, bool timerActive, bool switchTimerActive) {
    eRecMenuState action = rmsInstantRecord;
    if (!timerActive) {
        if (tvguideConfig.instRecFolderMode == eFolderSelect)
            action = rmsInstantRecordFolder;
        AddMenuItem(new cRecMenuItemButton(tr("Instant Record"), action, true, false, false, true));
    } else {
        AddMenuItem(new cRecMenuItemButton(tr("Delete Timer"), rmsDeleteTimer, true, false, false, true));
        AddMenuItem(new cRecMenuItemButton(tr("Edit Timer"), rmsEditTimer, false, false, false, true));
    }

    AddMenuItem(new cRecMenuItemButton(tr("Timer Timeline"), rmsTimeline, false, false, false, true));

    if (epgSearchAvailable) {
        AddMenuItem(new cRecMenuItemButton(tr("Create Search Timer"), rmsSearchTimer, false, false, false, true));
        AddMenuItem(new cRecMenuItemButton(tr("Search Timers"), rmsSearchTimers, false, false, false, true));
    }

    if (tvguideConfig.instRecFolderMode == eFolderSelect)
        action = rmsSeriesTimerFolder;
    else
        action = rmsSeriesTimer;
    AddMenuItem(new cRecMenuItemButton(tr("Create Series Timer"), action, false, false, false, true));

    if (epgSearchAvailable) {
        if (!switchTimerActive) {
            AddMenuItem(new cRecMenuItemButton(tr("Create Switch Timer"), rmsSwitchTimer, false, false, false, true));
        } else {
            AddMenuItem(new cRecMenuItemButton(tr("Delete Switch Timer"), rmsSwitchTimerDelete, false, false, false, true));
        }
        AddMenuItem(new cRecMenuItemButton(tr("Search"), rmsSearch, false, false, false, true));
    }

    if (epgSearchAvailable) {
        AddMenuItem(new cRecMenuItemButton(tr("Check for Timer Conflicts"), rmsTimerConflicts, false, false, false, true));
    }

    AddMenuItem(new cRecMenuItemButton(tr("Search in Recordings"), rmsRecordingSearch, false, false, false, true));

    int menuWidth = CalculateOptimalWidth() + 4 * border;
    SetWidthPixel(menuWidth);
            
    CalculateHeight();
    CreatePixmap();
    Arrange();
};

/******************************************************************************************
*   Instant Timer Menus
******************************************************************************************/

// --- cRecMenuAskFolder  ---------------------------------------------------------
cRecMenuAskFolder::cRecMenuAskFolder(const cEvent *event, eRecMenuState nextAction) {
    SetWidthPercent(80);
    NextAction = nextAction;
    cString message = tr("Set Folder for");
    cString headerText = cString::sprintf("%s\n\"%s\"", *message, event->Title());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*headerText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    AddMenuItemInitial(new cRecMenuItemButton(tr("root video folder"), nextAction, true, false, true));
    
    ReadRecordingDirectories(&folders, NULL, "");
    int numFolders = folders.size();
    for (int i=0; i < numFolders; i++) {
        if (!AddMenuItemInitial(new cRecMenuItemButton(folders[i].c_str(), nextAction, false, false, true)))
            break;
    }
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuAskFolder::GetMenuItem(int number) { 
    if (number == 0) {
        cRecMenuItem *result = new cRecMenuItemButton(tr("root video folder"), rmsInstantRecord, false, false, true);
        return result;
    } else if ((number > 0) && (number < folders.size()+1)) {
        cRecMenuItem *result = new cRecMenuItemButton(folders[number-1].c_str(), rmsInstantRecord, false, false, true);
        return result;
    }
    return NULL;
}

int cRecMenuAskFolder::GetTotalNumMenuItems(void) { 
    return folders.size()+1;
}

std::string cRecMenuAskFolder::GetFolder(void) {
    std::string folder = "";
    int folderActive = GetActive();
    if (folderActive > 0 && folderActive < folders.size() + 1)
        folder = folders[folderActive - 1];
    return folder;
}

// --- cRecMenuConfirmTimer  ---------------------------------------------------------
cRecMenuConfirmTimer::cRecMenuConfirmTimer(const cEvent *event) {
    SetWidthPercent(50);
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cString channelName = Channels->GetByChannelID(event->ChannelID())->Name();
#else
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
#endif
    cString message;
    bool eventHasTimer = false;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        if (rtMatch.timerMatch == tmFull) {
            eventHasTimer = true;                
        }
    } else {
        eventHasTimer = event->HasTimer();
    }
    if (eventHasTimer) {
        message = tr("Timer created");
    } else {
        message = tr("Timer NOT created");
    }
    cString text = cString::sprintf("%s\n%s\n%s %s - %s\n%s", 
                                    *message, 
                                    *channelName,
                                    *event->GetDateString(),
                                    *event->GetTimeString(),
                                    *event->GetEndTimeString(),
                                    event->Title()
                                );
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuConfirmDeleteTimer  ---------------------------------------------------------
cRecMenuConfirmDeleteTimer::cRecMenuConfirmDeleteTimer(const cEvent *event) {
    SetWidthPercent(50);
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cString channelName = Channels->GetByChannelID(event->ChannelID())->Name();
#else
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
#endif
    cString text = cString::sprintf("%s\n%s\n%s %s - %s\n%s", 
                                    tr("Timer deleted"), 
                                    *channelName,
                                    *event->GetDateString(),
                                    *event->GetTimeString(),
                                    *event->GetEndTimeString(),
                                    event->Title()
                                );
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 4 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuAskDeleteTimer ---------------------------------------------------------
cRecMenuAskDeleteTimer::cRecMenuAskDeleteTimer(const cEvent *event) {
    SetWidthPercent(50);
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cString channelName = Channels->GetByChannelID(event->ChannelID())->Name();
#else
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
#endif
    cString text = cString::sprintf("%s \"%s, %s\" %s", 
                                    tr("Timer"),
                                    *channelName,
                                    event->Title(),
                                    tr("still recording - really delete?"));
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 4 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Yes"), tr("No"), rmsDeleteTimerConfirmation, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuTimerConflicts  ---------------------------------------------------------
cRecMenuTimerConflicts::cRecMenuTimerConflicts(cTVGuideTimerConflicts *conflicts) {
    int numConflicts = conflicts->NumConflicts();
    
    cString text;
    if (numConflicts == 1) {
        text = cString::sprintf("%s %s %s", tr("One"), tr("Timer Conflict"), tr("detected"));
    } else {
        text = cString::sprintf("%d %s %s", conflicts->NumConflicts(), tr("Timer Conflicts"), tr("detected"));
    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text, true);
    
    for (int i=0; i<numConflicts; i++) {
        cTVGuideTimerConflict *conflict = conflicts->GetConflict(i);
        if (!conflict)
            continue;
        cString dateTime = DayDateTime(conflict->time);
        int numTimers = conflict->timerIDs.size();
        cString textConflict = cString::sprintf("%s: %s (%d %s)", tr("Show conflict"), *dateTime, numTimers, tr("timers involved"));
        bool isActive = (i==0)?true:false;
        AddMenuItem(new cRecMenuItemButton(*textConflict, rmsTimerConflict, isActive));
    }
    
    SetWidthPixel(CalculateOptimalWidth() + 4*border);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("Ignore Conflicts"), rmsClose, false));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

int cRecMenuTimerConflicts::GetTimerConflict(void) {
    return GetActive();
}

// --- cRecMenuTimerConflict  ---------------------------------------------------------
cRecMenuTimerConflict::cRecMenuTimerConflict(cTVGuideTimerConflict *conflict) {
    SetWidthPercent(95);
    this->conflict = conflict;
    SetHeader(new cRecMenuItemTimerConflictHeader(conflict->timeStart, 
                                                  conflict->timeStop, 
                                                  conflict->overlapStart, 
                                                  conflict->overlapStop));
    SetFooter(new cRecMenuItemButton(tr("Ignore Conflict"), rmsIgnoreTimerConflict, false, true));
    int i=0;
    for(std::vector<int>::iterator it = conflict->timerIDs.begin(); it != conflict->timerIDs.end(); it++) {
#if VDRVERSNUM >= 20301
        LOCK_TIMERS_READ;
        const cTimer *timer = Timers->Get(*it);
#else
        const cTimer *timer = Timers.Get(*it);
#endif
        if (timer) {
            if (!AddMenuItemInitial(new cRecMenuItemTimer(  timer,
                                                      rmsTimerConflictShowInfo,
                                                      rmsDeleteTimerConflictMenu, 
                                                      rmsEditTimerConflictMenu,
                                                      rmsSearchRerunsTimerConflictMenu,
                                                      conflict->timeStart, 
                                                      conflict->timeStop, 
                                                      conflict->overlapStart, 
                                                      conflict->overlapStop, 
                                                      (!i)?true:false)
            ))
                break;
            i++;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuItem *cRecMenuTimerConflict::GetMenuItem(int number) { 
    if ((number >= 0) && (number < conflict->timerIDs.size())) {
#if VDRVERSNUM >= 20301
        LOCK_TIMERS_READ;
        const cTimer *timer = Timers->Get(conflict->timerIDs[number]);
#else
        const cTimer *timer = Timers.Get(conflict->timerIDs[number]);
#endif
        cRecMenuItem *result = new cRecMenuItemTimer( timer,
                                                      rmsTimerConflictShowInfo,
                                                      rmsDeleteTimerConflictMenu, 
                                                      rmsEditTimerConflictMenu,
                                                      rmsSearchRerunsTimerConflictMenu,
                                                      conflict->timeStart, 
                                                      conflict->timeStop, 
                                                      conflict->overlapStart, 
                                                      conflict->overlapStop, 
                                                      false);
        return result;
    }
    return NULL;
}

int cRecMenuTimerConflict::GetTotalNumMenuItems(void) { 
    return conflict->timerIDs.size(); 
}

int cRecMenuTimerConflict::GetTimerConflictIndex(void) {
    return GetActive();
}

// --- cRecMenuNoTimerConflict ---------------------------------------------------------
cRecMenuNoTimerConflict::cRecMenuNoTimerConflict(void) {
    SetWidthPercent(50);
    cString text = tr("No Timer Conflicts found");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 4 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("Close"), rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuRerunResults ---------------------------------------------------------
cRecMenuRerunResults::cRecMenuRerunResults(const cEvent *original, const cEvent **reruns, int numReruns) {
    this->reruns = reruns;
    this->numReruns = numReruns;
    SetWidthPercent(70);
    this->numReruns = numReruns;
    cString message1 = tr("reruns for");
    cString message2 = tr("rerun for");
    cString message3 = tr("found");
    cString infoText = cString::sprintf("%d %s:\n\"%s\" %s", numReruns, (numReruns>1)?(*message1):(*message2), original->Title(), *message3);
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *button = new cRecMenuItemButton(tr("Ignore reruns"), rmsTimerConflictIgnoreReruns, false);
    SetFooter(button);
    
    if (reruns && (numReruns > 0)) {
        for (int i=0; i<numReruns; i++) {
            if (!AddMenuItemInitial(new cRecMenuItemEvent(reruns[i], rmsSearchShowInfo, rmsTimerConflictRecordRerun, (i==0)?true:false)))
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuRerunResults::GetMenuItem(int number) {
   if ((number >= 0) && (number < numReruns)) {
        cRecMenuItem *result = new cRecMenuItemEvent(reruns[number], rmsSearchShowInfo, rmsTimerConflictRecordRerun, false);
        return result;
    }
    return NULL;
}

int cRecMenuRerunResults::GetTotalNumMenuItems(void) {
    return numReruns;
}

const cEvent *cRecMenuRerunResults::GetRerunEvent(void) {
    cRecMenuItemEvent *activeItem = dynamic_cast<cRecMenuItemEvent*>(GetActiveMenuItem());
    return activeItem->GetEventValue();
}

// --- cRecMenuNoRerunsFound  ---------------------------------------------------------
cRecMenuNoRerunsFound::cRecMenuNoRerunsFound(cString searchString) {
    SetWidthPercent(50);
    cString message = tr("No reruns found for Event");
    cString text = cString::sprintf("%s\n\"%s\"", 
                                    *message, 
                                    *searchString);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsTimerConflictIgnoreReruns, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuConfirmRerunUsed  ---------------------------------------------------------
cRecMenuConfirmRerunUsed::cRecMenuConfirmRerunUsed(const cEvent *original, const cEvent *replace) {
    SetWidthPercent(70);
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cString channelOrig = Channels->GetByChannelID(original->ChannelID())->Name();
    const cString channelReplace = Channels->GetByChannelID(replace->ChannelID())->Name();
#else
    cString channelOrig = Channels.GetByChannelID(original->ChannelID())->Name();
    cString channelReplace = Channels.GetByChannelID(replace->ChannelID())->Name();
#endif
    cString message1 = tr("Timer for");
    cString message2 = tr("replaced by rerun");
    cString text = cString::sprintf("%s\n\"%s\", %s %s, %s\n%s\n\"%s\", %s %s, %s", 
                                    *message1, 
                                    original->Title(),
                                    *original->GetDateString(),
                                    *original->GetTimeString(),
                                    *channelOrig,
                                    *message2,
                                    replace->Title(),
                                    *replace->GetDateString(),
                                    *replace->GetTimeString(),
                                    *channelReplace);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsTimerConflicts, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuEditTimer  ---------------------------------------------------------
#if VDRVERSNUM >= 20301
cRecMenuEditTimer::cRecMenuEditTimer(const cTimer *timer, eRecMenuState nextState) {
    const cTimer *originalTimer;
#else
cRecMenuEditTimer::cRecMenuEditTimer(cTimer *timer, eRecMenuState nextState) {
#endif
    SetWidthPercent(70);
    if (!timer)
        return;
    originalTimer = timer;
    cString title("");
    cString channelName("");
    if (timer->Event())
        title = timer->Event()->Title();
    if (timer->Channel())
        channelName = timer->Channel()->Name();
    cString infoText = cString::sprintf("%s:\n %s, %s", tr("Edit Timer"), *title, *channelName);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItemInitial(infoItem);
                
    timerActive = false;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = timer->Event();
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        if (rtMatch.timer) {
            if (rtMatch.timerMatch == tmFull)
                timerActive = true;
        }
    } else
        timerActive = timer->HasFlags(tfActive);

    day = timer->Day();
    start = timer->Start();
    stop = timer->Stop();
    prio = timer->Priority();
    lifetime = timer->Lifetime();
    strncpy(folder, GetDirectoryFromTimer(timer->File()).c_str(), TEXTINPUTLENGTH);
    
    AddMenuItemInitial(new cRecMenuItemBool(tr("Timer Active"), timerActive, false, true, &timerActive));
    AddMenuItemInitial(new cRecMenuItemInt(tr("Priority"), prio, 0, MAXPRIORITY, false, &prio));
    AddMenuItemInitial(new cRecMenuItemInt(tr("Lifetime"), lifetime, 0, MAXLIFETIME, false, &lifetime));
    AddMenuItemInitial(new cRecMenuItemDay(tr("Day"), day, false, &day));
    AddMenuItemInitial(new cRecMenuItemTime(tr("Timer start time"), start, false, &start));
    AddMenuItemInitial(new cRecMenuItemTime(tr("Timer stop time"), stop, false, &stop));
    cString fileInfo = cString::sprintf("%s:\n%s", tr("Timer File"), timer->File());
    cRecMenuItemInfo *fileInfoItem = new cRecMenuItemInfo(*fileInfo, false);
    fileInfoItem->CalculateHeight(width - 2 * border);
    AddMenuItemInitial(fileInfoItem);
    AddMenuItemInitial(new cRecMenuItemSelectDirectory(tr("New Folder"), std::string(folder), false, folder));
    if (nextState == rmsTimelineTimerSave) {
        AddMenuItemInitial(new cRecMenuItemButton(tr("Delete Timer"), rmsTimelineTimerDelete, false, false));
        AddMenuItemInitial(new cRecMenuItemButtonYesNo(tr("Save"), tr("Cancel"), nextState, rmsTimeline, false));
    } else {
        AddMenuItemInitial(new cRecMenuItemButtonYesNo(tr("Save"), tr("Cancel"), nextState, rmsClose, false));
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

#if VDRVERSNUM >= 20301
const cTimer *cRecMenuEditTimer::GetOriginalTimer(void) {
#else
cTimer *cRecMenuEditTimer::GetOriginalTimer(void) {
#endif
    return originalTimer;
}

cTimer cRecMenuEditTimer::GetTimer(void) {
    cTimer t;
    if (timerActive)
        t.SetFlags(tfActive);
    else 
        t.SetFlags(tfNone);
    t.SetDay(day);
    t.SetStart(start);
    t.SetStop(stop);
    t.SetPriority(prio);
    t.SetLifetime(lifetime);
    std::string newFolder(folder);
    std::string newFile = originalTimer->File();
    size_t found = newFile.find_last_of('~');
    if (found != std::string::npos) {
        std::string fileName = newFile.substr(found+1);
        if (newFolder.size() > 0)
            newFile = *cString::sprintf("%s~%s", newFolder.c_str(), fileName.c_str());
        else
            newFile = fileName;
    } else {
        if (newFolder.size() > 0)
            newFile = *cString::sprintf("%s~%s", newFolder.c_str(), newFile.c_str());
    }
    std::replace(newFile.begin(), newFile.end(), '/', '~');
    t.SetFile(newFile.c_str());
    return t;
}

/******************************************************************************************
*   Series Timer Menus
******************************************************************************************/

// --- cRecMenuSeriesTimer ---------------------------------------------------------
#if VDRVERSNUM >= 20301
cRecMenuSeriesTimer::cRecMenuSeriesTimer(const cChannel *initialChannel, const cEvent *event, std::string folder) {
#else
cRecMenuSeriesTimer::cRecMenuSeriesTimer(cChannel *initialChannel, const cEvent *event, std::string folder) {
#endif
    if (!initialChannel)
        return;
    timerActive = true;
    channel = initialChannel->Number();
    dayOfWeek = 127; 
    priority =  MAXPRIORITY;
    lifetime = MAXLIFETIME;
    CalculateTimes(event);
    this->folder = folder;

    SetWidthPercent(70);
    cString title = tr("Create Series Timer based on");
    cString infoText = cString::sprintf("%s:\n%s", *title, event->Title());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);

    AddMenuItem(new cRecMenuItemBool(tr("Timer Active"), timerActive, false, false, &timerActive));
    AddMenuItem(new cRecMenuItemChannelChooser(tr("Channel"), initialChannel, false, &channel));
    AddMenuItem(new cRecMenuItemTime(tr("Series Timer start time"), start, false, &start));
    AddMenuItem(new cRecMenuItemTime(tr("Series Timer stop time"), stop, false, &stop));
    AddMenuItem(new cRecMenuItemDayChooser(tr("Days to record"), dayOfWeek, false, &dayOfWeek));
    AddMenuItem(new cRecMenuItemDay(tr("Day to start"), tstart, false, &tstart));
    AddMenuItem(new cRecMenuItemInt(tr("Priority"), priority, 0, MAXPRIORITY, false, &priority));
    AddMenuItem(new cRecMenuItemInt(tr("Lifetime"), lifetime, 0, MAXLIFETIME, false, &lifetime));

    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create Timer"), tr("Cancel"), rmsSeriesTimerCreate, rmsClose, true));

    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cTimer *cRecMenuSeriesTimer::GetTimer(void) {
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cChannel *chan = Channels->GetByNumber(channel);
#else
    cChannel *chan = Channels.GetByNumber(channel);
#endif
    cTimer *seriesTimer = new cTimer(NULL, NULL, chan);
    cString fileName = "TITLE EPISODE";
    if (folder.size() > 0) {
        std::replace(folder.begin(), folder.end(), '/', '~');
        fileName = cString::sprintf("%s~%s", folder.c_str(), *fileName);
    }
    seriesTimer->SetDay(tstart);
    seriesTimer->SetStart(start);
    seriesTimer->SetStop(stop);
    seriesTimer->SetPriority(priority);
    seriesTimer->SetLifetime(lifetime);
    seriesTimer->SetWeekDays(dayOfWeek);
    seriesTimer->SetFile(*fileName);
    if (timerActive)
        seriesTimer->SetFlags(tfActive);
    else 
        seriesTimer->SetFlags(tfNone);
    return seriesTimer;
}

void cRecMenuSeriesTimer::CalculateTimes(const cEvent *event) {
    tstart = event->StartTime();
    tstart -= Setup.MarginStart * 60;
    time_t tstop = tstart + event->Duration();
    tstop  += Setup.MarginStop * 60;
    
    struct tm tm_r;
    struct tm *time = localtime_r(&tstart, &tm_r);
    start = time->tm_hour * 100 + time->tm_min;
    time = localtime_r(&tstop, &tm_r);
    stop = time->tm_hour * 100 + time->tm_min;
    if (stop >= 2400)
        stop -= 2400;
}

// --- cRecMenuConfirmSeriesTimer  ---------------------------------------------------------
cRecMenuConfirmSeriesTimer::cRecMenuConfirmSeriesTimer(cTimer *seriesTimer) {
    SetWidthPercent(50);
    cString message = tr("Series Timer created");
    cString infoText = message;
    if (seriesTimer) {
        cString days = cTimer::PrintDay(seriesTimer->Day(), seriesTimer->WeekDays(), true);
        infoText = cString::sprintf("%s\n%s, %s: %s, %s: %s", *message, *days, tr("Start"), *TimeString(seriesTimer->StartTime()), tr("Stop"), *TimeString(seriesTimer->StopTime()));
    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

/******************************************************************************************
*   SearchTimer Menus
******************************************************************************************/

// --- cRecMenuSearchTimer  ---------------------------------------------------------
cRecMenuSearchTimer::cRecMenuSearchTimer(const cEvent *event) {
    SetWidthPercent(70);
    cString message = tr("Configure Search Timer based on");
    cString infoText = cString::sprintf("%s:\n\"%s\"", *message, event->Title());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    strncpy(searchString, event->Title(), TEXTINPUTLENGTH);
    AddMenuItemInitial(new cRecMenuItemText(tr("Search Expression:"), searchString, TEXTINPUTLENGTH, false));
    AddMenuItemInitial(new cRecMenuItemButtonYesNo(tr("Continue"), tr("Cancel"), rmsSearchTimerOptions, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimerTemplates  ---------------------------------------------------------
cRecMenuSearchTimerTemplates::cRecMenuSearchTimerTemplates(cTVGuideSearchTimer searchTimer, std::vector<TVGuideEPGSearchTemplate> templates) {
    this->searchTimer = searchTimer;
    this->templates = templates;
    SetWidthPercent(70);
    cString message = tr("Configure Search Timer for Search String");
    cString infoText = cString::sprintf("%s:\n%s", *message, searchTimer.SearchString().c_str());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    AddMenuItemInitial(new cRecMenuItemButton(tr("Manually configure Options"), rmsSearchTimerEdit, true));
    
    numTemplates = templates.size();
    for (int i=0; i<numTemplates; i++) {
        cString buttonText = cString::sprintf("%s \"%s\"", tr("Use Template"), templates[i].name.c_str());
        if (!AddMenuItemInitial(new cRecMenuItemButton(*buttonText, rmsSearchTimerCreateWithTemplate, false)))
            break;
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuItem *cRecMenuSearchTimerTemplates::GetMenuItem(int number) {
    if (number == 0) {
        return new cRecMenuItemButton(tr("Manually configure Options"), rmsSearchTimerEdit, false);
    } else if ((number > 0) && (number < numTemplates + 1)) {
        cString buttonText = cString::sprintf("%s \"%s\"", tr("Use Template"), templates[number-1].name.c_str());
        cRecMenuItem *templ = new cRecMenuItemButton(*buttonText, rmsSearchTimerCreateWithTemplate, false);
        return templ;
    }
    return NULL;
}

int cRecMenuSearchTimerTemplates::GetTotalNumMenuItems(void) { 
    return numTemplates + 1;
}

TVGuideEPGSearchTemplate cRecMenuSearchTimerTemplates::GetTemplate(void) {
    TVGuideEPGSearchTemplate templ;
    int tmplActive = GetActive() - 1;
    if (tmplActive >= 0 && tmplActive < templates.size())
        templ = templates[tmplActive];
    return templ;
}
// --- cRecMenuSearchTimers  ---------------------------------------------------------
cRecMenuSearchTimers::cRecMenuSearchTimers(std::vector<cTVGuideSearchTimer> searchTimers) {
    this->searchTimers = searchTimers;
    numSearchTimers = searchTimers.size();
    SetWidthPercent(70);
    cString headline;
    if (numSearchTimers > 0) {
        headline = tr("EPGSearch Search Timers");
    } else {
        headline = tr("No Search Timers Configured");
    } 
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*headline, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    cRecMenuItem *button = new cRecMenuItemButton(tr("Close"), rmsClose, (!numSearchTimers)?true:false);
    SetFooter(button);
    if (numSearchTimers > 0)
        SetMenuItems();    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuSearchTimers::~cRecMenuSearchTimers(void) {

}

void cRecMenuSearchTimers::SetMenuItems(void) {
    for (int i = 0; i < numSearchTimers; i++) {
        if (!AddMenuItemInitial(new cRecMenuItemSearchTimer(searchTimers[i], rmsSearchTimerTest, rmsSearchTimerEdit, rmsSearchTimerDeleteConfirm, (i==0)?true:false)))
            break;
    }
}

cTVGuideSearchTimer cRecMenuSearchTimers::GetSearchTimer(void) {
    cRecMenuItemSearchTimer *activeItem = dynamic_cast<cRecMenuItemSearchTimer*>(GetActiveMenuItem());
    return activeItem->GetTimer();
}

cRecMenuItem *cRecMenuSearchTimers::GetMenuItem(int number) {
    if (number < 0 || number >= numSearchTimers)
        return NULL;
    return new cRecMenuItemSearchTimer(searchTimers[number], rmsSearchTimerTest, rmsSearchTimerEdit, rmsSearchTimerDeleteConfirm, false);
}

int cRecMenuSearchTimers::GetTotalNumMenuItems(void) {
    return numSearchTimers;
}

// --- cRecMenuSearchTimerEdit  ---------------------------------------------------------
cRecMenuSearchTimerEdit::cRecMenuSearchTimerEdit(cTVGuideSearchTimer searchTimer, bool advancedOptions) {
    deleteMenuItems = false;
    this->advancedOptions = advancedOptions;
    this->searchTimer = searchTimer;
    strncpy(searchString, searchTimer.SearchString().c_str(), TEXTINPUTLENGTH);
    timerActive = searchTimer.Active();
    mode = searchTimer.SearchMode();
    useTitle = searchTimer.UseTitle();
    useSubtitle = searchTimer.UseSubtitle();
    useDescription = searchTimer.UseDescription();
    useChannel = searchTimer.UseChannel();
    startChannel = searchTimer.StartChannel();
    stopChannel = searchTimer.StopChannel();
    useTime = searchTimer.UseTime();
    startTime = searchTimer.StartTime();
    stopTime = searchTimer.StopTime();
    useDayOfWeek = searchTimer.UseDayOfWeek();
    dayOfWeek = searchTimer.DayOfWeek();
    priority = searchTimer.Priority();
    lifetime = searchTimer.Lifetime();
    useEpisode = searchTimer.UseEpisode();
    std::string dir = searchTimer.Directory();
    strncpy(directory, dir.c_str(), TEXTINPUTLENGTH);
    marginStart = searchTimer.MarginStart();
    marginStop = searchTimer.MarginStop();
    useVPS = searchTimer.UseVPS();
    avoidRepeats = searchTimer.AvoidRepeats();
    allowedRepeats = searchTimer.AllowedRepeats();
    compareTitle = searchTimer.CompareTitle();
    compareSubtitle = searchTimer.CompareSubtitle();
    compareSummary = searchTimer.CompareSummary();
    useInFavorites = searchTimer.UseInFavorites();

    SetWidthPercent(70);
    cString infoText;
    if (searchTimer.GetID() > -1) {
        infoText = tr("Configure Search Timer Options");
    } else {
        infoText = tr("Create Search Timer");
    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    cRecMenuItemButtonYesNo *footerButton = new cRecMenuItemButtonYesNo(tr("Save Search Timer"), tr("Cancel"), rmsSearchTimerSave, rmsSearchTimers, (advancedOptions)?false:true);
    SetFooter(footerButton);
    InitMenuItems();
    CreateMenuItems();
}

cRecMenuSearchTimerEdit::~cRecMenuSearchTimerEdit(void) {
    for (std::vector<cRecMenuItem*>::iterator it = mainMenuItems.begin(); it != mainMenuItems.end(); it++) {
        delete *it;
    }
    mainMenuItems.clear();
    for (std::vector<cRecMenuItem*>::iterator it = useChannelSubMenu.begin(); it != useChannelSubMenu.end(); it++) {
        delete *it;
    }
    useChannelSubMenu.clear();
    for (std::vector<cRecMenuItem*>::iterator it = useTimeSubMenu.begin(); it != useTimeSubMenu.end(); it++) {
        delete *it;
    }
    useTimeSubMenu.clear();
    for (std::vector<cRecMenuItem*>::iterator it = useDayOfWeekSubMenu.begin(); it != useDayOfWeekSubMenu.end(); it++) {
        delete *it;
    }
    useDayOfWeekSubMenu.clear();
    for (std::vector<cRecMenuItem*>::iterator it = avoidRepeatSubMenu.begin(); it != avoidRepeatSubMenu.end(); it++) {
        delete *it;
    }
    avoidRepeatSubMenu.clear();
    currentMenuItems.clear();
}

void cRecMenuSearchTimerEdit::InitMenuItems(void) {

    useChannelPos = 6;
    useTimePos = 7;
    useDayOfWeekPos = 8;
    avoidRepeatsPos = 14;

    mainMenuItems.push_back(new cRecMenuItemText(tr("Search String"), searchString, TEXTINPUTLENGTH, false, searchString));
    mainMenuItems.push_back(new cRecMenuItemBool(tr("Active"), timerActive, false, false, &timerActive, rmsSearchTimerSave));
    std::vector<std::string> searchModes;
    searchTimer.GetSearchModes(&searchModes);
    mainMenuItems.push_back(new cRecMenuItemSelect(tr("Search Mode"), searchModes, mode, false, &mode, rmsSearchTimerSave));
    mainMenuItems.push_back(new cRecMenuItemBool(tr("Use Title"), useTitle, false, false, &useTitle, rmsSearchTimerSave));
    mainMenuItems.push_back(new cRecMenuItemBool(tr("Use Subtitle"), useSubtitle, false, false, &useSubtitle, rmsSearchTimerSave));
    mainMenuItems.push_back(new cRecMenuItemBool(tr("Use Description"), useDescription, false, false, &useDescription, rmsSearchTimerSave));
    mainMenuItems.push_back(new cRecMenuItemBool(tr("Limit Channels"), useChannel, true, false, &useChannel, rmsSearchTimerSave));
    mainMenuItems.push_back(new cRecMenuItemBool(tr("Use Time"), useTime, true, false, &useTime, rmsSearchTimerSave));
    if (!advancedOptions) {
        mainMenuItems.push_back(new cRecMenuItemButton(tr("Display advanced Options"), rmsSearchTimerEditAdvanced, false));
    } else {
        mainMenuItems.push_back(new cRecMenuItemBool(tr("Limit Days of the Week"), useDayOfWeek, true, false, &useDayOfWeek, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemInt(tr("Priority"), priority, 0, 99, false, &priority, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemInt(tr("Lifetime"), lifetime, 0, 99, false, &lifetime, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemInt(tr("Time margin for start in minutes"), marginStart, 0, 30, false, &marginStart, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemInt(tr("Time margin for stop in minutes"), marginStop, 0, 30, false, &marginStop, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemBool(tr("Series Recording"), useEpisode, false, false, &useEpisode, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemSelectDirectory(tr("Folder"), std::string(directory), false, directory, rmsSearchTimerSave, true));
        mainMenuItems.push_back(new cRecMenuItemBool(tr("Use VPS"), useVPS, false, false, &useVPS, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemBool(tr("Avoid Repeats"), avoidRepeats, true, false, &avoidRepeats, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemBool(tr("Use in Favorites"), useInFavorites, false, false, &useInFavorites, rmsSearchTimerSave));
        mainMenuItems.push_back(new cRecMenuItemButton(tr("Hide advanced Options"), rmsSearchTimerEdit, false));
    }
    mainMenuItems.push_back(new cRecMenuItemButton(tr("Display Results for Search Timer"), rmsSearchTimerTest, false));

    if (startChannel == 0)
        startChannel = 1;
    if (stopChannel == 0)
        stopChannel = 1;
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    useChannelSubMenu.push_back(new cRecMenuItemChannelChooser(tr("Start Channel"), Channels->GetByNumber(startChannel), false, &startChannel, rmsSearchTimerSave));
    useChannelSubMenu.push_back(new cRecMenuItemChannelChooser(tr("Stop Channel"), Channels->GetByNumber(stopChannel), false, &stopChannel, rmsSearchTimerSave));
#else
    useChannelSubMenu.push_back(new cRecMenuItemChannelChooser(tr("Start Channel"), Channels.GetByNumber(startChannel), false, &startChannel, rmsSearchTimerSave));
    useChannelSubMenu.push_back(new cRecMenuItemChannelChooser(tr("Stop Channel"), Channels.GetByNumber(stopChannel), false, &stopChannel, rmsSearchTimerSave));
#endif

    useTimeSubMenu.push_back(new cRecMenuItemTime(tr("Start after"), startTime, false, &startTime, rmsSearchTimerSave));
    useTimeSubMenu.push_back(new cRecMenuItemTime(tr("Start before"), stopTime, false, &stopTime, rmsSearchTimerSave));

    if (advancedOptions) {
        useDayOfWeekSubMenu.push_back(new cRecMenuItemDayChooser(tr("Select Days"), dayOfWeek, false, &dayOfWeek));
        avoidRepeatSubMenu.push_back(new cRecMenuItemInt(tr("Number of allowed repeats"), allowedRepeats, 0, 30, false, &allowedRepeats, rmsSearchTimerSave));
        avoidRepeatSubMenu.push_back(new cRecMenuItemBool(tr("Compare Title"), compareTitle, false, false, &compareTitle, rmsSearchTimerSave));
        avoidRepeatSubMenu.push_back(new cRecMenuItemBool(tr("Compare Subtitle"), compareSubtitle, false, false, &compareSubtitle, rmsSearchTimerSave));
        avoidRepeatSubMenu.push_back(new cRecMenuItemBool(tr("Compare Description"), compareSummary, false, false, &compareSummary, rmsSearchTimerSave));
    }
}


void cRecMenuSearchTimerEdit::CreateMenuItems(void) {
    bool reDraw = false;
    if (GetCurrentNumMenuItems() > 0) {
        InitMenu(false);
        currentMenuItems.clear();
        reDraw = true;
    }
    
   int numMainMenuItems = mainMenuItems.size();
    
    for (int i = 0; i < numMainMenuItems; i++) {
        currentMenuItems.push_back(mainMenuItems[i]);
        if ((i == useChannelPos) && useChannel)
            AddSubMenu(&useChannelSubMenu);
        else if ((i == useTimePos) && useTime)
            AddSubMenu(&useTimeSubMenu);
        else if (advancedOptions && (i == useDayOfWeekPos) && useDayOfWeek)
            AddSubMenu(&useDayOfWeekSubMenu);
        else if (advancedOptions && (i == avoidRepeatsPos) && avoidRepeats)
            AddSubMenu(&avoidRepeatSubMenu);
    }    

    int numMenuItemsAll = currentMenuItems.size();
    int start = GetStartIndex();
     for (int i = start; i < numMenuItemsAll; i++) {
        if ((i == start) && !reDraw && advancedOptions) {
            currentMenuItems[i]->setActive();
        }
        if (!AddMenuItemInitial(currentMenuItems[i])) {
            break;
        }
    }
    numMenuItems = currentMenuItems.size();
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

void cRecMenuSearchTimerEdit::AddSubMenu(std::vector<cRecMenuItem*> *subMenu) {
    for (std::vector<cRecMenuItem*>::iterator it = subMenu->begin(); it < subMenu->end(); it++) {
        currentMenuItems.push_back(*it);
    }
}


cTVGuideSearchTimer cRecMenuSearchTimerEdit::GetSearchTimer(void) {
    searchTimer.SetSearchString(searchString);
    searchTimer.SetActive(timerActive);
    searchTimer.SetSearchMode(mode);
    searchTimer.SetUseTitle(useTitle);
    searchTimer.SetUseSubtitle(useSubtitle);
    searchTimer.SetUseDesription(useDescription);
    searchTimer.SetUseChannel(useChannel);
    if (useChannel) {
        searchTimer.SetStartChannel(startChannel);
        searchTimer.SetStopChannel(stopChannel);
    }
    searchTimer.SetUseTime(useTime);
    if (useTime) {
        searchTimer.SetStartTime(startTime);
        searchTimer.SetStopTime(stopTime);
    }
    searchTimer.SetUseDayOfWeek(useDayOfWeek);
    if (useDayOfWeek) {
        searchTimer.SetDayOfWeek(dayOfWeek);
    }
    searchTimer.SetPriority(priority);
    searchTimer.SetLifetime(lifetime);
    searchTimer.SetUseEpisode(useEpisode);
    std::string dir(directory);
    std::replace(dir.begin(), dir.end(), '/', '~');
    searchTimer.SetDirectory(dir);
    searchTimer.SetMarginStart(marginStart);
    searchTimer.SetMarginStop(marginStop);
    searchTimer.SetUseVPS(useVPS);
    searchTimer.SetAvoidRepeats(avoidRepeats);
    if (avoidRepeats) {
        searchTimer.SetAllowedRepeats(allowedRepeats);
        searchTimer.SetCompareTitle(compareTitle);
        searchTimer.SetCompareSubtitle(compareSubtitle);
        searchTimer.SetCompareSummary(compareSummary);
    }
    searchTimer.SetUseInFavorites(useInFavorites);
    return searchTimer;
}

int cRecMenuSearchTimerEdit::GetTotalNumMenuItems(void) { 
    return numMenuItems; 
}

cRecMenuItem *cRecMenuSearchTimerEdit::GetMenuItem(int number) {
    if ((number > -1) && (number < numMenuItems)) {
        return currentMenuItems[number];
    }
    return NULL;
}

// --- cRecMenuSearchTimerDeleteConfirm ---------------------------------------------
cRecMenuSearchTimerDeleteConfirm::cRecMenuSearchTimerDeleteConfirm(cTVGuideSearchTimer searchTimer) {
    this->searchTimer = searchTimer;
    SetWidthPercent(70);
    cString message = tr("Really delete Search Timer");
    cString infoText = cString::sprintf("%s \"%s\"?", *message, searchTimer.SearchString().c_str());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("Delete only Search Timer"), rmsSearchTimerDelete, true));
    AddMenuItem(new cRecMenuItemButton(tr("Delete Search Timer and created Timers"), rmsSearchTimerDeleteWithTimers, false));
    SetFooter(new cRecMenuItemButton(tr("Cancel"), rmsClose, false));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuSearchTimerDeleteConfirm::~cRecMenuSearchTimerDeleteConfirm(void) {

}

cTVGuideSearchTimer cRecMenuSearchTimerDeleteConfirm::GetSearchTimer(void) {
    return searchTimer;
}

// --- cRecMenuSearchTimerCreateConfirm  ---------------------------------------------------------
cRecMenuSearchTimerCreateConfirm::cRecMenuSearchTimerCreateConfirm(bool success) {
    SetWidthPercent(50);
    
    cString message1a = tr("Search Timer sucessfully created.");
    cString message1b = tr ("Search Timer update initialised");
    cString message1 = cString::sprintf("%s\n%s", *message1a, *message1b);
    cString message2 = tr("Search Timer NOT sucessfully created");
    cString infoText = success?message1:message2;
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimerTemplatesCreate  ---------------------------------------------------------
cRecMenuSearchTimerTemplatesCreate::cRecMenuSearchTimerTemplatesCreate(TVGuideEPGSearchTemplate templ, cTVGuideSearchTimer searchTimer) {
    this->templ = templ;
    this->searchTimer = searchTimer;
    SetWidthPercent(70);

    cString message1 = tr("Creating Search Timer");
    cString message2 = tr("Search Term");
    cString message3 = tr("Using Template");
    
    cString infoText = cString::sprintf("%s\n%s: \"%s\"\n%s \"%s\"", *message1, *message2, searchTimer.SearchString().c_str(), *message3, templ.name.c_str());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("Display Results for Search Timer"), rmsSearchTimerTest, true));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create Search Timer"), tr("Use other Template"), rmsSearchTimerSave, rmsSearchTimerOptions, false));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}


// --- cRecMenuSearchTimerResults ---------------------------------------------------------
cRecMenuSearchTimerResults::cRecMenuSearchTimerResults(std::string searchString, const cEvent **searchResults, int numResults, std::string templateName, eRecMenuState action2) {
    this->searchResults = searchResults;
    this->action2 = action2;
    this->numResults = numResults;
    SetWidthPercent(70);
    cString message1 = "", message2 = "", message3 = "", infoText = "";
    if (action2 == rmsFavoritesRecord) {
        message1 = tr("search results for Favorite");
        message2 = tr("search result for Favorite");
    } else {
        message1 = tr("search results for Search Timer");
        message2 = tr("search result for Search Timer");
        message3 = tr("Using Template");
    }
    if (templateName.size() > 0) {
        infoText = cString::sprintf("%d %s:\n\"%s\"\n%s \"%s\"", numResults, (numResults>1)?(*message1):(*message2), searchString.c_str(), *message3, templateName.c_str());
    } else {
        infoText = cString::sprintf("%d %s:\n\"%s\"", numResults, (numResults>1)?(*message1):(*message2), searchString.c_str());
    }
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *buttons = new cRecMenuItemButton(tr("Close"), rmsClose, false);
    SetFooter(buttons);
    if (searchResults && (numResults > 0)) {
        for (int i=0; i<numResults; i++) {
            if (!AddMenuItemInitial(new cRecMenuItemEvent(searchResults[i], rmsSearchShowInfo, action2, (i==0)?true:false)))
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuItem *cRecMenuSearchTimerResults::GetMenuItem(int number) { 
    if ((number >= 0) && (number < numResults)) {
        cRecMenuItem *result = new cRecMenuItemEvent(searchResults[number], rmsSearchShowInfo, action2, false);
        return result;
    }
    return NULL;
}

int cRecMenuSearchTimerResults::GetTotalNumMenuItems(void) { 
    return numResults;
}

const cEvent *cRecMenuSearchTimerResults::GetEvent(void) {
    const cEvent *ev = NULL;
    if (cRecMenuItemEvent *activeItem = dynamic_cast<cRecMenuItemEvent*>(GetActiveMenuItem()))
        ev = activeItem->GetEventValue();
    return ev;
}

// --- cRecMenuSearchTimerNothingFound  ---------------------------------------------------------
cRecMenuSearchTimerNothingFound::cRecMenuSearchTimerNothingFound(std::string searchString) {
    SetWidthPercent(50);
    cString message = tr("Nothing found for Search String");
    cString text;
    text = cString::sprintf("%s\n\"%s\"", 
                            *message, 
                            searchString.c_str());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

/******************************************************************************************
*   SwitchTimer Menus
******************************************************************************************/

// --- cRecMenuSwitchTimer  ---------------------------------------------------------
cRecMenuSwitchTimer::cRecMenuSwitchTimer(void) {
    switchMinsBefore = 2;
    announceOnly = 0;

    SetWidthPercent(60);
    
    cString infoText = tr("Configure Options for Switchtimer");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemInt(tr("Minutes before switching"), switchMinsBefore, 0, 10, false, &switchMinsBefore));
    std::vector<std::string> switchModes;
    switchModes.push_back(tr("switch"));
    switchModes.push_back(tr("announce only"));
    switchModes.push_back(tr("ask for switch"));
    AddMenuItem(new cRecMenuItemSelect(tr("Switch Mode"), switchModes, announceOnly, false, &announceOnly));
    
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create"), tr("Cancel"), rmsSwitchTimerCreate, rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cSwitchTimer cRecMenuSwitchTimer::GetSwitchTimer(void) {
    cSwitchTimer st;
    st.switchMinsBefore = switchMinsBefore;
    st.announceOnly = announceOnly;
    return st;
}

// --- cRecMenuSwitchTimerConfirm  ---------------------------------------------------------
cRecMenuSwitchTimerConfirm::cRecMenuSwitchTimerConfirm(bool success) {
    SetWidthPercent(50);
    
    cString message1 = tr("Switch Timer sucessfully created");
    cString message2 = tr("Switch Timer NOT sucessfully created");
    cString infoText = success?message1:message2;
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSwitchTimerDelete  ---------------------------------------------------------
cRecMenuSwitchTimerDelete::cRecMenuSwitchTimerDelete(void) {
    SetWidthPercent(50);
    
    cString infoText = tr("Switch Timer deleted");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

/******************************************************************************************
*   Search Menus
******************************************************************************************/


// --- cRecMenuSearch  ---------------------------------------------------------
cRecMenuSearch::cRecMenuSearch(std::string searchString, bool withOptions) {
    strncpy(this->searchString, searchString.c_str(), TEXTINPUTLENGTH);
    mode = 0;
    channelNr = 0;
    useTitle = true;
    useSubTitle = true;
    useDescription = false;
    SetWidthPercent(60);
    cString infoText = tr("Search");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    AddMenuItem(new cRecMenuItemText(tr("Search Expression:"), this->searchString, TEXTINPUTLENGTH, false, this->searchString));
    if (withOptions) {
        std::vector<std::string> searchModes;
        cTVGuideSearchTimer searchTimer;
        searchTimer.GetSearchModes(&searchModes);
        AddMenuItemInitial(new cRecMenuItemSelect(tr("Search Mode"), searchModes, 0, false, &mode));
        AddMenuItemInitial(new cRecMenuItemChannelChooser(tr("Channel to Search"), NULL, false, &channelNr));
        AddMenuItemInitial(new cRecMenuItemBool(tr("Search in title"), true, false, false, &useTitle));
        AddMenuItemInitial(new cRecMenuItemBool(tr("Search in Subtitle"), true, false, false, &useSubTitle));
        AddMenuItemInitial(new cRecMenuItemBool(tr("Search in Description"), false, false, false, &useDescription));
    } else {
        AddMenuItemInitial(new cRecMenuItemButton(tr("Show Search Options"), rmsSearchWithOptions, false));
    }
    cRecMenuItemButtonYesNo *button = new cRecMenuItemButtonYesNo(tr("Perform Search"), tr("Cancel"), rmsSearchPerform, rmsClose, true);
    SetFooter(button);
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

Epgsearch_searchresults_v1_0 cRecMenuSearch::GetEPGSearchStruct(void) {
    Epgsearch_searchresults_v1_0 data;
    data.query = searchString;
    data.mode = mode;
    data.channelNr = channelNr;
    data.useTitle = useTitle;
    data.useSubTitle = useSubTitle;
    data.useDescription = useDescription;
    return data;
}


// --- cRecMenuSearchResults  ---------------------------------------------------------
cRecMenuSearchResults::cRecMenuSearchResults(std::string searchString, const cEvent **searchResults, int numResults) {
    this->searchResults = searchResults;
    SetWidthPercent(70);
    this->searchString = searchString;
    this->numResults = numResults;
    cString message1 = tr("search results for");
    cString message2 = tr("search result for");
    cString infoText = cString::sprintf("%d %s:\n\"%s\"", numResults, (numResults>1)?(*message1):(*message2), searchString.c_str());
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *buttons = new cRecMenuItemButtonYesNo(tr("Adapt Search"), tr("Close"), rmsSearch, rmsClose, false);
    SetFooter(buttons);
    if (searchResults && (numResults > 0)) {
        for (int i=0; i<numResults; i++) {
            if (!AddMenuItemInitial(new cRecMenuItemEvent(searchResults[i], rmsSearchShowInfo, rmsSearchRecord, (i==0)?true:false)))
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuSearchResults::GetMenuItem(int number) { 
    if ((number >= 0) && (number < numResults)) {
        cRecMenuItem *result = new cRecMenuItemEvent(searchResults[number], rmsSearchShowInfo, rmsSearchRecord, false);
        return result;
    }
    return NULL;
}

int cRecMenuSearchResults::GetTotalNumMenuItems(void) { 
    return numResults;
}

const cEvent *cRecMenuSearchResults::GetEvent(void) {
    const cEvent *ev = NULL;
    if (cRecMenuItemEvent *activeItem = dynamic_cast<cRecMenuItemEvent*>(GetActiveMenuItem()))
        ev = activeItem->GetEventValue();
    return ev;
}


// --- cRecMenuSearchConfirmTimer  ---------------------------------------------------------
cRecMenuSearchConfirmTimer::cRecMenuSearchConfirmTimer(const cEvent *event, eRecMenuState nextAction) {
    SetWidthPercent(50);
#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    const cString channelName = Channels->GetByChannelID(event->ChannelID())->Name();
#else
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
#endif
    cString message = tr("Timer created");
    cString text = cString::sprintf("%s\n%s\n%s %s - %s\n%s", 
                                    *message, 
                                    *channelName,
                                    *event->GetDateString(),
                                    *event->GetTimeString(),
                                    *event->GetEndTimeString(),
                                    event->Title()
                                );
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), nextAction, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchNothingFound  ---------------------------------------------------------
cRecMenuSearchNothingFound::cRecMenuSearchNothingFound(std::string searchString, bool tooShort) {
    SetWidthPercent(50);
    cString text;
    if (!tooShort) {
        cString message = tr("Nothing found for Search String");
        text = cString::sprintf("%s\n\"%s\"", 
                                *message, 
                                searchString.c_str());
    } else {
        cString message = tr("Search String has to have at least three letters");
        text = cString::sprintf("%s\n\"%s\"", 
                                *message, 
                                searchString.c_str());

    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsSearchNothingFoundConfirm, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

/******************************************************************************************
*   Recording Search Menus
******************************************************************************************/

// --- cRecMenuRecordingSearch  ---------------------------------------------------------
cRecMenuRecordingSearch::cRecMenuRecordingSearch(std::string search) {
    strncpy(searchString, search.c_str(), TEXTINPUTLENGTH);
    SetWidthPercent(60);
    cString infoText = tr("Search in Recordings");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    AddMenuItem(new cRecMenuItemText(tr("Search Expression:"), searchString, TEXTINPUTLENGTH, false, searchString));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Perform Search"), tr("Cancel"), rmsRecordingSearchResult, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuRecordingSearchResults  ---------------------------------------------------------
#if VDRVERSNUM >= 20301
cRecMenuRecordingSearchResults::cRecMenuRecordingSearchResults(std::string searchString, const cRecording **searchResults, int numResults) {
#else
cRecMenuRecordingSearchResults::cRecMenuRecordingSearchResults(std::string searchString, cRecording **searchResults, int numResults) {
#endif
    this->searchString = searchString;
    this->searchResults = searchResults;
    SetWidthPercent(80);
    cString message1 = tr("Found");
    cString message2 = tr("recording");
    cString message3 = tr("recordings");
    cString message4 = tr("for");
    this->numResults = numResults;
    cString infoText = cString::sprintf("%s %d %s %s:\n\"%s\"", *message1, numResults, (numResults>1)?(*message3):(*message2), *message4, searchString.c_str());
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText, true);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *buttons = new cRecMenuItemButtonYesNo(tr("Adapt Search"), tr("Close"), rmsRecordingSearch, rmsClose, false);
    SetFooter(buttons);
    if (searchResults && (numResults > 0)) {
        for (int i=0; i<numResults; i++) {
            if (!AddMenuItemInitial(new cRecMenuItemRecording(searchResults[i], (i==0)?true:false)))
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuRecordingSearchResults::GetMenuItem(int number) { 
    if ((number >= 0) && (number < numResults)) {
        cRecMenuItem *result = new cRecMenuItemRecording(searchResults[number], false);
        return result;
    }
    return NULL;
}

int cRecMenuRecordingSearchResults::GetTotalNumMenuItems(void) { 
    return numResults;
}

// --- cRecMenuRecordingSearchNotFound  ---------------------------------------------------------
cRecMenuRecordingSearchNotFound::cRecMenuRecordingSearchNotFound(std::string searchString) {
    SetWidthPercent(50);
    cString message = tr("No recordings found for");
    cString text = cString::sprintf("%s\n\"%s\"", 
                                    *message, 
                                    searchString.c_str());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

/******************************************************************************************
*   Timeline
******************************************************************************************/

// --- cRecMenuTimeline  ---------------------------------------------------------
cRecMenuTimeline::cRecMenuTimeline(cTVGuideTimerConflicts *timerConflicts) {
    this->timerConflicts = timerConflicts;
    SetStartStop();
    conflictsToday = timerConflicts->GetConflictsBetween(timeStart, timeStop);
    GetTimersForDay();
    SetWidthPercent(95);
    header = new cRecMenuItemTimelineHeader(timeStart, conflictsToday);
    SetHeader(header);
    cRecMenuItem *footer = new cRecMenuItemButton(tr("Close"), rmsClose, false, true);
    SetFooter(footer);
    SetTimers();
}

void cRecMenuTimeline::SetStartStop(void) {
    time_t now = time(0);
    tm *timeStruct = localtime(&now);
    timeStart = now - timeStruct->tm_hour * 3600 - timeStruct->tm_min * 60 - timeStruct->tm_sec;
    today = timeStart;
    timeStop = timeStart + 24*3600 - 1;
}


void cRecMenuTimeline::GetTimersForDay(void) {
    timersToday.clear();
#if VDRVERSNUM >= 20301
    LOCK_TIMERS_READ;
    const cTimers* timers = Timers;
    for (const cTimer *t = Timers->First(); t; t = Timers->Next(t)) {
#else
    for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
#endif
        if (((t->StartTime() > timeStart) && (t->StartTime() <= timeStop)) || ((t->StopTime() > timeStart) && (t->StopTime() <= timeStop))) {
            timersToday.push_back(t);
        }
    }
    numTimersToday = timersToday.size();
}

void cRecMenuTimeline::SetTimers(void) {
    ClearMenu();
    if (numTimersToday == 0) {
        AddMenuItem(new cRecMenuItemTimelineTimer(NULL, 0, 0, conflictsToday, header, false));
        header->UnsetCurrentTimer();
        footer->setActive();
    } else {
        for (int i=0; i<numTimersToday; i++) {
            cRecMenuItemTimelineTimer *item = new cRecMenuItemTimelineTimer(timersToday[i], timeStart, timeStop, conflictsToday, header, false);
            if (i==0)
                item->setActive();
            if (!AddMenuItemInitial(item))
                break;
        }
        footer->setInactive();
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

void cRecMenuTimeline::PrevDay(void) {
    if ((timeStart - 3600*24) < today)
        return;
    timeStart -= 3600*24;
    timeStop -= 3600*24;
    conflictsToday = timerConflicts->GetConflictsBetween(timeStart, timeStop);
    SetWidthPercent(95);
    header->SetDay(timeStart);
    header->UnsetCurrentTimer();
    header->RefreshTimerDisplay();
    GetTimersForDay();
    SetTimers();
    Display();
}

void cRecMenuTimeline::NextDay(void) {
    timeStart += 3600*24;
    timeStop += 3600*24;
    conflictsToday = timerConflicts->GetConflictsBetween(timeStart, timeStop);
    SetWidthPercent(95);
    header->SetDay(timeStart);
    header->UnsetCurrentTimer();
    header->RefreshTimerDisplay();
    GetTimersForDay();
    SetTimers();
    Display();
}

cRecMenuItem *cRecMenuTimeline::GetMenuItem(int number) { 
    if (number < 0)
        return NULL;
    if (number >= numTimersToday)
        return NULL;
    return new cRecMenuItemTimelineTimer(timersToday[number], timeStart, timeStop, conflictsToday, header, false);
}

int cRecMenuTimeline::GetTotalNumMenuItems(void) { 
    return numTimersToday;
}

void cRecMenuTimeline::ClearMenu(void) {
    InitMenu(true);
    header->UnsetCurrentTimer();
}

#if VDRVERSNUM >= 20301
const cTimer *cRecMenuTimeline::GetTimer(void) {
#else
cTimer *cRecMenuTimeline::GetTimer(void) {
#endif
    if (cRecMenuItemTimelineTimer *activeItem = dynamic_cast<cRecMenuItemTimelineTimer*>(GetActiveMenuItem()))
        return activeItem->GetTimerValue();
    return NULL;
}


eRecMenuState cRecMenuTimeline::ProcessKey(eKeys Key) {
    eRecMenuState state = rmsContinue;
    switch (Key & ~k_Repeat) {
        case kLeft:
            PrevDay();
            state = rmsConsumed;
            break;
        case kRight:
            NextDay();
            state = rmsConsumed;
            break;
        default:
            break;
    }
    if (state != rmsConsumed) {
        state = cRecMenu::ProcessKey(Key);
    }
    return state;
}

/******************************************************************************************
*   Favorites
******************************************************************************************/

// --- cRecMenuFavorites  ---------------------------------------------------------

cRecMenuFavorites::cRecMenuFavorites(std::vector<cTVGuideSearchTimer> favorites) {
    deleteMenuItems = false;
    this->favorites = favorites;
    CreateFavoritesMenuItems();
    numFavorites = myMenuItems.size();
    SetWidthPercent(70);
    cString header;
    if (numFavorites > 0) {
        header = tr("Favorites");
    } else {
        header = tr("No Favorites available");
    }
    cRecMenuItemInfo *headerItem = new cRecMenuItemInfo(*header, true);
    headerItem->CalculateHeight(width - 2 * border);
    SetHeader(headerItem);

    cRecMenuItem *button = new cRecMenuItemButton(tr("Close"), rmsClose, (numFavorites==0)?true:false);
    SetFooter(button);

    for (int i=0; i<numFavorites; i++) {
        if (i==0)
            myMenuItems[i]->setActive();
        if (!AddMenuItemInitial(myMenuItems[i]))
            break;
    }

    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuFavorites::~cRecMenuFavorites(void) {
    for (std::vector<cRecMenuItem*>::iterator it = myMenuItems.begin(); it != myMenuItems.end(); it++) {
        delete *it;
    }
    myMenuItems.clear();
}

void cRecMenuFavorites::CreateFavoritesMenuItems(void) {
    if (tvguideConfig.favWhatsOnNow) {
        myMenuItems.push_back(new cRecMenuItemFavoriteStatic(tr("What's on now"), rmsFavoritesNow, false));
    }
    if (tvguideConfig.favWhatsOnNext) {
        myMenuItems.push_back(new cRecMenuItemFavoriteStatic(tr("What's on next"), rmsFavoritesNext, false));
    }
    if (tvguideConfig.favUseTime1) {
        std::string desc = *cString::sprintf("%s (%s)", tvguideConfig.descUser1.c_str(), NiceTime(tvguideConfig.favTime1).c_str());
        myMenuItems.push_back(new cRecMenuItemFavoriteStatic(desc, rmsFavoritesUser1, false));
    }
    if (tvguideConfig.favUseTime2) {
        std::string desc = *cString::sprintf("%s (%s)", tvguideConfig.descUser2.c_str(), NiceTime(tvguideConfig.favTime2).c_str());
        myMenuItems.push_back(new cRecMenuItemFavoriteStatic(desc, rmsFavoritesUser2, false));
    }
    if (tvguideConfig.favUseTime3) {
        std::string desc = *cString::sprintf("%s (%s)", tvguideConfig.descUser3.c_str(), NiceTime(tvguideConfig.favTime3).c_str());
        myMenuItems.push_back(new cRecMenuItemFavoriteStatic(desc, rmsFavoritesUser3, false));
    }
    if (tvguideConfig.favUseTime4) {
        std::string desc = *cString::sprintf("%s (%s)", tvguideConfig.descUser4.c_str(), NiceTime(tvguideConfig.favTime4).c_str());
        myMenuItems.push_back(new cRecMenuItemFavoriteStatic(desc, rmsFavoritesUser4, false));
    }

    int numAdditionalFavs = favorites.size();
    for (int i = 0; i < numAdditionalFavs; i++) {
        myMenuItems.push_back(new cRecMenuItemFavorite(favorites[i], rmsSearchTimerTest, false));
    }

}

std::string cRecMenuFavorites::NiceTime(int favTime) {
    int hours = favTime/100;
    int mins = favTime - hours * 100;
    return *cString::sprintf("%02d:%02d", hours, mins);
}

cRecMenuItem *cRecMenuFavorites::GetMenuItem(int number) {
    if (number > -1 && number < numFavorites)
        return myMenuItems[number];
    return NULL;
}

int cRecMenuFavorites::GetTotalNumMenuItems(void) {
    return numFavorites;
}

cTVGuideSearchTimer cRecMenuFavorites::GetFavorite(void) {
    cRecMenuItemFavorite *activeItem = dynamic_cast<cRecMenuItemFavorite*>(GetActiveMenuItem());
    return activeItem->GetFavorite();
}