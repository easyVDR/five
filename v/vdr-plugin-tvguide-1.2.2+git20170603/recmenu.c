#include <list>
#include "recmenu.h"

// --- cRecMenu  -------------------------------------------------------------

cRecMenu::cRecMenu(void) {
    border = geoManager.borderRecMenus;
    height = 2*border;
    headerHeight = 0;
    footerHeight = 0;
    currentHeight = 0;
    deleteMenuItems = true;
    scrollable = false;
    scrollbarWidth = 3 * border;
    pixmapScrollBar = NULL;
    imgScrollBar = NULL;
    startIndex = 0;
    stopIndex = 0;
    numItems = 0;
    header = NULL;
    footer = NULL;
}

cRecMenu::~cRecMenu(void) {
    if (header)
        delete header;
    ClearMenuItems(true);
    if (footer)
        delete footer;
    if (pixmapScrollBar)
        osdManager.releasePixmap(pixmapScrollBar);
    if (imgScrollBar)
        delete imgScrollBar;
}

void cRecMenu::SetWidthPercent(int percentOSDWidth) {
    width = geoManager.osdWidth * percentOSDWidth / 100; 
    x = (geoManager.osdWidth - width) / 2;
}

void cRecMenu::SetWidthPixel(int pixel) {
    width = pixel;
    x = (geoManager.osdWidth - width) / 2;
}

int cRecMenu::CalculateOptimalWidth(void) {
    int optWidth = 0;
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        int itemWidth = (*item)->GetWidth();
        if (itemWidth > optWidth)
            optWidth = itemWidth;
    }
    return optWidth;
}

bool cRecMenu::CalculateHeight(bool reDraw) {
    int newHeight = 2*border;
    if (header)
        newHeight += headerHeight;
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        newHeight += (*item)->GetHeight(); 
    }
    if (footer)
        newHeight += footerHeight;

    y = (geoManager.osdHeight - newHeight) / 2;

    if (newHeight != height) {
        height = newHeight;
        if (scrollable && !reDraw) {
            width += scrollbarWidth + border;
        }
        return true;
    }
    return false;
}

void cRecMenu::CreatePixmap(void) {
    if (pixmap)
        osdManager.releasePixmap(pixmap);
    pixmap = osdManager.requestPixmap(3, cRect(x, y, width, height));
    if (scrollable) {
        int scrollBarX = x + width - scrollbarWidth - border;
        int scrollBarY = y + border + headerHeight;
        int scrollBarHeight = height - headerHeight - footerHeight - 2 * border;
        if (pixmapScrollBar)
            osdManager.releasePixmap(pixmapScrollBar);
        pixmapScrollBar = osdManager.requestPixmap(4, cRect(scrollBarX, scrollBarY, scrollbarWidth, scrollBarHeight));
    } else 
        pixmapScrollBar = NULL;
}

void cRecMenu::SetHeader(cRecMenuItem *header) { 
    this->header = header; 
    headerHeight = header->GetHeight();
    height += headerHeight;
}

void cRecMenu::SetFooter(cRecMenuItem *footer) {
    this->footer = footer;
    footerHeight = footer->GetHeight();
    height += footerHeight;
}

void cRecMenu::ClearMenuItems(bool destructor) { 
    for (std::list<cRecMenuItem*>::iterator it = menuItems.begin(); it != menuItems.end(); it++) {
        if (deleteMenuItems)
            delete *it;
        else if (!destructor)
            (*it)->Hide();
    }
    menuItems.clear();
};

void cRecMenu::InitMenu(bool complete) {
    currentHeight = 0;
    numItems = 0;
    if (scrollable) {
        width -= scrollbarWidth + border;
        osdManager.releasePixmap(pixmapScrollBar);
        pixmapScrollBar = NULL;
        delete imgScrollBar;
        imgScrollBar = NULL;
    }
    osdManager.releasePixmap(pixmap);
    pixmap = NULL;
    for (std::list<cRecMenuItem*>::iterator it = menuItems.begin(); it != menuItems.end(); it++) {
        if (deleteMenuItems)
            delete *it;
        else
            (*it)->Hide();
    }
    menuItems.clear();
    if (complete) {
        startIndex = 0;
        stopIndex = 0;
        scrollable = false;
    } else {
        stopIndex = startIndex;
    }

}


void cRecMenu::AddMenuItem(cRecMenuItem *item, bool inFront) {
    item->Show();
    if (!inFront)
        menuItems.push_back(item);
    else
        menuItems.push_front(item);
}

bool cRecMenu::AddMenuItemInitial(cRecMenuItem *item, bool inFront) {
    currentHeight += item->GetHeight();
    int totalHeight = headerHeight + footerHeight + currentHeight + 2*border;
    if (totalHeight >= geoManager.osdHeight - 10) {
        scrollable = true;
        currentHeight -= item->GetHeight();
        if (deleteMenuItems) {
            delete item;
        }
        return false;
    }
    numItems++;
    if (!inFront) {
        stopIndex++;
        menuItems.push_back(item);
    } else {
        startIndex--;
        menuItems.push_front(item);
    }
    return true;
}

void cRecMenu::Activate(cRecMenuItem *itemOld, cRecMenuItem *item) {
    itemOld->setInactive();
    itemOld->setBackground();
    itemOld->Draw();
    item->setActive();
    item->setBackground();
    item->Draw();
}

bool cRecMenu::ActivatePrev(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!scrollable && footer && footer->isActive()) {
        if (menuItems.size() > 0) {
            cRecMenuItem *itemLast = menuItems.back();
            Activate(footer, itemLast);
            return true;
        }
    } else if (activeItem) {
        cRecMenuItem *prev = NULL;
        bool foundActive = false;
        for (std::list<cRecMenuItem*>::iterator item = menuItems.end(); item != menuItems.begin(); ) {
            item--;
            if (*item == activeItem) {
                foundActive = true;
                continue;
            }
            if (!foundActive)
                continue;
            if ((*item)->isSelectable()) {
                prev = *item;
                break;
            }
        }    
        if (prev) {
            Activate(activeItem , prev);
            return true;
        }
    }
    return false;
}

bool cRecMenu::ScrollUp(void) {
    if (footer && footer->isActive()) {
        if (menuItems.size() > 0)
            Activate(footer, menuItems.back());
    } else {
        //get perv x items
        int numNewItems = numItems / 2;
        int numAdded = 0;
        cRecMenuItem *newItem = NULL;
        while (newItem = GetMenuItem(startIndex-1)) {
            AddMenuItem(newItem, true);
            cRecMenuItem *last = menuItems.back();
            if (deleteMenuItems) {
                delete last;
            } else {
                last->setInactive();
                last->Hide();
            }
            menuItems.pop_back();
            stopIndex--;
            startIndex--;
            numAdded++;
            if (numAdded >= numNewItems)
                break;
        }
        if (numAdded != 0) {
            scrollable = true;
            if (CalculateHeight(true))
                CreatePixmap();
            Arrange(deleteMenuItems);
            Display(deleteMenuItems);
            ActivatePrev();
        } else {
            return false;
        }
    }
    return true;
}

bool cRecMenu::ActivateNext(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (activeItem) {
        cRecMenuItem *next = NULL;
        bool foundActive = false;
        for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
            if (*item == activeItem) {
                foundActive = true;
                continue;
            }
            if (!foundActive)
                continue;
            if ((*item)->isSelectable()) {
                next = *item;
                break;
            }
        }    
        if (next) {
            Activate(activeItem , next);
            return true;
        } else if (!scrollable && footer && footer->isSelectable() && !footer->isActive()) {
            Activate(activeItem , footer);
            return true;
        }
    } 
    return false;
}

bool cRecMenu::ScrollDown(void) {
    //get next x items
    int numNewItems = numItems / 2;
    int numAdded = 0;
    cRecMenuItem *newItem = NULL;
    while (newItem = GetMenuItem(stopIndex)) {
        menuItems.push_back(newItem);
        cRecMenuItem *first = menuItems.front();
        if (deleteMenuItems) {
            delete first;
        } else {
            first->setInactive();
            first->Hide();
        }
        menuItems.pop_front();
        startIndex++;
        stopIndex++;
        numAdded++;
        if (numAdded >= numNewItems)
            break;
    }
    if (numAdded != 0) {
        scrollable = true;
        if (CalculateHeight(true))
            CreatePixmap();
        Arrange(deleteMenuItems);
        Display(deleteMenuItems);
        ActivateNext();
    } else {
        //last item reached, activate footer if not already active
        if ((footer) && !(footer->isActive())) {
            cRecMenuItem *activeItem = GetActiveMenuItem();
            Activate(activeItem , footer);
        } else {
            return false;
        }
    }
    return true;
}

void cRecMenu::PageUp(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!activeItem)
        return;
    if (!scrollable) {
        JumpBegin();
        return;
    }
    if (footer && activeItem == footer) {
        Activate(footer, menuItems.front());
        return;
    }
    int newActive = GetActive() - numItems;
    if (newActive < 0)
        newActive = 0;
    activeItem->setInactive();
    activeItem->setBackground();
    ClearMenuItems();
    currentHeight = 0;
    stopIndex = startIndex;
    numItems = 0;
    cRecMenuItem *newItem = NULL;
    bool spaceLeft = true;
    while (newItem = GetMenuItem(startIndex-1)) {
        if (startIndex-1 == newActive)
            newItem->setActive();
        spaceLeft = AddMenuItemInitial(newItem, true);
        if (!spaceLeft)
            break;
    }
    if (spaceLeft) {
        while (newItem = GetMenuItem(stopIndex)) {
            spaceLeft = AddMenuItemInitial(newItem);
            if (!spaceLeft)
                break;
        }
    }
    if (GetActive() == numItems)
            menuItems.front()->setActive();
    if (CalculateHeight(true))
        CreatePixmap();
    Arrange(deleteMenuItems);
    Display(deleteMenuItems);
}

void cRecMenu::PageDown(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!activeItem)
        return;
    if (footer && activeItem == footer) {
        return;
    }
    if (!scrollable) {
        JumpEnd();
        return;
    }
    int newActive = GetActive() + numItems;
    activeItem->setInactive();
    activeItem->setBackground();
    ClearMenuItems();
    currentHeight = 0;
    numItems = 0;
    startIndex = stopIndex;
    cRecMenuItem *newItem = NULL;
    bool spaceLeft = true;
    while (newItem = GetMenuItem(stopIndex)) {
        if (stopIndex == newActive)
            newItem->setActive();
        spaceLeft = AddMenuItemInitial(newItem);
        if (!spaceLeft)
            break;
    }
    if (spaceLeft) {
        while (newItem = GetMenuItem(startIndex-1)) {
            spaceLeft = AddMenuItemInitial(newItem, true);
            if (!spaceLeft)
                break;
        }
    }
    if (GetActive() == GetTotalNumMenuItems())
            menuItems.back()->setActive();
    if (CalculateHeight(true))
        CreatePixmap();
    Arrange(deleteMenuItems);
    Display(deleteMenuItems);
}

void cRecMenu::JumpBegin(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!scrollable) {
        cRecMenuItem *firstSelectable= NULL;
        for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
            if ((*item)->isSelectable()) {
                firstSelectable = *item;
                break;
            }
        }
        if (activeItem && firstSelectable) {
            Activate(activeItem , firstSelectable);
        }
    } else {
        activeItem->setInactive();
        activeItem->setBackground();
        if (footer)
            footer->Draw();
        ClearMenuItems();
        int currentItem = 0;
        cRecMenuItem *newItem = NULL;
        while (newItem = GetMenuItem(currentItem)) {
            AddMenuItem(newItem);
            currentItem++;
            if (currentItem >= numItems)
                break;
        }
        Arrange(true);
        startIndex = 0;
        stopIndex = numItems-1;
        cRecMenuItem *first = menuItems.front();
        first->setActive();
        first->setBackground();
        first->Draw();
        Display(true);    
    }
}

void cRecMenu::JumpEnd(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!activeItem)
        return;
    if (!scrollable) {
        cRecMenuItem *lastSelectable= NULL;
        if (footer && footer->isSelectable()) {
            lastSelectable = footer;
        } else {
            for (std::list<cRecMenuItem*>::iterator item = menuItems.end(); item != menuItems.begin(); ) {
                item--;
                if ((*item)->isSelectable()) {
                    lastSelectable = *item;
                    break;
                }
            }
        }
        if (lastSelectable) {
            Activate(activeItem , lastSelectable);
        }
    } else {
        activeItem->setInactive();
        activeItem->setBackground();
        ClearMenuItems();
        int totalNumItems = GetTotalNumMenuItems();
        int currentItem = totalNumItems-1;
        int itemsAdded = 0;
        cRecMenuItem *newItem = NULL;
        while (newItem = GetMenuItem(currentItem)) {
            AddMenuItem(newItem, true);
            currentItem--;
            itemsAdded++;
            if (itemsAdded >= numItems)
                break;
        }
        Arrange(true);
        stopIndex = totalNumItems;
        startIndex = stopIndex - numItems;
        if (footer) {
            footer->setActive();
            footer->setBackground();
            footer->Draw();
        } else {
            cRecMenuItem *last = menuItems.back();
            last->setActive();
            last->setBackground();
            last->Draw();
        }
        Display(true);
    }
}

void cRecMenu::Arrange(bool scroll) {
    int xElement = x + border;
    int yElement = y + border;
    int widthElement = width - 2 * border;
    if (scrollable)
        widthElement -= scrollbarWidth + border;

    if (header) {
        if (!scroll) {
            header->SetGeometry(xElement, yElement, widthElement);
            header->SetPixmaps();
            header->setBackground();
        }
        yElement += header->GetHeight();
    }
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        (*item)->SetGeometry(xElement, yElement, widthElement);
        (*item)->SetPixmaps();
        yElement += (*item)->GetHeight();
    }
    if (footer && !scroll) {
        footer->SetGeometry(xElement, yElement, widthElement);
        footer->SetPixmaps();
        footer->setBackground();
    }
}

void cRecMenu::Display(bool scroll) {
    if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgRecMenuBack);
    } else {
        pixmap->Fill(theme.Color(clrBackground));
        drawBorder();
    }
    if (header && !scroll) {
        header->setBackground();
        header->Draw();
    }
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        (*item)->setBackground();
        (*item)->Show();
        (*item)->Draw();
    }
    if (footer && !scroll) {
        footer->setBackground();
        footer->Draw();
    }
    if (scrollable)
        DrawScrollBar();
}

void cRecMenu::Hide(void) {
    pixmap->SetLayer(-1);
    if (pixmapScrollBar)
        pixmapScrollBar->SetLayer(-1);
    if (header)
        header->Hide();
    if (footer)
        footer->Hide();
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        (*item)->Hide();
    }
}

void cRecMenu::Show(void) {
    pixmap->SetLayer(3);
    if (pixmapScrollBar)
        pixmapScrollBar->SetLayer(3);
    if (header)
        header->Show();
    if (footer)
        footer->Show();
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        (*item)->Show();
    }
}

void cRecMenu::UpdateActiveMenuItem(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (activeItem)
        activeItem->Draw();   
}


void cRecMenu::DrawScrollBar(void) {
    pixmapScrollBar->Fill(theme.Color(clrBorder));
    pixmapScrollBar->DrawRectangle(cRect(2,2,pixmapScrollBar->ViewPort().Width()-4, pixmapScrollBar->ViewPort().Height() - 4), theme.Color(clrBackground));
    
    int totalNumItems = GetTotalNumMenuItems();
    if (imgScrollBar == NULL) {
        int scrollBarImgHeight = (pixmapScrollBar->ViewPort().Height() - 8) * numItems / totalNumItems;
        imgScrollBar = createScrollbar(pixmapScrollBar->ViewPort().Width()-8, scrollBarImgHeight, theme.Color(clrHighlight), theme.Color(clrHighlightBlending));
    }
    int offset = (pixmapScrollBar->ViewPort().Height() - 8) * startIndex / totalNumItems;
    pixmapScrollBar->DrawImage(cPoint(4, 2 + offset), *imgScrollBar);
}

cRecMenuItem *cRecMenu::GetActiveMenuItem(void) {
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        if ((*item)->isActive())
            return *item;
    }
    if (footer && footer->isActive())
        return footer;
    return NULL;
}

cRecMenuItem *cRecMenu::GetMenuItemAt(int num) {
    int current = 0;
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        if (current == num)
            return *item;
        current++;
    }
    return NULL;
}

int cRecMenu::GetActive(void) {
    int numActive = startIndex;
    for (std::list<cRecMenuItem*>::iterator item = menuItems.begin(); item != menuItems.end(); item++) {
        if ((*item)->isActive()) {
            break;
        }
        numActive++;
    }
    return numActive;
}

eRecMenuState cRecMenu::ProcessKey(eKeys Key) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    eRecMenuState state = rmsContinue;
    if (!activeItem)
        return state;

    state = activeItem->ProcessKey(Key);
    if (state == rmsRefresh) {
        CreateMenuItems();
        Display();
    } else if (state == rmsNotConsumed) {
        switch (Key & ~k_Repeat) {
            case kUp:       
                if (!ActivatePrev())
                    if (!ScrollUp())
                        JumpEnd();
                state = rmsConsumed;
                break;
            case kDown:
                if (!ActivateNext())
                    if (!ScrollDown())
                        JumpBegin();
                state = rmsConsumed;
                break;
            case kLeft:
                PageUp();
                state = rmsConsumed;
                break;
            case kRight:
                PageDown();
                state = rmsConsumed;
                break;
            default:
                break;
        }
    }
    return state;
}

cImage *cRecMenu::createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend) {
    cImage *image = new cImage(cSize(width, height));
    image->Fill(clrBgr);
    if (tvguideConfig.style != eStyleFlat) {
        int numSteps = 64;
        int alphaStep = 0x03;
        if (height < 30)
            return image;
        else if (height < 100) {
            numSteps = 32;
            alphaStep = 0x06;
        }
        int stepY = 0.5*height / numSteps;
        if (stepY == 0)
            stepY = 1;
        int alpha = 0x40;
        tColor clr;
        for (int i = 0; i<numSteps; i++) {
            clr = AlphaBlend(clrBgr, clrBlend, alpha);
            for (int y = i*stepY; y < (i+1)*stepY; y++) {
                for (int x=0; x<width; x++) {
                    image->SetPixel(cPoint(x,y), clr);
                }
            }
            alpha += alphaStep;
        }
    }
    return image;
}