#ifndef __VIEWELEMENTSDM_H
#define __VIEWELEMENTSDM_H

#include "viewelement.h"
#include "../extensions/scrapmanager.h"

/******************************************************************
* cVeDmHeader
******************************************************************/
class cVeDmHeader : public cViewElement {
private:
    bool changed;
    char *title;
    char *channelName;
    int channelNumber;
    char *channelId;
    bool epgSearchFav;
public:
    cVeDmHeader(void);
    virtual ~cVeDmHeader(void);
    void SetTokenContainer(void);
    void SetTitle(const char *title);
    void SetChannel(const cChannel *channel);
    void ClearChannel(void);
    void Set(eMenuCategory menuCat);
    void IsEpgSearchFav(bool isFav) { epgSearchFav = isFav;} ;
};

/******************************************************************
* cVeDmSortmode
******************************************************************/
class cVeDmSortmode : public cViewElement {
private:
    eMenuSortMode sortMode;
    eMenuSortMode lastSortMode;
public:
    cVeDmSortmode(void);
    virtual ~cVeDmSortmode(void);
    void Reset(void) { lastSortMode = msmUnknown; }
    void SetTokenContainer(void);
    void Set(eMenuSortMode sortMode);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmColorbuttons
******************************************************************/
class cVeDmColorbuttons : public cViewElement {
private:
    bool changed;
    char *red;
    char *green;
    char *yellow;
    char *blue;
public:
    cVeDmColorbuttons(void);
    virtual ~cVeDmColorbuttons(void);
    void SetTokenContainer(void);
    void SetButtons(const char *red, const char *green, const char *yellow, const char *blue);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmScrollbar
******************************************************************/
class cVeDmScrollbar : public cViewElement {
private:
public:
    cVeDmScrollbar(void);
    virtual ~cVeDmScrollbar(void);
    void SetTokenContainer(void);
    void SetList(int numDisplayed, int offset, int numMax);
    void SetDetail(int height, int offset, bool end);
};


/******************************************************************
* cVeDmTimers
******************************************************************/
class cVeDmTimers : public cViewElement {
private:
    int timerIndex;
public:
    cVeDmTimers(void);
    virtual ~cVeDmTimers(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmCurrentschedule
******************************************************************/
class cVeDmCurrentschedule : public cViewElement, public cScrapManager {
private:
    const char *rec;
    void ParseFromChannel(const cChannel *channel);
    void ParseFromRecording(const cRecording *recording);
    void RecName(string &path, string &name, string &folder);
public:
    cVeDmCurrentschedule(void);
    virtual ~cVeDmCurrentschedule(void);
    void SetTokenContainer(void);
    void SetRecording(const char *currentRec);
    bool Parse(bool forced = false);    
};

/******************************************************************
* cVeDmDiscusage
******************************************************************/
class cVeDmDiscusage : public cViewElement {
private:
public:
    cVeDmDiscusage(void);
    virtual ~cVeDmDiscusage(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmSystemload
******************************************************************/
class cVeDmSystemload : public cViewElement {
private:
    double lastSystemLoad;
public:
    cVeDmSystemload(void);
    virtual ~cVeDmSystemload(void);
    void Close(void);
    void Reset(void) { lastSystemLoad = -1.0f; }
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmSystemmemory
******************************************************************/
class cVeDmSystemmemory : public cViewElement {
private:
    int lastMemUsage;
public:
    cVeDmSystemmemory(void);
    virtual ~cVeDmSystemmemory(void);
    void Close(void);
    void Reset(void) { lastMemUsage = -1; }
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmTemperatures
******************************************************************/
class cVeDmTemperatures : public cViewElement {
private:
    int lastCpuTemp;
    int lastGpuTemp;
public:
    cVeDmTemperatures(void);
    virtual ~cVeDmTemperatures(void);
    void Close(void);
    void Reset(void) { lastCpuTemp = -1; lastGpuTemp = -1; }
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmVdrstatistics
******************************************************************/
class cVeDmVdrstatistics : public cViewElement {
private:
    string lastVdrCPU;
    string lastVdrMEM;
public:
    cVeDmVdrstatistics(void);
    virtual ~cVeDmVdrstatistics(void);
    void Close(void);
    void Reset(void) { lastVdrCPU = "undefined"; lastVdrMEM = "undefined"; }
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmLastrecordings
******************************************************************/
class cVeDmLastrecordings : public cViewElement, public cScrapManager {
private:
    int recIndex;
    void RecName(string &path, string &name);
public:
    cVeDmLastrecordings(void);
    virtual ~cVeDmLastrecordings(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmDetailheaderEpg
******************************************************************/
class cVeDmDetailheaderEpg : public cViewElement, public cScrapManager {
private:
    const cEvent *event;
public:
    cVeDmDetailheaderEpg(void);
    virtual ~cVeDmDetailheaderEpg(void);
    void SetTokenContainer(void);
    void SetEvent(const cEvent *event);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmDetailheaderRec
******************************************************************/
class cVeDmDetailheaderRec : public cViewElement, public cScrapManager {
private:
    const cRecording *recording;
public:
    cVeDmDetailheaderRec(void);
    virtual ~cVeDmDetailheaderRec(void);
    void SetTokenContainer(void);
    void SetRecording(const cRecording *rec);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmDetailheaderPlugin
******************************************************************/
class cVeDmDetailheaderPlugin : public cViewElement {
private:
    int plugId;
    int plugMenuId;
public:
    cVeDmDetailheaderPlugin(void);
    virtual ~cVeDmDetailheaderPlugin(void);
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    void SetTokenContainer(void);
    void Set(skindesignerapi::cTokenContainer *tk);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDmTablabels
******************************************************************/
class cVeDmTablabels : public cViewElement {
private:
    int tabIndex;
    int activeTab;
    vector<const char*> tabs;
public:
    cVeDmTablabels(void);
    virtual ~cVeDmTablabels(void);
    void SetTokenContainer(void);
    void SetTabs(vector<const char*> &newTabs);
    void SetActiveTab(int activeTab) { SetDirty(); this->activeTab = activeTab; };
    bool Parse(bool forced = false);
};
#endif //__VIEWELEMENTSDM_H