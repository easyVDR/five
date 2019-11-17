#ifndef __VIEWDETAIL_H
#define __VIEWDETAIL_H

#include "../services/epgsearch.h"
#include "../extensions/scrapmanager.h"
#include "viewelement.h"
/******************************************************************
* cViewDetail
******************************************************************/
class cViewDetail : public cViewElement, public cScrapManager {
protected:
    int plugId;
    int plugMenuId;
    cArea *activeTab;
    int activeTabIndex;
    int numTabs;
    void SetActiveTab(void);
public:
    cViewDetail(void);
    virtual ~cViewDetail(void);
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    int GetWidth(void);
    void ResetTabs(void);
    void Clear(bool forceClearBackground = false);
    void Close(void);
    void Render(void);
    void Scrollbar(int &barheight, int &offset, bool &end);
    bool ScrollUp(bool page = false);
    bool ScrollDown(bool page = false);
    int GetTabs(vector<const char*> &tabs);
    int NumTabs(void) { return numTabs; };
    int ActiveTab(void) { return activeTabIndex; };
    void NextTab(void);
    void PrevTab(void);
    void SetTransparency(int transparency, bool forceDetached = false);
};
/******************************************************************
* cViewDetailEpg
******************************************************************/
class cViewDetailEpg : public cViewDetail {
protected:
    const cEvent *event;
    int rerunsIndex;
    int actorsIndex;
    cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *LoadReruns(void);
    int NumReruns(cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *reruns);
    void SetReruns(cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *reruns);
    void SetEpgPictures(int eventId);
public:
    cViewDetailEpg(void);
    virtual ~cViewDetailEpg(void);
    void SetTokenContainer(void);
    void SetEvent(const cEvent *event) { this->event = event; };
    bool Parse(bool forced = false);
};
/******************************************************************
* cViewDetailRec
******************************************************************/
class cViewDetailRec : public cViewDetail {
protected:
    const cRecording *recording;
    int actorsIndex;
    void SetRecInfos(void);
    int ReadSizeVdr(const char *strPath);
    string StripXmlTag(string &Line, const char *Tag);
    void SetRecordingImages(const char *recPath);
public:
    cViewDetailRec(void);
    virtual ~cViewDetailRec(void);
    void SetTokenContainer(void);
    void SetRecording(const cRecording *recording) { this->recording = recording; };
    bool Parse(bool forced = false);
};
/******************************************************************
* cViewDetailText
******************************************************************/
class cViewDetailText : public cViewDetail {
protected:
    const char *text;
public:
    cViewDetailText(void);
    virtual ~cViewDetailText(void);
    void SetTokenContainer(void);
    void SetText(const char *text) { this->text = text; };
    bool Parse(bool forced = false);
};

/******************************************************************
* cViewDetailPlugin
******************************************************************/
class cViewDetailPlugin : public cViewDetail {
protected:
public:
    cViewDetailPlugin(void);
    virtual ~cViewDetailPlugin(void);
    void SetTokenContainer(void);
    void Set(skindesignerapi::cTokenContainer *tk);
    bool Parse(bool forced = false);
};

/******************************************************************
* cViewDetailAdvancedPlugin
******************************************************************/
class cViewDetailAdvancedPlugin : public cViewDetail {
protected:
    int plugViewId;
public:
    cViewDetailAdvancedPlugin(int viewId, int plugId);
    virtual ~cViewDetailAdvancedPlugin(void);
    void SetTokenContainer(void);
    void Set(skindesignerapi::cTokenContainer *tk);
    bool Parse(bool forced = false);
};
#endif //__VIEWDETAIL_H