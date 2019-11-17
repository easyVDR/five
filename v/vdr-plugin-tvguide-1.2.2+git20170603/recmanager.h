#ifndef __TVGUIDE_RECMMANAGER_H
#define __TVGUIDE_RECMMANAGER_H

#include <string>
#include <vector>
#include <vdr/plugin.h>
#include "services/epgsearch.h"
#include "detailview.h"
#include "recmenu.h"
#include "searchtimer.h"
#include "switchtimer.h"
#include "timerconflict.h"

struct TVGuideEPGSearchTemplate {
public:
    std::string name;
    std::string templValue;
};

// --- cRecManager  -------------------------------------------------------------
class cRecManager   {
private:
    cPlugin *epgSearchPlugin;
    bool epgSearchAvailable;
public:
    cRecManager(void);
    void SetEPGSearchPlugin(void);
    bool EpgSearchAvailable(void) {return epgSearchAvailable;};
    bool RefreshRemoteTimers(void);
    bool CheckEventForTimer(const cEvent *event);
#if VDRVERSNUM >= 20301
    const cTimer *GetTimerForEvent(const cEvent *event);
#else
    cTimer *GetTimerForEvent(const cEvent *event);
#endif
    cTimer *createTimer(const cEvent *event, std::string path = "");
    cTimer *createLocalTimer(const cEvent *event, std::string path);
    cTimer *createRemoteTimer(const cEvent *event, std::string path);
    void SetTimerPath(cTimer *timer, const cEvent *event, std::string path);
#if VDRVERSNUM >= 20301
    void DeleteTimer(const cTimer *timer);
#else
    void DeleteTimer(cTimer *timer);
#endif
    void DeleteTimer(int timerID);
    void DeleteTimer(const cEvent *event);
    void DeleteLocalTimer(const cEvent *event);
    void DeleteRemoteTimer(const cEvent *event);
#if VDRVERSNUM >= 20301
    void SaveTimer(const cTimer *timer, cTimer newTimerSettings);
#else
    void SaveTimer(cTimer *timer, cTimer newTimerSettings);
#endif
    bool IsRecorded(const cEvent *event);
    cTVGuideTimerConflicts *CheckTimerConflict(void);
    void CreateSeriesTimer(cTimer *seriesTimer);
    const cEvent **PerformSearchTimerSearch(std::string epgSearchString, int &numResults);
    const cEvent **PerformSearch(Epgsearch_searchresults_v1_0 data, int &numResults);
    void ReadEPGSearchTemplates(std::vector<TVGuideEPGSearchTemplate> *epgTemplates);
    void GetSearchTimers(std::vector<cTVGuideSearchTimer> *timers);
    int CreateSearchTimer(std::string epgSearchString);
    bool SaveSearchTimer(cTVGuideSearchTimer *searchTimer);
    void DeleteSearchTimer(cTVGuideSearchTimer *searchTimer, bool delTimers);
    void UpdateSearchTimers(void);
    bool CreateSwitchTimer(const cEvent *event, cSwitchTimer switchTimer);
    void DeleteSwitchTimer(const cEvent *event);
#if VDRVERSNUM >= 20301
    const cRecording **SearchForRecordings(std::string searchString, int &numResults);
#else
    cRecording **SearchForRecordings(std::string searchString, int &numResults);
#endif
    const cEvent **LoadReruns(const cEvent *event, int &numResults);
    void GetFavorites(std::vector<cTVGuideSearchTimer> *favorites);
    const cEvent **WhatsOnNow(bool nowOrNext, int &numResults);
    const cEvent **UserDefinedTime(int userTime, int &numResults);
    virtual ~cRecManager  (void);
};

#endif //__TVGUIDE_RECMMANAGER_H