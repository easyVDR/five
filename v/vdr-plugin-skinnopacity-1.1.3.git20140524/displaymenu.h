#ifndef __NOPACITY_DISPLAYMENU_H
#define __NOPACITY_DISPLAYMENU_H

#include "nopacity.h"
#include "menuitem.h"
#include <vdr/thread.h>

class cNopacityDisplayMenu : public cSkinDisplayMenu , cThread {
private:
    cImageCache *imgCache;
    cNopacityDisplayMenuView *menuView;
    cNopacityDetailView *detailView;
    cOsd *osd;
    eMenuCategory menuCategoryLast;
    int FrameTime;
    int FadeTime;
    bool initial;
    bool initMenu;
    bool diskUsageDrawn;
    int lastDiskUsageState;
    int lastTimersState;
    bool timersDrawn;
    int menuItemIndexLast;
    int currentNumItems;
    cList<cNopacityTimer> timers;
    cList<cNopacityMenuItem> menuItems;
    int positionButtons[4];
    cRect videoWindowRect;
    int currentFeed;
    void SetNextFeed(void);
    void DrawDisk(void);
    int CheckTimerConflict(bool timersChanged);
    void DrawTimers(bool timersChanged, int numConflicts);
    void SplitItem(const char *Text, cString *strItems, int *tabItems);
    void SetButtonPositions(void);
    virtual void Action(void);
protected:
    int Tab(int n);
public:
    cNopacityDisplayMenu(cImageCache *imgCache);
    virtual ~cNopacityDisplayMenu();
    virtual void Scroll(bool Up, bool Page);
    virtual int MaxItems(void);
    virtual void Clear(void);
    virtual void SetMenuCategory(eMenuCategory MenuCategory);
    virtual void SetTitle(const char *Title);
    virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
    virtual bool SetItemEvent(const cEvent *Event, int Index, bool Current, bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch);
    virtual bool SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable);
    virtual bool SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider);
    virtual bool SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable, int Level, int Total, int New);
    virtual void SetScrollbar(int Total, int Offset);
    virtual void SetEvent(const cEvent *Event);
    virtual void SetRecording(const cRecording *Recording);
    virtual void SetText(const char *Text, bool FixedFont);
    virtual void Flush(void);
    virtual void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
    virtual int GetTextAreaWidth(void) const;
    virtual const cFont *GetTextAreaFont(bool FixedFont) const;
};

#endif //__NOPACITY_DISPLAYMENU_H