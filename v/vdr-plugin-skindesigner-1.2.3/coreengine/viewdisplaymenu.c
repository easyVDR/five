#include "viewdisplaymenu.h"
#include "../config.h"

/************************************************************************************
* cViewMenu
************************************************************************************/
cViewMenu::cViewMenu(void) {
    menuDefault = NULL;
    menuMain = NULL;
    menuSetup = NULL;
    menuSchedules = NULL;
    menuChannels = NULL;
    menuTimers = NULL;
    menuRecordings = NULL;
    menuDetailedEpg = NULL;
    menuDetailedRec = NULL;
    menuDetailedText = NULL;
    ClearVariables();
    viewId = eViewType::DisplayMenu;
    viewName = strdup("displaymenu");
    numViewElements = (int)eVeDisplayMenu::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    viewElementsHorizontal = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElementsHorizontal[i] = NULL;
    }
    SetViewElements();
    int numPluginMenus = plgManager->GetNumPluginMenus();
    numSubviews = (int)eSvDisplayMenu::count + numPluginMenus;
    subViews = new cSubView*[numSubviews];
    for (int i=0; i < numSubviews; i++) {
        subViews[i] = NULL;
    }
    SetSubViews();
    menuCat = mcUnknown;
    plugName = NULL;
}

cViewMenu::~cViewMenu() {
    for (int i=0; i< numSubviews; i++)
        delete subViews[i];
    delete[] subViews;
}

void cViewMenu::SetGlobals(cGlobals *globals) {
    cView::SetGlobals(globals);
    for (int i=0; i < numSubviews; ++i) {
        if (subViews[i]) {
            subViews[i]->SetGlobals(globals);
        }
    }
}

void cViewMenu::PreCache(void) {
    cView::PreCache();
    for (int i=0; i < numSubviews; i++) {
        if (subViews[i]) {
            subViews[i]->SetContainer(0, 0, attribs->Width(), attribs->Height());
            subViews[i]->PreCache();
            //setting default viewelements for subviews
            for (int ve = (int)eVeDisplayMenu::background; ve < (int)eVeDisplayMenu::count; ve++) {
                if (viewElements[ve] && !subViews[i]->ViewElementSet(ve)) {
                    subViews[i]->SetViewElement((eVeDisplayMenu)ve, viewElements[ve]);
                }
            }
            for (int ve = (int)eVeDisplayMenu::background; ve < (int)eVeDisplayMenu::count; ve++) {
                if (viewElementsHorizontal[ve] && !subViews[i]->ViewElementHorizontalSet(ve)) {
                    subViews[i]->SetViewElementHorizontal((eVeDisplayMenu)ve, viewElementsHorizontal[ve]);
                }
            }
        }
    }
}

void cViewMenu::SetViewElementObjects(void) {
    //setting subviews
    for (int i=0; i < numSubviews; i++) {
        if (!subViews[i])
            continue;
        if (i == (int)eSvDisplayMenu::menudefault)
            menuDefault = dynamic_cast<cViewMenuDefault*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::main)
            menuMain = dynamic_cast<cViewMenuMain*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::setup)
            menuSetup = dynamic_cast<cViewMenuSetup*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::schedules)
            menuSchedules = dynamic_cast<cViewMenuSchedules*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::channels)
            menuChannels = dynamic_cast<cViewMenuChannels*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::timers)
            menuTimers = dynamic_cast<cViewMenuTimers*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::recordings)
            menuRecordings = dynamic_cast<cViewMenuRecordings*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::detailepg)
            menuDetailedEpg = dynamic_cast<cViewMenuDetail*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::detailrec)
            menuDetailedRec = dynamic_cast<cViewMenuDetail*>(subViews[i]);
        else if (i == (int)eSvDisplayMenu::detailtext)
            menuDetailedText = dynamic_cast<cViewMenuDetail*>(subViews[i]);
    }
}

void cViewMenu::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayMenu::background));
    viewElementNames.insert(pair<string, int>("header", (int)eVeDisplayMenu::header));
    viewElementNames.insert(pair<string, int>("datetime", (int)eVeDisplayMenu::datetime));
    viewElementNames.insert(pair<string, int>("time", (int)eVeDisplayMenu::time));
    viewElementNames.insert(pair<string, int>("colorbuttons", (int)eVeDisplayMenu::colorbuttons));
    viewElementNames.insert(pair<string, int>("message", (int)eVeDisplayMenu::message));
    viewElementNames.insert(pair<string, int>("scrollbar", (int)eVeDisplayMenu::scrollbar));
    viewElementNames.insert(pair<string, int>("sortmode", (int)eVeDisplayMenu::sortmode));
}

void cViewMenu::SetSubViews(void) {
    //adding VDR submenus
    subviewNames.insert(pair<string, int>("menudefault", (int)eSvDisplayMenu::menudefault));
    subviewNames.insert(pair<string, int>("menumain", (int)eSvDisplayMenu::main));
    subviewNames.insert(pair<string, int>("menusetup", (int)eSvDisplayMenu::setup));
    subviewNames.insert(pair<string, int>("menuschedules", (int)eSvDisplayMenu::schedules));
    subviewNames.insert(pair<string, int>("menuchannels", (int)eSvDisplayMenu::channels));
    subviewNames.insert(pair<string, int>("menutimers", (int)eSvDisplayMenu::timers));
    subviewNames.insert(pair<string, int>("menurecordings", (int)eSvDisplayMenu::recordings));
    subviewNames.insert(pair<string, int>("menudetailedepg", (int)eSvDisplayMenu::detailepg));
    subviewNames.insert(pair<string, int>("menudetailedrecording", (int)eSvDisplayMenu::detailrec));
    subviewNames.insert(pair<string, int>("menudetailedtext", (int)eSvDisplayMenu::detailtext));
    //adding additional plugin subviews
    int subViewNumber = (int)eSvDisplayMenu::count;
    map <int,skindesignerapi::sPlugMenu> *plugMenus = NULL;
    string plugName = "";
    int plugId = -1;
    plgManager->InitPluginMenuIterator();
    while ( plugMenus = plgManager->GetPluginMenus(plugName, plugId) ) {
        for (map <int,skindesignerapi::sPlugMenu>::iterator it = plugMenus->begin(); it != plugMenus->end(); it++) {
            int menuNumber = it->first;
            cString menuName = cString::sprintf("menuplugin-%s-%d", plugName.c_str(), menuNumber);
            subviewNames.insert(pair<string, int>(*menuName, subViewNumber));
            plgManager->AddSubviewMapping(plugId, menuNumber, subViewNumber);
            subViewNumber++;
        }
    }
}

void cViewMenu::ClearVariables(void) {
    cView::ClearVariables();
    menuChange = true;
    listChange = true;
    detailViewInit = false;
    menuCat = mcUnknown;
    pluginIdSet = false;
    plugId = -1;
    plugMenuId = -1;
    activeSubview = NULL;
    activeSubviewLast = NULL;
}

int cViewMenu::SubviewId(const char *name) {
    map<string, int>::iterator hit = subviewNames.find(name);
    if (hit != subviewNames.end())
        return (int)hit->second;
    return ATTR_UNKNOWN;
}

bool cViewMenu::ValidSubView(const char *subView) {
    if (SubviewId(subView) != ATTR_UNKNOWN)
        return true;
    return false;
}

cSubView *cViewMenu::CreateSubview(const char *name) {
    cSubView *sv = NULL;
    if (!strcmp(name, "menudefault"))
        sv = new cViewMenuDefault(name);
    else if (!strcmp(name, "menumain"))
        sv = new cViewMenuMain(name);
    else if (!strcmp(name, "menusetup"))
        sv = new cViewMenuSetup(name);
    else if (!strcmp(name, "menuschedules"))
        sv = new cViewMenuSchedules(name);
    else if (!strcmp(name, "menuchannels"))
        sv = new cViewMenuChannels(name);
    else if (!strcmp(name, "menutimers"))
        sv = new cViewMenuTimers(name);
    else if (!strcmp(name, "menurecordings"))
        sv = new cViewMenuRecordings(name);
    else if (!strcmp(name, "menudetailedepg"))
        sv = new cViewMenuDetail(name);
    else if (!strcmp(name, "menudetailedrecording"))
        sv = new cViewMenuDetail(name);
    else if (!strcmp(name, "menudetailedtext"))
        sv = new cViewMenuDetail(name);
    return sv;
}

cSubView *cViewMenu::CreatePluginview(const char *plugname, int plugId, int menuNumber, int menuType) {
    cString menuName = cString::sprintf("menuplugin-%s-%d", plugname, menuNumber);
    cSubView *pv = NULL;
    if (menuType == skindesignerapi::mtList)
        pv = new cViewMenuPlugin(*menuName);
    else if (menuType == skindesignerapi::mtText)
        pv = new cViewMenuDetail(*menuName);
    if (pv) {
        pv->SetPlugId(plugId);
        pv->SetPlugMenuId(menuNumber);
    }
    return pv;
}


void cViewMenu::AddPluginview(cSubView *plugView) {
    int id = SubviewId(plugView->GetViewName());
    if (id == ATTR_UNKNOWN)
        return;
    subViews[id] = plugView;
}

void cViewMenu::AddSubview(const char *sSubView, cSubView *subView) {
    int id = SubviewId(sSubView);
    if (id == ATTR_UNKNOWN)
        return;
    subViews[id] = subView;
}

/* eMenuCategory:
    -1 mcUndefined,
    0  mcUnknown,
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
    11 mcRecordingEdit,
    12 mcPlugin,
    13 mcPluginSetup,
    14 mcSetup,
    15 mcSetupOsd,
    16 mcSetupEpg,
    17 mcSetupDvb,
    18 mcSetupLnb,
    19 mcSetupCam,
    20 mcSetupRecord,
    21 mcSetupReplay,
    22 mcSetupMisc,
    23 mcSetupPlugins,
    24 mcCommand,
    25 mcEvent,
    26 mcText,
    27 mcFolder,
    28 mcCam
*/
void cViewMenu::SetSubView(eMenuCategory MenuCat) {
    //menuCat --> old menucat
    //MenuCat --> new menucat
    if (menuCat == mcMain) {
        cViewMenuMain *menuMain = dynamic_cast<cViewMenuMain*>(activeSubview);
        if (menuMain) {
            plugName = menuMain->GetPlugin();
        }
    }
    //first check plugin menu
    if (MenuCat == mcPlugin) {
        bool plugMenuFound = SetPluginSubView(MenuCat);
        if (plugMenuFound) {
            menuCat = MenuCat;
            return;
        }
    }
    //then check regular menus
    bool catChange = false;
    if (menuCat != MenuCat) {
        catChange = true;
    } else {
        //assume at least menuChange for mcSchedules menu
        //that correct channel will be set in header
        if (menuCat == mcSchedule)
            menuChange = true;
        return;
    }
    cSubView *newSubview = NULL;
    menuCat = MenuCat;
    switch (MenuCat) {
        case mcMain:
            newSubview = subViews[(int)eSvDisplayMenu::main];
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            newSubview = subViews[(int)eSvDisplayMenu::schedules];
            break;
        case mcChannel:
            newSubview = subViews[(int)eSvDisplayMenu::channels];
            break;
        case mcTimer:
            newSubview = subViews[(int)eSvDisplayMenu::timers];
            break;
        case mcRecording:
            newSubview = subViews[(int)eSvDisplayMenu::recordings];
            break;
        case mcSetup:
            newSubview = subViews[(int)eSvDisplayMenu::setup];
            break;
        case mcEvent:
            newSubview = subViews[(int)eSvDisplayMenu::detailepg];
            break;
        case mcRecordingInfo:
            newSubview = subViews[(int)eSvDisplayMenu::detailrec];
            break;
        case mcText:
            newSubview = subViews[(int)eSvDisplayMenu::detailtext];
            break;
        default:
            newSubview = subViews[(int)eSvDisplayMenu::menudefault];
            //setting plugin name for detecting plugins in default menus
            cViewMenuDefault *menuDefault = dynamic_cast<cViewMenuDefault*>(newSubview);
            if (menuDefault) {
                menuDefault->SetPlugin(plugName);
            }
            break;
    };
    if (catChange) {
        menuChange = true;
        activeSubview->Clear();
        activeSubviewLast = activeSubview;
        activeSubview = newSubview;
        if (!activeSubview)
            activeSubview = subViews[(int)eSvDisplayMenu::menudefault];    
        activeSubview->SetMenuCategory(MenuCat);
    }
}

bool cViewMenu::SetPluginSubView(eMenuCategory menuCat) {
    if (!pluginIdSet)
        return false;
    cSubView *newSubview = NULL;
    if (plugId >= 0 && plugMenuId >= 0) {
        int subViewId = plgManager->GetSubviewId(plugId, plugMenuId);
        if (subViewId >= 0 && subViews[subViewId]) {
            newSubview = subViews[subViewId];
            menuChange = true;
            activeSubview->Clear();
            activeSubviewLast = activeSubview;
            activeSubview = newSubview;
            activeSubview->SetMenuCategory(menuCat);
            return true;
        }
    } 
    return false;
}

void cViewMenu::WakeViewElements(void) {
    cView::WakeViewElements();
    if (activeSubview)
        activeSubview->WakeViewElements();    
}


void cViewMenu::SetSortMode(eMenuSortMode sortMode) {
    if (activeSubview)
        activeSubview->SetSortMode(sortMode);
}

void cViewMenu::SetPluginMenu(int plugId, int plugMenuId) { 
    this->plugId = plugId; 
    this->plugMenuId = plugMenuId;
    pluginIdSet = true;
}

int cViewMenu::NumListItems(void) {
    return activeSubview->NumListItems();
}

eMenuOrientation cViewMenu::MenuOrientation(void) {
    return activeSubview->MenuOrientation();    
}

const cFont *cViewMenu::GetTextAreaFont(void) {
    return activeSubview->GetTextAreaFont();
}

int cViewMenu::GetTextAreaWidth(void) {
    if (!menuDetailedText)
        return 0;
    return menuDetailedText->GetWidth();
}

int cViewMenu::GetListWidth(void) {
    return activeSubview->GetListWidth();
}

void cViewMenu::SetTitleHeader(const char *title) {
    activeSubview->SetTitle(title);
}

void cViewMenu::SetMessage(eMessageType type, const char *text) {
    activeSubview->SetMessage(type, text);    
}

void cViewMenu::SetChannelHeader(const cEvent *event) {
    if (!event)
        return;

    if (menuChange && menuCat == mcSchedule) {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(event->ChannelID());
#else
        const cChannel *channel = Channels.GetByChannelID(event->ChannelID());
#endif
        if (channel)
            activeSubview->SetChannel(channel);
    }
}

void cViewMenu::SetMenuButtons(const char *red, const char *green, const char *yellow, const char *blue) {
    activeSubview->SetMenuButtons(red, green, yellow, blue);
}

void cViewMenu::SetScrollbar(int total, int offset) {
    activeSubview->SetScrollbar(total, offset);    
}

void cViewMenu::SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5) {
    menuDefault->SetTabs(tab1, tab2, tab3, tab4, tab5);
}

void cViewMenu::SetItem(const char *text, int index, bool current, bool selectable) {
    if (menuCat == mcMain && menuMain) {
        menuMain->SetItem(text, index, current, selectable);
    } else if (menuCat == mcSetup && menuSetup) {
        menuSetup->SetItem(text, index, current, selectable);
    } else {
        menuDefault->SetItem(text, index, current, selectable);
    }
    listChange = true;
}

bool cViewMenu::SetItemEvent(const cEvent *event, int index, bool current, bool selectable, 
                             const cChannel *channel, bool withDate, eTimerMatch timerMatch) {
    if (!menuSchedules)
        return false;
    menuSchedules->SetItem(event, index, current, selectable, channel, withDate, timerMatch);
    listChange = true;
    return true;
}

bool cViewMenu::SetItemTimer(const cTimer *timer, int index, bool current, bool selectable) {
    if (!menuTimers)
        return false;
    menuTimers->SetItem(timer, index, current, selectable);
    listChange = true;
    return true;
}

bool cViewMenu::SetItemChannel(const cChannel *channel, int index, bool current, bool selectable, bool withProvider) {
    if (!menuChannels)
        return false;
    menuChannels->SetItem(channel, index, current, selectable, withProvider);
    listChange = true;
    return true;
}

bool cViewMenu::SetItemRecording(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New) {
    if (!menuRecordings)
        return false;
    menuRecordings->SetItem(recording, index, current, selectable, level, total, New);    
    listChange = true;
    return true;
}

bool cViewMenu::SetItemPlugin(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable) {
    cViewMenuPlugin *menuPlugin = dynamic_cast<cViewMenuPlugin*>(activeSubview);
    if (!menuPlugin) {
        return false;
    }
    menuPlugin->SetItem(tk, index, current, selectable);    
    listChange = true;
    return true;    
}

void cViewMenu::SetEvent(const cEvent *event) {
    menuDetailedEpg->SetEvent(event);
    detailViewInit = true;
}

void cViewMenu::SetRecording(const cRecording *recording) {
    menuDetailedRec->SetRecording(recording);
    detailViewInit = true;    
}

void cViewMenu::SetText(const char *text) {
    menuDetailedText->SetText(text);
    detailViewInit = true;    
}

bool cViewMenu::SetPluginText(skindesignerapi::cTokenContainer *tk) {
    cViewMenuDetail *menuPlugin = dynamic_cast<cViewMenuDetail*>(activeSubview);
    if (!menuPlugin) {
        return false;
    }
    menuPlugin->SetPluginText(tk);  
    detailViewInit = true;
    return true;
}

void cViewMenu::SetCurrentRecording(const char *currentRec) {
    if (menuMain) {
        menuMain->SetCurrentRecording(currentRec);    }
}

void cViewMenu::KeyDetailView(bool up, bool page) {
    cViewMenuDetail *detailView = dynamic_cast<cViewMenuDetail*>(activeSubview);
    if (!detailView)
        return;
    if (up && page) {
        detailView->KeyLeft();
    } else if (!up && page) {
        detailView->KeyRight();
    } else if (up && !page) {
        detailView->KeyUp();
    } else if (!up && !page) {
        detailView->KeyDown();
    }
}

bool cViewMenu::Init(void) {
    activeSubview = subViews[(int)eSvDisplayMenu::menudefault];
    return cView::Init(); 
}

void cViewMenu::Close(void) {
    if (animator) {
        animator->Stop();
        animator->Finish();
        delete animator;
        animator = NULL;
    }
    for (int i=0; i < numSubviews; i++) {
        if (subViews[i]) {
            subViews[i]->Close();
        }
    }
    for (int i=0; i < numViewElements; i++) {
        if (viewElements[i]) {
            viewElements[i]->Close();
        }
        if (viewElementsHorizontal[i]) {
            viewElementsHorizontal[i]->Close();
        }
    }
    UnScaleTv();
    ClearVariables();
    sdOsd.Flush();
    sdOsd.DeleteOsd();
}

void cViewMenu::Clear(void) {
    activeSubview->ClearViewList();
}

void cViewMenu::Flush(void) {
    bool staticInitiated = false;
    if (menuChange) {
        newTvFrame = activeSubview->GetTvFrame();
        menuInit = true;
        activeSubview->DrawStaticVEs();
        pluginIdSet = false;
        menuChange = false;
        staticInitiated = true;
    }
    if (listChange) {
        activeSubview->DrawList();
        listChange = false;
    }
    if (detailViewInit) {
        if (!staticInitiated)
            activeSubview->DrawStaticVEs();
        activeSubview->DrawDetailedView();
        pluginIdSet = false;
        detailViewInit = false;
    }
    activeSubview->DrawDynamicVEs();
    cView::Flush();
}

void cViewMenu::SetTransparency(int transparency, bool forceDetached) {
    activeSubview->SetTransparency(transparency, forceDetached);
    if (menuDetailedEpg)
        menuDetailedEpg->SetTransparency(transparency);
    if (menuDetailedRec)
        menuDetailedRec->SetTransparency(transparency);
    if (menuDetailedText)
        menuDetailedText->SetTransparency(transparency);
}

void cViewMenu::Debug(void) {
    cView::Debug();
    for (int i=0; i < numSubviews; i++) {
        if (subViews[i]) {
            subViews[i]->Debug();
        }
    }
}

/***********************************************************
* cSubView
***********************************************************/
cSubView::cSubView(const char *name) {
    menuCat = mcUnknown;
    plugId = -1;
    plugMenuId = -1;
    this->viewName = strdup(name);
    numViewElements = (int)eVeDisplayMenu::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    viewElementsHorizontal = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElementsHorizontal[i] = NULL;
    }
    background = NULL;
    header = NULL;
    datetime = NULL;
    time = NULL;
    message = NULL;
    sortmode = NULL;
    colorbuttons = NULL;
    scrollbar = NULL;
    viewList = NULL;
    SetViewElements();
}

cSubView::~cSubView(void) {
    for(vector<cViewList*>::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        delete (*it);
    }
}

void cSubView::SetGlobals(cGlobals *globals) {
    cView::SetGlobals(globals);
    for(vector<cViewList*>::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        (*it)->SetGlobals(globals);
    }
}

void cSubView::PreCache(void) {
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->Cache();
    for(vector<cViewList*>::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        cViewList *vl = (*it);
        vl->SetPlugId(plugId);
        vl->SetPlugMenuId(plugMenuId);
        vl->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
        vl->PreCache();
    }
    cView::PreCache();
}

bool cSubView::ViewElementSet(int ve) {
    if (ve >= 0 && ve < (int)eVeDisplayMenu::count && viewElements[ve])
        return true;
    return false;
}

bool cSubView::ViewElementHorizontalSet(int ve) {
    if (ve >= 0 && ve < (int)eVeDisplayMenu::count && viewElementsHorizontal[ve])
        return true;
    return false;
}

void cSubView::SetViewElement(eVeDisplayMenu ve, cViewElement *viewElement) {
    switch (ve) {
        case eVeDisplayMenu::background:
            if (!background)
                background = viewElement;
            break;
        case eVeDisplayMenu::header:
            if (!header)
                header = dynamic_cast<cVeDmHeader*>(viewElement);
            break;
        case eVeDisplayMenu::datetime:
            if (!datetime)
                datetime = dynamic_cast<cVeDateTime*>(viewElement);
            break;
        case eVeDisplayMenu::time:
            if (!time)
                time = dynamic_cast<cVeTime*>(viewElement);
            break;
        case eVeDisplayMenu::message:
            if (!message)
                message = dynamic_cast<cVeMessage*>(viewElement);
            break;
        case eVeDisplayMenu::sortmode:
            if (!sortmode)
                sortmode = dynamic_cast<cVeDmSortmode*>(viewElement);
            break;
        case eVeDisplayMenu::colorbuttons:
            if (!colorbuttons)
                colorbuttons = dynamic_cast<cVeDmColorbuttons*>(viewElement);
            break;
        case eVeDisplayMenu::scrollbar:
            if (!scrollbar)
                scrollbar = dynamic_cast<cVeDmScrollbar*>(viewElement);
            break;
        default:
            break;
    };
}

void cSubView::SetViewElementHorizontal(eVeDisplayMenu ve, cViewElement *viewElement) {
    eOrientation orientation = attribs->Orientation();
    if (orientation != eOrientation::horizontal)
        return;
    switch (ve) {
        case eVeDisplayMenu::background:
            if (!background)
                background = viewElement;
            break;
        case eVeDisplayMenu::header:
            if (!header)
                header = dynamic_cast<cVeDmHeader*>(viewElement);
            break;
        case eVeDisplayMenu::datetime:
            if (!datetime)
                datetime = dynamic_cast<cVeDateTime*>(viewElement);
            break;
        case eVeDisplayMenu::time:
            if (!time)
                time = dynamic_cast<cVeTime*>(viewElement);
            break;
        case eVeDisplayMenu::message:
            if (!message)
                message = dynamic_cast<cVeMessage*>(viewElement);
            break;
        case eVeDisplayMenu::sortmode:
            if (!sortmode)
                sortmode = dynamic_cast<cVeDmSortmode*>(viewElement);
            break;
        case eVeDisplayMenu::colorbuttons:
            if (!colorbuttons)
                colorbuttons = dynamic_cast<cVeDmColorbuttons*>(viewElement);
            break;
        case eVeDisplayMenu::scrollbar:
            if (!scrollbar)
                scrollbar = dynamic_cast<cVeDmScrollbar*>(viewElement);
            break;
        default:
            break;
    };
}

void cSubView::AddViewList(cViewList *viewList) {
    viewLists.push_back(viewList);
}

int cSubView::NumListItems(void) {
    if (viewList)
        return viewList->NumItems();
    return 0;
}

eMenuOrientation cSubView::MenuOrientation(void) {
    eOrientation orientation = attribs->Orientation();
    if (orientation == eOrientation::horizontal)
        return moHorizontal;
    return moVertical;
}

void cSubView::SetTitle(const char *title) {
    if (header) {
        header->SetTitle(title);
        header->ClearChannel();
    }
}

void cSubView::SetChannel(const cChannel *channel) {
    if (header) {
        header->SetChannel(channel);
    }
}

void cSubView::SetMessage(eMessageType type, const char *text) {
    if (!message)
        return;
    if (!text) {
        message->Clear();
        message->Hide();
        return;
    }
    message->Set(type, text);
    message->Show();
    if (message->Parse())
        message->Render(); 
}

void cSubView::SetMenuButtons(const char *red, const char *green, const char *yellow, const char *blue) {
    if (!colorbuttons)
        return;
    colorbuttons->SetButtons(red, green, yellow, blue);
    if (colorbuttons->Parse()) {
        colorbuttons->Show();
        colorbuttons->Render();
    }
}

void cSubView::SetScrollbar(int total, int offset) {
    if (!scrollbar)
        return;
    scrollbar->SetList(total, offset, NumListItems());
    if (scrollbar->Parse()) {
        scrollbar->Show();
        scrollbar->Render();
    }
}

void cSubView::SetSortMode(eMenuSortMode sortMode) {
    if (!sortmode)
        return;
    sortmode->Set(sortMode);
    if (sortmode->Parse()) {
        sortmode->Show();
        sortmode->Render();
    }
}

void cSubView::Close(void) {
    ClearVariables();
    for (int i=0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        viewElements[i]->Close();
    }
    for (int i=0; i < numViewElements; i++) {
        if (!viewElementsHorizontal[i])
            continue;
        viewElementsHorizontal[i]->Close();
    }
    if (viewList)
        viewList->Close();
}

void cSubView::ClearViewList(void) {
    if (viewList)
        viewList->Clear();
}

void cSubView::WakeViewElements(void) {
    for (int i = 0; i < numViewElements; i++) {
        if (viewElements[i]) {
            viewElements[i]->WakeUp();
        }
        if (viewElementsHorizontal[i]) {
            viewElementsHorizontal[i]->WakeUp();
        }
    }
}

void cSubView::Clear(void) {
    if (background)   background->Hide();
    if (datetime)     datetime->Hide();
    if (time)         time->Hide();
    if (header)       header->Hide();
    if (colorbuttons) colorbuttons->Hide();
    if (scrollbar)    scrollbar->Hide();
    if (sortmode) {
        sortmode->Reset(); 
        sortmode->Hide();
    }

    if (viewList)     viewList->Close();
}

void cSubView::DrawStaticVEs(void) {
    if (background) {
        background->Show();
        background->Render();
    }
}

void cSubView::DrawDynamicVEs(void) {
    if (header) {
        header->Show();
        header->Set(menuCat);
        if (header->Parse())
            header->Render();
    }
    if (datetime) {
        datetime->Show();
        if (datetime->Parse())
            datetime->Render();
    }
    if (time) {
        time->Show();
        if (time->Parse()) {
            time->Render();
        }
    }
}

void cSubView::DrawList(void) {
    if (viewList) {
        viewList->Draw(menuCat);
        viewList->ResetItemCount();
    }
}

void cSubView::SetTransparency(int transparency, bool forceDetached) {
    for (int i = 0; i < numViewElements; i++) {
        if (viewElements[i] && (!viewElements[i]->Detached() || forceDetached)) {
            viewElements[i]->SetTransparency(transparency);
        }
        if (viewElementsHorizontal[i] && (!viewElementsHorizontal[i]->Detached() || forceDetached)) {
            viewElementsHorizontal[i]->SetTransparency(transparency);
        }
    }
    if (viewList)
        viewList->SetTransparency(transparency);
}
/***********************************************************
* Protected Functions
***********************************************************/
void cSubView::SetViewElementObjects(void) {
    eOrientation orientation = attribs->Orientation();

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::background])
        background = viewElementsHorizontal[(int)eVeDisplayMenu::background];
    else if (viewElements[(int)eVeDisplayMenu::background])
        background = viewElements[(int)eVeDisplayMenu::background];

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::header])
        header = dynamic_cast<cVeDmHeader*>(viewElementsHorizontal[(int)eVeDisplayMenu::header]);
    else if (viewElements[(int)eVeDisplayMenu::header])
        header = dynamic_cast<cVeDmHeader*>(viewElements[(int)eVeDisplayMenu::header]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::datetime])
        datetime = dynamic_cast<cVeDateTime*>(viewElementsHorizontal[(int)eVeDisplayMenu::datetime]);
    else if (viewElements[(int)eVeDisplayMenu::datetime])
        datetime = dynamic_cast<cVeDateTime*>(viewElements[(int)eVeDisplayMenu::datetime]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::time])
        time = dynamic_cast<cVeTime*>(viewElementsHorizontal[(int)eVeDisplayMenu::time]);
    else if (viewElements[(int)eVeDisplayMenu::time])
        time = dynamic_cast<cVeTime*>(viewElements[(int)eVeDisplayMenu::time]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::message])
        message = dynamic_cast<cVeMessage*>(viewElementsHorizontal[(int)eVeDisplayMenu::message]);
    else if (viewElements[(int)eVeDisplayMenu::message])
        message = dynamic_cast<cVeMessage*>(viewElements[(int)eVeDisplayMenu::message]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::sortmode])
        sortmode = dynamic_cast<cVeDmSortmode*>(viewElementsHorizontal[(int)eVeDisplayMenu::sortmode]);
    else if (viewElements[(int)eVeDisplayMenu::sortmode])
        sortmode = dynamic_cast<cVeDmSortmode*>(viewElements[(int)eVeDisplayMenu::sortmode]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::colorbuttons])
        colorbuttons = dynamic_cast<cVeDmColorbuttons*>(viewElementsHorizontal[(int)eVeDisplayMenu::colorbuttons]);
    else if (viewElements[(int)eVeDisplayMenu::colorbuttons])
        colorbuttons = dynamic_cast<cVeDmColorbuttons*>(viewElements[(int)eVeDisplayMenu::colorbuttons]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenu::scrollbar])
        scrollbar = dynamic_cast<cVeDmScrollbar*>(viewElementsHorizontal[(int)eVeDisplayMenu::scrollbar]);
    else if (viewElements[(int)eVeDisplayMenu::scrollbar])
        scrollbar = dynamic_cast<cVeDmScrollbar*>(viewElements[(int)eVeDisplayMenu::scrollbar]);

    //set appropriate viewlist, orientation and condition have to be considered
    for(vector<cViewList*>::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        cViewList *vl = (*it);
        if (vl->Orientation() == orientation && vl->Execute()) {
            viewList = vl;
            break;
        }
    }
    if (!viewList && viewLists.size() > 0) {
        viewList = viewLists.front();
    }
}

void cSubView::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayMenu::background));
    viewElementNames.insert(pair<string, int>("header", (int)eVeDisplayMenu::header));
    viewElementNames.insert(pair<string, int>("datetime", (int)eVeDisplayMenu::datetime));
    viewElementNames.insert(pair<string, int>("time", (int)eVeDisplayMenu::time));
    viewElementNames.insert(pair<string, int>("colorbuttons", (int)eVeDisplayMenu::colorbuttons));
    viewElementNames.insert(pair<string, int>("message", (int)eVeDisplayMenu::message));
    viewElementNames.insert(pair<string, int>("scrollbar", (int)eVeDisplayMenu::scrollbar));
    viewElementNames.insert(pair<string, int>("sortmode", (int)eVeDisplayMenu::sortmode));
}

/***********************************************************
* cViewMenuDefault
***********************************************************/
cViewMenuDefault::cViewMenuDefault(const char *name) : cSubView(name) {
    listDefault = NULL;
}

cViewMenuDefault::~cViewMenuDefault(void) {
}

void cViewMenuDefault::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listDefault = dynamic_cast<cViewListDefault*>(viewList);
}

void cViewMenuDefault::SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5) {
    if (listDefault)
        listDefault->SetTabs(tab1, tab2, tab3, tab4, tab5);
}

void cViewMenuDefault::SetPlugin(const char *plugName) {
    listDefault->SetPlugin(plugName);
}

void cViewMenuDefault::SetItem(const char *text, int index, bool current, bool selectable) {
    listDefault->Set(text, index, current, selectable);
}

const cFont *cViewMenuDefault::GetTextAreaFont(void) {
    if (!listDefault)
        return NULL;
    return listDefault->GetListFont();
}

int cViewMenuDefault::GetListWidth(void) {
    if (!listDefault)
        return 0;
    return listDefault->GetListWidth();
}

/************************************************************************************
* cViewMenuMain
************************************************************************************/
cViewMenuMain::cViewMenuMain(const char *name) : cSubView(name) {
    for (int i=0; i < numViewElements; i++) {
        delete viewElements[i];
    }
    delete[] viewElements;
    numViewElements = (int)eVeDisplayMenuMain::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    delete[] viewElementsHorizontal;
    viewElementsHorizontal = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElementsHorizontal[i] = NULL;
    }
    listMain = NULL;
    timers = NULL;
    devices = NULL;
    weather = NULL;
    discusage = NULL;
    load = NULL;
    memory = NULL;
    vdrstats = NULL;
    temperatures = NULL;
    currentSchedule = NULL;
    lastRecordings = NULL;
    customTokens = NULL;
    lastDrawDynamic = 0;
    SetViewElements();
}

cViewMenuMain::~cViewMenuMain() {
}

void cViewMenuMain::ClearVariables(void) {
    init = true;
    lastDrawDynamic = 0;
}

void cViewMenuMain::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("timers", (int)eVeDisplayMenuMain::timers));
    viewElementNames.insert(pair<string, int>("devices", (int)eVeDisplayMenuMain::devices));
    viewElementNames.insert(pair<string, int>("currentweather", (int)eVeDisplayMenuMain::currentweather));
    viewElementNames.insert(pair<string, int>("currentschedule", (int)eVeDisplayMenuMain::currentschedule));
    viewElementNames.insert(pair<string, int>("discusage", (int)eVeDisplayMenuMain::discusage));
    viewElementNames.insert(pair<string, int>("systemload", (int)eVeDisplayMenuMain::systemload));
    viewElementNames.insert(pair<string, int>("systemmemory", (int)eVeDisplayMenuMain::systemmemory));
    viewElementNames.insert(pair<string, int>("vdrstatistics", (int)eVeDisplayMenuMain::vdrstatistics));
    viewElementNames.insert(pair<string, int>("temperatures", (int)eVeDisplayMenuMain::temperatures));
    viewElementNames.insert(pair<string, int>("lastrecordings", (int)eVeDisplayMenuMain::lastrecordings));
    viewElementNames.insert(pair<string, int>("customtokens", (int)eVeDisplayMenuMain::customtokens));
}

void cViewMenuMain::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    eOrientation orientation = attribs->Orientation();

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::timers])
        timers = dynamic_cast<cVeDmTimers*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::timers]);
    else if (viewElements[(int)eVeDisplayMenuMain::timers])
        timers = dynamic_cast<cVeDmTimers*>(viewElements[(int)eVeDisplayMenuMain::timers]);
    
    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::devices])
        devices = dynamic_cast<cVeDevices*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::devices]);
    else if (viewElements[(int)eVeDisplayMenuMain::devices])
        devices = dynamic_cast<cVeDevices*>(viewElements[(int)eVeDisplayMenuMain::devices]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::currentweather])
        weather = dynamic_cast<cVeCurrentWeather*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::currentweather]);
    else if (viewElements[(int)eVeDisplayMenuMain::currentweather])
        weather = dynamic_cast<cVeCurrentWeather*>(viewElements[(int)eVeDisplayMenuMain::currentweather]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::currentschedule])
        currentSchedule = dynamic_cast<cVeDmCurrentschedule*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::currentschedule]);
    else if (viewElements[(int)eVeDisplayMenuMain::currentschedule])
        currentSchedule = dynamic_cast<cVeDmCurrentschedule*>(viewElements[(int)eVeDisplayMenuMain::currentschedule]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::discusage])
        discusage = dynamic_cast<cVeDmDiscusage*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::discusage]);
    else if (viewElements[(int)eVeDisplayMenuMain::discusage])
        discusage = dynamic_cast<cVeDmDiscusage*>(viewElements[(int)eVeDisplayMenuMain::discusage]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::systemload])
        load = dynamic_cast<cVeDmSystemload*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::systemload]);
    else if (viewElements[(int)eVeDisplayMenuMain::systemload])
        load = dynamic_cast<cVeDmSystemload*>(viewElements[(int)eVeDisplayMenuMain::systemload]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::systemmemory])
        memory = dynamic_cast<cVeDmSystemmemory*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::systemmemory]);
    else if (viewElements[(int)eVeDisplayMenuMain::systemmemory])
        memory = dynamic_cast<cVeDmSystemmemory*>(viewElements[(int)eVeDisplayMenuMain::systemmemory]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::vdrstatistics])
        vdrstats = dynamic_cast<cVeDmVdrstatistics*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::vdrstatistics]);
    else if (viewElements[(int)eVeDisplayMenuMain::vdrstatistics])
        vdrstats = dynamic_cast<cVeDmVdrstatistics*>(viewElements[(int)eVeDisplayMenuMain::vdrstatistics]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::temperatures])
        temperatures = dynamic_cast<cVeDmTemperatures*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::temperatures]);
    else if (viewElements[(int)eVeDisplayMenuMain::temperatures])
        temperatures = dynamic_cast<cVeDmTemperatures*>(viewElements[(int)eVeDisplayMenuMain::temperatures]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::lastrecordings])
        lastRecordings = dynamic_cast<cVeDmLastrecordings*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::lastrecordings]);
    else if (viewElements[(int)eVeDisplayMenuMain::lastrecordings])
        lastRecordings = dynamic_cast<cVeDmLastrecordings*>(viewElements[(int)eVeDisplayMenuMain::lastrecordings]);

    if (orientation == eOrientation::horizontal && viewElementsHorizontal[(int)eVeDisplayMenuMain::customtokens])
        customTokens = dynamic_cast<cVeCustomTokens*>(viewElementsHorizontal[(int)eVeDisplayMenuMain::customtokens]);
    else if (viewElements[(int)eVeDisplayMenuMain::customtokens])
        customTokens = dynamic_cast<cVeCustomTokens*>(viewElements[(int)eVeDisplayMenuMain::customtokens]);

    if (devices) devices->SetDetached();
    if (weather) weather->SetDetached();
    if (discusage) discusage->SetDetached();
    if (load) load->SetDetached();
    if (memory) memory->SetDetached();
    if (vdrstats) vdrstats->SetDetached();
    if (temperatures) temperatures->SetDetached();
    if (timers) timers->SetDetached();
    if (currentSchedule) currentSchedule->SetDetached();
    if (lastRecordings) lastRecordings->SetDetached();
    if (customTokens) customTokens->SetDetached();

    listMain = dynamic_cast<cViewListMain*>(viewList);
}

void cViewMenuMain::Clear(void) {
    cSubView::Clear();
    lastDrawDynamic = 0;

    if (devices)
        devices->Hide();

    if (weather)
        weather->Hide();

    if (discusage)
        discusage->Hide();
    
    if (load) {
        load->Reset();
        load->Hide();
    }
    
    if (memory) {
        memory->Reset();
        memory->Hide();
    }
    
    if (vdrstats) {
        vdrstats->Reset();
        vdrstats->Hide();
    }
    
    if (temperatures) {
        temperatures->Reset();
        temperatures->Hide();
    }
    
    if (timers) {
        timers->Hide();
    }
    
    if (currentSchedule) {
        currentSchedule->Hide();
    }
    
    if (lastRecordings) {
        lastRecordings->Hide();
    }
    
    if (customTokens) {
        customTokens->Reset();
        customTokens->Hide();
    }
}

void cViewMenuMain::SetItem(const char *text, int index, bool current, bool selectable) {
    listMain->Set(text, index, current, selectable);
}

void cViewMenuMain::SetCurrentRecording(const char *currentRec) {
    if (currentSchedule)
        currentSchedule->SetRecording(currentRec);
}

void cViewMenuMain::DrawStaticVEs(void) {
    cSubView::DrawStaticVEs();
    if (weather) {
        weather->Show();
        if (weather->Parse())
            weather->Render();
    }
    if (discusage) {
        discusage->Show();
        if (discusage->Parse())
            discusage->Render();
    }
    if (timers) {
        timers->Show();
        if (timers->Parse())
            timers->Render();
    }
    if (currentSchedule) {
        currentSchedule->Show();
        if (currentSchedule->Parse())
            currentSchedule->Render();
    }
    if (lastRecordings) {
        lastRecordings->Show();
        if (lastRecordings->Parse())
            lastRecordings->Render();
    }
}

void cViewMenuMain::DrawDynamicVEs(void) {
    cSubView::DrawDynamicVEs();
    //draw main menu dynamic elements 
    //only every 3 seconds
    uint64_t now = cTimeMs::Now();
    if (now - lastDrawDynamic < 3000)
        return;
    else {
        lastDrawDynamic = now;
    }

    if (devices) {
        devices->Show();
        if (devices->Parse())
            devices->Render();
    }
    if (load) {
        load->Show();
        if (load->Parse())
            load->Render();
    }
    if (memory) {
        memory->Show();
        if (memory->Parse())
            memory->Render();
    }
    if (vdrstats) {
        vdrstats->Show();
        if (vdrstats->Parse())
            vdrstats->Render();
    }
    if (temperatures) {
        temperatures->Show();
        if (temperatures->Parse())
            temperatures->Render();
    }
    if (customTokens) {
        customTokens->Show();
        if (customTokens->Parse())
            customTokens->Render();
    }
}

const char *cViewMenuMain::GetPlugin(void) {
    return listMain->GetPlugin();
}

/***********************************************************
* cViewMenuSetup
***********************************************************/
cViewMenuSetup::cViewMenuSetup(const char *name) : cSubView(name) {
    listSetup = NULL;
}

cViewMenuSetup::~cViewMenuSetup(void) {
    
}

void cViewMenuSetup::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listSetup = dynamic_cast<cViewListMain*>(viewList);
}

void cViewMenuSetup::SetItem(const char *text, int index, bool current, bool selectable) {
    listSetup->Set(text, index, current, selectable);
}

/***********************************************************
* cViewMenuSchedules
***********************************************************/
cViewMenuSchedules::cViewMenuSchedules(const char *name) : cSubView(name) {
    listSchedules = NULL;
}

cViewMenuSchedules::~cViewMenuSchedules(void) {
}

void cViewMenuSchedules::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listSchedules = dynamic_cast<cViewListSchedules*>(viewList);
}

void cViewMenuSchedules::SetItem(const cEvent *event, int index, bool current, bool selectable, 
                             const cChannel *channel, bool withDate, eTimerMatch timerMatch) {
    
    bool epgSearchFavMenu = (menuCat == mcSchedule && channel) ? true : false;
    listSchedules->IsEpgSearchFav(epgSearchFavMenu);
    if (header) header->IsEpgSearchFav(epgSearchFavMenu);
    listSchedules->Set(event, index, current, selectable, channel, withDate, timerMatch);
}

/***********************************************************
* cViewMenuChannels
***********************************************************/
cViewMenuChannels::cViewMenuChannels(const char *name) : cSubView(name) {
    listChannels = NULL;
}

cViewMenuChannels::~cViewMenuChannels(void) {
}

void cViewMenuChannels::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listChannels = dynamic_cast<cViewListChannels*>(viewList);
}

void cViewMenuChannels::SetItem(const cChannel *channel, int index, bool current, bool selectable, bool withProvider) {
    listChannels->Set(channel, index, current, selectable, withProvider);
}

/***********************************************************
* cViewMenuTimers
***********************************************************/
cViewMenuTimers::cViewMenuTimers(const char *name) : cSubView(name) {
    listTimers = NULL;
}

cViewMenuTimers::~cViewMenuTimers(void) {    
}

void cViewMenuTimers::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listTimers = dynamic_cast<cViewListTimers*>(viewList);
}

void cViewMenuTimers::SetItem(const cTimer *timer, int index, bool current, bool selectable) {
    listTimers->Set(timer, index, current, selectable);
}

/***********************************************************
* cViewMenuRecordings
***********************************************************/
cViewMenuRecordings::cViewMenuRecordings(const char *name) : cSubView(name) {
    listRecordings = NULL;
}

cViewMenuRecordings::~cViewMenuRecordings(void) {
}

void cViewMenuRecordings::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listRecordings = dynamic_cast<cViewListRecordings*>(viewList);
}

void cViewMenuRecordings::SetItem(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New) {
    listRecordings->Set(recording, index, current, selectable, level, total, New);
}

/***********************************************************
* cViewMenuPlugin
***********************************************************/
cViewMenuPlugin::cViewMenuPlugin(const char *name) : cSubView(name) {
    listPlugin = NULL;
}

cViewMenuPlugin::~cViewMenuPlugin(void) {
}

void cViewMenuPlugin::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    listPlugin = dynamic_cast<cViewListPlugin*>(viewList);
}

void cViewMenuPlugin::SetItem(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable) {
    listPlugin->Set(tk, index, current, selectable);
}

/***********************************************************
* cViewMenuDetail
***********************************************************/
cViewMenuDetail::cViewMenuDetail(const char *name) : cSubView(name) {
    firstTab = true;
    for (int i=0; i < numViewElements; i++) {
        delete viewElements[i];
    }
    delete[] viewElements;
    numViewElements = (int)eVeDisplayDetailedMenu::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    delete[] viewElementsHorizontal;
    viewElementsHorizontal = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElementsHorizontal[i] = NULL;
    }    
    SetViewElements();
    detailedheaderEpg = NULL;
    detailedheaderRec = NULL;
    detailedheaderPlug = NULL;
    tablabels = NULL;
    detailViewEpg = NULL;
    detailViewRec = NULL;
    detailViewText = NULL;
    detailViewPlugin = NULL;
    SetDetailedView();
}

cViewMenuDetail::~cViewMenuDetail(void) {
}

void cViewMenuDetail::SetDetailedView(void) {
    if (!strcmp(viewName, "menudetailedepg")) {
        detailViewEpg = new cViewDetailEpg();
        detailView = detailViewEpg;
    } else if (!strcmp(viewName, "menudetailedrecording")) {
        detailViewRec = new cViewDetailRec();
        detailView = detailViewRec;
    } else if (!strcmp(viewName, "menudetailedtext")) {
        detailViewText = new cViewDetailText();
        detailView = detailViewText;
    } else if (startswith(viewName, "menuplugin")) {
        detailViewPlugin = new cViewDetailPlugin();
        detailView = detailViewPlugin;
    } else {
        esyslog("skindesigner: invalid detailedview %s", viewName);
    }    
    if (detailView) {
        detailView->SetOsd(&sdOsd);
    }
}

void cViewMenuDetail::AddTab(cArea *tab) {
    if (firstTab) {
        tab->SetActiveTab(true);
        firstTab = false;
    }
    detailView->AddArea(tab);
}

void cViewMenuDetail::SetGlobals(cGlobals *globals) {
    cView::SetGlobals(globals);
    detailView->SetGlobals(globals);
}

void cViewMenuDetail::PreCache(void) {
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->Cache();
    if (plugId >= 0 && plugMenuId >= 0) {
        cVeDmDetailheaderPlugin *dhPlug = dynamic_cast<cVeDmDetailheaderPlugin*>(viewElements[(int)eVeDisplayDetailedMenu::detailheader]);
        dhPlug->SetPlugId(plugId);
        dhPlug->SetPlugMenuId(plugMenuId);
        dhPlug->SetTokenContainer();
    }
    cView::PreCache();
    detailView->SetPlugId(plugId);
    detailView->SetPlugMenuId(plugMenuId);
    detailView->SetTokenContainer();
    detailView->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    detailView->Cache();
}

int cViewMenuDetail::GetWidth(void) {
    return detailView->GetWidth();
}

void cViewMenuDetail::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("detailheader", (int)eVeDisplayDetailedMenu::detailheader));
    viewElementNames.insert(pair<string, int>("tablabels", (int)eVeDisplayDetailedMenu::tablabels));
}

void cViewMenuDetail::SetViewElementObjects(void) {
    cSubView::SetViewElementObjects();
    if (viewElements[(int)eVeDisplayDetailedMenu::detailheader]) {
        if (!strcmp(viewName, "menudetailedepg"))
            detailedheaderEpg = dynamic_cast<cVeDmDetailheaderEpg*>(viewElements[(int)eVeDisplayDetailedMenu::detailheader]);
        else if (!strcmp(viewName, "menudetailedrecording"))
            detailedheaderRec = dynamic_cast<cVeDmDetailheaderRec*>(viewElements[(int)eVeDisplayDetailedMenu::detailheader]);
        else if (startswith(viewName, "menuplugin"))
            detailedheaderPlug = dynamic_cast<cVeDmDetailheaderPlugin*>(viewElements[(int)eVeDisplayDetailedMenu::detailheader]);
    }
    
    if (viewElements[(int)eVeDisplayDetailedMenu::tablabels])
        tablabels = dynamic_cast<cVeDmTablabels*>(viewElements[(int)eVeDisplayDetailedMenu::tablabels]);
}

void cViewMenuDetail::SetEvent(const cEvent *event) {
    if (detailedheaderEpg)
        detailedheaderEpg->SetEvent(event);
    if (detailViewEpg)
        detailViewEpg->SetEvent(event);
}

void cViewMenuDetail::SetRecording(const cRecording *recording) {
    if (detailedheaderRec)
        detailedheaderRec->SetRecording(recording);
    if (detailViewRec)
        detailViewRec->SetRecording(recording);
}

void cViewMenuDetail::SetText(const char *text) {
    if (detailViewText)
        detailViewText->SetText(text);
}

void cViewMenuDetail::SetPluginText(skindesignerapi::cTokenContainer *tk) {
    if (detailedheaderPlug)
        detailedheaderPlug->Set(tk);
    if (detailViewPlugin)
        detailViewPlugin->Set(tk);
}

void cViewMenuDetail::Clear(void) {
    cSubView::Clear();
    cSubView::Close();
    if (detailedheaderEpg) {
        detailedheaderEpg->Close();
    } else if (detailedheaderRec) {
        detailedheaderRec->Close();
    } else if (detailedheaderPlug) {
        detailedheaderPlug->Close();
    }
    if (header) {
        header->Hide();
    }
    if (detailView) {
        detailView->Close();
    }
}

void cViewMenuDetail::Close(void) {
    cSubView::Clear();
    cSubView::Close();
    if (detailView) {
        detailView->Close();
    }
}

void cViewMenuDetail::DrawStaticVEs(void) {
    cSubView::DrawStaticVEs();
    if (header) {
        header->Show();
        header->Set(menuCat);
        if (header->Parse())
            header->Render();
    }
    if (detailedheaderEpg && detailedheaderEpg->Parse()) {
        detailedheaderEpg->Render();
    } else if (detailedheaderRec && detailedheaderRec->Parse()) {
        detailedheaderRec->Render();
    } else if (detailedheaderPlug && detailedheaderPlug->Parse()) {
        detailedheaderPlug->Render();
    }
}

void cViewMenuDetail::DrawDynamicVEs(void) {
    if (datetime) {
        datetime->Show();
        if (datetime->Parse())
            datetime->Render();
    }
    if (time) {
        time->Show();
        if (time->Parse())
            time->Render();
    }
}

void cViewMenuDetail::DrawDetailedView(void) {
    detailView->Clear();
    detailView->ResetTabs();
    if (detailView->Parse()) {
        detailView->Render();
    }
    if (scrollbar) {
        scrollbar->Show();
        DrawScrollbar();
    }
    vector<const char*> tabs;
    int activeTab = detailView->GetTabs(tabs);
    if (tablabels) {        
        tablabels->SetTabs(tabs);
        tablabels->SetActiveTab(activeTab);
        if (tablabels->Parse())
            tablabels->Render();
    }
}

void cViewMenuDetail::KeyLeft(void) {
    if (detailView->NumTabs() > 1) {
        detailView->Clear();
        detailView->PrevTab();
        detailView->SetDirty();
        if (tablabels) {
            tablabels->Clear();
            tablabels->SetActiveTab(detailView->ActiveTab());
            if (tablabels->Parse())
                tablabels->Render();
        }
        sdOsd.Flush();
        detailView->Render();
        if (scrollbar) {
            DrawScrollbar();
        }
    } else {
        //scroll page
        if (detailView->ScrollUp(true)) {
            if (scrollbar)
                DrawScrollbar();
        }
    }
}

void cViewMenuDetail::KeyRight(void) {
    if (detailView->NumTabs() > 1) {
        detailView->Clear();
        detailView->NextTab();
        detailView->SetDirty();
        if (tablabels) {
            tablabels->Clear();
            tablabels->SetActiveTab(detailView->ActiveTab());
            if (tablabels->Parse())
                tablabels->Render();
        }
        sdOsd.Flush();
        detailView->Render();
        if (scrollbar) {
            DrawScrollbar();
        }
        
    } else {
        //scroll page
        if (detailView->ScrollDown(true)) {
            if (scrollbar)
                DrawScrollbar();
        }
    }
}

void cViewMenuDetail::KeyUp(void) {
    if (detailView->ScrollUp()) {
        if (scrollbar)
            DrawScrollbar();
    }
}

void cViewMenuDetail::KeyDown(void) {
    if (scrollbar && detailView->ScrollDown()) {
        if (scrollbar)
            DrawScrollbar();
    }
}

void cViewMenuDetail::SetTransparency(int transparency, bool forceDetached) {
    if (detailedheaderEpg)
        detailedheaderEpg->SetTransparency(transparency);
    if (detailedheaderRec)
        detailedheaderRec->SetTransparency(transparency);
    if (tablabels)
        tablabels->SetTransparency(transparency);
    if (detailView)
        detailView->SetTransparency(transparency);
}

void cViewMenuDetail::DrawScrollbar(void) {
    int barheight = 0;
    int offset = 0;
    bool end = true;
    detailView->Scrollbar(barheight, offset, end);
    scrollbar->SetDetail(barheight, offset, end);
    scrollbar->Render();    
}
