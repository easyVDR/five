#define __STL_CONFIG_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include <vdr/menu.h>
#include "services/remotetimers.h"
#include "tools.h"
#include "switchtimer.h"
#include "timerconflict.h"
#include <vdr/timers.h>
#include "recmanager.h"

static int CompareRecording(const void *p1, const void *p2) {
   return (int)((*(cRecording **)p1)->Start() - (*(cRecording **)p2)->Start());
}

cRecManager::cRecManager(void) {
    epgSearchPlugin = NULL;
    epgSearchAvailable = false;
}

cRecManager::~cRecManager(void) {
}

void cRecManager::SetEPGSearchPlugin(void) {
	epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
	if (epgSearchPlugin) {
		epgSearchAvailable = true;
	}
}

bool cRecManager::RefreshRemoteTimers(void) {
    cString errorMsg;
    if (!pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg)) {
        esyslog("tvguide: %s", *errorMsg);
        return false;
    }
    return true;
}

bool cRecManager::CheckEventForTimer(const cEvent *event) {
    bool hasTimer = false;
    
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        if (rtMatch.timerMatch == tmFull)
            hasTimer = true;
    } else
        hasTimer = event->HasTimer();

    return hasTimer;
}

#if VDRVERSNUM >= 20301
const cTimer *cRecManager::GetTimerForEvent(const cEvent *event) {
    const cTimer *timer = NULL;
#else
cTimer *cRecManager::GetTimerForEvent(const cEvent *event) {
    cTimer *timer = NULL;
#endif
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        timer = rtMatch.timer;
#if VDRVERSNUM >= 20301
        return timer;
    }
        LOCK_TIMERS_READ;
        timer = Timers->GetMatch(event);
#else
    } else
        timer = Timers.GetMatch(event);    
#endif
    return timer;
}

cTimer *cRecManager::createTimer(const cEvent *event, std::string path) {
    cTimer *timer = NULL;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        timer = createRemoteTimer(event, path);
    } else {
        timer = createLocalTimer(event, path);
    }
    return timer;
}

cTimer *cRecManager::createLocalTimer(const cEvent *event, std::string path) {
    cTimer *timer = new cTimer(event);
#if VDRVERSNUM >= 20301
    LOCK_TIMERS_WRITE;
    cTimer *t = Timers->GetTimer(timer);
#else
    cTimer *t = Timers.GetTimer(timer);
#endif
    if (t) {
        t->OnOff();
#if VDRVERSNUM >= 20301
        t->SetEvent(event);
#else
        t->SetEventFromSchedule();
#endif
        delete timer;
        timer = t;
        isyslog("timer %s reactivated", *t->ToDescr());
    } else {
#if VDRVERSNUM >= 20301
        Timers->Add(timer);
#else
        Timers.Add(timer);
#endif
        isyslog("timer %s added (active)", *timer->ToDescr());
    }
    SetTimerPath(timer, event, path);
#if VDRVERSNUM >= 20301
    Timers->SetModified();
#else
    Timers.SetModified();
#endif
    return timer;
}

cTimer *cRecManager::createRemoteTimer(const cEvent *event, std::string path) {
    cTimer *t = new cTimer(event);
    SetTimerPath(t, event, path);
    RemoteTimers_Timer_v1_0 rt;
    rt.timer = t;
    pRemoteTimers->Service("RemoteTimers::GetTimer-v1.0", &rt.timer);
    if (rt.timer) {
        rt.timer->OnOff();
        if (!pRemoteTimers->Service("RemoteTimers::ModTimer-v1.0", &rt))
            rt.timer = NULL;
    } else {
        rt.timer = t;
        if (!pRemoteTimers->Service("RemoteTimers::NewTimer-v1.0", &rt))
            isyslog("%s", *rt.errorMsg);
    }
    RefreshRemoteTimers();
    return rt.timer;
}

void cRecManager::SetTimerPath(cTimer *timer, const cEvent *event, std::string path) {
    if (tvguideConfig.instRecFolderMode == eFolderFixed) {
        Epgsearch_services_v1_2 *epgSearch = new Epgsearch_services_v1_2;
        std::string recDir = tvguideConfig.instRecFixedFolder;
        std::replace(recDir.begin(), recDir.end(), '/', '~');
        if (strchr(recDir.c_str(), '%') != NULL) {
            if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
                std::string newFileName = epgSearch->handler->Evaluate(recDir, event);
                if (strchr(newFileName.c_str(), '%') == NULL) // only set directory to new value if all categories could have been replaced
                    timer->SetFile(newFileName.c_str());
                else 
                    esyslog("tvguide: timer path not set because replacing variable was not successfull: %s", newFileName.c_str());
            }
        } else {
            cString newFileName;
            if (recDir.size() > 0) {
                newFileName = cString::sprintf("%s~%s", recDir.c_str(), timer->File());
                timer->SetFile(*newFileName);
            }
        }
        return;
    }
    //Set choosen path
    cString newFileName;
    if (path.size() > 0) {
        std::replace(path.begin(), path.end(), '/', '~');
        newFileName = cString::sprintf("%s~%s", path.c_str(), timer->File());
    } else {
        newFileName = event->Title();
    }
    timer->SetFile(*newFileName);
}

void cRecManager::DeleteTimer(int timerID) {
#if VDRVERSNUM >= 20301
    LOCK_TIMERS_READ;
    const cTimer *t = Timers->Get(timerID);
#else
    cTimer *t = Timers.Get(timerID);
#endif
    if (!t)
        return;
    DeleteTimer(t);
}

void cRecManager::DeleteTimer(const cEvent *event) {
    if (!event)
        return;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        DeleteRemoteTimer(event);
    } else {
        DeleteLocalTimer(event);
    }
}

void cRecManager::DeleteLocalTimer(const cEvent *event) {
#if VDRVERSNUM >= 20301
    LOCK_TIMERS_READ;
    const cTimer *t = Timers->GetMatch(event);
#else
    cTimer *t = Timers.GetMatch(event);
#endif
    if (!t)
        return;
    DeleteTimer(t);
}


#if VDRVERSNUM >= 20301
void cRecManager::DeleteTimer(const cTimer *timer) {
    LOCK_TIMERS_WRITE;
    cTimers* timers = Timers;
    cTimer* t = timers->GetTimer((cTimer*)timer);  // #TODO dirty cast
   
    if (t->Recording()) {
       t->Skip();
       cRecordControls::Process(timers, time(NULL));
    }
#else
void cRecManager::DeleteTimer(cTimer *timer) {
    if (timer->Recording()) {
        timer->Skip();
        cRecordControls::Process(time(NULL));
    }
#endif
    isyslog("timer %s deleted", *timer->ToDescr());
#if VDRVERSNUM >= 20301
    timers->Del(t, true);
    timers->SetModified();
#else
    Timers.Del(timer, true);
    Timers.SetModified();
#endif
}

void cRecManager::DeleteRemoteTimer(const cEvent *event) {
    RemoteTimers_GetMatch_v1_0 rtMatch;
    rtMatch.event = event;
    pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
    if (rtMatch.timer) {
        RemoteTimers_Timer_v1_0 rt;
        rt.timer = rtMatch.timer;
        isyslog("remotetimer %s deleted", *rt.timer->ToDescr());
        if (!pRemoteTimers->Service("RemoteTimers::DelTimer-v1.0", &rt))
            isyslog("remotetimer error");
        RefreshRemoteTimers();
    }
}

#if VDRVERSNUM >= 20301
void cRecManager::SaveTimer(const cTimer *timer, cTimer newTimerSettings) {
#else
void cRecManager::SaveTimer(cTimer *timer, cTimer newTimerSettings) {
#endif
    if (!timer)
        return;
    
    bool active = newTimerSettings.HasFlags(tfActive);
    int prio = newTimerSettings.Priority();
    int lifetime = newTimerSettings.Lifetime();
    time_t day = newTimerSettings.Day();
    int start = newTimerSettings.Start();
    int stop = newTimerSettings.Stop();
    std::string fileName = newTimerSettings.File();

#if VDRVERSNUM >= 20301
    ((cTimer*)timer)->SetDay(day);
    ((cTimer*)timer)->SetStart(start);
    ((cTimer*)timer)->SetStop(stop);
    ((cTimer*)timer)->SetPriority(prio);
    ((cTimer*)timer)->SetLifetime(lifetime);
    ((cTimer*)timer)->SetFile(fileName.c_str());
#else
    timer->SetDay(day);
    timer->SetStart(start);
    timer->SetStop(stop);
    timer->SetPriority(prio);
    timer->SetLifetime(lifetime);
    timer->SetFile(fileName.c_str());
#endif

    if (timer->HasFlags(tfActive) && !active)
#if VDRVERSNUM >= 20301
        ((cTimer*)timer)->ClrFlags(tfActive);
    else if (!timer->HasFlags(tfActive) && active)
        ((cTimer*)timer)->SetFlags(tfActive);

#else
        timer->ClrFlags(tfActive);
    else if (!timer->HasFlags(tfActive) && active)
        timer->SetFlags(tfActive);

    timer->SetEventFromSchedule();
#endif
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_Timer_v1_0 rt;
#if VDRVERSNUM >= 20301
        rt.timer = (cTimer*)timer;
#else
        rt.timer = timer;
#endif
        if (!pRemoteTimers->Service("RemoteTimers::ModTimer-v1.0", &rt))
            rt.timer = NULL;
        RefreshRemoteTimers();
    } else {
#if VDRVERSNUM >= 20301
        LOCK_TIMERS_WRITE;
        Timers->SetModified();
#else
        Timers.SetModified();
#endif
    }          
}


bool cRecManager::IsRecorded(const cEvent *event) {
#if VDRVERSNUM >= 20301
    LOCK_TIMERS_WRITE;
    cTimer *timer = Timers->GetMatch(event);
#else
    cTimer *timer = Timers.GetMatch(event);
#endif
    if (!timer)
        return false;
    return timer->Recording();
}

cTVGuideTimerConflicts *cRecManager::CheckTimerConflict(void) {
    cTVGuideTimerConflicts *conflictList = new cTVGuideTimerConflicts();
    if (!epgSearchAvailable)
        return conflictList;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        std::list<std::string> conflicts = epgSearch->handler->TimerConflictList();
        int numConflicts = conflicts.size();
        if (numConflicts == 0)
            return conflictList;
        for (std::list<std::string>::iterator it=conflicts.begin(); it != conflicts.end(); ++it) {
            conflictList->AddConflict(*it);
        }
    }
    delete epgSearch;
    conflictList->CalculateConflicts();
    return conflictList;
}

void cRecManager::CreateSeriesTimer(cTimer *seriesTimer) {
#if VDRVERSNUM >= 20301
#else
    seriesTimer->SetEventFromSchedule();
#endif
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_Timer_v1_0 rt;
        rt.timer = seriesTimer;
        if (!pRemoteTimers->Service("RemoteTimers::NewTimer-v1.0", &rt))
            isyslog("%s", *rt.errorMsg);
        RefreshRemoteTimers();
    } else {
#if VDRVERSNUM >= 20301
        LOCK_TIMERS_WRITE;
        cTimers* timers = Timers;
        timers->Add(seriesTimer);
        timers->SetModified();
#else
        Timers.Add(seriesTimer);
        Timers.SetModified();
#endif
    }
}


void cRecManager::ReadEPGSearchTemplates(std::vector<TVGuideEPGSearchTemplate> *epgTemplates) {
    cString ConfigDir = cPlugin::ConfigDirectory("epgsearch");
    cString epgsearchConf = "epgsearchtemplates.conf";
    cString fileName = AddDirectory(*ConfigDir,  *epgsearchConf);
    if (access(fileName, F_OK) == 0) {
        FILE *f = fopen(fileName, "r");
        if (f) {
            char *s;
            cReadLine ReadLine;
            while ((s = ReadLine.Read(f)) != NULL) {
                char *p = strchr(s, '#');
                if (p)
                    *p = 0;
                stripspace(s);
                try {
                    if (!isempty(s)) {
                        std::string templ = s;
                        int posID = templ.find_first_of(":");
                        int posName = templ.find_first_of(":", posID+1);
                        std::string name = templ.substr(posID+1, posName - posID - 1);
                        std::string templValue = templ.substr(posName);
                        TVGuideEPGSearchTemplate tmp;
                        tmp.name = name;
                        tmp.templValue = templValue;
                        epgTemplates->push_back(tmp);
                    }
                } catch (...){}
            }
        }
    }
}

const cEvent **cRecManager::PerformSearchTimerSearch(std::string epgSearchString, int &numResults) {
    if (!epgSearchAvailable)
        return NULL;
    const cEvent **searchResults = NULL;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        std::list<std::string> results = epgSearch->handler->QuerySearch(epgSearchString);
        numResults = results.size();
        if (numResults > 0) {
            searchResults = new const cEvent *[numResults];
#if VDRVERSNUM >= 20301
            LOCK_SCHEDULES_READ;
            const cSchedules* schedules = Schedules;
#else
            cSchedulesLock schedulesLock;
            const cSchedules *schedules;
            schedules = cSchedules::Schedules(schedulesLock);
#endif
            const cEvent *event = NULL;
            int index=0;
            for (std::list<std::string>::iterator it=results.begin(); it != results.end(); ++it) {
                try {
                    splitstring s(it->c_str());
                    std::vector<std::string> flds = s.split(':', 1);
                    int eventID = atoi(flds[1].c_str());
                    std::string channelID = flds[7];
                    tChannelID chanID = tChannelID::FromString(channelID.c_str());
#if VDRVERSNUM >= 20301
                    LOCK_CHANNELS_READ;
                    const cChannel *channel = Channels->GetByChannelID(chanID);
#else
                    cChannel *channel = Channels.GetByChannelID(chanID);
#endif
                    if (channel) {
                        const cSchedule *Schedule = NULL;
                        Schedule = schedules->GetSchedule(channel);
                        event = Schedule->GetEvent(eventID);
                        if (event) {
                            searchResults[index] = event;                        
                        } else
                            return NULL;
                    } else
                        return NULL;
                    index++;
                } catch (...){}
            }
        }
    }
    return searchResults;
}

const cEvent **cRecManager::PerformSearch(Epgsearch_searchresults_v1_0 data, int &numResults) {
    if (epgSearchAvailable) {
        if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
            cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *list = data.pResultList;
            if (!list)
                return NULL;
            int numElements = list->Count();
            const cEvent **searchResults = NULL;
            if (numElements > 0) {
                searchResults = new const cEvent *[numElements];
                numResults = numElements;
                int index = 0;
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r ; r = list->Next(r)) {
                    searchResults[index] = r->event;
                    index++;
                }
            }
            delete list;
            return searchResults;
        }
    }
    return NULL;
}

void cRecManager::GetSearchTimers(std::vector<cTVGuideSearchTimer> *searchTimer) {
    if (!epgSearchAvailable) {
            return;
    }
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        std::list<std::string> searchTimerList;
        searchTimerList = epgSearch->handler->SearchTimerList();
        for(std::list<std::string>::iterator it = searchTimerList.begin(); it != searchTimerList.end(); it++) {
            cTVGuideSearchTimer timer;
            timer.SetEPGSearchString(it->c_str());
            if (timer.Parse())
                searchTimer->push_back(timer);
        }
    }
    std::sort(searchTimer->begin(), searchTimer->end());
}

int cRecManager::CreateSearchTimer(std::string epgSearchString) {
    int timerID = -1;
    if (!epgSearchAvailable)
        return timerID;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        timerID = epgSearch->handler->AddSearchTimer(epgSearchString);
    }
    return timerID;
}

bool cRecManager::SaveSearchTimer(cTVGuideSearchTimer *searchTimer) {
    if (!epgSearchAvailable)
        return false;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (searchTimer->GetID() > -1) {
        if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
            bool success = epgSearch->handler->ModSearchTimer(searchTimer->BuildSearchString());
            if (success) {
                esyslog("tvguide: search timer with id %d sucessfully modified", searchTimer->GetID());
                return true;
            } else {
                esyslog("tvguide: error modifying search timer with id %d, build string %s", searchTimer->GetID(), searchTimer->BuildSearchString().c_str());
                return false;
            }
        }
    } else {
        if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
            int timerID = epgSearch->handler->AddSearchTimer(searchTimer->BuildSearchString());
            if (timerID >=0) {
                esyslog("tvguide: search timer with id %d sucessfully created", timerID);
                return true;
            } else {
                esyslog("tvguide: error creating search timer, build string %s", searchTimer->BuildSearchString().c_str());
                return false;
            }
        }
    }
    return false;
}

void cRecManager::DeleteSearchTimer(cTVGuideSearchTimer *searchTimer, bool delTimers) {
    if (!epgSearchAvailable)
        return;
    int searchTimerID = searchTimer->GetID();
    if (delTimers) {
#if VDRVERSNUM >= 20301
        LOCK_TIMERS_WRITE;
        cTimer *timer = Timers->First();
#else
        cTimer *timer = Timers.First();
#endif
        while(timer) {
            if (!timer->Recording()) {
                char* searchID = GetAuxValue(timer, "s-id");
                if (searchID) {
                    if (searchTimerID == atoi(searchID)) {
#if VDRVERSNUM >= 20301
                        cTimer* timerNext = Timers->Next(timer);
#else
                        cTimer* timerNext = Timers.Next(timer);
#endif
                        DeleteTimer(timer);
                        timer = timerNext;
                    } else {
#if VDRVERSNUM >= 20301
                        timer = Timers->Next(timer);
#else
                        timer = Timers.Next(timer);
#endif
                    }
                    free(searchID);
                } else {
#if VDRVERSNUM >= 20301
                    timer = Timers->Next(timer);
#else
                    timer = Timers.Next(timer);
#endif
                }
            } else {
#if VDRVERSNUM >= 20301
                timer = Timers->Next(timer);
#else
                timer = Timers.Next(timer);
#endif
            }
        }
    }
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        bool success = epgSearch->handler->DelSearchTimer(searchTimerID);
        if (success) {
            esyslog("tvguide: search timer \"%s\" sucessfully deleted", searchTimer->SearchString().c_str());
        } else {
            esyslog("tvguide: error deleting search timer \"%s\"", searchTimer->SearchString().c_str());
        }
    }   
}

void cRecManager::UpdateSearchTimers(void) {
    if (epgSearchAvailable) {
        Epgsearch_updatesearchtimers_v1_0 data;
        data.showMessage = false;
        epgSearchPlugin->Service("Epgsearch-updatesearchtimers-v1.0", &data);
    }
}

// announceOnly: 0 = switch, 1 = announce only, 2 = ask for switch
bool cRecManager::CreateSwitchTimer(const cEvent *event, cSwitchTimer switchTimer) {
    if (epgSearchAvailable && event) {
        Epgsearch_switchtimer_v1_0 data;
        data.event = event;
        data.mode = 1;
        data.switchMinsBefore = switchTimer.switchMinsBefore;
        data.announceOnly = switchTimer.announceOnly;
        data.success = false;
        epgSearchPlugin->Service("Epgsearch-switchtimer-v1.0", &data);
        cSwitchTimer *t = new cSwitchTimer(event);
        SwitchTimers.Add(t);
        return data.success;
    }
    return false;
}

void cRecManager::DeleteSwitchTimer(const cEvent *event) {
    SwitchTimers.DeleteSwitchTimer(event);
    if (epgSearchAvailable) {
        Epgsearch_switchtimer_v1_0 data;
        data.event = event;
        data.mode = 2;
        data.switchMinsBefore = 0;
        data.announceOnly = 0;
        data.success = false;
        epgSearchPlugin->Service("Epgsearch-switchtimer-v1.0", &data);
    }
}

#if VDRVERSNUM >= 20301
const cRecording **cRecManager::SearchForRecordings(std::string searchString, int &numResults) {

    const cRecording **matchingRecordings = NULL;
#else
cRecording **cRecManager::SearchForRecordings(std::string searchString, int &numResults) {
    
    cRecording **matchingRecordings = NULL;
#endif
    int num = 0;
    numResults = 0;
    
#if VDRVERSNUM >= 20301
    LOCK_RECORDINGS_READ;
    for (const cRecording *recording = Recordings->First(); recording; recording = Recordings->Next(recording)) {
#else
    for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
#endif
        std::string s1 = recording->Name();
        std::string s2 = searchString;
        if (s1.empty() || s2.empty()) continue;
        
        // tolerance for fuzzy searching: 90% of the shorter text length, but at least 1
        int tolerance = std::max(1, (int)std::min(s1.size(), s2.size()) / 10); 

        bool match = FindIgnoreCase(s1, s2) >= 0 || FindIgnoreCase(s2, s1) >= 0;
        
        if (!match) {
            AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
            if (s1.size() > 32) s1 = s1.substr(0, 32);
            afuzzy_init(s1.c_str(), tolerance, 0, &af);
            /* Checking substring */
            int res = afuzzy_checkSUB(s2.c_str(), &af);
            afuzzy_free(&af);
            match = (res > 0);
        }
        
        if (!match) {
            AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
            if (s2.size() > 32) s2 = s2.substr(0, 32);
            afuzzy_init(s2.c_str(), tolerance, 0, &af);
            /* Checking substring */
            int res = afuzzy_checkSUB(s1.c_str(), &af);
            afuzzy_free(&af);
            match = (res > 0);
        }
        
        if (match) {
#if VDRVERSNUM >= 20301
            matchingRecordings = (const cRecording **)realloc(matchingRecordings, (num + 1) * sizeof(cRecording *));
#else
            matchingRecordings = (cRecording **)realloc(matchingRecordings, (num + 1) * sizeof(cRecording *));
#endif
            matchingRecordings[num++] = recording;
        }
    }
    if (num > 0) {
        qsort(matchingRecordings, num, sizeof(cRecording *), CompareRecording);
        numResults = num;
        return matchingRecordings;
    }
    return NULL;
}

const cEvent **cRecManager::LoadReruns(const cEvent *event, int &numResults) {
    if (epgSearchAvailable && !isempty(event->Title())) {
        Epgsearch_searchresults_v1_0 data;
        std::string strQuery = event->Title();
        if (tvguideConfig.useSubtitleRerun > 0) {
            if (tvguideConfig.useSubtitleRerun == 2 || !isempty(event->ShortText()))
                strQuery += "~";
            if (!isempty(event->ShortText()))
                strQuery += event->ShortText();
                data.useSubTitle = true;
        } else {
            data.useSubTitle = false;
        }
        data.query = (char *)strQuery.c_str();
        data.mode = 0;
        data.channelNr = 0;
        data.useTitle = true;
        data.useDescription = false;
        
        if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
            cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
            if (!list)
                return NULL;
            const cEvent **searchResults = NULL;
            int numElements = list->Count();
            if (numElements > 0) {
                searchResults = new const cEvent *[numElements];
                int index = 0;
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r; r = list->Next(r)) {
                    if ((event->ChannelID() == r->event->ChannelID()) && (event->StartTime() == r->event->StartTime()))
                        continue;
                    searchResults[index] = r->event;
                    index++;
                }
                delete list;
                numResults = index;
                return searchResults;
            }
        }
    }
    return NULL;
}

void cRecManager::GetFavorites(std::vector<cTVGuideSearchTimer> *favorites) {
    if (!epgSearchAvailable) {
            return;
    }
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        std::list<std::string> searchTimerList;
        searchTimerList = epgSearch->handler->SearchTimerList();
        for(std::list<std::string>::iterator it = searchTimerList.begin(); it != searchTimerList.end(); it++) {
            cTVGuideSearchTimer timer;
            timer.SetEPGSearchString(it->c_str());
            if (timer.Parse()) {
                if (timer.UseInFavorites())
                    favorites->push_back(timer);
            }
        }
    }

}

const cEvent **cRecManager::WhatsOnNow(bool nowOrNext, int &numResults) {
    std::vector<const cEvent*> tmpResults;
#if VDRVERSNUM >= 20301
    LOCK_SCHEDULES_READ;
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
#endif
    const cChannel *startChannel = NULL, *stopChannel = NULL;
    if (tvguideConfig.favLimitChannels) {
#if VDRVERSNUM >= 20301
        startChannel = Channels->GetByNumber(tvguideConfig.favStartChannel);
        stopChannel = Channels->GetByNumber(tvguideConfig.favStopChannel);
    }
    if (!startChannel)
        startChannel = Channels->First();

    for (const cChannel *channel = startChannel; channel; channel = Channels->Next(channel)) {
#else
        startChannel = Channels.GetByNumber(tvguideConfig.favStartChannel);
        stopChannel = Channels.GetByNumber(tvguideConfig.favStopChannel);
    }
    if (!startChannel)
        startChannel = Channels.First();

    for (const cChannel *channel = startChannel; channel; channel = Channels.Next(channel)) {
#endif
        if (channel->GroupSep()) continue;
        const cSchedule *Schedule = schedules->GetSchedule(channel);
        if (!Schedule) continue;

        const cEvent *event = NULL;
        if (nowOrNext)
            event = Schedule->GetPresentEvent();
        else
            event = Schedule->GetFollowingEvent();
        if (event) {
            tmpResults.push_back(event);
        }
        if (stopChannel && (stopChannel->Number() <= channel->Number()))
            break;
    }
    numResults = tmpResults.size();
    const cEvent **results = new const cEvent *[numResults];
    for (int i=0; i<numResults; i++) {
        results[i] = tmpResults[i];        
    }

    return results;
}

const cEvent **cRecManager::UserDefinedTime(int userTime, int &numResults) {
    std::vector<const cEvent*> tmpResults;
    int favTime = 0;
    if (userTime == 1) {
        favTime = tvguideConfig.favTime1;
    } else if (userTime == 2) {
        favTime = tvguideConfig.favTime2;
    } else if (userTime == 3) {
        favTime = tvguideConfig.favTime3;
    } else if (userTime == 4) {
        favTime = tvguideConfig.favTime4;
    }

    time_t now = time(0);
    tm *midn = localtime(&now);
    midn->tm_sec = 0;
    midn->tm_min = 0;
    midn->tm_hour = 0;
    time_t midnight = mktime(midn);
    int hours = favTime/100;
    int mins = favTime - hours * 100;
    time_t searchTime = midnight + hours*60*60 + mins*60;
    if (searchTime < now)
        searchTime += 24*60*60;

#if VDRVERSNUM >= 20301
    LOCK_CHANNELS_READ;
    LOCK_SCHEDULES_READ;
    const cChannels* channels = Channels;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
#endif
    const cChannel *startChannel = NULL, *stopChannel = NULL;
    if (tvguideConfig.favLimitChannels) {
#if VDRVERSNUM >= 20301
        startChannel = Channels->GetByNumber(tvguideConfig.favStartChannel);
        stopChannel = Channels->GetByNumber(tvguideConfig.favStopChannel);
    }
    if (!startChannel)
        startChannel = Channels->First();

    for (const cChannel *channel = startChannel; channel; channel = Channels->Next(channel)) {
#else
        startChannel = Channels.GetByNumber(tvguideConfig.favStartChannel);
        stopChannel = Channels.GetByNumber(tvguideConfig.favStopChannel);
    }
    if (!startChannel)
        startChannel = Channels.First();

    for (const cChannel *channel = startChannel; channel; channel = Channels.Next(channel)) {
#endif
        if (channel->GroupSep()) continue;
        const cSchedule *Schedule = schedules->GetSchedule(channel);
        if (!Schedule) continue;
        const cEvent *event = Schedule->GetEventAround(searchTime);
        if (!event) continue;
        //if event is more or less over (only 15mns left), take next
        if ((event->EndTime() - searchTime) < 15*60) {
            event = Schedule->Events()->Next(event);
        }
        if (event)
            tmpResults.push_back(event);
        if (stopChannel && (stopChannel->Number() <= channel->Number()))
            break;
    }

    numResults = tmpResults.size();
    const cEvent **results = new const cEvent *[numResults];
    for (int i=0; i<numResults; i++) {
        results[i] = tmpResults[i];        
    }
    return results;
}
