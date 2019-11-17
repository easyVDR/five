#ifndef __DISPLAYMENU_H
#define __DISPLAYMENU_H

#include "libskindesignerapi/skindesignerapi.h"
#include <vdr/skins.h>
#include "coreengine/viewdisplaymenu.h"

class cSDDisplayMenu : public skindesignerapi::ISDDisplayMenu {
private:
    cViewMenu *view;
    bool ok;
    void SetCurrentRecording(void);
public:
    cSDDisplayMenu(cViewMenu *menuView);
    virtual ~cSDDisplayMenu();
    virtual void Scroll(bool Up, bool Page);
    virtual int MaxItems(void);
    virtual void Clear(void);
    virtual void SetMenuCategory(eMenuCategory MenuCat);
    virtual void SetMenuSortMode(eMenuSortMode MenuSortMode);
    virtual eMenuOrientation MenuOrientation(void);
    virtual void SetPluginMenu(int plugId, int menuId, int type, bool init);
    virtual void SetTitle(const char *Title);
    virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual bool SetItemEvent(const cEvent *Event, int Index, bool Current, bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch);
    virtual bool SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable);
    virtual bool SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider);
    virtual bool SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable, int Level, int Total, int New);
    virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
    virtual bool SetItemPlugin(skindesignerapi::cTokenContainer *tk, int Index, bool Current, bool Selectable);
    virtual void SetScrollbar(int Total, int Offset);
    virtual void SetEvent(const cEvent *Event);
    virtual void SetRecording(const cRecording *Recording);
    virtual void SetText(const char *Text, bool FixedFont);
    virtual bool SetPluginText(skindesignerapi::cTokenContainer *tk);
    virtual void Flush(void);
    virtual void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
    virtual int GetTextAreaWidth(void) const;
    virtual const cFont *GetTextAreaFont(bool FixedFont) const;
};

#endif //__DISPLAYMENU_H
