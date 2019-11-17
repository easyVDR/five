#ifndef __TVGUIDE_RECMENU_H
#define __TVGUIDE_RECMENU_H

#include "recmenuitem.h"

// --- cRecMenu  -------------------------------------------------------------

class cRecMenu : public cStyledPixmap {
private:
    std::list<cRecMenuItem*> menuItems;
    int scrollbarWidth;
    int numItems;
    int startIndex, stopIndex;
    void Activate(cRecMenuItem *itemOld, cRecMenuItem *item);
    bool ActivateNext(void);
    bool ActivatePrev(void);
    bool ScrollUp(void);
    bool ScrollDown(void);
    void PageUp(void);
    void PageDown(void);
    void JumpBegin(void);
    void JumpEnd(void);
protected:
    int x, y;
    int width, height;
    int headerHeight, footerHeight;
    int currentHeight;
    bool deleteMenuItems;
    cPixmap *pixmapScrollBar;
    cImage *imgScrollBar;
    int border;
    bool scrollable;
    cRecMenuItem *header;
    cRecMenuItem *footer;
    void SetWidthPercent(int percentOSDWidth);
    void SetWidthPixel(int pixel);
    int CalculateOptimalWidth(void);
    bool CalculateHeight(bool reDraw = false);
    void CreatePixmap(void);
    void SetHeader(cRecMenuItem *header);
    void SetFooter(cRecMenuItem *footer);
    void ClearMenuItems(bool destructor = false);
    void InitMenu(bool complete);
    bool AddMenuItemInitial(cRecMenuItem *item, bool inFront = false);
    void AddMenuItem(cRecMenuItem *item, bool inFront = false);
    void DrawScrollBar(void);
    cImage *createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend);
    void Arrange(bool scroll = false);
    virtual cRecMenuItem *GetMenuItem(int number) { return NULL; };
    cRecMenuItem *GetActiveMenuItem(void);
    cRecMenuItem *GetMenuItemAt(int num);
    int GetCurrentNumMenuItems(void) { return menuItems.size(); };
    virtual int GetTotalNumMenuItems(void) { return 0; };
    virtual void CreateMenuItems(void) {};
    int GetStartIndex(void) { return startIndex; };
    int GetActive(void);
public:
    cRecMenu(void);
    virtual ~cRecMenu(void);
    void Display(bool scroll = false);
    void Hide(void);
    void Show(void);
    void UpdateActiveMenuItem(void);
    virtual eRecMenuState ProcessKey(eKeys Key);
};
#endif //__TVGUIDE_RECMENU_H