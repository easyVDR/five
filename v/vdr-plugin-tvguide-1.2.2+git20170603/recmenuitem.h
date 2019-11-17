#ifndef __TVGUIDE_RECMENUITEM_H
#define __TVGUIDE_RECMENUITEM_H

#define AUTO_ADVANCE_TIMEOUT 1500

#include <string>
#include <vdr/tools.h> 
#include "styledpixmap.h"
#include "timerconflict.h"
#include "searchtimer.h"

enum eRecMenuState {
    rmsConsumed,
    rmsNotConsumed,
    rmsRefresh,
    rmsContinue,
    rmsClose,
    rmsDisabled,
    //INSTANT TIMER
    rmsInstantRecord,
    rmsInstantRecordFolder,
    rmsIgnoreTimerConflict,
    rmsDeleteTimerConflictMenu,
    rmsEditTimerConflictMenu,
    rmsSearchRerunsTimerConflictMenu,
    rmsSaveTimerConflictMenu,
    rmsTimerConflictShowInfo,
    rmsDeleteTimer,
    rmsDeleteTimerConfirmation,
    rmsEditTimer,
    rmsSaveTimer,
    //SEARCH
    rmsSearch,
    rmsSearchWithOptions,
    rmsSearchPerform,
    rmsSearchShowInfo,
    rmsSearchRecord,
    rmsSearchRecordConfirm,
    rmsSearchNothingFoundConfirm,
    //SERIES TIMER
    rmsSeriesTimer,
    rmsSeriesTimerFolder,
    rmsSeriesTimerCreate,
    //SEARCHTIMER
    rmsSearchTimer,
    rmsSearchTimerOptions,
    rmsSearchTimers,
    rmsSearchTimerEdit,
    rmsSearchTimerEditAdvanced,
    rmsSearchTimerTest,
    rmsSearchTimerSave,
    rmsSearchTimerCreateWithTemplate,
    rmsSearchTimerDeleteConfirm,
    rmsSearchTimerDelete,
    rmsSearchTimerDeleteWithTimers,
    rmsSearchTimerRecord,
    //SWITCHTIMER
    rmsSwitchTimer,
    rmsSwitchTimerCreate,
    rmsSwitchTimerDelete,
    //RECORDINGS SEARCH
    rmsRecordingSearch,
    rmsRecordingSearchResult,
    //TIMER CONFLICTS
    rmsTimerConflict,
    rmsTimerConflicts,
    rmsTimerConflictIgnoreReruns,
    rmsTimerConflictRecordRerun,
    //TIMELINE
    rmsTimeline,
    rmsTimelineTimerEdit,
    rmsTimelineTimerSave,
    rmsTimelineTimerDelete,
    //FAVORITES
    rmsFavoritesRecord,
    rmsFavoritesRecordConfirm,
    rmsFavoritesNow,
    rmsFavoritesNext,    
    rmsFavoritesUser1,
    rmsFavoritesUser2,
    rmsFavoritesUser3,
    rmsFavoritesUser4,
};

enum eDependend {
    eGreater,
    eLower,
};

// --- cRecMenuItem  -------------------------------------------------------------
class cRecMenuItem : public cListObject, public cStyledPixmap {
protected:
    int x, y;
    int width, height;
    bool selectable;
    bool active;
    bool defaultBackground;
    bool drawn;
    eRecMenuState action;
    tColor colorText;
    tColor colorTextBack;
    const cFont *font;
    const cFont *fontSmall;
    const cFont *fontLarge;
public:
    cRecMenuItem(void);
    virtual ~cRecMenuItem(void);
    void SetGeometry(int x, int y, int width);
    virtual void SetPixmaps(void);
    virtual int GetHeight(void) { return height; };
    virtual int GetWidth(void) { return 0; };
    virtual void CalculateHeight(int textWidth) {};
    virtual void setActive(void) { this->active = true; }
    virtual void setInactive(void) { this->active = false; }
    bool isSelectable(void) { return selectable; }
    bool isActive(void) { return active; }
    virtual void setBackground(void);
    virtual void Draw(void) {};
    virtual void Hide(void) { if (pixmap) pixmap->SetLayer(-1);};
    virtual void Show(void) { if (pixmap) pixmap->SetLayer(4);};
    virtual int GetIntValue(void) { return -1; };
    virtual time_t GetTimeValue(void) { return 0; };
    virtual bool GetBoolValue(void) { return false; };
    virtual cString GetStringValue(void) { return cString(""); };
    virtual const cEvent *GetEventValue(void) { return NULL; };
#if VDRVERSNUM >= 20301
    virtual const cTimer *GetTimerValue(void) { return NULL; };
#else
    virtual cTimer *GetTimerValue(void) { return NULL; };
#endif
    virtual eRecMenuState ProcessKey(eKeys Key) { return rmsNotConsumed; };
};

// --- cRecMenuItemButton  -------------------------------------------------------
class cRecMenuItemButton : public cRecMenuItem {
private:
    cString text;
    bool halfWidth;
    bool alignLeft;
    const cFont *fontButtons;
    cPixmap *pixmapText;
public:
    cRecMenuItemButton(const char *text, eRecMenuState action, bool active, bool halfWidth = false, bool alignLeft = false, bool largeFont = false);
    virtual ~cRecMenuItemButton(void);
    int GetWidth(void);
    void SetPixmaps(void);
    void Draw(void);
    void Hide(void);
    void Show(void);
    cString GetStringValue(void) { return text; };
    eRecMenuState ProcessKey(eKeys Key);
};

// --- cRecMenuItemButtonYesNo  -------------------------------------------------------
class cRecMenuItemButtonYesNo : public cRecMenuItem {
private:
    cString textYes;
    cString textNo;
    eRecMenuState actionNo;
    bool yesActive;
    cStyledPixmap *pixmapNo;
    tColor colorTextNo;
    tColor colorTextNoBack;
public:
    cRecMenuItemButtonYesNo(cString textYes,
                            cString textNo,
                            eRecMenuState actionYes, 
                            eRecMenuState actionNo,
                            bool active);
    virtual ~cRecMenuItemButtonYesNo(void);
    void SetPixmaps(void);
    void setBackground(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemInfo  -------------------------------------------------------
class cRecMenuItemInfo : public cRecMenuItem {
private:
    cString text;
    cTextWrapper wrapper;
    int border;
    const cFont *fontInfo;
public:
    cRecMenuItemInfo(const char *text, bool largeFont = false);
    virtual ~cRecMenuItemInfo(void);
    void setBackground(void);
    void CalculateHeight(int textWidth);
    void Draw(void);
};

// --- cRecMenuItemInt  -------------------------------------------------------
class cRecMenuItemInt : public cRecMenuItem {
private:
    cString text;
    int currentVal;
    int *callback;
    int minVal;
    int maxVal;
    cPixmap *pixmapVal;
    bool fresh;
    void DrawValue(void);
public:
    cRecMenuItemInt(cString text,
                    int initialVal,
                    int minVal,
                    int maxVal,
                    bool active = false,
                    int *callback = NULL,
                    eRecMenuState action = rmsNotConsumed);
    virtual ~cRecMenuItemInt(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    void setBackground(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return currentVal; };
};

// --- cRecMenuItemBool  -------------------------------------------------------
class cRecMenuItemBool : public cRecMenuItem {
private:
    cString text;
    bool yes;
    bool *callback;
    cPixmap *pixmapVal;
    bool refresh;
    void DrawValue(void);
public:
    cRecMenuItemBool(cString text,
                     bool initialVal,
                     bool refresh = false,
                     bool active = false,
                     bool *callback = NULL,
                     eRecMenuState action = rmsNotConsumed);
    virtual ~cRecMenuItemBool(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    bool GetBoolValue(void) { return yes; };
};

// --- cRecMenuItemSelect  -------------------------------------------------------
class cRecMenuItemSelect : public cRecMenuItem {
private:
    cString text;
    int currentVal;
    int *callback;
    std::vector<std::string> strings;
    int numValues;
    cPixmap *pixmapVal;
    void DrawValue(void);
public:
    cRecMenuItemSelect(cString text,
                       std::vector<std::string> Strings,
                       int initialVal,
                       bool active = false,
                       int *callback = NULL,
                       eRecMenuState action = rmsNotConsumed);
    virtual ~cRecMenuItemSelect(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return currentVal; };
    cString GetStringValue(void) { return strings[currentVal].c_str(); };
};

// --- cRecMenuItemSelectDirectory  -------------------------------------------------------
class cRecMenuItemSelectDirectory : public cRecMenuItem {
private:
    cString text;
    std::string originalFolder;
    std::vector<std::string> folders;
    int currentVal;
    char *callback;
    int numValues;
    cPixmap *pixmapVal;
    void DrawValue(void);
    int GetInitial(void);
    void SetCallback(void);
public:
    cRecMenuItemSelectDirectory(cString text,
                                std::string originalFolder,
                                bool active = false,
                                char *callback = NULL,
                                eRecMenuState action = rmsNotConsumed,
                                bool isSearchTimer = false);
    virtual ~cRecMenuItemSelectDirectory(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemText  -------------------------------------------------------
class cRecMenuItemText : public cRecMenuItem {
private:
    cString title;
    char *value;
    char *callback;
    int length;
    const char *allowed;
    int pos, offset;
    bool insert, newchar, uppercase;
    int lengthUtf8;
    uint *valueUtf8;
    uint *allowedUtf8;
    uint *charMapUtf8;
    uint *currentCharUtf8;
    eKeys lastKey;
    cTimeMs autoAdvanceTimeout;
    cPixmap *pixmapVal;
    cStyledPixmap *pixmapKeyboard;
    cPixmap *pixmapKeyboardHighlight;
    cPixmap *pixmapKeyboardIcons;
    int keyboardWidth;
    int gridWidth;
    int gridHeight;
    int keyboardHeight;
    bool keyboardDrawn;
    uint *IsAllowed(uint c);
    void AdvancePos(void);
    uint Inc(uint c, bool Up);
    void Type(uint c);
    void Insert(void);
    void Delete(void);
    void EnterEditMode(void);
    void LeaveEditMode(bool SaveValue = false);
    bool InEditMode(void) { return valueUtf8 != NULL; };    
    void SetText(void);
    void ActivateKeyboard(void);
    void DeactivateKeyboard(void);
    void HighlightSMSKey(int num);
    void ClearSMSKey(void);
    char *GetSMSKeys(int num);
    void DrawValue(char *newValue);
public:
    cRecMenuItemText(cString title,
                     char *initialVal,
                     int length,
                     bool active = false,
                     char *callback = NULL);
    virtual ~cRecMenuItemText(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    void setBackground(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    cString GetStringValue(void) { return value; };
};


// --- cRecMenuItemTime  -------------------------------------------------------
class cRecMenuItemTime : public cRecMenuItem {
private:
    cString text;
    int value;
    int *callback;
    int mm;
    int hh;
    int pos;
    bool fresh;
    cPixmap *pixmapVal;
    void DrawValue(void);
public:
    cRecMenuItemTime(cString text,
                    int initialVal,
                    bool active = false,
                    int *callback = NULL,
                    eRecMenuState action = rmsNotConsumed);
    virtual ~cRecMenuItemTime(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return value; };
};

// --- cRecMenuItemDay  -------------------------------------------------------
class cRecMenuItemDay : public cRecMenuItem {
private:
    cString text;
    time_t currentVal;
    time_t *callback;
    cPixmap *pixmapVal;
    void DrawValue(void);
public:
    cRecMenuItemDay(cString text,
                    time_t initialVal,
                    bool active = false,
                    time_t *callback = NULL,
                    eRecMenuState action = rmsNotConsumed);
    virtual ~cRecMenuItemDay(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    time_t GetTimeValue(void) { return currentVal; };
};

// --- cRecMenuItemTimer  -------------------------------------------------------
class cRecMenuItemTimer : public cRecMenuItem {
private:
    const cTimer *timer;
    eRecMenuState action2;
    eRecMenuState action3;
    eRecMenuState action4;
    int iconActive;
    cPixmap *pixmapIcons;
    cPixmap *pixmapStatus;
    time_t conflictStart;
    time_t conflictStop;
    time_t overlapStart;
    time_t overlapStop;
    int DrawIcons(void);
    void DrawTimerConflict(void);
public:
    cRecMenuItemTimer(const cTimer *timer, 
                      eRecMenuState action1, 
                      eRecMenuState action2,
                      eRecMenuState action3,
                      eRecMenuState action4,
                      time_t conflictStart,
                      time_t conflictStop,
                      time_t overlapStart,
                      time_t overlapStop,
                      bool active);
    virtual ~cRecMenuItemTimer(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemTimerConflictHeader -------------------------------------------------------

class cRecMenuItemTimerConflictHeader: public cRecMenuItem {
private:
    cPixmap *pixmapStatus;
    time_t conflictStart;
    time_t conflictStop;
    time_t overlapStart;
    time_t overlapStop;
public:
    cRecMenuItemTimerConflictHeader(time_t conflictStart,
                                    time_t conflictStop,
                                    time_t overlapStart,
                                    time_t overlapStop);
    virtual ~cRecMenuItemTimerConflictHeader(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    void setBackground(void);
    void Draw(void);
};

// --- cRecMenuItemEvent  -------------------------------------------------------
class cRecMenuItemEvent : public cRecMenuItem {
private:
    const cEvent *event;
    eRecMenuState action2;
    int iconActive;
    cPixmap *pixmapText;
    cPixmap *pixmapIcons;
    int DrawIcons(void);
public:
    cRecMenuItemEvent(const cEvent *event, 
                      eRecMenuState action1, 
                      eRecMenuState action2, 
                      bool active);
    virtual ~cRecMenuItemEvent(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    const cEvent *GetEventValue(void) { return event; };
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemChannelChooser -------------------------------------------------------
class cRecMenuItemChannelChooser : public cRecMenuItem {
private:
    cString text;
#if VDRVERSNUM >= 20301
    const cChannel *channel;
#else
    cChannel *channel;
#endif
    int channelNumber;
    int *callback;
    bool initialChannelSet;
    bool fresh;
    cPixmap *pixmapChannel;
    void DrawValue(void);
public:
    cRecMenuItemChannelChooser (cString text,
#if VDRVERSNUM >= 20301
                                const cChannel *initialChannel,
#else
                                cChannel *initialChannel,
#endif
                                bool active = false,
                                int *callback = NULL,
                                eRecMenuState action = rmsNotConsumed);
    virtual ~cRecMenuItemChannelChooser(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void);
};

// --- cRecMenuItemDayChooser -------------------------------------------------------
class cRecMenuItemDayChooser : public cRecMenuItem {
private:
    cString text;
    int weekdays;
    int *callback;
    bool epgsearchMode;
    std::string days;
    int daysX;
    int daysY;
    int daysSize;
    int selectedDay;
    cPixmap *pixmapWeekdays;
    cPixmap *pixmapWeekdaysSelect;
    void SetSizes(void);
    void DrawDays(void);
    void DrawHighlight(int day);
    void ToggleDay(void);
    bool WeekDaySet(unsigned day);
public:
    cRecMenuItemDayChooser (cString text,
                            int weekdays,
                            bool active = false,
                            int *callback = NULL);
    virtual ~cRecMenuItemDayChooser(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void setBackground(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return weekdays;} ;
};

// --- cRecMenuItemRecording  -------------------------------------------------------
class cRecMenuItemRecording : public cRecMenuItem {
private:
#if VDRVERSNUM >= 20301
    const cRecording *recording;
#else
    cRecording *recording;
#endif
    cPixmap *pixmapText;
public:
#if VDRVERSNUM >= 20301
    cRecMenuItemRecording(const cRecording *recording, bool active);
#else
    cRecMenuItemRecording(cRecording *recording, bool active);
#endif
    virtual ~cRecMenuItemRecording(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
};

// --- cRecMenuItemTimelineHeader  -------------------------------------------------------
class cRecMenuItemTimelineHeader : public cRecMenuItem {
private:
    time_t day;
#if VDRVERSNUM >= 20301
    const cTimer *timer;
#else
    cTimer *timer;
#endif
    std::vector<cTVGuideTimerConflict*> conflicts;
    cPixmap *pixmapTimeline;
    cPixmap *pixmapTimerInfo;
    cPixmap *pixmapTimerConflicts;
    int width5Mins;
    int x0;
    bool timelineDrawn;
    void DrawTimeline(void);
    void DrawTimerConflicts(void);
    void DrawCurrentTimer(void);
public:
    cRecMenuItemTimelineHeader(time_t day, std::vector<cTVGuideTimerConflict*> conflictsToday);
    virtual ~cRecMenuItemTimelineHeader(void);
    void SetDay(time_t day) { this->day = day; };
    void SetPixmaps(void);
#if VDRVERSNUM >= 20301
    void SetCurrentTimer(const cTimer *timer) { this->timer = timer; };
#else
    void SetCurrentTimer(cTimer *timer) { this->timer = timer; };
#endif
    void UnsetCurrentTimer(void) { timer = NULL; };
    void RefreshTimerDisplay(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
};

// --- cRecMenuItemTimelineTimer  -------------------------------------------------------
class cRecMenuItemTimelineTimer : public cRecMenuItem {
private:
#if VDRVERSNUM >= 20301
    const cTimer *timer;
#else
    cTimer *timer;
#endif
    std::vector<cTVGuideTimerConflict*> conflicts;
    cPixmap *pixmapBack;
    cPixmap *pixmapTimerConflicts;
    cRecMenuItemTimelineHeader *header;
    int x0;
    int width5Mins;
    time_t start;
    time_t stop;
    void DrawBackground(void);
    void DrawTimerBar(void);
    void DrawTimeScale(void);
    void DrawTimerConflicts(void);
    void DrawNoTimerInfo(void);
public:
#if VDRVERSNUM >= 20301
    cRecMenuItemTimelineTimer(const cTimer *timer, time_t start, time_t stop, std::vector<cTVGuideTimerConflict*> conflictsToday, cRecMenuItemTimelineHeader *header, bool active);
#else
    cRecMenuItemTimelineTimer(cTimer *timer, time_t start, time_t stop, std::vector<cTVGuideTimerConflict*> conflictsToday, cRecMenuItemTimelineHeader *header, bool active);
#endif
    virtual ~cRecMenuItemTimelineTimer(void);
    void setActive(void);
    void setInactive(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
#if VDRVERSNUM >= 20301
    const cTimer *GetTimerValue(void);
#else
    cTimer *GetTimerValue(void);
#endif
    eRecMenuState ProcessKey(eKeys Key);
};

// --- cRecMenuItemSearchTimer  -------------------------------------------------------
class cRecMenuItemSearchTimer : public cRecMenuItem {
private:
    cTVGuideSearchTimer timer;
    eRecMenuState action1;
    eRecMenuState action2;
    eRecMenuState action3;
    int iconActive;
    cPixmap *pixmapText;
    cPixmap *pixmapIcons;
    int DrawIcons(void);
public:
    cRecMenuItemSearchTimer(cTVGuideSearchTimer timer, 
                            eRecMenuState action1,
                            eRecMenuState action2,
                            eRecMenuState action3,
                            bool active);
    virtual ~cRecMenuItemSearchTimer(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
    cTVGuideSearchTimer GetTimer(void) { return timer; };
    eRecMenuState ProcessKey(eKeys Key);
};

// --- cRecMenuItemFavorite  -------------------------------------------------------
class cRecMenuItemFavorite : public cRecMenuItem {
private:
    cTVGuideSearchTimer favorite;
    eRecMenuState action1;
    cPixmap *pixmapText;
    cPixmap *pixmapIcons;
    int DrawIcons(void);
public:
    cRecMenuItemFavorite(cTVGuideSearchTimer favorite, 
                         eRecMenuState action1,
                         bool active);
    virtual ~cRecMenuItemFavorite(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
    cTVGuideSearchTimer GetFavorite(void) { return favorite; };
    eRecMenuState ProcessKey(eKeys Key);
};

// --- cRecMenuItemFavoriteStatic  -------------------------------------------------------
class cRecMenuItemFavoriteStatic : public cRecMenuItem {
private:
    std::string text;
    cPixmap *pixmapText;
    cPixmap *pixmapIcons;
    int DrawIcons(void);
public:
    cRecMenuItemFavoriteStatic(std::string text, eRecMenuState action, bool active);
    virtual ~cRecMenuItemFavoriteStatic(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
    eRecMenuState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_RECMENUITEM_H