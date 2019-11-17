#include "recmenu.h" 
#include "recmenus.h"
#include "switchtimer.h"
#include "timerconflict.h"
#include "recmenumanager.h"

cRecMenuManager::cRecMenuManager(void) {
    footer = NULL;
    active = false;
    activeMenu = NULL;
    activeMenuBuffer = NULL;
    activeMenuBuffer2 = NULL;
    recManager = new cRecManager();
    recManager->SetEPGSearchPlugin();
    timerConflicts = NULL;
    detailViewActive = false;
}

cRecMenuManager::~cRecMenuManager(void) {
    if (activeMenu) {
        active = false;
        delete activeMenu;
        activeMenu = NULL;
    }
    if (timerConflicts) {
        delete timerConflicts;
        timerConflicts = NULL;
    }
    delete recManager;
}

void cRecMenuManager::Start(const cEvent *event) {
    active = true;
    activeMenuBuffer = NULL;
    detailViewActive = false;
    SetBackground();
    this->event = event;
    activeMenu = new cRecMenuMain(recManager->EpgSearchAvailable(), recManager->CheckEventForTimer(event), SwitchTimers.EventInSwitchList(event));
    activeMenu->Display();
    osdManager.flush();
}

void cRecMenuManager::StartFavorites(void) {
    active = true;
    activeMenuBuffer = NULL;
    detailViewActive = false;
    SetBackground();
    std::vector<cTVGuideSearchTimer> favorites;
    recManager->GetFavorites(&favorites);
    activeMenu = new cRecMenuFavorites(favorites);
    activeMenu->Display();
    osdManager.flush();
}


void cRecMenuManager::Close(void) {
    event = NULL;
    active = false;
    if (activeMenu) {
        delete activeMenu;
        activeMenu = NULL;
    }
    if (timerConflicts) {
        delete timerConflicts;
        timerConflicts = NULL;
    }
    DeleteBackground();
}

void cRecMenuManager::SetBackground(void) {
    int backgroundWidth = geoManager.osdWidth;
    int backgroundHeight = geoManager.osdHeight;
    pixmapBackground = osdManager.requestPixmap(3, cRect(0, 0, backgroundWidth, backgroundHeight));
    pixmapBackground->Fill(theme.Color(clrRecMenuBackground));
    if (tvguideConfig.scaleVideo) {
        int tvHeight = geoManager.statusHeaderHeight;
        int tvWidth = tvHeight * 16 / 9;
        int tvX = geoManager.osdWidth - tvWidth;
        pixmapBackground->DrawRectangle(cRect(tvX, 0, tvWidth, tvHeight), clrTransparent);
    }
}

void cRecMenuManager::DeleteBackground(void) {
    osdManager.releasePixmap(pixmapBackground);
}

eOSState cRecMenuManager::StateMachine(eRecMenuState nextState) {
    eOSState state = osContinue;
    switch (nextState) {
        /*************************************************************************************** 
        *    INSTANT RECORDING 
        ****************************************************************************************/
        case rmsInstantRecord: {
        //caller: main menu or folder chooser
        //Creating timer for active Event, if no conflict, confirm and exit
            std::string recFolder = "";
            if (cRecMenuAskFolder *menu = dynamic_cast<cRecMenuAskFolder*>(activeMenu)) {
                recFolder = menu->GetFolder();
            }
            delete activeMenu;
            cTimer *timer = recManager->createTimer(event, recFolder);
            if (!DisplayTimerConflict(timer)) {
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            }
            break; }
        case rmsInstantRecordFolder:
        //caller: main menu
        //Asking for Folder
            delete activeMenu;
            activeMenu = new cRecMenuAskFolder(event, rmsInstantRecord);
            activeMenu->Display();
            break;
        case rmsIgnoreTimerConflict:
        //caller: cRecMenuTimerConflict
        //Confirming created Timer
            delete activeMenu;
            activeMenu = new cRecMenuConfirmTimer(event);
            activeMenu->Display();
            break;
        case rmsTimerConflictShowInfo: {
            //caller: cRecMenuTimerConflict
            int timerIndex;
            if (cRecMenuTimerConflict *menu = dynamic_cast<cRecMenuTimerConflict*>(activeMenu)) {
                timerIndex = menu->GetTimerConflictIndex();
            } else break;
            int timerID = timerConflicts->GetCurrentConflictTimerID(timerIndex);
            cTimer *t = Timers.Get(timerID);
            if (t) {
                const cEvent *ev = t->Event();
                if (ev) {
                    DisplayDetailedView(ev);
                }
            }
            break; }
        case rmsDeleteTimerConflictMenu: {
            //caller: cRecMenuTimerConflict
            //delete timer out of current timer conflict
            int timerIndex;
            if (cRecMenuTimerConflict *menu = dynamic_cast<cRecMenuTimerConflict*>(activeMenu)) {
                timerIndex = menu->GetTimerConflictIndex();
            } else break;
            int timerID = timerConflicts->GetCurrentConflictTimerID(timerIndex);
            recManager->DeleteTimer(timerID);
            delete activeMenu;
            if (!DisplayTimerConflict(timerID)) {
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            }
            break; }
        case rmsEditTimerConflictMenu: {
            //caller: cRecMenuTimerConflict
            //edit timer out of current timer conflict
            int timerIndex;
            if (cRecMenuTimerConflict *menu = dynamic_cast<cRecMenuTimerConflict*>(activeMenu)) {
                timerIndex = menu->GetTimerConflictIndex();
            } else break;
            int timerID = timerConflicts->GetCurrentConflictTimerID(timerIndex);
            cTimer *timer = Timers.Get(timerID);
            if (timer) {
                delete activeMenu;
                activeMenu = new cRecMenuEditTimer(timer, rmsSaveTimerConflictMenu);
                activeMenu->Display();
            }
            break; }
        case rmsSaveTimerConflictMenu: {
            //caller: cRecMenuEditTimer
            //save timer from current timer conflict
            cTimer timerModified;
            cTimer *originalTimer;
            if (cRecMenuEditTimer *menu = dynamic_cast<cRecMenuEditTimer*>(activeMenu)) {
                timerModified = menu->GetTimer();
                originalTimer = menu->GetOriginalTimer();
            } else break;
            recManager->SaveTimer(originalTimer, timerModified);
            delete activeMenu;
            if (!DisplayTimerConflict(originalTimer)) {
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            }
            break; }
        case rmsDeleteTimer:
            //caller: main menu
            //delete timer for active event
            delete activeMenu;
            if (recManager->IsRecorded(event)) {
                activeMenu = new cRecMenuAskDeleteTimer(event);
            } else {
                recManager->DeleteTimer(event);
                activeMenu = new cRecMenuConfirmDeleteTimer(event);
            }
            activeMenu->Display();
            break;
        case rmsDeleteTimerConfirmation:
            //delete running timer for active event
            recManager->DeleteTimer(event);
            delete activeMenu;
            activeMenu = new cRecMenuConfirmDeleteTimer(event);
            activeMenu->Display();
            break;
        case rmsEditTimer: {
            //edit timer for active event
            cTimer *timer = recManager->GetTimerForEvent(event);
            if (timer) {
                delete activeMenu;
                activeMenu = new cRecMenuEditTimer(timer, rmsSaveTimer);
                activeMenu->Display();
            }
            break; }
        case rmsSaveTimer: {
            //caller: cRecMenuEditTimer
            //save timer for active event
            cTimer timerModified;
            cTimer *originalTimer;
            if (cRecMenuEditTimer *menu = dynamic_cast<cRecMenuEditTimer*>(activeMenu)) {
                timerModified = menu->GetTimer();
                originalTimer = menu->GetOriginalTimer();
            } else break;
            recManager->SaveTimer(originalTimer, timerModified);
            state = osEnd;
            Close();
            break; }
        /*************************************************************************************** 
        *    SERIES TIMER
        ****************************************************************************************/
        case rmsSeriesTimer: {
            //caller: main menu oder folder chooser
            std::string recFolder = "";
            if (cRecMenuAskFolder *menu = dynamic_cast<cRecMenuAskFolder*>(activeMenu)) {
                recFolder = menu->GetFolder();
            }
            delete activeMenu;
            cChannel *channel = Channels.GetByChannelID(event->ChannelID());
            activeMenu = new cRecMenuSeriesTimer(channel, event, recFolder);
            activeMenu->Display();
            break; }
        case rmsSeriesTimerFolder:
            //caller: main menu
            //Asking for Folder
            delete activeMenu;
            activeMenu = new cRecMenuAskFolder(event, rmsSeriesTimer);
            activeMenu->Display();
            break;
        case rmsSeriesTimerCreate: {
            //caller: cRecMenuSeriesTimer
            cTimer *seriesTimer;
            if (cRecMenuSeriesTimer *menu = dynamic_cast<cRecMenuSeriesTimer*>(activeMenu)) {
                seriesTimer = menu->GetTimer();
            } else break;
            recManager->CreateSeriesTimer(seriesTimer);
            delete activeMenu;
            activeMenu = new cRecMenuConfirmSeriesTimer(seriesTimer);
            activeMenu->Display();
            break; }
        /********************************************************************************************** 
         *    SEARCH TIMER 
         ***********************************************************************************************/
        case rmsSearchTimer:
            //Caller: main menu
            //set search String for search timer
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimer(event);
            activeMenu->Display();
            break;
        case rmsSearchTimerOptions: {
            //Caller: cRecMenuSearchTimer, cRecMenuSearchTimerTemplates
            //Choose to set options manually or by template
            std::string searchString;
            cTVGuideSearchTimer searchTimer;
            bool reload = false;
            if (cRecMenuSearchTimer *menu = dynamic_cast<cRecMenuSearchTimer*>(activeMenu)) {
                searchString = menu->GetSearchString();
            } else if (cRecMenuSearchTimerTemplatesCreate *menu = dynamic_cast<cRecMenuSearchTimerTemplatesCreate*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
                reload = true;
            } else break;
            delete activeMenu;
            if (searchString.size() < 4) {
                activeMenu = new cRecMenuSearchTimer(event);
            } else { 
                if (!reload) {
                    searchTimer.SetSearchString(searchString);
                }
                std::vector<TVGuideEPGSearchTemplate> epgSearchTemplates;
                recManager->ReadEPGSearchTemplates(&epgSearchTemplates);
                if (epgSearchTemplates.size() > 0) {
                    activeMenu = new cRecMenuSearchTimerTemplates(searchTimer, epgSearchTemplates);
                } else {
                    activeMenu = new cRecMenuSearchTimerEdit(searchTimer, false);
                }
            }
            activeMenu->Display();
            break; }
        case rmsSearchTimers: {
            //caller: main menu
            DisplaySearchTimerList();
            break; }
        case rmsSearchTimerEdit:
        case rmsSearchTimerEditAdvanced: {
            //caller: cRecMenuSearchTimers, cRecMenuSearchTimerEdit, cRecMenuSearchTimerTemplates
            cTVGuideSearchTimer searchTimer;
            bool advancedOptions = false;
            if (cRecMenuSearchTimers *menu = dynamic_cast<cRecMenuSearchTimers*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
            } else if (cRecMenuSearchTimerEdit *menu = dynamic_cast<cRecMenuSearchTimerEdit*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
                advancedOptions = (nextState == rmsSearchTimerEditAdvanced)?true:false;
            } else if (cRecMenuSearchTimerTemplates *menu = dynamic_cast<cRecMenuSearchTimerTemplates*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
            } else break;
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimerEdit(searchTimer, advancedOptions);
            activeMenu->Display();
            break; }
        case rmsSearchTimerTest: {
            //caller: cRecMenuSearchTimerEdit, cRecMenuSearchTimerTemplatesCreate, cRecMenuSearchTimers, cRecMenuFavorites
            //show results of currently choosen search timer
            cTVGuideSearchTimer searchTimer;
            eRecMenuState recState = rmsDisabled;
            if (cRecMenuSearchTimerEdit *menu = dynamic_cast<cRecMenuSearchTimerEdit*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
                recState = rmsSearchTimerRecord;
            } else if  (cRecMenuSearchTimers *menu = dynamic_cast<cRecMenuSearchTimers*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
                recState = rmsSearchTimerRecord;
            } else if (cRecMenuSearchTimerTemplatesCreate *menu = dynamic_cast<cRecMenuSearchTimerTemplatesCreate*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
                TVGuideEPGSearchTemplate tmpl = menu->GetTemplate();
                searchTimer.SetTemplate(tmpl.templValue);
                searchTimer.Parse(true);
            } else if (cRecMenuFavorites *menu = dynamic_cast<cRecMenuFavorites*>(activeMenu)) {
                searchTimer = menu->GetFavorite();
                recState = rmsFavoritesRecord;
            } else break;
            int numSearchResults = 0;
            std::string searchString = searchTimer.BuildSearchString();
            const cEvent **searchResult = recManager->PerformSearchTimerSearch(searchString, numSearchResults);
            if (numSearchResults) {
                activeMenuBuffer = activeMenu;
                activeMenuBuffer->Hide();
                activeMenu = new cRecMenuSearchTimerResults(searchTimer.SearchString(), searchResult, numSearchResults, "", recState);
                activeMenu->Display();
            } else {
               activeMenuBuffer = activeMenu;
               activeMenuBuffer->Hide();
               activeMenu = new cRecMenuSearchTimerNothingFound(searchTimer.SearchString());
               activeMenu->Display();
            }
            break; }
        case rmsSearchTimerSave: {
            //caller: cRecMenuSearchTimerEdit, cRecMenuSearchTimerTemplatesCreate
            //create new or modify existing search timer
            cTVGuideSearchTimer searchTimer;
            if (cRecMenuSearchTimerEdit *menu = dynamic_cast<cRecMenuSearchTimerEdit*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
            } else if (cRecMenuSearchTimerTemplatesCreate *menu = dynamic_cast<cRecMenuSearchTimerTemplatesCreate*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
                TVGuideEPGSearchTemplate tmpl = menu->GetTemplate();
                searchTimer.SetTemplate(tmpl.templValue);
                searchTimer.Parse(true);
            } else break;
            bool success = recManager->SaveSearchTimer(&searchTimer);
            recManager->UpdateSearchTimers();
            if (searchTimer.GetID() >= 0) {
                //Timer modified, show list
                DisplaySearchTimerList();
            } else {
                //new timer, confirm
                delete activeMenu;
                activeMenu = new cRecMenuSearchTimerCreateConfirm(success);
                activeMenu->Display();
            }
            break; }
        case rmsSearchTimerCreateWithTemplate: {
            //caller: cRecMenuSearchTimerTemplates
            //create new search timer from template
            TVGuideEPGSearchTemplate templ;
            cTVGuideSearchTimer searchTimer;
            if (cRecMenuSearchTimerTemplates *menu = dynamic_cast<cRecMenuSearchTimerTemplates*>(activeMenu)) {
                templ = menu->GetTemplate();
                searchTimer = menu->GetSearchTimer();
            } else break;
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimerTemplatesCreate(templ, searchTimer);
            activeMenu->Display();
            break; }
        case rmsSearchTimerDeleteConfirm: {
            //caller: cRecMenuSearchTimers
            //Ask for confirmation and if timers created by this search timer should alo be deleted
            cTVGuideSearchTimer searchTimer;
            if (cRecMenuSearchTimers *menu = dynamic_cast<cRecMenuSearchTimers*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
            } else break;
            activeMenuBuffer = activeMenu;
            activeMenuBuffer->Hide();
            activeMenu = new cRecMenuSearchTimerDeleteConfirm(searchTimer);
            activeMenu->Display();
            break; }
        case rmsSearchTimerDelete: 
        case rmsSearchTimerDeleteWithTimers: {
            //caller: cRecMenuSearchTimerDeleteConfirm
            //actually delete searchtimer
            cTVGuideSearchTimer searchTimer;
            if (cRecMenuSearchTimerDeleteConfirm *menu = dynamic_cast<cRecMenuSearchTimerDeleteConfirm*>(activeMenu)) {
                searchTimer = menu->GetSearchTimer();
            } else break;
            bool delTimers = (nextState==rmsSearchTimerDeleteWithTimers)?true:false;
            recManager->DeleteSearchTimer(&searchTimer, delTimers);
            delete activeMenuBuffer;
            activeMenuBuffer = NULL;
            DisplaySearchTimerList();
            break; }
        case rmsSearchTimerRecord: {
            //caller: cRecMenuSearchTimerResults
            const cEvent *ev = NULL;
            if (cRecMenuSearchTimerResults *menu = dynamic_cast<cRecMenuSearchTimerResults*>(activeMenu)) {
                ev = menu->GetEvent();
            } else break;
            if (!ev)
                break;
            recManager->createTimer(ev, "");
            activeMenuBuffer2 = activeMenu;
            activeMenuBuffer2->Hide();
            activeMenu = new cRecMenuSearchConfirmTimer(ev, rmsFavoritesRecordConfirm);
            activeMenu->Display();
            break; }
        /********************************************************************************************** 
         *    SWITCH TIMER 
         ***********************************************************************************************/
        case rmsSwitchTimer:
            delete activeMenu;
            activeMenu = new cRecMenuSwitchTimer();
            activeMenu->Display();
            break;
        case rmsSwitchTimerCreate: {
            cSwitchTimer switchTimer;
            if (cRecMenuSwitchTimer *menu = dynamic_cast<cRecMenuSwitchTimer*>(activeMenu)) {
                switchTimer = menu->GetSwitchTimer();
            } else break;
            bool success = recManager->CreateSwitchTimer(event, switchTimer);
            delete activeMenu;
            activeMenu = new cRecMenuSwitchTimerConfirm(success);
            activeMenu->Display();
            break; }
        case rmsSwitchTimerDelete: 
            recManager->DeleteSwitchTimer(event);
            delete activeMenu;
            activeMenu = new cRecMenuSwitchTimerDelete();
            activeMenu->Display();
            break;
        /********************************************************************************************** 
         *    RECORDINGS SEARCH 
         ***********************************************************************************************/
        case rmsRecordingSearch: {
            //caller: main menu or rmsRecordingSearchResult
            std::string searchString = event->Title();
            if (cRecMenuRecordingSearchResults *menu = dynamic_cast<cRecMenuRecordingSearchResults*>(activeMenu)) {
                searchString = menu->GetSearchString();
            };
            delete activeMenu;
            activeMenu = new cRecMenuRecordingSearch(searchString);
            activeMenu->Display();
            break; }
        case rmsRecordingSearchResult:  {
            //caller: cRecMenuRecordingSearch
            std::string searchString;
            if (cRecMenuRecordingSearch *menu = dynamic_cast<cRecMenuRecordingSearch*>(activeMenu)) {
                searchString = menu->GetSearchString();
            } else break;
            delete activeMenu;
            if (searchString.size() < 4) {
                activeMenu = new cRecMenuRecordingSearch(searchString);
            } else {
                int numSearchResults = 0;
                cRecording **searchResult = recManager->SearchForRecordings(searchString, numSearchResults);
                if (numSearchResults == 0) {
                    activeMenu = new cRecMenuRecordingSearchNotFound(searchString);
                } else {
                    activeMenu = new cRecMenuRecordingSearchResults(searchString, searchResult, numSearchResults);
                }
            }
            activeMenu->Display();
            break; }
        /********************************************************************************************** 
         *    SEARCH 
         ***********************************************************************************************/
        case rmsSearch:
        case rmsSearchWithOptions: {
            //caller: main menu, cRecMenuSearch, cRecMenuSearchResults
            bool withOptions = false;
            std::string searchString = event->Title();
            if (cRecMenuSearch *menu = dynamic_cast<cRecMenuSearch*>(activeMenu)) {
                withOptions = true;
                searchString = menu->GetSearchString();
            } else if (cRecMenuSearchResults *menu = dynamic_cast<cRecMenuSearchResults*>(activeMenu)) {
                searchString = menu->GetSearchString();
            }
            delete activeMenu;
            activeMenu = new cRecMenuSearch(searchString, withOptions);
            activeMenu->Display();
            break; }
        case rmsSearchPerform: {
            //caller: cRecMenuSearch
            Epgsearch_searchresults_v1_0 epgSearchData;
            if (cRecMenuSearch *menu = dynamic_cast<cRecMenuSearch*>(activeMenu)) {
                epgSearchData = menu->GetEPGSearchStruct();
            } else break;
            std::string searchString = epgSearchData.query;
            if (searchString.size() < 3) {
                activeMenuBuffer = activeMenu;
                activeMenuBuffer->Hide();
                activeMenu = new cRecMenuSearchNothingFound(searchString, true);
            } else {
                int numSearchResults = 0;
                const cEvent **searchResult = recManager->PerformSearch(epgSearchData, numSearchResults);
                if (searchResult) {
                    delete activeMenu;
                    activeMenu = new cRecMenuSearchResults(searchString, searchResult, numSearchResults);
                } else {
                    activeMenuBuffer = activeMenu;
                    activeMenuBuffer->Hide();
                    activeMenu = new cRecMenuSearchNothingFound(searchString);
                }
            }
            activeMenu->Display();
            break; }
        case rmsSearchNothingFoundConfirm:
            delete activeMenu;
            activeMenu = activeMenuBuffer;
            activeMenuBuffer = NULL;
            activeMenu->Show();
            break;
        case rmsSearchShowInfo: {
            //caller: cRecMenuSearchResults, cRecMenuSearchTimerResults
            const cEvent *ev = NULL;
            if (cRecMenuSearchResults *menu = dynamic_cast<cRecMenuSearchResults*>(activeMenu)) {
                ev = menu->GetEvent();
            } else if (cRecMenuSearchTimerResults *menu = dynamic_cast<cRecMenuSearchTimerResults*>(activeMenu)) {
                ev = menu->GetEvent();
            } else break;
            if (ev) {
               DisplayDetailedView(ev);
            }
            break;}
        case rmsSearchRecord: {
            //caller: cRecMenuSearchResults
            const cEvent *ev = NULL;
            if (cRecMenuSearchResults *menu = dynamic_cast<cRecMenuSearchResults*>(activeMenu)) {
                ev = menu->GetEvent();
            } else break;
            if (!ev)
                break;
            recManager->createTimer(ev, "");
            activeMenuBuffer = activeMenu;
            activeMenuBuffer->Hide();
            activeMenu = new cRecMenuSearchConfirmTimer(ev, rmsSearchRecordConfirm);
            activeMenu->Display();
            break;}
        case rmsSearchRecordConfirm:
            delete activeMenu;
            activeMenu = activeMenuBuffer;
            activeMenuBuffer = NULL;
            activeMenu->UpdateActiveMenuItem();
            activeMenu->Show();
            break;
        /********************************************************************************************** 
         *    CHECK FOR TIMER CONFLICTS 
         ***********************************************************************************************/
        case rmsTimerConflicts: {
            //caller: main menu
            //Show timer conflict
            if (timerConflicts) {
                delete timerConflicts;
            }
            timerConflicts = recManager->CheckTimerConflict();
            delete activeMenu;
            int numConflicts = timerConflicts->NumConflicts();
            if (numConflicts > 0) {
                activeMenu = new cRecMenuTimerConflicts(timerConflicts);
            } else {
                activeMenu = new cRecMenuNoTimerConflict();
            }
            activeMenu->Display();
            break; }
        case rmsTimerConflict: {
            //caller: cRecMenuTimerConflicts
            //Show timer conflict
            if (!timerConflicts)
                break;
            int timerConflict;
            if (cRecMenuTimerConflicts *menu = dynamic_cast<cRecMenuTimerConflicts*>(activeMenu)) {
                timerConflict = menu->GetTimerConflict();
            } else break;
            timerConflicts->SetCurrentConflict(timerConflict);
            delete activeMenu;
            activeMenu = new cRecMenuTimerConflict(timerConflicts->GetCurrentConflict());
            activeMenu->Display();
            break; }
        case rmsSearchRerunsTimerConflictMenu: {
            //caller: cRecMenuTimerConflict
            //Show reruns for timer from timer conflict
            if (!timerConflicts)
                break;
            int timerConflict;
            if (cRecMenuTimerConflict *menu = dynamic_cast<cRecMenuTimerConflict*>(activeMenu)) {
                timerConflict = menu->GetTimerConflictIndex();
            } else break;
            int timerID = timerConflicts->GetCurrentConflictTimerID(timerConflict);
            cTimer *timer = Timers.Get(timerID);
            if (timer) {
                const cEvent *event = timer->Event();
                if (event) {
                    int numReruns = 0;
                    const cEvent **reruns = recManager->LoadReruns(event, numReruns);
                    if (reruns && (numReruns > 0)) {
                        activeMenuBuffer = activeMenu;
                        activeMenuBuffer->Hide();
                        activeMenu = new cRecMenuRerunResults(event, reruns, numReruns);
                        activeMenu->Display();
                    } else {
                        activeMenuBuffer = activeMenu;
                        activeMenuBuffer->Hide();
                        activeMenu = new cRecMenuNoRerunsFound((event->Title())?event->Title():"");
                        activeMenu->Display();
                    }
                }
            }
            break; }
        case rmsTimerConflictIgnoreReruns: {
            delete activeMenu;
            activeMenu = activeMenuBuffer;
            activeMenuBuffer = NULL;
            activeMenu->Show();
            break; }
        case rmsTimerConflictRecordRerun: {
            //caller: cRecMenuRerunResults
            //buffer: cRecMenuTimerConflict
            if (!activeMenuBuffer)
                break;
            if (!timerConflicts)
                break;
            const cEvent *replace;
            int originalConflictIndex;
            if (cRecMenuRerunResults *menu = dynamic_cast<cRecMenuRerunResults*>(activeMenu)) {
                replace = menu->GetRerunEvent();
            } else break;
            if (cRecMenuTimerConflict *menu = dynamic_cast<cRecMenuTimerConflict*>(activeMenuBuffer)) {
                originalConflictIndex = menu->GetTimerConflictIndex();
            } else break;
            int originalTimerID = timerConflicts->GetCurrentConflictTimerID(originalConflictIndex);
            cTimer *timerOriginal = Timers.Get(originalTimerID);
            if (replace && timerOriginal) {
                recManager->DeleteTimer(timerOriginal->Event());
                recManager->createTimer(replace);
                delete activeMenu;
                if (activeMenuBuffer) {
                    delete activeMenuBuffer;
                    activeMenuBuffer = NULL;
                }
                activeMenu = new cRecMenuConfirmRerunUsed(timerOriginal->Event(), replace);
                activeMenu->Display();
            }
            break; }
        /********************************************************************************************** 
         *    TIMELINE
         ***********************************************************************************************/
       case rmsTimeline: {
            if (timerConflicts) {
                delete timerConflicts;
            }
            timerConflicts = recManager->CheckTimerConflict();
            delete activeMenu;
            activeMenu = new cRecMenuTimeline(timerConflicts);
            activeMenu->Display();
            break; } 
        case rmsTimelineTimerEdit: {
            cTimer *timer;
            if (cRecMenuTimeline *menu = dynamic_cast<cRecMenuTimeline*>(activeMenu)) {
                timer = menu->GetTimer();
            } else break;
            if (timer) {
                delete activeMenu;
                activeMenu = new cRecMenuEditTimer(timer, rmsTimelineTimerSave);
                activeMenu->Display();
            }
            break;}
        case rmsTimelineTimerSave: {
            cTimer timerModified;
            cTimer *originalTimer;
            if (cRecMenuEditTimer *menu = dynamic_cast<cRecMenuEditTimer*>(activeMenu)) {
                timerModified = menu->GetTimer();
                originalTimer = menu->GetOriginalTimer();
            } else break;
            recManager->SaveTimer(originalTimer, timerModified);
            delete activeMenu;
            if (timerConflicts) {
                delete timerConflicts;
            }
            timerConflicts = recManager->CheckTimerConflict();
            activeMenu = new cRecMenuTimeline(timerConflicts);
            activeMenu->Display();
            break; }
        case rmsTimelineTimerDelete: {
            cTimer *timer;
            if (cRecMenuEditTimer *menu = dynamic_cast<cRecMenuEditTimer*>(activeMenu)) {
                timer = menu->GetOriginalTimer();
            } else break;
            recManager->DeleteTimer(timer);
            delete activeMenu;
            if (timerConflicts) {
                delete timerConflicts;
            }
            timerConflicts = recManager->CheckTimerConflict();
            activeMenu = new cRecMenuTimeline(timerConflicts);
            activeMenu->Display();
            break; }
        /********************************************************************************************** 
         *    FAVORITES
         *********************************************************************************************/
        case rmsFavoritesRecord: {
            //caller: cRecMenuSearchTimerResults
            const cEvent *ev = NULL;
            if (cRecMenuSearchTimerResults *menu = dynamic_cast<cRecMenuSearchTimerResults*>(activeMenu)) {
                ev = menu->GetEvent();
            } else break;
            if (!ev)
                break;
            recManager->createTimer(ev, "");
            activeMenuBuffer2 = activeMenu;
            activeMenuBuffer2->Hide();
            activeMenu = new cRecMenuSearchConfirmTimer(ev, rmsFavoritesRecordConfirm);
            activeMenu->Display();
            break;}
        case rmsFavoritesRecordConfirm:
            delete activeMenu;
            activeMenu = activeMenuBuffer2;
            activeMenuBuffer2 = NULL;
            activeMenu->UpdateActiveMenuItem();
            activeMenu->Show();
            break;
        case rmsFavoritesNow:
        case rmsFavoritesNext: {
            int numResults = 0;
            bool nowOrNext;
            std::string header;
            if (nextState == rmsFavoritesNow) {
                nowOrNext = true;
                header = tr("What's on now");
            } else {
                nowOrNext = false;
                header = tr("What's on next");
            }
            const cEvent **result = recManager->WhatsOnNow(nowOrNext, numResults);
            DisplayFavoriteResults(header, result, numResults);
            break; }
        case rmsFavoritesUser1: {
            int numResults = 0;
            const cEvent **result = recManager->UserDefinedTime(1, numResults);
            DisplayFavoriteResults(tvguideConfig.descUser1, result, numResults);
            break; }
        case rmsFavoritesUser2: {
            int numResults = 0;
            const cEvent **result = recManager->UserDefinedTime(2, numResults);
            DisplayFavoriteResults(tvguideConfig.descUser2, result, numResults);
            break; }
        case rmsFavoritesUser3: {
            int numResults = 0;
            const cEvent **result = recManager->UserDefinedTime(3, numResults);
            DisplayFavoriteResults(tvguideConfig.descUser3, result, numResults);
            break; }
        case rmsFavoritesUser4: {
            int numResults = 0;
            const cEvent **result = recManager->UserDefinedTime(4, numResults);
            DisplayFavoriteResults(tvguideConfig.descUser4, result, numResults);
            break; }

        /********************************************************************************************** 
         *    COMMON
         *********************************************************************************************/
        case rmsClose: {
            if (activeMenuBuffer == NULL) {
                state = osEnd;
                Close();
            } else {
                delete activeMenu;
                activeMenu = activeMenuBuffer;
                activeMenuBuffer = NULL;
                activeMenu->UpdateActiveMenuItem();
                activeMenu->Show();
                state = osContinue;
            }
            break; }
        default:        
            break;
    }
    return state;
}

void cRecMenuManager::DisplayFavoriteResults(std::string header, const cEvent **result, int numResults) {
    if (numResults) {
        activeMenuBuffer = activeMenu;
        activeMenuBuffer->Hide();
        activeMenu = new cRecMenuSearchTimerResults(header, result, numResults, "", rmsFavoritesRecord);
        activeMenu->Display();
    } else {
        activeMenuBuffer = activeMenu;
        activeMenuBuffer->Hide();
        activeMenu = new cRecMenuSearchTimerNothingFound(header);
        activeMenu->Display();
    }
}

void cRecMenuManager::DisplaySearchTimerList(void) {
    delete activeMenu;
    std::vector<cTVGuideSearchTimer> searchTimers;
    recManager->GetSearchTimers(&searchTimers);
    activeMenu = new cRecMenuSearchTimers(searchTimers);
    activeMenu->Display();
}

bool cRecMenuManager::DisplayTimerConflict(cTimer *timer) {
    int timerID = 0;
    for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
        if (t == timer)
            return DisplayTimerConflict(timerID);
        timerID++;
    }
    return false;
}

bool cRecMenuManager::DisplayTimerConflict(int timerID) {
    if (timerConflicts)
        delete timerConflicts;
    timerConflicts = recManager->CheckTimerConflict();
    if (!timerConflicts)
        return false;
    int showTimerConflict = timerConflicts->GetCorrespondingConflict(timerID);
    if (showTimerConflict > -1) {
        timerConflicts->SetCurrentConflict(showTimerConflict);
        cTVGuideTimerConflict *conflict = timerConflicts->GetCurrentConflict();
        if (!conflict)
            return false;
        activeMenu = new cRecMenuTimerConflict(conflict);
        activeMenu->Display();
        return true;
    }
    return false;
}

void cRecMenuManager::DisplayDetailedView(const cEvent *ev) {
    activeMenu->Hide();
    detailView = new cDetailView(ev, footer);
    footer->SetDetailedViewMode(true);
    detailView->Start();
    detailViewActive = true;
}

eOSState cRecMenuManager::ProcessKey(eKeys Key) {
    eOSState state = osContinue;
    eRecMenuState nextState = rmsContinue;
    if (!activeMenu)
        return state;
    if (detailViewActive) {
        state = detailView->ProcessKey(Key);
        if (state == osEnd) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            activeMenu->Show();
            state = osContinue;
        }
    } else {
        nextState = activeMenu->ProcessKey(Key);
        if ((nextState == rmsClose) || ((nextState == rmsNotConsumed)&&(Key == kBack))){
            if (activeMenuBuffer == NULL) {
                state = osEnd;
                Close();
            } else {
                delete activeMenu;
                activeMenu = activeMenuBuffer;
                activeMenuBuffer = NULL;
                activeMenu->UpdateActiveMenuItem();
                activeMenu->Show();
                state = osContinue;
                osdManager.flush();
            }
            return state;
        }
        state = StateMachine(nextState);
    }
    osdManager.flush();
    return state;
}