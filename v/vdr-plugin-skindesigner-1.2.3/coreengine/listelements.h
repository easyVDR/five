#ifndef __LISTELEMENTS_H
#define __LISTELEMENTS_H

#include "viewelement.h"
#include "../extensions/scrapmanager.h"

#define MAX_TABS 6
/******************************************************************
* cListElement
******************************************************************/
class cListElement : public cViewElement , public cListShiftable {
protected:
    eMenuCategory menuCat;
    eOrientation orientation;
    int num;
    bool current;
    bool wasCurrent;
    bool selectable;
    bool selectedFromTop;
    bool suppressAnimation;
    cListShifter *listShifter;
    cViewElement *currentElement;
    char *ParseSeparator(const char *text);
    void StartListAnimation(void);
public:
    cListElement(void);
    cListElement(const cListElement &other);
    virtual ~cListElement(void) {};
    void SetMenuCategory(eMenuCategory menuCat) { this->menuCat = menuCat; };
    void SetOrientation(eOrientation orientation) { this->orientation = orientation; };
    void SetNumber(int number) { num = number; };
    void SetCurrent(bool cur);
    bool Current(void) { return current; };
    bool WasCurrent(void) { return wasCurrent; };
    void WakeCurrent(void);
    void SetSelectable(bool sel) { selectable = sel; };
    void SetSelectedFromTop(void) { selectedFromTop = true; };
    void SetSelectedFromBottom(void) { selectedFromTop = false; };
    void SetSuppressAnimation(bool suppress) { suppressAnimation = suppress; };
    bool DoScroll(void) { return current; };
    void Render(void);
    virtual void RenderCurrent(void) { };
    virtual void Close(void);
    int ListShiftTime(void) { return ShiftTime(); };
    int ShiftDistance(void);
    eOrientation ShiftOrientation(void);
    void SetIndicatorPosition(cPoint &position);
    void SetTransparency(int transparency, bool force = false);
    void StopListAnimation(void);
    virtual void Clear(bool forceClearBackground = false);
};

/******************************************************************
* cCurrentElement
******************************************************************/
class cCurrentElement : public cViewElement {
protected:
    int listX;
    int listY;
    int listWidth;
    int listHeight;
    int listNum;
public:
    cCurrentElement(void);
    virtual ~cCurrentElement(void) {};
    void SetListPosition(int x, int y, int width, int height, int num);
    void SetListTokens(skindesignerapi::cTokenContainer *tokenContainer);
};

/******************************************************************
* cLeMenuDefault
******************************************************************/
class cLeMenuDefault : public cListElement {
private:
    char *text;
    int *colX;
    int *colWidths;
    const char *plugName;
    const char *GetTabbedText(const char *s, int tab);
    void SetMenuCategory(void);
    void CheckProgressBar(const char *text, int tab);
public:
    cLeMenuDefault(void);
    cLeMenuDefault(const cLeMenuDefault &other);
    virtual ~cLeMenuDefault(void);
    void SetListInfo(int *colX, int *colWidths);
    void SetText(const char *text);
    void SetPlugin(const char *plugName) { this->plugName = plugName; };
    void SetTokenContainer(void);
    bool Parse(bool forced = true);
    void Clear(bool forceClearBackground = false);
};

/******************************************************************
* cVeMenuMain
******************************************************************/
class cVeMenuMain {
protected:
    char *text;
    char *number;
    char *label;
    void SplitText(void);
public:
    cVeMenuMain(void);
    virtual ~cVeMenuMain(void);
    void SetText(const char *text);
};

/******************************************************************
* cLeMenuMain
******************************************************************/
class cCeMenuMain;
class cLeMenuMain : public cListElement, public cVeMenuMain {
private:
    cCeMenuMain *currentMain;
public:
    cLeMenuMain(void);
    cLeMenuMain(const cLeMenuMain &other);
    virtual ~cLeMenuMain(void);
    void SetTokenContainer(void);
    void SetCurrentElement(cCeMenuMain *cur) { currentMain = cur; currentElement = (cViewElement*)cur; };
    void ClearCurrentElement(void);
    void SetText(const char *text);
    bool Parse(bool forced = true);
    void RenderCurrent(void);
    const char *PluginName(void);
};

/******************************************************************
* cCeMenuMain
******************************************************************/
class cCeMenuMain : public cCurrentElement, public cVeMenuMain {
private:
public:
    cCeMenuMain(void);
    virtual ~cCeMenuMain(void);
    void SetTokenContainer(void);
    void SetText(const char *text);
    bool Parse(bool forced = true);
};

/******************************************************************
* cVeMenuSchedules
******************************************************************/
class cVeMenuSchedules {
protected:
    const cEvent *event;
    const cChannel *channel;
    bool withDate;
    eTimerMatch timerMatch;
    bool epgSearchFav;
public:
    cVeMenuSchedules(void);
    virtual ~cVeMenuSchedules(void){};
    void SetEpgSearchFav(bool isFav) { epgSearchFav = isFav; };
};

/******************************************************************
* cLeMenuSchedules
******************************************************************/
class cCeMenuSchedules;
class cLeMenuSchedules : public cListElement, public cVeMenuSchedules {
private:
    cCeMenuSchedules *currentSchedules;
public:
    cLeMenuSchedules(void);
    cLeMenuSchedules(const cLeMenuSchedules &other);
    virtual ~cLeMenuSchedules(void);
    void SetTokenContainer(void);
    void SetCurrentElement(cCeMenuSchedules *cur) { currentSchedules = cur; currentElement = (cViewElement*)cur; };
    void ClearCurrentElement(void);
    void Set(const cEvent *event, const cChannel *channel, bool withDate, eTimerMatch timerMatch);
    bool Parse(bool forced = true);
    void RenderCurrent(void);
};

/******************************************************************
* cCeMenuSchedules
******************************************************************/
class cCeMenuSchedules : public cCurrentElement, public cVeMenuSchedules, public cScrapManager {
private:
    eMenuCategory menuCat;
    int schedulesIndex;
public:
    cCeMenuSchedules(void);
    virtual ~cCeMenuSchedules(void);
    void SetTokenContainer(void);
    void Set(const cEvent *event, const cChannel *channel, bool withDate, eTimerMatch timerMatch, eMenuCategory menuCat);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeMenuChannels
******************************************************************/
class cCeMenuChannels;
class cLeMenuChannels : public cListElement {
private:
    cCeMenuChannels *currentChannel;
    const cChannel *channel;
    bool withProvider;
public:
    cLeMenuChannels(void);
    cLeMenuChannels(const cLeMenuChannels &other);
    virtual ~cLeMenuChannels(void);
    void SetTokenContainer(void);
    void SetCurrentElement(cCeMenuChannels *cur) { currentChannel = cur; currentElement = (cViewElement*)cur; };
    void ClearCurrentElement(void);
    void Set(const cChannel *channel, bool withProvider);
    bool Parse(bool forced = true);
    void RenderCurrent(void);
};

/******************************************************************
* cCeMenuChannels
******************************************************************/
class cCeMenuChannels : public cCurrentElement, public cScrapManager {
private:
    const cChannel *channel;
    bool withProvider;
    int schedulesIndex;
public:
    cCeMenuChannels(void);
    virtual ~cCeMenuChannels(void);
    void SetTokenContainer(void);
    void Set(const cChannel *channel, bool withProvider);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeMenuTimers
******************************************************************/
class cCeMenuTimers;
class cLeMenuTimers : public cListElement {
private:
    cCeMenuTimers *currentTimer;
    const cTimer *timer;
public:
    cLeMenuTimers(void);
    cLeMenuTimers(const cLeMenuTimers &other);
    virtual ~cLeMenuTimers(void);
    void SetTokenContainer(void);
    void SetCurrentElement(cCeMenuTimers *cur) { currentTimer = cur; currentElement = (cViewElement*)cur; };
    void ClearCurrentElement(void);
    void Set(const cTimer *timer);
    bool Parse(bool forced = true);
    void RenderCurrent(void);
};

/******************************************************************
* cCeMenuTimers
******************************************************************/
class cCeMenuTimers : public cCurrentElement, public cScrapManager {
private:
    const cTimer *timer;
public:
    cCeMenuTimers(void);
    virtual ~cCeMenuTimers(void);
    void SetTokenContainer(void);
    void Set(const cTimer *timer);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeMenuRecordings
******************************************************************/
class cCeMenuRecordings;
class cLeMenuRecordings : public cListElement, public cScrapManager {
private:
    cCeMenuRecordings *currentRecording;
    const cRecording *recording;
    int level;
    int total;
    int New;
    char *RecName(const char *path, int level);
    char *FolderName(const char *path, int level);
public:
    cLeMenuRecordings(void);
    cLeMenuRecordings(const cLeMenuRecordings &other);
    virtual ~cLeMenuRecordings(void);
    void SetTokenContainer(void);
    void SetCurrentElement(cCeMenuRecordings *cur) { currentRecording = cur; currentElement = (cViewElement*)cur; };
    void ClearCurrentElement(void);
    void Set(const cRecording *recording, int level, int total, int New);
    bool Parse(bool forced = true);
    void RenderCurrent(void);
    void Clear(bool forceClearBackground = false);
    void Close(void);
};

/******************************************************************
* cCeMenuRecordings
******************************************************************/
class cCeMenuRecordings : public cCurrentElement, public cScrapManager {
private:
    const cRecording *recording;
    int level;
    int total;
    int New;
public:
    cCeMenuRecordings(void);
    virtual ~cCeMenuRecordings(void);
    void SetTokenContainer(void);
    void Set(const cRecording *recording, int level, int total, int New);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeMenuPlugin
******************************************************************/
class cCeMenuPlugin;
class cLeMenuPlugin : public cListElement {
private:
    int plugId;
    int plugMenuId;
    cCeMenuPlugin *currentPlugin;
public:
    cLeMenuPlugin(void);
    cLeMenuPlugin(const cLeMenuPlugin &other);
    virtual ~cLeMenuPlugin(void);
    void SetTokenContainer(void);
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    void SetCurrentElement(cCeMenuPlugin *cur) { currentPlugin = cur; currentElement = (cViewElement*)cur; };
    void ClearCurrentElement(void);
    void Set(skindesignerapi::cTokenContainer *tk);
    bool Parse(bool forced = true);
    void RenderCurrent(void);
};

/******************************************************************
* cCeMenuPlugin
******************************************************************/
class cCeMenuPlugin : public cCurrentElement {
private:
    int plugId;
    int plugMenuId;
public:
    cCeMenuPlugin(void);
    virtual ~cCeMenuPlugin(void);
    void SetTokenContainer(void);
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    void Set(skindesignerapi::cTokenContainer *tk);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeAudioTracks
******************************************************************/
class cLeAudioTracks : public cListElement {
private:
    char *text;
public:
    cLeAudioTracks(void);
    virtual ~cLeAudioTracks(void);
    void SetTokenContainer(void);
    void Set(const char *text);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeChannelList
******************************************************************/
class cLeChannelList : public cListElement {
private:
    const cChannel *channel;
public:
    cLeChannelList(void);
    cLeChannelList(const cLeChannelList &other);
    virtual ~cLeChannelList(void);
    void SetTokenContainer(void);
    void Set(const cChannel *channel);
    bool Parse(bool forced = true);
};

/******************************************************************
* cLeGroupList
******************************************************************/
class cLeGroupList : public cListElement {
private:
    const char *group;
    int numChannels;
public:
    cLeGroupList(void);
    cLeGroupList(const cLeGroupList &other);
    virtual ~cLeGroupList(void);
    void SetTokenContainer(void);
    void Set(const char *group, int numChannels);
    bool Parse(bool forced = true);
};

#endif //__LISTELEMENTS_H