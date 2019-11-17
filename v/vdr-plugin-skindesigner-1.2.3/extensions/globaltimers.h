#ifndef __GLOBALTIMERS_H
#define __GLOBALTIMERS_H

#include <vdr/timers.h>
#include <vdr/plugin.h>

class cRemoteTimerRefresh;

class cGlobalTimers : public cVector<const cTimer *> {
    private:
        static bool initial;
        static cRemoteTimerRefresh *remoteTimerRefresh;
        bool *localTimer;
        cPlugin *pEpg2Vdr;
        cPlugin *pRemoteTimers;
        cPlugin *pEpgSearch;
        bool isEpg2VdrTimers;
        void SetLocalTimers(void);
        void SetRemoteTimers(bool initial);
        bool SetEpg2VdrTimers(void);
    public:
        cGlobalTimers(void);
        virtual ~cGlobalTimers(void);
        void LoadTimers(void);
        void SortTimers(void);
        void MarkLocalTimers(void);
        int NumTimerConfilicts(void);
        bool IsRemoteTimer(int i);
        const char* RemoteHost(int i);
        bool IsRecording(const cRecording *rec);
        void ClearTimers(void);
        static void StartRefreshThread(void);
        static void StopRefreshThread(void);
};

class cRemoteTimerRefresh: public cThread {
    private:
        cPlugin* pRemoteTimers;
    protected:
        virtual void Action(void);
    public:
        cRemoteTimerRefresh(void);
        virtual ~cRemoteTimerRefresh(void);
};
#endif //__GLOBALTIMERS_H
