#ifndef __VIEWELEMENTSDC_H
#define __VIEWELEMENTSDC_H

#include "viewelement.h"
#include "../extensions/scrapmanager.h"
#include "../extensions/globaltimers.h"
#include "../services/dvbapi.h"

/******************************************************************
* cVeDcChannelInfo
******************************************************************/
class cVeDcChannelInfo : public cViewElement {
private:
public:
    cVeDcChannelInfo(void);
    virtual ~cVeDcChannelInfo(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c, int number);
};
/******************************************************************
* cVeDcChannelGroup
******************************************************************/
class cVeDcChannelGroup : public cViewElement {
private:
    const char *GetChannelSep(const cChannel *c, bool prev);
public:
    cVeDcChannelGroup(void);
    virtual ~cVeDcChannelGroup(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c);
};
/******************************************************************
* cVeDcEpgInfo
******************************************************************/
class cVeDcEpgInfo : public cViewElement {
private:
    cGlobalTimers *globalTimers;
    bool EventHasTimer(const cEvent *e);
public:
    cVeDcEpgInfo(void);
    virtual ~cVeDcEpgInfo(void);
    void SetGlobalTimers(cGlobalTimers *globalTimers) { this->globalTimers = globalTimers; };
    void SetTokenContainer(void);
    void Set(const cEvent *p, const cEvent *f);
    void Close(void);
};
/******************************************************************
* cVeDcProgressBar
******************************************************************/
class cVeDcProgressBar : public cViewElement {
private:
    int currentLast;
    int startTime;
    int duration;
    int GetLiveBuffer(void);
public:
    cVeDcProgressBar(void);
    virtual ~cVeDcProgressBar(void);
    void Close(void);
    void SetTokenContainer(void);
    void Set(const cEvent *p);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcStatusInfo
******************************************************************/
class cVeDcStatusInfo : public cViewElement {
private:
    cGlobalTimers *globalTimers;
    bool CheckMails(void);
public:
    cVeDcStatusInfo(void);
    virtual ~cVeDcStatusInfo(void);
    void SetGlobalTimers(cGlobalTimers *globalTimers) { this->globalTimers = globalTimers; };
    void SetTokenContainer(void);
    void Set(const cChannel *c);
};
/******************************************************************
* cVeDcAudioInfo
******************************************************************/
class cVeDcAudioInfo : public cViewElement {
private:
    int lastNumAudioTracks;
    int lastAudioChannel;
    char *lastTracDesc;
    char *lastTrackLang;
public:
    cVeDcAudioInfo(void);
    virtual ~cVeDcAudioInfo(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcScreenResolution
******************************************************************/
class cVeDcScreenResolution : public cViewElement {
private:
    int lastScreenWidth;
    int lastScreenHeight;
    double lastAspect;
public:
    cVeDcScreenResolution(void);
    virtual ~cVeDcScreenResolution(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcSignalQuality
******************************************************************/
class cVeDcSignalQuality : public cViewElement {
private:
    int lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
public:
    cVeDcSignalQuality(void);
    virtual ~cVeDcSignalQuality(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcScraperContent
******************************************************************/
class cVeDcScraperContent : public cViewElement, public cScrapManager {
private:
public:
    cVeDcScraperContent(void);
    virtual ~cVeDcScraperContent(void);
    void Close(void);
    void SetTokenContainer(void);
    void Set(const cEvent *e);
};
/******************************************************************
* cVeDcEcmInfo
******************************************************************/
class cVeDcEcmInfo : public cViewElement {
private:
    int channelSid;
    sDVBAPIEcmInfo lastEcmInfo;
    bool CompareECMInfos(sDVBAPIEcmInfo *ecmInfo);
public:
    cVeDcEcmInfo(void);
    virtual ~cVeDcEcmInfo(void);
    void Close(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDcChannelHints
******************************************************************/
class cVeDcChannelHints : public cViewElement {
private:
    const cChannel **hints;
    int numHints;
    int current;
    int hintsIndex;
    bool active;
public:
    cVeDcChannelHints(void);
    virtual ~cVeDcChannelHints(void);
    void Close(void);
    void SetTokenContainer(void);
    void SetNumHints(int num);
    void SetHint(const cChannel *c);
    bool Parse(bool forced = false);
    bool Active(void) { return active; };
};


/******************************************************************
* cVeDcChannelDetail
******************************************************************/
class cVeDcChannelDetail : public cViewElement, public cScrapManager {
private:
    const cChannel *channel;
    int actorsIndex;
public:
    cVeDcChannelDetail(void);
    virtual ~cVeDcChannelDetail(void);
    void Close(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c);
    bool Parse(bool forced = false);
};

class cVeDcChannelListDetail : public cVeDcChannelDetail {
public:
    cVeDcChannelListDetail(void) {};
    virtual ~cVeDcChannelListDetail(void) {};
};

class cVeDcGroupChannelListDetail : public cVeDcChannelDetail {
public:
    cVeDcGroupChannelListDetail(void) {};
    virtual ~cVeDcGroupChannelListDetail(void) {};
};
#endif //__VIEWELEMENTSDC_H