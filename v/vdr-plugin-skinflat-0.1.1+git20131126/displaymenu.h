#pragma once

#include "baserender.h"

class cFlatDisplayMenu : public cFlatBaseRender,  public cSkinDisplayMenu {
    private:

        cPixmap *menuPixmap;
        int menuTop, menuWidth;

        eMenuCategory menuCategory;
        int VideoDiskUsageState;
    
        int contentTop;
    
        cPixmap *scrollbarPixmap;
        int scrollBarTop, scrollBarWidth, scrollBarHeight;

        int itemPadding;
        int itemHeight, itemChannelHeight;

        void DrawScrollbar(int Total, int Offset, int Shown, int Top, int Height, bool CanScrollUp, bool CanScrollDown);
        int ItemsHeight(void);
        bool CheckProgressBar(const char *text);
        void DrawProgressBarFromText(int Top, int Left, int Width, const char *bar, tColor ColorFg, tColor ColorBarFg, tColor ColorBg);
    public:
        cFlatDisplayMenu(void);
        virtual ~cFlatDisplayMenu();
        virtual void Scroll(bool Up, bool Page);
        virtual int MaxItems(void);
        virtual void Clear(void);
        
        virtual void SetMenuCategory(eMenuCategory MenuCategory);
        //virtual void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
    
        virtual void SetTitle(const char *Title);
        virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
        virtual void SetMessage(eMessageType Type, const char *Text);
        virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
        
        //virtual bool SetItemEvent(const cEvent *Event, int Index, bool Current, bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch);
        //virtual bool SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable);
        //virtual bool SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider);
        //virtual bool SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable, int Level, int Total, int New);

        virtual void SetScrollbar(int Total, int Offset);
        virtual void SetEvent(const cEvent *Event);
        virtual void SetRecording(const cRecording *Recording);
        virtual void SetText(const char *Text, bool FixedFont);
        virtual int GetTextAreaWidth(void) const;
        virtual const cFont *GetTextAreaFont(bool FixedFont) const;
        virtual void Flush(void);
};
