#ifndef __VIEWDISPLAYMENU_H
#define __VIEWDISPLAYMENU_H

#include "view.h"
#include "viewdetail.h"

#if defined(APIVERSNUM) && APIVERSNUM < 20301
#ifndef MENU_ORIENTATION_DEFINED
enum eMenuOrientation {
    moVertical = 0,
    moHorizontal
};
#endif
#endif

/***********************************************************
* cViewMenu
***********************************************************/
class cSubView;
class cViewMenuDefault;
class cViewMenuMain;
class cViewMenuSetup;
class cViewMenuSchedules;
class cViewMenuChannels;
class cViewMenuTimers;
class cViewMenuRecordings;
class cViewMenuDetail;

class cViewMenu : public cView {
protected:
    map<string,int> subviewNames;
    cSubView **subViews;
    int numSubviews;
    cSubView *activeSubview;
    cSubView *activeSubviewLast;
    cViewMenuDefault    *menuDefault;
    cViewMenuMain       *menuMain;
    cViewMenuSetup      *menuSetup;
    cViewMenuSchedules  *menuSchedules;
    cViewMenuChannels   *menuChannels;
    cViewMenuTimers     *menuTimers;
    cViewMenuRecordings *menuRecordings;
    cViewMenuDetail     *menuDetailedEpg;
    cViewMenuDetail     *menuDetailedRec;
    cViewMenuDetail     *menuDetailedText;
    eMenuCategory menuCat;
    //name of current plugin for menu icon
    const char *plugName;
    //external plugin menus
    bool pluginIdSet;
    int plugId;
    int plugMenuId;
    //status variables
    bool menuChange;
    bool listChange;
    bool detailViewInit;
    void SetViewElements(void);
    void SetViewElementObjects(void);
    void SetSubViews(void);
    void ClearVariables(void);
    int SubviewId(const char *name);
    bool SetPluginSubView(eMenuCategory menuCat);
    void WakeViewElements(void);
public:
    cViewMenu(void);
    virtual ~cViewMenu(void);
    void SetGlobals(cGlobals *globals);
    void PreCache(void);
    bool ValidSubView(const char *subView);
    static cSubView *CreateSubview(const char *name);
    static cSubView *CreatePluginview(const char *plugname, int plugId, int menuNumber, int menuType);
    void AddSubview(const char *sSubView, cSubView *subView);
    void AddPluginview(cSubView *plugView);
    void SetSubView(eMenuCategory MenuCat);
    eMenuCategory MenuCat(void) { return menuCat; };
    void SetSortMode(eMenuSortMode sortMode);
    void SetPluginMenu(int plugId, int plugMenuId);
    int NumListItems(void);
    eMenuOrientation MenuOrientation(void);
    const cFont *GetTextAreaFont(void);
    int GetTextAreaWidth(void);
    int GetListWidth(void);
    void SetTitleHeader(const char *title);
    void SetChannelHeader(const cEvent *event);
    void SetMessage(eMessageType type, const char *text);
    void SetMenuButtons(const char *red, const char *green, const char *yellow, const char *blue);
    void SetScrollbar(int total, int offset);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    void SetItem(const char *text, int index, bool current, bool selectable);
    bool SetItemEvent(const cEvent *event, int index, bool current, bool selectable, const cChannel *channel, bool withDate, eTimerMatch timerMatch);
    bool SetItemTimer(const cTimer *timer, int index, bool current, bool selectable);
    bool SetItemChannel(const cChannel *channel, int index, bool current, bool selectable, bool withProvider);
    bool SetItemRecording(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New);
    bool SetItemPlugin(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable);
    void SetEvent(const cEvent *event);
    void SetRecording(const cRecording *recording);
    void SetText(const char *text);
    bool SetPluginText(skindesignerapi::cTokenContainer *tk);
    void SetCurrentRecording(const char *currentRec);
    void KeyDetailView(bool up, bool page);
    bool Init(void);
    void Close(void);
    void Clear(void);
    void Flush(void);
    void SetTransparency(int transparency, bool forceDetached = false);
    void Debug(void);
};

/***********************************************************
* cSubView
***********************************************************/
class cSubView : public cView {
protected:
    eMenuCategory menuCat;
    int plugId;
    int plugMenuId;
    cViewList *viewList;
    vector<cViewList*> viewLists;
    cViewElement *background;
    cVeDmHeader *header;
    cVeDateTime *datetime;
    cVeTime *time;
    cVeMessage *message;
    cVeDmSortmode *sortmode;
    cVeDmColorbuttons *colorbuttons;
    cVeDmScrollbar *scrollbar;
    virtual void SetViewElementObjects(void);
    virtual void SetViewElements(void);
public:
    cSubView(const char *name);
    virtual ~cSubView(void);
    virtual void SetGlobals(cGlobals *globals);
    virtual void PreCache(void);
    bool ViewElementSet(int ve);
    bool ViewElementHorizontalSet(int ve);
    void SetViewElement(eVeDisplayMenu ve, cViewElement *viewElement);
    void SetViewElementHorizontal(eVeDisplayMenu ve, cViewElement *viewElement);
    void AddViewList(cViewList *viewList);
    virtual void AddTab(cArea *tab) {};
    int NumListItems(void);
    eMenuOrientation MenuOrientation(void);
    void SetMenuCategory(eMenuCategory menuCat) { this->menuCat = menuCat; };
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    void SetTitle(const char *title);
    void SetMessage(eMessageType type, const char *text);
    void SetChannel(const cChannel *channel);
    void SetMenuButtons(const char *red, const char *green, const char *yellow, const char *blue);
    void SetScrollbar(int total, int offset);
    void SetSortMode(eMenuSortMode sortMode);
    virtual void Close(void);
    virtual void Clear(void);
    void ClearViewList(void);
    void WakeViewElements(void);
    virtual void DrawStaticVEs(void);
    virtual void DrawDynamicVEs(void);
    void DrawList(void);
    virtual void DrawDetailedView(void) {};
    virtual void UpdateDetailedView(void) {};
    void SetTransparency(int transparency, bool forceDetached = false);
};
/***********************************************************
* cViewMenuDefault
***********************************************************/
class cViewMenuDefault : public cSubView {
private:
    cViewListDefault *listDefault;
    void SetViewElementObjects(void);
public:
    cViewMenuDefault(const char *name);
    virtual ~cViewMenuDefault(void);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    void SetPlugin(const char *plugName);
    void SetItem(const char *text, int index, bool current, bool selectable);
    const cFont *GetTextAreaFont(void);
    int GetListWidth(void);
};
/***********************************************************
* cViewMenuMain
***********************************************************/
class cViewMenuMain : public cSubView {
private:
    cViewListMain *listMain;
    cVeDmTimers *timers;
    cVeDevices *devices;
    cVeCurrentWeather *weather;
    cVeDmDiscusage *discusage;
    cVeDmSystemload *load;
    cVeDmSystemmemory *memory;
    cVeDmVdrstatistics *vdrstats;
    cVeDmTemperatures *temperatures;
    cVeDmCurrentschedule *currentSchedule;
    cVeDmLastrecordings *lastRecordings;
    cVeCustomTokens *customTokens;
    uint64_t lastDrawDynamic;
    void ClearVariables(void);
    void SetViewElements(void);
    void SetViewElementObjects(void);
public:
    cViewMenuMain(const char *name);
    virtual ~cViewMenuMain(void);
    void Clear(void);
    void SetItem(const char *text, int index, bool current, bool selectable);
    void SetCurrentRecording(const char *currentRec);
    void DrawStaticVEs(void);
    void DrawDynamicVEs(void);
    const char *GetPlugin(void);
};
/***********************************************************
* cViewMenuSetup
***********************************************************/
class cViewMenuSetup : public cSubView {
private:
    cViewListMain *listSetup;
    void SetViewElementObjects(void);
public:
    cViewMenuSetup(const char *name);
    virtual ~cViewMenuSetup(void);
    void SetItem(const char *text, int index, bool current, bool selectable);
};
/***********************************************************
* cViewMenuSchedules
***********************************************************/
class cViewMenuSchedules : public cSubView {
private:
    cViewListSchedules *listSchedules;
    void SetViewElementObjects(void);
public:
    cViewMenuSchedules(const char *name);
    virtual ~cViewMenuSchedules(void);
    void SetItem(const cEvent *event, int index, bool current, bool selectable, const cChannel *channel, bool withDate, eTimerMatch timerMatch);
};
/***********************************************************
* cViewMenuChannels
***********************************************************/
class cViewMenuChannels : public cSubView {
private:
    cViewListChannels *listChannels;
    void SetViewElementObjects(void);
public:
    cViewMenuChannels(const char *name);
    virtual ~cViewMenuChannels(void);
    void SetItem(const cChannel *channel, int index, bool current, bool selectable, bool withProvider);
};
/***********************************************************
* cViewMenuTimers
***********************************************************/
class cViewMenuTimers : public cSubView {
private:
    cViewListTimers *listTimers;
    void SetViewElementObjects(void);
public:
    cViewMenuTimers(const char *name);
    virtual ~cViewMenuTimers(void);
    void SetItem(const cTimer *timer, int index, bool current, bool selectable);
};
/***********************************************************
* cViewMenuRecordings
***********************************************************/
class cViewMenuRecordings : public cSubView {
private:
    cViewListRecordings *listRecordings;
    void SetViewElementObjects(void);
public:
    cViewMenuRecordings(const char *name);
    virtual ~cViewMenuRecordings(void);
    void SetItem(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New);
};

/***********************************************************
* cViewMenuPlugins
***********************************************************/
class cViewMenuPlugin : public cSubView {
private:
    cViewListPlugin *listPlugin;
    void SetViewElementObjects(void);
public:
    cViewMenuPlugin(const char *name);
    virtual ~cViewMenuPlugin(void);
    void SetItem(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable);
};

/***********************************************************
* cViewMenuDetail
***********************************************************/
class cViewMenuDetail : public cSubView {
private:
    bool firstTab;
    cVeDmDetailheaderEpg *detailedheaderEpg;
    cVeDmDetailheaderRec *detailedheaderRec;
    cVeDmDetailheaderPlugin *detailedheaderPlug;
    cVeDmTablabels *tablabels;
    cViewDetail *detailView;
    cViewDetailEpg *detailViewEpg;
    cViewDetailRec *detailViewRec;
    cViewDetailText *detailViewText;
    cViewDetailPlugin *detailViewPlugin;
    void SetDetailedView(void);
    void SetViewElements(void);
    void SetViewElementObjects(void);
    void DrawScrollbar(void);
public:
    cViewMenuDetail(const char *name);
    virtual ~cViewMenuDetail(void);
    void SetGlobals(cGlobals *globals);
    void AddTab(cArea *tab);
    void PreCache(void);
    int GetWidth(void);
    void SetEvent(const cEvent *event);
    void SetRecording(const cRecording *recording);
    void SetText(const char *text);
    void SetPluginText(skindesignerapi::cTokenContainer *tk);
    void Clear(void);
    void Close(void);
    void DrawStaticVEs(void);
    void DrawDynamicVEs(void);
    void DrawDetailedView(void);
    void KeyLeft(void);
    void KeyRight(void);
    void KeyUp(void);
    void KeyDown(void);
    void SetTransparency(int transparency, bool forceDetached = false);
};

#endif //__VIEWDISPLAYMENU_H