#include "services/remotetimers.h"
namespace PluginRemoteTimers {
    static int CompareTimers(const void *a, const void *b) {
        return (*(const cTimer **)a)->Compare(**(const cTimer **)b);
    }
}

#include "displaymenuview.h"
#include "displaymenu.h"
#include "config.h"
#include <string>
#include "services/epgsearch.h"

cNopacityDisplayMenu::cNopacityDisplayMenu(cImageCache *imgCache) {
    this->imgCache = imgCache;
    menuCategoryLast = mcUndefined;
    FadeTime = config.GetValue("menuFadeTime");
    FrameTime = FadeTime / 10;
    initial = true;
    initMenu = true;
    diskUsageDrawn = false;
    timersDrawn = false;
    lastDiskUsageState = -1;
    lastTimersState = -1;
    menuItemIndexLast = -1;
    currentNumItems = 0;
    detailView = NULL;
    SetButtonPositions();
    menuView = new cNopacityDisplayMenuView(imgCache);
    osd = menuView->createOsd();
    menuView->SetDescriptionTextWindowSize();
    menuView->CreatePixmaps();
    menuView->SetAvrgFontWidth();
    menuView->DrawHeaderLogo();
    menuView->DrawBorderDecoration();
    currentFeed = 0;
}

cNopacityDisplayMenu::~cNopacityDisplayMenu() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    delete menuView;
    menuItems.Clear();
    if (detailView) {
        delete detailView;
    }
    timers.Clear();
    delete osd;
    cDevice::PrimaryDevice()->ScaleVideo(cRect::Null);
}

void cNopacityDisplayMenu::DrawDisk(void) {
    if (!config.GetValue("narrowMainMenu"))
        return;
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain)&&!diskUsageDrawn)) {
        if (cVideoDiskUsage::HasChanged(lastDiskUsageState)) {
            menuView->DrawDiskUsage();
        } else {
            menuView->ShowDiskUsage(true);
        }
        diskUsageDrawn = true;
    }
}

int cNopacityDisplayMenu::CheckTimerConflict(bool timersChanged) {
    int numConflicts = 0;
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain))) {
        if (timersChanged) {
            cPlugin *p = cPluginManager::GetPlugin("epgsearch");
            if (p) {
                Epgsearch_lastconflictinfo_v1_0 *serviceData = new Epgsearch_lastconflictinfo_v1_0;
                if (serviceData) {
                    serviceData->nextConflict = 0;
                    serviceData->relevantConflicts = 0;
                    serviceData->totalConflicts = 0;
                    p->Service("Epgsearch-lastconflictinfo-v1.0", serviceData);
                    if (serviceData->relevantConflicts > 0) {
                        numConflicts = serviceData->relevantConflicts;
                    }
                    delete serviceData;
                }
            }
        }
    }
    return numConflicts;
}

void cNopacityDisplayMenu::DrawTimers(bool timersChanged, int numConflicts) {
    if (!config.GetValue("narrowMainMenu"))
        return;
    int maxTimersHeight = menuView->GetTimersMaxHeight();
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain)&&!timersDrawn)) {
        if (timersChanged) {
            //check if remotetimers plugin is available
            static cPlugin* pRemoteTimers = cPluginManager::GetPlugin("remotetimers");
            bool drawRemoteTimers = false;
            cString errorMsg;
            if (pRemoteTimers) {
                drawRemoteTimers = pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg);
            }
            timers.Clear();
            cSortedTimers SortedTimers;
            //if remotetimers plugin is available, take timers also from him
            if (drawRemoteTimers) {
                cTimer* remoteTimer = NULL;
                int numRemoteTimers = 0;
                while (pRemoteTimers->Service("RemoteTimers::ForEach-v1.0", &remoteTimer) && remoteTimer != NULL) {
                    SortedTimers.Append(remoteTimer);
                    numRemoteTimers++;
                }
                SortedTimers.Sort(PluginRemoteTimers::CompareTimers);
            }
            int numTimers = SortedTimers.Size();
            int currentHeight = menuView->GetTimersInitHeight();
            if (numConflicts > 0) {
                cNopacityTimer *t = menuView->DrawTimerConflict(numConflicts, currentHeight);
                if (initial)
                    if (FadeTime)
                        t->SetAlpha(0);
                currentHeight += t->GetHeight() + geoManager->menuSpace;
                timers.Add(t);
            }
            int numTimersDisplayed = 0;
            for (int i = 0; i < numTimers; i++) {
                if (const cTimer *Timer = SortedTimers[i]) {
                    if (Timer->HasFlags(tfActive)) {
                        cNopacityTimer *t = menuView->DrawTimer(Timer, currentHeight);
                        if (initial)
                            if (FadeTime)
                                t->SetAlpha(0);
                        currentHeight += t->GetHeight() + geoManager->menuSpace;
                        if (currentHeight < maxTimersHeight) {
                            timers.Add(t);
                        numTimersDisplayed++;
                        if (numTimersDisplayed == config.GetValue("numberTimers"))
                            break;
                        } else {
                            delete t;
                            break;
                        }
                    }
                }
            }
        } else {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->Show();
            }
        }
        timersDrawn = true;
    }
}

void cNopacityDisplayMenu::Scroll(bool Up, bool Page) {
    if (!detailView)
        return;
    detailView->KeyInput(Up, Page);
}

int cNopacityDisplayMenu::MaxItems(void) {
    int maxItems = 0;
    switch (MenuCategory()) {
        case mcMain:
            if (config.GetValue("narrowMainMenu"))
                maxItems = config.GetValue("numMainMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcSetup:
            if (config.GetValue("narrowSetupMenu"))
                maxItems = config.GetValue("numMainMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            if (config.GetValue("narrowScheduleMenu"))
                maxItems = config.GetValue("numSchedulesMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcChannel:
            if (config.GetValue("narrowChannelMenu"))
                maxItems = config.GetValue("numSchedulesMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcTimer:
            if (config.GetValue("narrowTimerMenu"))
                maxItems = config.GetValue("numSchedulesMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcRecording:
            if (config.GetValue("narrowRecordingMenu"))
                maxItems = config.GetValue("numRecordingsMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        default:
            maxItems = config.GetValue("numDefaultMenuItems");
    }
    currentNumItems = maxItems;
    return maxItems;
}

void cNopacityDisplayMenu::Clear(void) {
    if (detailView) {
        delete detailView;
        detailView = NULL;
    }
    menuItemIndexLast = -1;
    initMenu = true;
    menuItems.Clear();
}

void cNopacityDisplayMenu::SetMenuCategory(eMenuCategory MenuCategory) {
      /* Categories:
      mcUndefined = -1,
      mcUnknown = 0,
      1  mcMain,
      2  mcSchedule,
      3  mcScheduleNow,
      4  mcScheduleNext,
      5  mcChannel,
      6  mcChannelEdit,
      7  mcTimer,
      8  mcTimerEdit,
      9  mcRecording,
      10 mcRecordingInfo,
      11 mcPlugin,
      12 mcPluginSetup,
      13 mcSetup,
      14 mcSetupOsd,
      15 mcSetupEpg,
      16 mcSetupDvb,
      17 mcSetupLnb,
      18 mcSetupCam,
      19 mcSetupRecord,
      20 mcSetupReplay,
      21 mcSetupMisc,
      22 mcSetupPlugins,
      23 mcCommand,
      24 mcEvent,
      25 mcText,
      26 mcFolder,
      27 mcCam
      */
    menuCategoryLast = this->MenuCategory();
    cSkinDisplayMenu::SetMenuCategory(MenuCategory);
    if ((menuCategoryLast == mcMain) && (MenuCategory != mcMain)) {
        if (config.GetValue("showDiscUsage")) {
            menuView->ShowDiskUsage(false);
            diskUsageDrawn = false;
        }
        if (config.GetValue("showTimers")) {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->Hide();
            }
            timersDrawn = false;
        }
    }
}

void cNopacityDisplayMenu::SetTitle(const char *Title) {
    //resetting menuitems if no call to clear
    if (!initMenu) {
        initMenu = true;
        menuItemIndexLast = -1;
        menuItems.Clear();
    }
    int left = 5;
    menuView->DestroyHeaderIcon();
    if (Title) {
        cString title = Title;
        switch (MenuCategory()) {
            case mcMain:
                switch (config.GetValue("mainMenuTitleStyle")) {
                    case 0:
                        title = cString::sprintf("%s %s", Title, VDRVERSION);
                        break;
                    case 1:
                        title = cString::sprintf("%s", VDRVERSION);
                        break;
                    case 2:
                        title = " ";
                        break;
                }
                left += menuView->ShowHeaderLogo(true);
                break;
            case mcSchedule:
                menuView->ShowHeaderLogo(false);
                left += menuView->ShowHeaderIconChannelLogo(Title);
                break;
            case mcRecording:
                menuView->ShowHeaderLogo(false);
                title = cString::sprintf("%s (%s)", Title, *cVideoDiskUsage::String());
                left += menuView->DrawHeaderIcon(MenuCategory());
                break;
            default:
                menuView->ShowHeaderLogo(false);
                left += menuView->DrawHeaderIcon(MenuCategory());
        }
        menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
        menuView->DrawHeaderLabel(left, title);
    }
}

void cNopacityDisplayMenu::SetButtonPositions(void) {
    for (int i=0; i < 4; i++) {
        positionButtons[i] = -1;
    }
    /*
    red button = 0
    green button = 1
    yellow button = 2
    blue button = 3
    */
    for (int button=0; button<4; button++) {
        if (Setup.ColorKey0 == button) {
            positionButtons[button] = 0;
            continue;
        }
        if (Setup.ColorKey1 == button) {
            positionButtons[button] = 1;
            continue;
        }
        if (Setup.ColorKey2 == button) {
            positionButtons[button] = 2;
            continue;
        }
        if (Setup.ColorKey3 == button) {
            positionButtons[button] = 3;
            continue;
        }
    }
}

void cNopacityDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    if (Red) {
        menuView->DrawButton(Red, seButtonRed, Theme.Color(clrButtonRed), Theme.Color(clrButtonRedBorder), Theme.Color(clrButtonRedFont), positionButtons[0]);
    } else
        menuView->ClearButton(positionButtons[0]);

    if (Green) {
        menuView->DrawButton(Green, seButtonGreen,Theme.Color(clrButtonGreen),  Theme.Color(clrButtonGreenBorder), Theme.Color(clrButtonGreenFont), positionButtons[1]);
    } else
        menuView->ClearButton(positionButtons[1]);

    if (Yellow) {
        menuView->DrawButton(Yellow, seButtonYellow, Theme.Color(clrButtonYellow), Theme.Color(clrButtonYellowBorder), Theme.Color(clrButtonYellowFont), positionButtons[2]);
    } else
        menuView->ClearButton(positionButtons[2]);

    if (Blue) {
        menuView->DrawButton(Blue, seButtonBlue, Theme.Color(clrButtonBlue), Theme.Color(clrButtonBlueBorder), Theme.Color(clrButtonBlueFont), positionButtons[3]);
    } else
        menuView->ClearButton(positionButtons[3]);
}

void cNopacityDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
    if (Text) {
        menuView->DrawMessage(Type, Text);
    } else {
        menuView->ClearMessage();
    }
}

bool cNopacityDisplayMenu::SetItemEvent(const cEvent *Event, int Index, bool Current,
                                        bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch) {

    if (!config.GetValue("narrowScheduleMenu"))
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityScheduleMenuItem(osd, imgCache, Event, Channel, TimerMatch, Selectable, MenuCategory(), &videoWindowRect);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(fontManager->menuItemSchedule);
        item->SetFontSmall(fontManager->menuItemScheduleSmall);
        item->SetFontEPGWindow(fontManager->menuEPGInfoWindow);
        item->SetFontEPGWindowLarge(fontManager->menuEPGInfoWindowLarge);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcSchedule));
        item->SetCurrent(Current);
        item->CreateText();
        int textWidth = item->CheckScrollable((Channel)?true:false);
        item->CreatePixmapBackground();
        if (config.GetValue("displayType") == dtGraphical) {
            item->CreatePixmapForeground();
        }
        item->CreatePixmapStatic();
        item->CreatePixmapTextScroller(textWidth);
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable) {
    if (!config.GetValue("narrowTimerMenu"))
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityTimerMenuItem(osd, imgCache, Timer, Selectable, &videoWindowRect);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(fontManager->menuItemTimers);
        item->SetFontSmall(fontManager->menuItemTimersSmall);
        item->SetFontEPGWindow(fontManager->menuEPGInfoWindow);
        item->SetFontEPGWindowLarge(fontManager->menuEPGInfoWindowLarge);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcTimer));
        item->SetCurrent(Current);
        item->CreateText();
        int textWidth = item->CheckScrollable(true);
        item->CreatePixmapBackground();
        if (config.GetValue("displayType") == dtGraphical) {
            item->CreatePixmapForeground();
        }
        item->CreatePixmapStatic();
        item->CreatePixmapTextScroller(textWidth);
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider) {
    if (!config.GetValue("narrowChannelMenu"))
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityChannelMenuItem(osd, imgCache, Channel, Selectable, &videoWindowRect);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(fontManager->menuItemChannel);
        item->SetFontSmall(fontManager->menuItemChannelSmall);
        item->SetFontEPGWindow(fontManager->menuEPGInfoWindow);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcChannel));
        item->SetCurrent(Current);
        item->CreateText();
        int textWidth = item->CheckScrollable(true);
        item->CreatePixmapBackground();
        item->CreatePixmapStatic();
        item->CreatePixmapTextScroller(textWidth);
        if (config.GetValue("displayType") == dtGraphical) {
            item->CreatePixmapForeground();
        }
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable,
                                            int Level, int Total, int New) {
    if (!config.GetValue("narrowRecordingMenu"))
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        bool isFolder = false;
        if (Total > 0)
            isFolder = true;
        cNopacityMenuItem *item = new cNopacityRecordingMenuItem(osd, imgCache, Recording, Selectable, isFolder, Level, Total, New, &videoWindowRect);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(fontManager->menuItemRecordings);
        item->SetFontSmall(fontManager->menuItemRecordingsSmall);
        item->SetFontEPGWindow(fontManager->menuEPGInfoWindow);
        item->SetFontEPGWindowLarge(fontManager->menuEPGInfoWindowLarge);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcRecording));
        item->SetCurrent(Current);
        item->CreateText();
        item->SetPoster();
        int textWidth = item->CheckScrollable(false);
        item->CreatePixmapBackground();
        item->CreatePixmapStatic();
        item->CreatePixmapTextScroller(textWidth);
        if (config.GetValue("displayType") == dtGraphical) {
            item->CreatePixmapForeground();
        }
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}


void cNopacityDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
    bool hasIcons = false;
    bool MainOrSetup = false;
    cString *strItems = new cString[MaxTabs];
    int *tabItems = new int[2*MaxTabs];
    for (int i=0; i<MaxTabs; i++) {
        strItems[i] = "";
        tabItems[i] = 0;
        tabItems[i+MaxTabs] = 0;
    }
    SplitItem(Text, strItems, tabItems);
    if (initMenu) {
        if (Index > menuItemIndexLast) {
            cNopacityMenuItem *item;
            cPoint itemSize;
            if (((MenuCategory() == mcMain)&&(config.GetValue("narrowMainMenu"))) || ((MenuCategory() == mcSetup)&&(config.GetValue("narrowSetupMenu")))){
                MainOrSetup = true;
                bool isSetup = (MenuCategory() == mcSetup)?true:false;
                item = new cNopacityMainMenuItem(osd, imgCache, Text, Selectable, isSetup);
                menuView->GetMenuItemSize(MenuCategory(), &itemSize);
                item->SetFont(fontManager->menuItemLarge);
                if (config.GetValue("useMenuIcons"))
                    hasIcons = true;
            } else {
                item = new cNopacityDefaultMenuItem(osd, imgCache, Text, Selectable);
                menuView->GetMenuItemSize(mcUnknown, &itemSize);
                item->SetFont(fontManager->menuItemDefault);
            }
            int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
            item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
            item->SetCurrent(Current);
            item->SetTabs(strItems, tabItems, MaxTabs);
            item->CreateText();
            int textWidth = item->CheckScrollable(hasIcons);
            item->CreatePixmapBackground();
            if (config.GetValue("displayType") == dtGraphical && MainOrSetup) {
                item->CreatePixmapForeground();
            }
            item->CreatePixmapStatic();
            if (textWidth > 0)
                item->CreatePixmapTextScroller(textWidth);
            menuItems.Add(item);
            item->Render();
            menuItemIndexLast = Index;
            if (initial) {
                if (FadeTime) {
                    item->SetAlpha(0);
                    item->SetAlphaIcon(0);
                    item->SetAlphaText(0);
                }
            }
        } else {
            //adjust Current if item was added twice
            cNopacityMenuItem *item = menuItems.Get(Index);
            item->SetCurrent(Current);
            item->Render();
        }
    } else {
        //redraw item when switching through menu
        cNopacityMenuItem *item = menuItems.Get(Index);
        if (item) {
            item->SetTabs(strItems, tabItems, MaxTabs);
            item->SetCurrent(Current);
            if ((MenuCategory() != mcMain) && (MenuCategory() != mcSetup))
                item->CheckScrollable(false);
            item->Render();
        }
    }
    SetEditableWidth(menuView->GetEditableWidth());
}

void cNopacityDisplayMenu::SplitItem(const char *Text, cString *strItems, int *tabItems) {
    int x = 0;
    for (int i = 0; i < MaxTabs; i++) {
        const char *s = GetTabbedText(Text, i);
        if (s) {
            strItems[i] = s;
            tabItems[i] = Tab(i);
        } else {
            if (i>0)
                tabItems[i - 1 + MaxTabs] = menuView->GetWidthDefaultMenu() - x;
            break;
        }
        if (i>0) {
            tabItems[(i-1) + MaxTabs] = Tab(i) - x;
            x += Tab(i) - x;
        }
        if (!Tab(i + 1)) {
            if (s)
                tabItems[i + MaxTabs] = menuView->GetWidthDefaultMenu() - x;
            else if (i==1) {
                tabItems[MaxTabs] = menuView->GetWidthDefaultMenu() - 1;
                tabItems[1] = 0;
                tabItems[MaxTabs+1] = 0;
            }
            break;
        }
    }
}

int cNopacityDisplayMenu::Tab(int n) {
    return (n >= 0 && n < MaxTabs) ? menuView->mytabs[n] : 0;
}

void cNopacityDisplayMenu::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5) {
    menuView->SetTabs(Tab1, Tab2, Tab3, Tab4, Tab5);
}

int cNopacityDisplayMenu::GetTextAreaWidth(void) const {
    return menuView->GetTextAreaWidth();
}

const cFont *cNopacityDisplayMenu::GetTextAreaFont(bool FixedFont) const {
    return menuView->GetTextAreaFont(FixedFont);
}

void cNopacityDisplayMenu::SetScrollbar(int Total, int Offset) {
    if (MaxItems() >= Total) {
        menuView->ClearScrollbar();
        return;
    }
    double height = (double)MaxItems()/(double)Total;
    double offset = (double)Offset/(double)Total;

    menuView->DrawScrollbar(height, offset);
}

void cNopacityDisplayMenu::SetEvent(const cEvent *Event) {
    if (!Event)
        return;
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityDetailView(dvEvent, osd, imgCache);
    menuView->SetDetailViewSize(dvEvent, detailView);
    detailView->SetEvent(Event);
    detailView->Start();
}

void cNopacityDisplayMenu::SetRecording(const cRecording *Recording) {
    if (!Recording)
        return;
    const cRecordingInfo *Info = Recording->Info();
    if (!Info) {
        return;
    }
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityDetailView(dvRecording, osd, imgCache);
    menuView->SetDetailViewSize(dvRecording, detailView);
    detailView->SetRecording(Recording);
    detailView->Start();
}

void cNopacityDisplayMenu::SetText(const char *Text, bool FixedFont) {
    if (!Text)
        return;
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityDetailView(dvText, osd, imgCache);
    menuView->SetDetailViewSize(dvText, detailView);
    detailView->SetText(Text);
    detailView->Start();
}

void cNopacityDisplayMenu::Flush(void) {
    //int start = cTimeMs::Now();
    menuView->DrawDate(initial);
    if (MenuCategory() == mcMain) {
        if (config.GetValue("showDiscUsage"))
            DrawDisk();
        bool timersChanged = Timers.Modified(lastTimersState);
        int numConflicts = 0;
        if (config.GetValue("checkTimerConflict"))
            numConflicts = CheckTimerConflict(timersChanged);
        if (config.GetValue("showTimers"))
            DrawTimers(timersChanged, numConflicts);
    }
    if (initial) {
        if (FadeTime)
            Start();
    }
    initMenu = false;
    initial = false;
    osd->Flush();
    cDevice::PrimaryDevice()->ScaleVideo(videoWindowRect);
}

void cNopacityDisplayMenu::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        menuView->SetPixmapAlpha(Alpha);
        for (cNopacityMenuItem *item = menuItems.First(); Running() && item; item = menuItems.Next(item)) {
            item->SetAlpha(Alpha);
            item->SetAlphaIcon(Alpha);
            item->SetAlphaText(Alpha);
        }
        for (cNopacityTimer *t = timers.First(); Running() && t; t = timers.Next(t))
            t->SetAlpha(Alpha);
        cPixmap::Unlock();
        if (Running())
            osd->Flush();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
