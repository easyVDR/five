#include "globaltimers.h"
#include "../services/epgsearch.h"
#include "../services/remotetimers.h"
#include "../services/epgtimer.h"

static int CompareTimers(const void *a, const void *b) {
    return (*(const cTimer **)a)->Compare(**(const cTimer **)b);
}

bool cGlobalTimers::initial = true;
cRemoteTimerRefresh *cGlobalTimers::remoteTimerRefresh = NULL;

cGlobalTimers::cGlobalTimers(void) : cVector<const cTimer*>(0) {
    pEpg2Vdr = cPluginManager::GetPlugin("epg2vdr");
    pRemoteTimers = cPluginManager::GetPlugin("remotetimers");
    pEpgSearch = cPluginManager::GetPlugin("epgsearch");
    localTimer = NULL;
    isEpg2VdrTimers = false;
}

cGlobalTimers::~cGlobalTimers(void) {
    if (localTimer) {
        delete[] localTimer;
    }
    ClearTimers();
}

void cGlobalTimers::LoadTimers(void) {
    isEpg2VdrTimers = false;
    bool epg2vdrOk = false;
    if (pEpg2Vdr) {
        epg2vdrOk = SetEpg2VdrTimers();
    }
    if (!epg2vdrOk) {
        SetLocalTimers();    
        if (pRemoteTimers) {
            SetRemoteTimers(initial);
        }        
    }
    initial = false;
}

void cGlobalTimers::SortTimers(void) {
    Sort(CompareTimers);
}

void cGlobalTimers::MarkLocalTimers(void) {
    if (isEpg2VdrTimers)
        return;

    if (localTimer) {
        delete[] localTimer;
        localTimer = NULL;
    }
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_TIMERS_READ;
    const cTimers* timers = Timers;
#else
    const cTimers* timers = &Timers;
#endif
    int numTimers = Size();
    if (numTimers > 0) {
        localTimer = new bool[numTimers];
        for (int i=0; i < numTimers; i++) {
            if (!pRemoteTimers) {
                localTimer[i] = true;
            } else {
                localTimer[i] = false;
                for (const cTimer *Timer = timers->First(); Timer; Timer = timers->Next(Timer)) {
                    if (Timer == At(i)) {
                        localTimer[i] = true;
                        break;
                    }
                }
            }
        }
    }
}

void cGlobalTimers::SetLocalTimers(void) {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_TIMERS_READ;
    const cTimers* timers = Timers;
#else
    const cTimers* timers = &Timers;
#endif
    for (const cTimer *Timer = timers->First(); Timer; Timer = timers->Next(Timer)) {
        if (Timer->HasFlags(tfActive))
            Append(Timer);
    }    
}

void cGlobalTimers::SetRemoteTimers(bool initial) {
    if (initial) {
        cString errorMsg;
        pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg);
    }
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_SCHEDULES_READ;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif
    cTimer* remoteTimer = NULL;
    while (pRemoteTimers->Service("RemoteTimers::ForEach-v1.0", &remoteTimer) && remoteTimer != NULL) {
        remoteTimer->SetEventFromSchedule(schedules); // make sure the event is current
        if (remoteTimer->HasFlags(tfActive))
            Append(remoteTimer);
    }
}

bool cGlobalTimers::SetEpg2VdrTimers(void) {
    bool ok = false;
    cEpgTimer_Service_V1 data;
    if (pEpg2Vdr->Service(EPG2VDR_TIMER_SERVICE, &data)) {
        for (std::list<cEpgTimer_Interface_V1*>::iterator it = data.epgTimers.begin(); it != data.epgTimers.end(); ++it) {
            ok = true;
            isEpg2VdrTimers = true;
            if ((*it)->HasFlags(tfActive)) {
                Append(*it);
            }
        }
    }
    return ok;
}

int cGlobalTimers::NumTimerConfilicts(void) {
    int numConflicts = 0;
    if (pEpgSearch) {
        Epgsearch_lastconflictinfo_v1_0 *serviceData = new Epgsearch_lastconflictinfo_v1_0;
        if (serviceData) {
            serviceData->nextConflict = 0;
            serviceData->relevantConflicts = 0;
            serviceData->totalConflicts = 0;
            pEpgSearch->Service("Epgsearch-lastconflictinfo-v1.0", serviceData);
            if (serviceData->relevantConflicts > 0) {
                numConflicts = serviceData->relevantConflicts;
            }
            delete serviceData;
        }
    }
    return numConflicts;
}

bool cGlobalTimers::IsRemoteTimer(int i) {
    if (isEpg2VdrTimers) {
        cEpgTimer_Interface_V1* epgTimer;
        if (epgTimer = dynamic_cast<cEpgTimer_Interface_V1*>((cTimer*)At(i)))
            return !epgTimer->isLocal();
        else
            return false;
    }
    if (!localTimer)
        return true;
    if (i >= Size())
        return true;
    return !(localTimer[i]);
}

const char* cGlobalTimers::RemoteHost(int i) {
    if (isEpg2VdrTimers) {
        cEpgTimer_Interface_V1* epgTimer;
        if (epgTimer = dynamic_cast<cEpgTimer_Interface_V1*>((cTimer*)At(i)))
            return epgTimer->VdrName();
    }
    return "";
}

bool cGlobalTimers::IsRecording(const cRecording *rec) {
    if (!rec || !rec->Name())
        return false;
    std::string recName = rec->Name();
    int size = Size();
    for (int i=0; i<size; i++) {
        const cTimer *t = At(i);
        const char *timerFile = t->File();
        if (!t->Matches() || !timerFile)
            continue;
        if (recName.find(timerFile) != std::string::npos)
            return true;
    }
    return false;
}

void cGlobalTimers::ClearTimers(void) {
    if (isEpg2VdrTimers) {
        int size = Size();
        for (int i=0; i<size; i++) {
            delete At(i);
        }
    }
    Clear();
}

void cGlobalTimers::StartRefreshThread(void) {
    if (remoteTimerRefresh == NULL) {
        remoteTimerRefresh = new cRemoteTimerRefresh();
    }
}

void cGlobalTimers::StopRefreshThread(void) {
    if (!remoteTimerRefresh)
        return;
    delete remoteTimerRefresh;
    remoteTimerRefresh = NULL;
    initial = true;
}

/*************************************************************************
* cRemoteTimerRefresh
*************************************************************************/
cRemoteTimerRefresh::cRemoteTimerRefresh(): cThread("skindesigner: RemoteTimers::RefreshTimers") {
    pRemoteTimers = cPluginManager::GetPlugin("remotetimers");
    if (pRemoteTimers)
        Start();
}

cRemoteTimerRefresh::~cRemoteTimerRefresh(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
}

void cRemoteTimerRefresh::Action(void) {    
#define REFESH_INTERVALL_MS 30000
    int sleepSlice = 1000;
    int slept = 0;
    while (Running()) {
        while (Running() && slept < REFESH_INTERVALL_MS) {
            cCondWait::SleepMs(sleepSlice);
            slept += sleepSlice;
        }
        slept = 0;
        // make sure that no timer is currently being edited
        if (!cOsd::IsOpen() && Running()) {
            cString errorMsg;
            pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg);
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
            LOCK_TIMERS_WRITE;
            Timers->SetModified();
#else
            Timers.SetModified();
#endif
        }
    }
}
