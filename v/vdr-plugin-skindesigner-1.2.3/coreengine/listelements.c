#include "listelements.h"
#include "../config.h"
#include "../services/epgtimer.h"
#include <sstream>
#include <algorithm>

/******************************************************************
* cListElement
******************************************************************/
cListElement::cListElement(void) { 
    dirty = false;
    num = -1;
    current = false;
    wasCurrent = false;
    selectable = false;
    selectedFromTop = true;
    suppressAnimation = false;
    listShifter = NULL;
    currentElement = NULL;
    menuCat = mcUndefined;
    orientation = eOrientation::vertical;
};

cListElement::cListElement(const cListElement &other) : cViewElement(other) {
    num = -1;
    current = false; 
    wasCurrent = false;
    selectable = false;     
    selectedFromTop = true;
    suppressAnimation = false;
    listShifter = NULL;
    currentElement = NULL;
    orientation = eOrientation::vertical;
}

void cListElement::SetCurrent(bool cur) { 
    if (!cur && current)
        wasCurrent = true;
    else
        wasCurrent = false;
    current = cur;
}

void cListElement::Close(void) {
    if (current && currentElement) {
        currentElement->Close();
    }
    cViewElement::Close();
    dirty = false;
}

void cListElement::Clear(bool forceClearBackground) {
    if (current && currentElement) {
        currentElement->Close();
    }
    cViewElement::Clear(forceClearBackground);
    dirty = false;
}

void cListElement::WakeCurrent(void) {
    if (currentElement) {
        currentElement->WakeUp();
    }
}

void cListElement::Render(void) {
    if (!dirty || blocked)
        return;

    if (attribs->DoDebug())
        Debug();
    bool animated = Fading() || Shifting();
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        //Check redraw of already scrolling list element
        if (drawn && node->Scrolling() && node->ScrollingStarted()) {
            if (DoScroll()) {
                //current list element
                continue;
            }
        }
        //don't clear animated list element if it was current 
        //and animation was not suppressed because of cleared list
        sdOsd->Lock();
        if (animated && wasCurrent && !suppressAnimation) {
            node->ClearWithoutIndicators();
        } else {
            node->Clear();
        }
        sdOsd->Unlock();
        if (!node->Execute())
            continue;
        sdOsd->Lock();
        node->Render();
        sdOsd->Unlock();
        
        if (DoScroll() && node->Scrolling() && !node->ScrollingStarted()) {
            cArea *scrollArea = node->ScrollingArea();
            if (scrollArea) {
                cScroller *scroller = new cScroller(scrollArea);
                scrollers.push_back(scroller);
                cView::AddAnimation(scroller);                
            }
        }
    }
    dirty = false;
    drawn = true;
    StartListAnimation();
}

int cListElement::ShiftDistance(void) {
    if (orientation == eOrientation::horizontal)
        return container.Width();
    return container.Height();
}

eOrientation cListElement::ShiftOrientation(void) {
    return orientation;
}

void cListElement::SetIndicatorPosition(cPoint &position) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        sdOsd->Lock();
        node->SetIndicatorPosition(position);
        sdOsd->Unlock();
    }
}

void cListElement::SetTransparency(int transparency, bool force) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        sdOsd->Lock();
        node->SetIndicatorTransparency(transparency);
        sdOsd->Unlock();
    }
}

char *cListElement::ParseSeparator(const char *text) {
    const char *start = text;
    while (*start && (*start == '-' || *start == ' ' || *start == 9)) {
        start++;
    }
    const char *end = start;
    while (*end && *end != '-') {
        end++;
    }
    int len = end - start;
    char *ret = (char*)malloc(len);
    memset(ret, 0, len);
    strncpy(ret, start, len-1);
    return ret;
}

void cListElement::StopListAnimation(void) {
    if (listShifter) {
        cView::RemoveAnimation(listShifter);
        listShifter = NULL;
    }
    if (fader) {
        cView::RemoveAnimation(fader);
        fader = NULL;
    }
}

void cListElement::StartListAnimation(void) {
    if (suppressAnimation)
        return;
    if (!Fading() && !Shifting())
        return;
    listShifter = NULL;
    fader = NULL;
    if (current) {
        if (ShiftTime() > 0) {
            listShifter = new cListShifter((cListShiftable*)this);
            listShifter->SetDirection(selectedFromTop);
            cView::AddAnimation(listShifter, true);
        } else if (FadeTime() > 0) {
            fader = new cFader((cFadable*)this);
            cView::AddAnimation(fader, true);
        }
    }
    if (wasCurrent) {
        if (ShiftTime() > 0) {
            listShifter = new cListShifter((cListShiftable*)this);
            listShifter->SetDirection(selectedFromTop);
            listShifter->SetShiftOut();
            cView::AddAnimation(listShifter, false);
        } else if (FadeTime() > 0) {
            fader = new cFader((cFadable*)this);
            fader->SetFadeOut();
            fader->SetHideWhenFinished();
            cView::AddAnimation(fader, false);
        }
    }
}

/******************************************************************
* cCurrentElement
******************************************************************/
cCurrentElement::cCurrentElement(void) {
    listX = 0;
    listY = 0;
    listWidth = 0;
    listHeight = 0;
    listNum = 0;
}

void cCurrentElement::SetListPosition(int x, int y, int width, int height, int num) {
    listX = x;
    listY = y;
    listWidth = width;
    listHeight = height;
    listNum = num;
}

void cCurrentElement::SetListTokens(skindesignerapi::cTokenContainer *tokenContainer) {
    tokenContainer->AddIntToken(0, listX - container.X());
    tokenContainer->AddIntToken(1, listY - container.Y());
    tokenContainer->AddIntToken(2, listWidth);
    tokenContainer->AddIntToken(3, listHeight);
    tokenContainer->AddIntToken(4, listNum);
}

/******************************************************************
* cLeMenuDefault
******************************************************************/
cLeMenuDefault::cLeMenuDefault(void) {
    text = NULL;
    colX = NULL;
    colWidths = NULL;
    plugName = NULL;
}

cLeMenuDefault::cLeMenuDefault(const cLeMenuDefault &other) : cListElement(other) {
    text = NULL;
    colX = NULL;
    colWidths = NULL;
    plugName = NULL;
}

cLeMenuDefault::~cLeMenuDefault(void) {

}

void cLeMenuDefault::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{column1}", (int)eLeMenuDefaultST::column1);
    tokenContainer->DefineStringToken("{column2}", (int)eLeMenuDefaultST::column2);
    tokenContainer->DefineStringToken("{column3}", (int)eLeMenuDefaultST::column3);
    tokenContainer->DefineStringToken("{column4}", (int)eLeMenuDefaultST::column4);
    tokenContainer->DefineStringToken("{column5}", (int)eLeMenuDefaultST::column5);
    tokenContainer->DefineStringToken("{column6}", (int)eLeMenuDefaultST::column6);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeMenuDefaultIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeMenuDefaultIT::current);
    tokenContainer->DefineIntToken("{separator}", (int)eLeMenuDefaultIT::separator);
    tokenContainer->DefineIntToken("{column2set}", (int)eLeMenuDefaultIT::column2set);
    tokenContainer->DefineIntToken("{column3set}", (int)eLeMenuDefaultIT::column3set);
    tokenContainer->DefineIntToken("{column4set}", (int)eLeMenuDefaultIT::column4set);
    tokenContainer->DefineIntToken("{column5set}", (int)eLeMenuDefaultIT::column5set);
    tokenContainer->DefineIntToken("{column6set}", (int)eLeMenuDefaultIT::column6set);
    tokenContainer->DefineIntToken("{column1x}", (int)eLeMenuDefaultIT::column1x);
    tokenContainer->DefineIntToken("{column2x}", (int)eLeMenuDefaultIT::column2x);
    tokenContainer->DefineIntToken("{column3x}", (int)eLeMenuDefaultIT::column3x);
    tokenContainer->DefineIntToken("{column4x}", (int)eLeMenuDefaultIT::column4x);
    tokenContainer->DefineIntToken("{column5x}", (int)eLeMenuDefaultIT::column5x);
    tokenContainer->DefineIntToken("{column6x}", (int)eLeMenuDefaultIT::column6x);
    tokenContainer->DefineIntToken("{column1width}", (int)eLeMenuDefaultIT::column1width);
    tokenContainer->DefineIntToken("{column2width}", (int)eLeMenuDefaultIT::column2width);
    tokenContainer->DefineIntToken("{column3width}", (int)eLeMenuDefaultIT::column3width);
    tokenContainer->DefineIntToken("{column4width}", (int)eLeMenuDefaultIT::column4width);
    tokenContainer->DefineIntToken("{column5width}", (int)eLeMenuDefaultIT::column5width);
    tokenContainer->DefineIntToken("{column6width}", (int)eLeMenuDefaultIT::column6width);
    tokenContainer->DefineIntToken("{column1pb}", (int)eLeMenuDefaultIT::column1pb);
    tokenContainer->DefineIntToken("{column2pb}", (int)eLeMenuDefaultIT::column2pb);
    tokenContainer->DefineIntToken("{column3pb}", (int)eLeMenuDefaultIT::column3pb);
    tokenContainer->DefineIntToken("{column4pb}", (int)eLeMenuDefaultIT::column4pb);
    tokenContainer->DefineIntToken("{column5pb}", (int)eLeMenuDefaultIT::column5pb);
    tokenContainer->DefineIntToken("{column6pb}", (int)eLeMenuDefaultIT::column6pb);
    tokenContainer->DefineIntToken("{column1pbtotalsize}", (int)eLeMenuDefaultIT::column1pbtotalsize);
    tokenContainer->DefineIntToken("{column2pbtotalsize}", (int)eLeMenuDefaultIT::column2pbtotalsize);
    tokenContainer->DefineIntToken("{column3pbtotalsize}", (int)eLeMenuDefaultIT::column3pbtotalsize);
    tokenContainer->DefineIntToken("{column4pbtotalsize}", (int)eLeMenuDefaultIT::column4pbtotalsize);
    tokenContainer->DefineIntToken("{column5pbtotalsize}", (int)eLeMenuDefaultIT::column5pbtotalsize);
    tokenContainer->DefineIntToken("{column6pbtotalsize}", (int)eLeMenuDefaultIT::column6pbtotalsize);
    tokenContainer->DefineIntToken("{column1pbsize}", (int)eLeMenuDefaultIT::column1pbsize);
    tokenContainer->DefineIntToken("{column2pbsize}", (int)eLeMenuDefaultIT::column2pbsize);
    tokenContainer->DefineIntToken("{column3pbsize}", (int)eLeMenuDefaultIT::column3pbsize);
    tokenContainer->DefineIntToken("{column4pbsize}", (int)eLeMenuDefaultIT::column4pbsize);
    tokenContainer->DefineIntToken("{column5pbsize}", (int)eLeMenuDefaultIT::column5pbsize);
    tokenContainer->DefineIntToken("{column6pbsize}", (int)eLeMenuDefaultIT::column6pbsize);
    tokenContainer->DefineIntToken("{unknown}", (int)eLeMenuDefaultIT::unknown);
    tokenContainer->DefineIntToken("{channeledit}", (int)eLeMenuDefaultIT::channeledit);
    tokenContainer->DefineIntToken("{timeredit}", (int)eLeMenuDefaultIT::timeredit);
    tokenContainer->DefineIntToken("{recordinginfo}", (int)eLeMenuDefaultIT::recordinginfo);
    tokenContainer->DefineIntToken("{recordingedit}", (int)eLeMenuDefaultIT::recordingedit);
    tokenContainer->DefineIntToken("{setup}", (int)eLeMenuDefaultIT::setup);
    tokenContainer->DefineIntToken("{commands}", (int)eLeMenuDefaultIT::commands);
    tokenContainer->DefineIntToken("{folder}", (int)eLeMenuDefaultIT::folder);
    tokenContainer->DefineIntToken("{cam}", (int)eLeMenuDefaultIT::cam);
    tokenContainer->DefineIntToken("{fritzbox}", (int)eLeMenuDefaultIT::fritzbox);
    tokenContainer->DefineIntToken("{systeminfo}", (int)eLeMenuDefaultIT::systeminfo);
    tokenContainer->DefineIntToken("{mailbox}", (int)eLeMenuDefaultIT::mailbox);
    tokenContainer->DefineIntToken("{neutrinoepg}", (int)eLeMenuDefaultIT::neutrinoepg);
    tokenContainer->DefineIntToken("{remotetimers}", (int)eLeMenuDefaultIT::remotetimers);
    tokenContainer->DefineIntToken("{zaphistory}", (int)eLeMenuDefaultIT::zaphistory);
    tokenContainer->DefineIntToken("{remoteosd}", (int)eLeMenuDefaultIT::remoteosd);
    tokenContainer->DefineIntToken("{filebrowser}", (int)eLeMenuDefaultIT::filebrowser);
    tokenContainer->DefineIntToken("{epgsearch}", (int)eLeMenuDefaultIT::epgsearch);
    InheritTokenContainer();
}

void cLeMenuDefault::SetListInfo(int *colX, int *colWidths) {
    this->colX = colX;
    this->colWidths = colWidths;
}

void cLeMenuDefault::SetText(const char *text) {
    dirty = true;
    free(this->text);
    this->text = strdup(text);
}

bool cLeMenuDefault::Parse(bool forced) {
    if (!dirty)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeMenuDefaultIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeMenuDefaultIT::current, current);
    tokenContainer->AddIntToken((int)eLeMenuDefaultIT::separator, !selectable);
    for (int i = 0; i < MAX_TABS; i++) {
        bool colUsed = false;
        const char *s = GetTabbedText(text, i);
        if (s) {
            tokenContainer->AddStringToken(i, s);
            colUsed = true;
            CheckProgressBar(s, i);
        }
        else
            tokenContainer->AddStringToken(i, "");
        if (i > 0)
            tokenContainer->AddIntToken(i + 2, colUsed);
        tokenContainer->AddIntToken(i + 8, colX[i]);
        tokenContainer->AddIntToken(i + 14, colWidths[i]);
    }
    SetMenuCategory();
    return true;
}

const char *cLeMenuDefault::GetTabbedText(const char *s, int tab) {
    if (!s)
        return NULL;
    static char buffer[1000];
    const char *a = s;
    const char *b = strchrnul(a, '\t');
    while (*b && tab-- > 0) {
        a = b + 1;
        b = strchrnul(a, '\t');
    }
    if (!*b)
        return (tab <= 0) ? a : NULL;
    unsigned int n = b - a;
    if (n >= sizeof(buffer))
        n = sizeof(buffer) - 1;
    strncpy(buffer, a, n);
    buffer[n] = 0;
    return buffer;
}

void cLeMenuDefault::SetMenuCategory(void) {
    switch (menuCat) {
        case mcUnknown:
        case mcUndefined:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::unknown, 1);
            break;
        case mcChannelEdit:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::channeledit, 1);
            break;
        case mcTimerEdit:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::timeredit, 1);
            break;
        case mcRecordingInfo:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::recordinginfo, 1);
            break;
        case mcRecordingEdit:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::recordingedit, 1);
            break;
        case mcPluginSetup:
        case mcSetup:
        case mcSetupOsd:
        case mcSetupEpg:
        case mcSetupDvb:
        case mcSetupLnb:
        case mcSetupCam:
        case mcSetupRecord:
        case mcSetupReplay:
        case mcSetupMisc:
        case mcSetupPlugins:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::setup, 1);
            return;
        case mcCommand:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::commands, 1);
            return;
        case mcFolder:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::folder, 1);
            break;
        case mcCam:
            tokenContainer->AddIntToken((int)eLeMenuDefaultIT::cam, 1);
            break;
        default:
            break;
    }

    if (!plugName) {
        return;
    }
    if (!strcmp(plugName, "fritzbox")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::fritzbox, 1);
    } else if (!strcmp(plugName, "systeminfo")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::systeminfo, 1);
    } else if (!strcmp(plugName, "mailbox")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::mailbox, 1);
    } else if (!strcmp(plugName, "neutrinoepg")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::neutrinoepg, 1);
    } else if (!strcmp(plugName, "remotetimers")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::remotetimers, 1);
    } else if (!strcmp(plugName, "zaphistory")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::zaphistory, 1);
    } else if (!strcmp(plugName, "remoteosd")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::remoteosd, 1);
    } else if (!strcmp(plugName, "filebrowser")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::filebrowser, 1);
    } else if (!strcmp(plugName, "epgsearch")) {
        tokenContainer->AddIntToken((int)eLeMenuDefaultIT::epgsearch, 1);
    }
}

void cLeMenuDefault::CheckProgressBar(const char *text, int tab) {
    const char *start = strchr(text, '[');
    if (!start)
        return;
    const char *stop = strchr(text, ']');
    if (!stop)
        return;
    int total = stop - start - 1;
    int len = 0;
    char *s = (char*)start + 1;
    while (*s && *s == '|') {
        len++;
        s++;
    }
    tokenContainer->AddIntToken(20 + tab, 1);
    tokenContainer->AddIntToken(26 + tab, total);
    tokenContainer->AddIntToken(32 + tab, len);
}

void cLeMenuDefault::Clear(bool forceClearBackground) {
    if (current && currentElement) {
        currentElement->Close();
    }
    cViewElement::Close();
    dirty = false;
}
/******************************************************************
* cVeMenuMain
******************************************************************/
cVeMenuMain::cVeMenuMain(void) {
    text = NULL;
    number = NULL;
    label = NULL;
}

cVeMenuMain::~cVeMenuMain(void) {
    free(text);
    free(number);
    free(label);
}

void cVeMenuMain::SplitText(void) {
    char *start = skipspace(text);
    bool found = false;
    bool doBreak = false;
    size_t i = 0;
    char *c = start;
    while (*c) {
        if (i==0) {
            //if text directly starts with nonnumeric, break
            if (!(*c >= '0' && *c <= '9')) {
                break;
            }
        }
        if (found) {
            //if current char is not a figure anymore, break
            if (!(*c >= '0' && *c <= '9')) {
                //there has to be a space after the menu item number 
                //plugins with figures in their name are eval :-)
                if (*c != ' ')
                    found = false;
                doBreak = true;
            }
        }
        if (*c >= '0' && *c <= '9') {
            found = true;
        }
        if (doBreak)
            break;
        if (i>4)
            break;
        c++;
        i++;
    }

    free(number);
    free(label);
    if (found) {
        number = (char*)malloc(i+1);
        memset(number, 0, i+1);
        strncpy(number, start, i);
    } else {
        number = (char*)malloc(2);
        memset(number, 0, 2);
        strncpy(number, "", 1);
    }
    label = strdup(skipspace(c));
}

/******************************************************************
* cLeMenuMain
******************************************************************/
cLeMenuMain::cLeMenuMain(void) {
    currentMain = NULL;
}

cLeMenuMain::cLeMenuMain(const cLeMenuMain &other) : cListElement(other) {
    text = NULL;
    number = NULL;
    label = NULL;
    currentMain = NULL;
}

cLeMenuMain::~cLeMenuMain(void) {
}

void cLeMenuMain::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{label}", (int)eLeMenuMainST::label);
    tokenContainer->DefineStringToken("{number}", (int)eLeMenuMainST::number);
    tokenContainer->DefineStringToken("{icon}", (int)eLeMenuMainST::icon);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeMenuMainIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeMenuMainIT::current);
    tokenContainer->DefineIntToken("{separator}", (int)eLeMenuMainIT::separator);
    InheritTokenContainer();
}

void cLeMenuMain::ClearCurrentElement(void) {
    if (wasCurrent && currentMain) {
        currentMain->Close();
    }
}

void cLeMenuMain::SetText(const char *text) {
    dirty = true;
    free(this->text);
    this->text = strdup(text);
}

bool cLeMenuMain::Parse(bool forced) {
    if (!dirty)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeMenuMainIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeMenuMainIT::current, current);
    tokenContainer->AddIntToken((int)eLeMenuMainIT::separator, !selectable);
    SplitText();
    tokenContainer->AddStringToken((int)eLeMenuMainST::number, number);
    tokenContainer->AddStringToken((int)eLeMenuMainST::label, label);
    tokenContainer->AddStringToken((int)eLeMenuMainST::icon, imgCache->GetIconName(label).c_str());
    return true;
}

void cLeMenuMain::RenderCurrent(void) {
    if (!currentMain)
        return;
    currentMain->SetText(text);
    currentMain->SetListPosition(container.X(), container.Y(), container.Width(), container.Height(), num);
    currentMain->Parse();
}

const char *cLeMenuMain::PluginName(void) {
    if (!label)
        return NULL;
    for (int i = 0; ; i++) {
        cPlugin *p = cPluginManager::GetPlugin(i);
        if (p) {
            const char *mainMenuEntry = p->MainMenuEntry();
            if (!mainMenuEntry)
                continue;
            if (!strcmp(mainMenuEntry, label)) {
                return p->Name();
            }
        } else
            break;
    }
    return NULL;
}

/******************************************************************
* cCeMenuMain
******************************************************************/
cCeMenuMain::cCeMenuMain(void) {

}

cCeMenuMain::~cCeMenuMain(void) {

}

void cCeMenuMain::SetText(const char *text) {
    dirty = true;
    free(this->text);
    this->text = strdup(text);
}

void cCeMenuMain::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{menuitemx}", (int)eCeMenuMainIT::menuitemx);
    tokenContainer->DefineIntToken("{menuitemy}", (int)eCeMenuMainIT::menuitemy);
    tokenContainer->DefineIntToken("{menuitemwidth}", (int)eCeMenuMainIT::menuitemwidth);
    tokenContainer->DefineIntToken("{menuitemheight}", (int)eCeMenuMainIT::menuitemheight);
    tokenContainer->DefineIntToken("{numitem}", (int)eCeMenuMainIT::numitem);
    tokenContainer->DefineStringToken("{label}", (int)eCeMenuMainST::label);
    tokenContainer->DefineStringToken("{number}", (int)eCeMenuMainST::number);
    tokenContainer->DefineStringToken("{icon}", (int)eCeMenuMainST::icon);
    InheritTokenContainer();
}

bool cCeMenuMain::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;
    tokenContainer->Clear();
    SplitText();
    SetListTokens(tokenContainer);
    tokenContainer->AddStringToken((int)eLeMenuMainST::number, number);
    tokenContainer->AddStringToken((int)eLeMenuMainST::label, label);
    tokenContainer->AddStringToken((int)eLeMenuMainST::icon, imgCache->GetIconName(label).c_str());
    return true;
}

/******************************************************************
* cVeMenuSchedules
******************************************************************/
cVeMenuSchedules::cVeMenuSchedules(void) {
    event = NULL;
    channel = NULL;
    withDate = false;
    timerMatch = tmNone;
    epgSearchFav = false;
}

/******************************************************************
* cLeMenuSchedules
******************************************************************/
cLeMenuSchedules::cLeMenuSchedules(void) {
    currentSchedules = NULL;
    menuCat = mcUnknown;
}

cLeMenuSchedules::cLeMenuSchedules(const cLeMenuSchedules &other) : cListElement(other) {
    currentSchedules = NULL;
}

cLeMenuSchedules::~cLeMenuSchedules(void) {
}

void cLeMenuSchedules::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eLeMenuSchedulesST::title);
    tokenContainer->DefineStringToken("{shorttext}", (int)eLeMenuSchedulesST::shorttext);
    tokenContainer->DefineStringToken("{start}", (int)eLeMenuSchedulesST::start);
    tokenContainer->DefineStringToken("{stop}", (int)eLeMenuSchedulesST::stop);
    tokenContainer->DefineStringToken("{day}", (int)eLeMenuSchedulesST::day);
    tokenContainer->DefineStringToken("{date}", (int)eLeMenuSchedulesST::date);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eLeMenuSchedulesST::durationminutes);
    tokenContainer->DefineStringToken("{channelname}", (int)eLeMenuSchedulesST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eLeMenuSchedulesST::channelid);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeMenuSchedulesIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeMenuSchedulesIT::current);
    tokenContainer->DefineIntToken("{separator}", (int)eLeMenuSchedulesIT::separator);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eLeMenuSchedulesIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eLeMenuSchedulesIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eLeMenuSchedulesIT::year);
    tokenContainer->DefineIntToken("{istoday}", (int)eLeMenuSchedulesIT::istoday);
    tokenContainer->DefineIntToken("{running}", (int)eLeMenuSchedulesIT::running);
    tokenContainer->DefineIntToken("{elapsed}", (int)eLeMenuSchedulesIT::elapsed);
    tokenContainer->DefineIntToken("{startsin}", (int)eLeMenuSchedulesIT::startsin);
    tokenContainer->DefineIntToken("{duration}", (int)eLeMenuSchedulesIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eLeMenuSchedulesIT::durationhours);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eLeMenuSchedulesIT::channelnumber);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eLeMenuSchedulesIT::channellogoexists);
    tokenContainer->DefineIntToken("{whatson}", (int)eLeMenuSchedulesIT::whatson);
    tokenContainer->DefineIntToken("{whatsonnow}", (int)eLeMenuSchedulesIT::whatsonnow);
    tokenContainer->DefineIntToken("{whatsonnext}", (int)eLeMenuSchedulesIT::whatsonnext);
    tokenContainer->DefineIntToken("{whatsonfavorites}", (int)eLeMenuSchedulesIT::whatsonfavorites);
    tokenContainer->DefineIntToken("{timerpartitial}", (int)eLeMenuSchedulesIT::timerpartitial);
    tokenContainer->DefineIntToken("{timerfull}", (int)eLeMenuSchedulesIT::timerfull);
    tokenContainer->DefineIntToken("{hasVPS}", (int)eLeMenuSchedulesIT::hasVPS);
    InheritTokenContainer();
}

void cLeMenuSchedules::ClearCurrentElement(void) {
    if (wasCurrent && currentSchedules) {
        currentSchedules->Close();
    }
}

void cLeMenuSchedules::Set(const cEvent *event, const cChannel *channel, bool withDate, eTimerMatch timerMatch) {
    dirty = true;
    this->event = event;
    this->channel = channel;
    this->withDate = withDate;
    this->timerMatch = timerMatch;
}

bool cLeMenuSchedules::Parse(bool forced) {
    if (!dirty)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::current, current);
    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::separator, !selectable);

    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::whatson, (menuCat == mcSchedule && !epgSearchFav) ? true : false);
    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::whatsonnow, menuCat == mcScheduleNow ? true : false);
    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::whatsonnext, menuCat == mcScheduleNext ? true : false);
    tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::whatsonfavorites, epgSearchFav);

    if (timerMatch == tmFull) {
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerpartitial, false);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerfull, true);
    } else if (timerMatch == tmPartial) {
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerpartitial, true);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerfull, false);
    } else {
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerpartitial, false);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerfull, false);
    }

    if (event) {
        if (selectable) {
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::title, event->Title());
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::shorttext, event->ShortText());
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::start, *(event->GetTimeString()));
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::stop, *(event->GetEndTimeString()));
            time_t startTime = event->StartTime();
            struct tm *sStartTime = localtime(&startTime);
            int day = sStartTime->tm_mday;
            int month = sStartTime->tm_mon;
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::day, *WeekDayName(startTime));
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::date, *ShortDateString(startTime));
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::year, sStartTime->tm_year + 1900);
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::daynumeric, day);
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::month, month + 1);

            bool isRunning = false;
            time_t now = time(NULL);
            if ((now >= event->StartTime()) && (now <= event->EndTime()))
                isRunning = true;
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::running, isRunning);
            if (isRunning) {
                tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::elapsed, (now - event->StartTime())/60);
                tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::startsin, 0);
            } else {
                tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::elapsed, 0);
                tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::startsin, (event->StartTime() - now)/60);
            }
            struct tm *sNow = localtime(&now);
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::istoday, (day == sNow->tm_mday && month == sNow->tm_mon) ? true : false);
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::duration, event->Duration() / 60);
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::durationhours, event->Duration() / 3600);
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::durationminutes, *cString::sprintf("%.2d", (event->Duration() / 60)%60));
            tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::hasVPS, (bool)event->Vps());
        } else {
            char *sep = ParseSeparator(event->Title());
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::title, sep);
            free(sep);
        }
    } else if (!channel) {
        //Hack für komischen patch von jinx
        //stringTokens.insert(pair<string,string>("title", dayseparator));        
    }
    if (channel) {
        tokenContainer->AddStringToken((int)eLeMenuSchedulesST::channelname, channel->Name());
        cString channelID = channel->GetChannelID().ToString();
        tokenContainer->AddStringToken((int)eLeMenuSchedulesST::channelid, *channelID);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::channelnumber, channel->Number());
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::channellogoexists, imgCache->LogoExists(*channelID));
        if (!event && !selectable) {
            tokenContainer->AddStringToken((int)eLeMenuSchedulesST::title, channel->Name());
        }
    }
    return true;
}

void cLeMenuSchedules::RenderCurrent(void) {
    if (!currentSchedules)
        return;
    currentSchedules->Set(event, channel, withDate, timerMatch, menuCat);
    currentSchedules->SetEpgSearchFav(epgSearchFav);
    currentSchedules->SetListPosition(container.X(), container.Y(), container.Width(), container.Height(), num);
    currentSchedules->Parse();
}

/******************************************************************
* cCeMenuSchedules
******************************************************************/
cCeMenuSchedules::cCeMenuSchedules(void) {
    schedulesIndex = -1;
}

cCeMenuSchedules::~cCeMenuSchedules(void) {
}

void cCeMenuSchedules::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eCeMenuSchedulesST::title);
    tokenContainer->DefineStringToken("{shorttext}", (int)eCeMenuSchedulesST::shorttext);
    tokenContainer->DefineStringToken("{description}", (int)eCeMenuSchedulesST::description);
    tokenContainer->DefineStringToken("{start}", (int)eCeMenuSchedulesST::start);
    tokenContainer->DefineStringToken("{stop}", (int)eCeMenuSchedulesST::stop);
    tokenContainer->DefineStringToken("{day}", (int)eCeMenuSchedulesST::day);
    tokenContainer->DefineStringToken("{date}", (int)eCeMenuSchedulesST::date);
    tokenContainer->DefineStringToken("{channelname}", (int)eCeMenuSchedulesST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eCeMenuSchedulesST::channelid);
    tokenContainer->DefineStringToken("{posterpath}", (int)eCeMenuSchedulesST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eCeMenuSchedulesST::bannerpath);
    tokenContainer->DefineIntToken("{menuitemx}", (int)eCeMenuSchedulesIT::menuitemx);
    tokenContainer->DefineIntToken("{menuitemy}", (int)eCeMenuSchedulesIT::menuitemy);
    tokenContainer->DefineIntToken("{menuitemwidth}", (int)eCeMenuSchedulesIT::menuitemwidth);
    tokenContainer->DefineIntToken("{menuitemheight}", (int)eCeMenuSchedulesIT::menuitemheight);
    tokenContainer->DefineIntToken("{numitem}", (int)eCeMenuSchedulesIT::numitem);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eCeMenuSchedulesIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eCeMenuSchedulesIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eCeMenuSchedulesIT::year);
    tokenContainer->DefineIntToken("{running}", (int)eCeMenuSchedulesIT::running);
    tokenContainer->DefineIntToken("{elapsed}", (int)eCeMenuSchedulesIT::elapsed);
    tokenContainer->DefineIntToken("{duration}", (int)eCeMenuSchedulesIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eCeMenuSchedulesIT::durationhours);
    tokenContainer->DefineIntToken("{durationminutes}", (int)eCeMenuSchedulesIT::durationminutes);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eCeMenuSchedulesIT::channelnumber);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eCeMenuSchedulesIT::channellogoexists);
    tokenContainer->DefineIntToken("{hasposter}", (int)eCeMenuSchedulesIT::hasposter);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eCeMenuSchedulesIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eCeMenuSchedulesIT::posterheight);
    tokenContainer->DefineIntToken("{hasbanner}", (int)eCeMenuSchedulesIT::hasbanner);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eCeMenuSchedulesIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eCeMenuSchedulesIT::bannerheight);
    tokenContainer->DefineIntToken("{whatson}", (int)eCeMenuSchedulesIT::whatson);
    tokenContainer->DefineIntToken("{whatsonnow}", (int)eCeMenuSchedulesIT::whatsonnow);
    tokenContainer->DefineIntToken("{whatsonnext}", (int)eCeMenuSchedulesIT::whatsonnext);
    tokenContainer->DefineIntToken("{whatsonfavorites}", (int)eCeMenuSchedulesIT::whatsonfavorites);
    tokenContainer->DefineIntToken("{timerpartitial}", (int)eCeMenuSchedulesIT::timerpartitial);
    tokenContainer->DefineIntToken("{timerfull}", (int)eCeMenuSchedulesIT::timerfull);
    tokenContainer->DefineLoopToken("{schedule[title]}", (int)eCeMenuSchedulesLT::title);
    tokenContainer->DefineLoopToken("{schedule[shorttext]}", (int)eCeMenuSchedulesLT::shorttext);
    tokenContainer->DefineLoopToken("{schedule[start]}", (int)eCeMenuSchedulesLT::start);
    tokenContainer->DefineLoopToken("{schedule[stop]}", (int)eCeMenuSchedulesLT::stop);
    schedulesIndex = tokenContainer->LoopIndex("schedule");
    InheritTokenContainer();    
}

void cCeMenuSchedules::Set(const cEvent *event, const cChannel *channel, bool withDate, eTimerMatch timerMatch, eMenuCategory menuCat) {
    dirty = true;
    this->menuCat = menuCat;
    this->event = event;
    this->channel = channel;
    this->withDate = withDate;
    this->timerMatch = timerMatch;
}

bool cCeMenuSchedules::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;
    tokenContainer->Clear();
    SetListTokens(tokenContainer);
    tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::whatson, (menuCat == mcSchedule && !epgSearchFav) ? true : false);
    tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::whatsonnow, menuCat == mcScheduleNow ? true : false);
    tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::whatsonnext, menuCat == mcScheduleNext ? true : false);
    tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::whatsonfavorites, epgSearchFav);
    if (timerMatch == tmFull) {
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerpartitial, false);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerfull, true);
    } else if (timerMatch == tmPartial) {
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerpartitial, true);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerfull, false);
    } else {
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerpartitial, false);
        tokenContainer->AddIntToken((int)eLeMenuSchedulesIT::timerfull, false);
    }

    if (event) {
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::title, event->Title());
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::shorttext, event->ShortText());
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::description, event->Description());
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::start, *(event->GetTimeString()));
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::stop, *(event->GetEndTimeString()));
        time_t startTime = event->StartTime();
        struct tm * sStartTime = localtime(&startTime);
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::day, *WeekDayName(startTime));
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::date, *ShortDateString(startTime));
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::year, sStartTime->tm_year + 1900);
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::daynumeric, sStartTime->tm_mday);
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::month, sStartTime->tm_mon+1);
        bool isRunning = false;
        time_t now = time(NULL);
        if ((now >= event->StartTime()) && (now <= event->EndTime()))
            isRunning = true;
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::running, isRunning);
        if (isRunning) {
            tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::elapsed, (now - event->StartTime())/60);
        } else {
            tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::elapsed, 0);
        }
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::duration, event->Duration() / 60);
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::durationhours, event->Duration() / 3600);
        if (LoadFullScrapInfo(event, NULL))
            SetScraperPosterBanner(tokenContainer);
    }
    if (channel) {
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::channelname, channel->Name());
        cString channelID = channel->GetChannelID().ToString();
        tokenContainer->AddStringToken((int)eCeMenuSchedulesST::channelid, *channelID);
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::channelnumber, channel->Number());
        tokenContainer->AddIntToken((int)eCeMenuSchedulesIT::channellogoexists, imgCache->LogoExists(*channelID));
    }

    if (menuCat == mcScheduleNow || menuCat == mcScheduleNext) {
        int eventsAvailable = 0;
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_SCHEDULES_READ;
        const cSchedules* schedules = Schedules;
#else
        cSchedulesLock schedulesLock;
        const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif
        const cSchedule *schedule = schedules->GetSchedule(channel);
        if (schedule) {
            for (const cEvent *e = schedule->GetPresentEvent(); e; e = schedule->Events()->Next(e)) {
                eventsAvailable++;
                if (eventsAvailable == 10)
                    break;
            }
        }
        vector<int> loopInfo;
        loopInfo.push_back(eventsAvailable);
        tokenContainer->CreateLoopTokenContainer(&loopInfo);        

        if (schedule) {
            int num = 0;
            for (const cEvent *e = schedule->GetPresentEvent(); e; e = schedule->Events()->Next(e)) {
                if (num < 2) {
                    num++;
                    continue;
                }
                tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::title, e->Title());
                tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::shorttext, e->ShortText());
                tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::start, *(e->GetTimeString()));
                tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::stop, *(e->GetEndTimeString()));
                num++;
                if (num > 11)
                    break;
            }            
        }
    }
    return true;
}

/******************************************************************
* cLeMenuChannels
******************************************************************/
cLeMenuChannels::cLeMenuChannels(void) {
    currentChannel = NULL;
    channel = NULL;
    withProvider = false;
}

cLeMenuChannels::cLeMenuChannels(const cLeMenuChannels &other) : cListElement(other) {
    currentChannel = NULL;
    channel = NULL;
    withProvider = false;
}

cLeMenuChannels::~cLeMenuChannels(void) {
}

void cLeMenuChannels::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eLeMenuChannelsST::name);
    tokenContainer->DefineStringToken("{channelid}", (int)eLeMenuChannelsST::channelid);
    tokenContainer->DefineStringToken("{source}", (int)eLeMenuChannelsST::source);
    tokenContainer->DefineStringToken("{sourcedescription}", (int)eLeMenuChannelsST::sourcedescription);
    tokenContainer->DefineStringToken("{position}", (int)eLeMenuChannelsST::position);
    tokenContainer->DefineStringToken("{presenteventtitle}", (int)eLeMenuChannelsST::presenteventtitle);
    tokenContainer->DefineStringToken("{presenteventstart}", (int)eLeMenuChannelsST::presenteventstart);
    tokenContainer->DefineStringToken("{presenteventstop}", (int)eLeMenuChannelsST::presenteventstop);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeMenuChannelsIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeMenuChannelsIT::current);
    tokenContainer->DefineIntToken("{separator}", (int)eLeMenuChannelsIT::separator);
    tokenContainer->DefineIntToken("{number}", (int)eLeMenuChannelsIT::number);
    tokenContainer->DefineIntToken("{transponder}", (int)eLeMenuChannelsIT::transponder);
    tokenContainer->DefineIntToken("{frequency}", (int)eLeMenuChannelsIT::frequency);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eLeMenuChannelsIT::channellogoexists);
    tokenContainer->DefineIntToken("{isAtsc}", (int)eLeMenuChannelsIT::isAtsc);
    tokenContainer->DefineIntToken("{isCable}", (int)eLeMenuChannelsIT::isCable);
    tokenContainer->DefineIntToken("{isSat}", (int)eLeMenuChannelsIT::isSat);
    tokenContainer->DefineIntToken("{isTerr}", (int)eLeMenuChannelsIT::isTerr);
    tokenContainer->DefineIntToken("{isEncrypted}", (int)eLeMenuChannelsIT::isEncrypted);
    InheritTokenContainer();    
}

void cLeMenuChannels::ClearCurrentElement(void) {
    if (wasCurrent && currentChannel) {
        currentChannel->Close();
    }
}

void cLeMenuChannels::Set(const cChannel *channel, bool withProvider) {
    dirty = true;
    this->channel = channel;
    this->withProvider = withProvider;
}

bool cLeMenuChannels::Parse(bool forced) {
    if (!dirty)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::current, current);
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::separator, !selectable);
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::number, channel->Number());
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::transponder, channel->Transponder());
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::frequency, channel->Frequency());
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::isEncrypted, channel->Ca());

    tokenContainer->AddStringToken((int)eLeMenuChannelsST::name, channel->Name());
    cString channelID = channel->GetChannelID().ToString();
    tokenContainer->AddStringToken((int)eLeMenuChannelsST::channelid, *channelID);
    tokenContainer->AddIntToken((int)eLeMenuChannelsIT::channellogoexists, imgCache->LogoExists(*channelID));

    //Channel Source Information
    const cSource *source = Sources.Get(channel->Source());
    if (source) {
        tokenContainer->AddStringToken((int)eLeMenuChannelsST::source, *cSource::ToString(source->Code()));
        tokenContainer->AddStringToken((int)eLeMenuChannelsST::sourcedescription, source->Description());
        tokenContainer->AddIntToken((int)eLeMenuChannelsIT::isAtsc, source->IsAtsc(source->Code()));
        tokenContainer->AddIntToken((int)eLeMenuChannelsIT::isCable, source->IsCable(source->Code()));
        tokenContainer->AddIntToken((int)eLeMenuChannelsIT::isSat, source->IsSat(source->Code()));
        tokenContainer->AddIntToken((int)eLeMenuChannelsIT::isTerr, source->IsTerr(source->Code()));
    }

    //current schedule
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_SCHEDULES_READ;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif
    const cSchedule *schedule = schedules->GetSchedule(channel);
    if (schedule) {
        const cEvent *presentEvent = schedule->GetPresentEvent();
        if (presentEvent) {
            tokenContainer->AddStringToken((int)eLeMenuChannelsST::presenteventtitle, presentEvent->Title());
            tokenContainer->AddStringToken((int)eLeMenuChannelsST::presenteventstart, *presentEvent->GetTimeString());
            tokenContainer->AddStringToken((int)eLeMenuChannelsST::presenteventstop, *presentEvent->GetEndTimeString());
        }
    }
    return true;
}

void cLeMenuChannels::RenderCurrent(void) {
    if (!currentChannel)
        return;
    currentChannel->Set(channel, withProvider);
    currentChannel->SetListPosition(container.X(), container.Y(), container.Width(), container.Height(), num);
    currentChannel->Parse();
}

/******************************************************************
* cCeMenuChannels
******************************************************************/
cCeMenuChannels::cCeMenuChannels(void) {
    schedulesIndex = -1;
    channel = NULL;
    withProvider = false;
}

cCeMenuChannels::~cCeMenuChannels(void) {
}

void cCeMenuChannels::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eCeMenuChannelsST::name);
    tokenContainer->DefineStringToken("{channelid}", (int)eCeMenuChannelsST::channelid);
    tokenContainer->DefineStringToken("{source}", (int)eCeMenuChannelsST::source);
    tokenContainer->DefineStringToken("{sourcedescription}", (int)eCeMenuChannelsST::sourcedescription);
    tokenContainer->DefineStringToken("{position}", (int)eCeMenuChannelsST::position);
    tokenContainer->DefineStringToken("{presenteventtitle}", (int)eCeMenuChannelsST::presenteventtitle);
    tokenContainer->DefineStringToken("{presenteventstart}", (int)eCeMenuChannelsST::presenteventstart);
    tokenContainer->DefineStringToken("{presenteventstop}", (int)eCeMenuChannelsST::presenteventstop);
    tokenContainer->DefineStringToken("{presenteventshorttext}", (int)eCeMenuChannelsST::presenteventshorttext);
    tokenContainer->DefineStringToken("{presenteventdescription}", (int)eCeMenuChannelsST::presenteventdescription);
    tokenContainer->DefineStringToken("{presenteventday}", (int)eCeMenuChannelsST::presenteventday);
    tokenContainer->DefineStringToken("{presenteventdate}", (int)eCeMenuChannelsST::presenteventdate);
    tokenContainer->DefineStringToken("{presenteventdurationminutes}", (int)eCeMenuChannelsST::presenteventdurationminutes);
    tokenContainer->DefineStringToken("{posterpath}", (int)eCeMenuChannelsST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eCeMenuChannelsST::bannerpath);
    tokenContainer->DefineStringToken("{nexteventtitle}", (int)eCeMenuChannelsST::nexteventtitle);
    tokenContainer->DefineStringToken("{nexteventstart}", (int)eCeMenuChannelsST::nexteventstart);
    tokenContainer->DefineStringToken("{nexteventstop}", (int)eCeMenuChannelsST::nexteventstop);
    tokenContainer->DefineStringToken("{nexteventshorttext}", (int)eCeMenuChannelsST::nexteventshorttext);
    tokenContainer->DefineStringToken("{nexteventdescription}", (int)eCeMenuChannelsST::nexteventdescription);
    tokenContainer->DefineStringToken("{nexteventdurationminutes}", (int)eCeMenuChannelsST::nexteventdurationminutes);
    tokenContainer->DefineStringToken("{nexteventday}", (int)eCeMenuChannelsST::nexteventday);
    tokenContainer->DefineStringToken("{nexteventdate}", (int)eCeMenuChannelsST::nexteventdate);
    tokenContainer->DefineIntToken("{menuitemx}", (int)eCeMenuChannelsIT::menuitemx);
    tokenContainer->DefineIntToken("{menuitemy}", (int)eCeMenuChannelsIT::menuitemy);
    tokenContainer->DefineIntToken("{menuitemwidth}", (int)eCeMenuChannelsIT::menuitemwidth);
    tokenContainer->DefineIntToken("{menuitemheight}", (int)eCeMenuChannelsIT::menuitemheight);
    tokenContainer->DefineIntToken("{numitem}", (int)eCeMenuChannelsIT::numitem);
    tokenContainer->DefineIntToken("{number}", (int)eCeMenuChannelsIT::number);
    tokenContainer->DefineIntToken("{transponder}", (int)eCeMenuChannelsIT::transponder);
    tokenContainer->DefineIntToken("{frequency}", (int)eCeMenuChannelsIT::frequency);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eCeMenuChannelsIT::channellogoexists);
    tokenContainer->DefineIntToken("{isAtsc}", (int)eCeMenuChannelsIT::isAtsc);
    tokenContainer->DefineIntToken("{isCable}", (int)eCeMenuChannelsIT::isCable);
    tokenContainer->DefineIntToken("{isSat}", (int)eCeMenuChannelsIT::isSat);
    tokenContainer->DefineIntToken("{isTerr}", (int)eCeMenuChannelsIT::isTerr);
    tokenContainer->DefineIntToken("{isEncrypted}", (int)eCeMenuChannelsIT::isEncrypted);
    tokenContainer->DefineIntToken("{presenteventelapsed}", (int)eCeMenuChannelsIT::presenteventelapsed);
    tokenContainer->DefineIntToken("{presenteventduration}", (int)eCeMenuChannelsIT::presenteventduration);
    tokenContainer->DefineIntToken("{presenteventdurationhours}", (int)eCeMenuChannelsIT::presenteventdurationhours);
    tokenContainer->DefineIntToken("{hasposter}", (int)eCeMenuChannelsIT::hasposter);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eCeMenuChannelsIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eCeMenuChannelsIT::posterheight);
    tokenContainer->DefineIntToken("{hasbanner}", (int)eCeMenuChannelsIT::hasbanner);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eCeMenuChannelsIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eCeMenuChannelsIT::bannerheight);
    tokenContainer->DefineIntToken("{nexteventduration}", (int)eCeMenuChannelsIT::nexteventduration);
    tokenContainer->DefineIntToken("{nexteventdurationhours}", (int)eCeMenuChannelsIT::nexteventdurationhours);
    tokenContainer->DefineLoopToken("{schedule[title]}", (int)eCeMenuChannelsLT::title);
    tokenContainer->DefineLoopToken("{schedule[shorttext]}", (int)eCeMenuChannelsLT::shorttext);
    tokenContainer->DefineLoopToken("{schedule[start]}", (int)eCeMenuChannelsLT::start);
    tokenContainer->DefineLoopToken("{schedule[stop]}", (int)eCeMenuChannelsLT::stop);
    schedulesIndex = tokenContainer->LoopIndex("schedule");
    InheritTokenContainer();
}

void cCeMenuChannels::Set(const cChannel *channel, bool withProvider) {
    dirty = true;
    this->channel = channel;
    this->withProvider = withProvider;
}

bool cCeMenuChannels::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;

    tokenContainer->Clear();

    SetListTokens(tokenContainer);

    tokenContainer->AddIntToken((int)eCeMenuChannelsIT::number, channel->Number());
    tokenContainer->AddIntToken((int)eCeMenuChannelsIT::transponder, channel->Transponder());
    tokenContainer->AddIntToken((int)eCeMenuChannelsIT::frequency, channel->Frequency());
    tokenContainer->AddIntToken((int)eCeMenuChannelsIT::isEncrypted, channel->Ca());

    tokenContainer->AddStringToken((int)eCeMenuChannelsST::name, channel->Name());
    cString channelID = channel->GetChannelID().ToString();
    tokenContainer->AddStringToken((int)eCeMenuChannelsST::channelid, *channelID);
    tokenContainer->AddIntToken((int)eCeMenuChannelsIT::channellogoexists, imgCache->LogoExists(*channelID));

    //Channel Source Information
    const cSource *source = Sources.Get(channel->Source());
    if (source) {
        tokenContainer->AddStringToken((int)eCeMenuChannelsST::source, *cSource::ToString(source->Code()));
        tokenContainer->AddStringToken((int)eCeMenuChannelsST::sourcedescription, source->Description());
        tokenContainer->AddIntToken((int)eCeMenuChannelsIT::isAtsc, source->IsAtsc(source->Code()));
        tokenContainer->AddIntToken((int)eCeMenuChannelsIT::isCable, source->IsCable(source->Code()));
        tokenContainer->AddIntToken((int)eCeMenuChannelsIT::isSat, source->IsSat(source->Code()));
        tokenContainer->AddIntToken((int)eCeMenuChannelsIT::isTerr, source->IsTerr(source->Code()));
    }

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_SCHEDULES_READ;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif
    const cSchedule *schedule = schedules->GetSchedule(channel);
    if (schedule) {
        const cEvent *presentEvent = schedule->GetPresentEvent();
        if (presentEvent) {
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventtitle, presentEvent->Title());
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventshorttext, presentEvent->ShortText());
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventdescription, presentEvent->Description());
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventstart, *presentEvent->GetTimeString());
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventstop, *presentEvent->GetEndTimeString());
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventday, *WeekDayName(presentEvent->StartTime()));
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventdate, *ShortDateString(presentEvent->StartTime()));
            tokenContainer->AddIntToken((int)eCeMenuChannelsIT::presenteventelapsed, (time(0) - presentEvent->StartTime())/60);
            tokenContainer->AddIntToken((int)eCeMenuChannelsIT::presenteventduration, presentEvent->Duration() / 60);
            tokenContainer->AddIntToken((int)eCeMenuChannelsIT::presenteventdurationhours, presentEvent->Duration() / 3600);
            tokenContainer->AddStringToken((int)eCeMenuChannelsST::presenteventdurationminutes, *cString::sprintf("%.2d", (presentEvent->Duration() / 60)%60));
            if (LoadFullScrapInfo(presentEvent, NULL))
                SetScraperPosterBannerChannel(tokenContainer);
        }
        const cList<cEvent> *events = schedule->Events();
        if (events && presentEvent) {
            const cEvent *nextEvent = events->Next(presentEvent);
            if (nextEvent) {
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventtitle, nextEvent->Title());
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventshorttext, nextEvent->ShortText());
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventdescription, nextEvent->Description());
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventstart, *nextEvent->GetTimeString());
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventstop, *nextEvent->GetEndTimeString());
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventday, *WeekDayName(nextEvent->StartTime()));
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventdate, *ShortDateString(nextEvent->StartTime()));
                tokenContainer->AddIntToken((int)eCeMenuChannelsIT::nexteventduration, nextEvent->Duration() / 60);
                tokenContainer->AddIntToken((int)eCeMenuChannelsIT::nexteventdurationhours, nextEvent->Duration() / 3600);
                tokenContainer->AddStringToken((int)eCeMenuChannelsST::nexteventdurationminutes, *cString::sprintf("%.2d", (nextEvent->Duration() / 60)%60));
            }
        }
    }
    
    vector<int> loopInfo;
    int eventsAvailable = 0;
    if (schedule) {
        for (const cEvent *e = schedule->GetPresentEvent(); e; e = schedule->Events()->Next(e)) {
            eventsAvailable++;
            if (eventsAvailable == 12)
                break;
        }
    }
    eventsAvailable -= 2;
    if (eventsAvailable < 0)
        eventsAvailable = 0;

    loopInfo.push_back(eventsAvailable);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);        

    if (schedule && eventsAvailable > 0) {
        int num = 0;
        for (const cEvent *e = schedule->GetPresentEvent(); e; e = schedule->Events()->Next(e)) {
            if (num < 2) {
                num++;
                continue;
            }
            tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::title, e->Title());
            tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::shorttext, e->ShortText());
            tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::start, *(e->GetTimeString()));
            tokenContainer->AddLoopToken(schedulesIndex, num-2, (int)eCeMenuChannelsLT::stop, *(e->GetEndTimeString()));
            num++;
            if (num > 11)
                break;
        }            
    }
    return true;
}

/******************************************************************
* cLeMenuTimers
******************************************************************/
cLeMenuTimers::cLeMenuTimers(void) {
    currentTimer = NULL;
    timer = NULL;
}

cLeMenuTimers::cLeMenuTimers(const cLeMenuTimers &other) : cListElement(other) {
    currentTimer = NULL;
    timer = NULL;
}

cLeMenuTimers::~cLeMenuTimers(void) {
}

void cLeMenuTimers::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eLeMenuTimersST::title);
    tokenContainer->DefineStringToken("{timerstart}", (int)eLeMenuTimersST::timerstart);
    tokenContainer->DefineStringToken("{timerstop}", (int)eLeMenuTimersST::timerstop);
    tokenContainer->DefineStringToken("{day}", (int)eLeMenuTimersST::day);
    tokenContainer->DefineStringToken("{dayname}", (int)eLeMenuTimersST::dayname);
    tokenContainer->DefineStringToken("{monthname}", (int)eLeMenuTimersST::monthname);
    tokenContainer->DefineStringToken("{channelname}", (int)eLeMenuTimersST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eLeMenuTimersST::channelid);
    tokenContainer->DefineStringToken("{eventtitle}", (int)eLeMenuTimersST::eventtitle);
    tokenContainer->DefineStringToken("{eventstart}", (int)eLeMenuTimersST::eventstart);
    tokenContainer->DefineStringToken("{eventstop}", (int)eLeMenuTimersST::eventstop);
    tokenContainer->DefineStringToken("{state}", (int)eLeMenuTimersST::state);
    tokenContainer->DefineStringToken("{stateinfo}", (int)eLeMenuTimersST::stateinfo);
    tokenContainer->DefineStringToken("{action}", (int)eLeMenuTimersST::action);
    tokenContainer->DefineStringToken("{vdrname}", (int)eLeMenuTimersST::vdrname);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeMenuTimersIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeMenuTimersIT::current);
    tokenContainer->DefineIntToken("{separator}", (int)eLeMenuTimersIT::separator);
    tokenContainer->DefineIntToken("{month}", (int)eLeMenuTimersIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eLeMenuTimersIT::year);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eLeMenuTimersIT::channellogoexists);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eLeMenuTimersIT::channelnumber);
    tokenContainer->DefineIntToken("{flagactive}", (int)eLeMenuTimersIT::flagactive);
    tokenContainer->DefineIntToken("{flaginstant}", (int)eLeMenuTimersIT::flaginstant);
    tokenContainer->DefineIntToken("{flagvps}", (int)eLeMenuTimersIT::flagvps);
    tokenContainer->DefineIntToken("{flagrecording}", (int)eLeMenuTimersIT::flagrecording);
    tokenContainer->DefineIntToken("{flagpending}", (int)eLeMenuTimersIT::flagpending);
    tokenContainer->DefineIntToken("{isvdrrunning}", (int)eLeMenuTimersIT::isvdrrunning);
    tokenContainer->DefineIntToken("{isremote}", (int)eLeMenuTimersIT::isremote);
    InheritTokenContainer();    
}

void cLeMenuTimers::ClearCurrentElement(void) {
   if (wasCurrent && currentTimer) {
        currentTimer->Close();
    }
}

void cLeMenuTimers::Set(const cTimer *timer) {
    this->timer = timer;
    dirty = true;
}

bool cLeMenuTimers::Parse(bool forced) {
    if (!dirty)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::current, current);
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::separator, !selectable);
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::flagactive, timer->HasFlags(tfActive));
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::flaginstant, timer->HasFlags(tfInstant));
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::flagvps, timer->HasFlags(tfVps));
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::flagrecording, timer->Recording());
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::flagpending, timer->Pending());
    
    const char *file = Setup.FoldersInTimerMenu ? NULL : strrchr(timer->File(), FOLDERDELIMCHAR);
    if (file && strcmp(file + 1, TIMERMACRO_TITLE) && strcmp(file + 1, TIMERMACRO_EPISODE))
        file++;
    else
        file = timer->File();
    tokenContainer->AddStringToken((int)eLeMenuTimersST::title, file);
    tokenContainer->AddStringToken((int)eLeMenuTimersST::timerstart, *cString::sprintf("%02d:%02d", timer->Start() / 100, timer->Start() % 100));
    tokenContainer->AddStringToken((int)eLeMenuTimersST::timerstop, *cString::sprintf("%02d:%02d", timer->Stop() / 100, timer->Stop() % 100));
    
    cString day("");
    cString dayName("");
    if (timer->WeekDays())
        day = timer->PrintDay(0, timer->WeekDays(), false);
    else if (timer->Day() - time(NULL) < 28 * SECSINDAY) {
        day = itoa(timer->GetMDay(timer->Day()));
        dayName = WeekDayName(timer->Day());
    } else {
        struct tm tm_r;
        time_t Day = timer->Day();
        localtime_r(&Day, &tm_r);
        char buffer[16];
        strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
        day = buffer;
    }
    
    struct tm tm_r;
    time_t timerDate = timer->Day();
    localtime_r(&timerDate, &tm_r);
    char buffer[4];
    strftime(buffer, sizeof(buffer), "%m", &tm_r);
    int month = atoi(buffer);
    char buffer2[6];
    strftime(buffer2, sizeof(buffer2), "%b", &tm_r);
    char buffer3[6];
    strftime(buffer3, sizeof(buffer3), "%Y", &tm_r);
    int year = atoi(buffer3);

    tokenContainer->AddStringToken((int)eLeMenuTimersST::day, *day);
    tokenContainer->AddStringToken((int)eLeMenuTimersST::dayname, *dayName);
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::month, month);
    tokenContainer->AddStringToken((int)eLeMenuTimersST::monthname, buffer2);
    tokenContainer->AddIntToken((int)eLeMenuTimersIT::year, year);

    const cChannel *channel = timer->Channel();
    if (channel) {
        tokenContainer->AddStringToken((int)eLeMenuTimersST::channelname, channel->Name());
        cString channelID = channel->GetChannelID().ToString();
        tokenContainer->AddStringToken((int)eLeMenuTimersST::channelid, *channelID);
        tokenContainer->AddIntToken((int)eLeMenuTimersIT::channellogoexists, imgCache->LogoExists(*channelID));
        tokenContainer->AddIntToken((int)eLeMenuTimersIT::channelnumber, channel->Number());
    }

    const cEvent *event = timer->Event();
    if (event) {
        tokenContainer->AddStringToken((int)eLeMenuTimersST::eventtitle, event->Title());
        tokenContainer->AddStringToken((int)eLeMenuTimersST::eventstart, *event->GetTimeString());
        tokenContainer->AddStringToken((int)eLeMenuTimersST::eventstop, *event->GetEndTimeString());
    }

    cEpgTimer_Interface_V1* epgTimer;
    if (epgTimer = dynamic_cast<cEpgTimer_Interface_V1*>((cTimer*)timer)) {
        tokenContainer->AddIntToken((int)eLeMenuTimersIT::isvdrrunning, epgTimer->isVdrRunning());
        tokenContainer->AddIntToken((int)eLeMenuTimersIT::isremote, epgTimer->isRemote());
        stringstream state;
        state << epgTimer->State();
        tokenContainer->AddStringToken((int)eLeMenuTimersST::state, state.str().c_str());
        tokenContainer->AddStringToken((int)eLeMenuTimersST::stateinfo, epgTimer->StateInfo());
        tokenContainer->AddStringToken((int)eLeMenuTimersST::vdrname, epgTimer->VdrName());
        stringstream action;
        action << epgTimer->Action();
        tokenContainer->AddStringToken((int)eLeMenuTimersST::action, action.str().c_str());
    }

    return true;
}

void cLeMenuTimers::RenderCurrent(void) {
    if (!currentTimer)
        return;
    currentTimer->Set(timer);
    currentTimer->SetListPosition(container.X(), container.Y(), container.Width(), container.Height(), num);
    currentTimer->Parse();
}

/******************************************************************
* cCeMenuTimers
******************************************************************/
cCeMenuTimers::cCeMenuTimers(void) {
    timer = NULL;
}

cCeMenuTimers::~cCeMenuTimers(void) {

}

void cCeMenuTimers::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eCeMenuTimersST::title);
    tokenContainer->DefineStringToken("{timerstart}", (int)eCeMenuTimersST::timerstart);
    tokenContainer->DefineStringToken("{timerstop}", (int)eCeMenuTimersST::timerstop);
    tokenContainer->DefineStringToken("{day}", (int)eCeMenuTimersST::day);
    tokenContainer->DefineStringToken("{dayname}", (int)eCeMenuTimersST::dayname);
    tokenContainer->DefineStringToken("{monthname}", (int)eCeMenuTimersST::monthname);
    tokenContainer->DefineStringToken("{channelname}", (int)eCeMenuTimersST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eCeMenuTimersST::channelid);
    tokenContainer->DefineStringToken("{eventtitle}", (int)eCeMenuTimersST::eventtitle);
    tokenContainer->DefineStringToken("{eventstart}", (int)eCeMenuTimersST::eventstart);
    tokenContainer->DefineStringToken("{eventstop}", (int)eCeMenuTimersST::eventstop);
    tokenContainer->DefineStringToken("{eventshorttext}", (int)eCeMenuTimersST::eventshorttext);
    tokenContainer->DefineStringToken("{eventdescription}", (int)eCeMenuTimersST::eventdescription);
    tokenContainer->DefineStringToken("{posterpath}", (int)eCeMenuTimersST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eCeMenuTimersST::bannerpath);
    tokenContainer->DefineStringToken("{state}", (int)eCeMenuTimersST::state);
    tokenContainer->DefineStringToken("{stateinfo}", (int)eCeMenuTimersST::stateinfo);
    tokenContainer->DefineStringToken("{action}", (int)eCeMenuTimersST::action);
    tokenContainer->DefineStringToken("{vdrname}", (int)eCeMenuTimersST::vdrname);
    tokenContainer->DefineIntToken("{menuitemx}", (int)eCeMenuTimersIT::menuitemx);
    tokenContainer->DefineIntToken("{menuitemy}", (int)eCeMenuTimersIT::menuitemy);
    tokenContainer->DefineIntToken("{menuitemwidth}", (int)eCeMenuTimersIT::menuitemwidth);
    tokenContainer->DefineIntToken("{menuitemheight}", (int)eCeMenuTimersIT::menuitemheight);
    tokenContainer->DefineIntToken("{numitem}", (int)eCeMenuTimersIT::numitem);
    tokenContainer->DefineIntToken("{month}", (int)eCeMenuTimersIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eCeMenuTimersIT::year);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eCeMenuTimersIT::channellogoexists);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eCeMenuTimersIT::channelnumber);
    tokenContainer->DefineIntToken("{flagactive}", (int)eCeMenuTimersIT::flagactive);
    tokenContainer->DefineIntToken("{flaginstant}", (int)eCeMenuTimersIT::flaginstant);
    tokenContainer->DefineIntToken("{flagvps}", (int)eCeMenuTimersIT::flagvps);
    tokenContainer->DefineIntToken("{flagrecording}", (int)eCeMenuTimersIT::flagrecording);
    tokenContainer->DefineIntToken("{flagpending}", (int)eCeMenuTimersIT::flagpending);
    tokenContainer->DefineIntToken("{hasposter}", (int)eCeMenuTimersIT::hasposter);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eCeMenuTimersIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eCeMenuTimersIT::posterheight);
    tokenContainer->DefineIntToken("{hasbanner}", (int)eCeMenuTimersIT::hasbanner);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eCeMenuTimersIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eCeMenuTimersIT::bannerheight);
    tokenContainer->DefineIntToken("{isvdrrunning}", (int)eCeMenuTimersIT::isvdrrunning);
    tokenContainer->DefineIntToken("{isremote}", (int)eCeMenuTimersIT::isremote);
    InheritTokenContainer();
}

void cCeMenuTimers::Set(const cTimer *timer) {
    dirty = true;
    this->timer = timer;
}

bool cCeMenuTimers::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;

    tokenContainer->Clear();

    SetListTokens(tokenContainer);
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::flagactive, timer->HasFlags(tfActive));
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::flaginstant, timer->HasFlags(tfInstant));
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::flagvps, timer->HasFlags(tfVps));
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::flagrecording, timer->Recording());
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::flagpending, timer->Pending());
    
    const char *file = Setup.FoldersInTimerMenu ? NULL : strrchr(timer->File(), FOLDERDELIMCHAR);
    if (file && strcmp(file + 1, TIMERMACRO_TITLE) && strcmp(file + 1, TIMERMACRO_EPISODE))
        file++;
    else
        file = timer->File();
    tokenContainer->AddStringToken((int)eCeMenuTimersST::title, file);
    tokenContainer->AddStringToken((int)eCeMenuTimersST::timerstart, *cString::sprintf("%02d:%02d", timer->Start() / 100, timer->Start() % 100));
    tokenContainer->AddStringToken((int)eCeMenuTimersST::timerstop, *cString::sprintf("%02d:%02d", timer->Stop() / 100, timer->Stop() % 100));
    
    cString day("");
    cString dayName("");
    if (timer->WeekDays())
        day = timer->PrintDay(0, timer->WeekDays(), false);
    else if (timer->Day() - time(NULL) < 28 * SECSINDAY) {
        day = itoa(timer->GetMDay(timer->Day()));
        dayName = WeekDayName(timer->Day());
    } else {
        struct tm tm_r;
        time_t Day = timer->Day();
        localtime_r(&Day, &tm_r);
        char buffer[16];
        strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
        day = buffer;
    }
    
    struct tm tm_r;
    time_t timerDate = timer->Day();
    localtime_r(&timerDate, &tm_r);
    char buffer[4];
    strftime(buffer, sizeof(buffer), "%m", &tm_r);
    int month = atoi(buffer);
    char buffer2[6];
    strftime(buffer2, sizeof(buffer2), "%b", &tm_r);
    char buffer3[6];
    strftime(buffer3, sizeof(buffer3), "%Y", &tm_r);
    int year = atoi(buffer3);

    tokenContainer->AddStringToken((int)eCeMenuTimersST::day, *day);
    tokenContainer->AddStringToken((int)eCeMenuTimersST::dayname, *dayName);
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::month, month);
    tokenContainer->AddStringToken((int)eCeMenuTimersST::monthname, buffer2);
    tokenContainer->AddIntToken((int)eCeMenuTimersIT::year, year);

    const cChannel *channel = timer->Channel();
    if (channel) {
        tokenContainer->AddStringToken((int)eCeMenuTimersST::channelname, channel->Name());
        cString channelID = channel->GetChannelID().ToString();
        tokenContainer->AddStringToken((int)eCeMenuTimersST::channelid, *channelID);
        tokenContainer->AddIntToken((int)eCeMenuTimersIT::channellogoexists, imgCache->LogoExists(*channelID));
        tokenContainer->AddIntToken((int)eCeMenuTimersIT::channelnumber, channel->Number());
    }

    const cEvent *event = timer->Event();
    if (event) {
        tokenContainer->AddStringToken((int)eCeMenuTimersST::eventtitle, event->Title());
        tokenContainer->AddStringToken((int)eCeMenuTimersST::eventstart, *event->GetTimeString());
        tokenContainer->AddStringToken((int)eCeMenuTimersST::eventstop, *event->GetEndTimeString());
        tokenContainer->AddStringToken((int)eCeMenuTimersST::eventshorttext, event->ShortText());
        tokenContainer->AddStringToken((int)eCeMenuTimersST::eventdescription, event->Description());
        if (LoadFullScrapInfo(event, NULL))
            SetScraperPosterBannerTimer(tokenContainer);
    }

    cEpgTimer_Interface_V1* epgTimer;
    if (epgTimer = dynamic_cast<cEpgTimer_Interface_V1*>((cTimer*)timer)) {
        tokenContainer->AddIntToken((int)eCeMenuTimersIT::isvdrrunning, epgTimer->isVdrRunning());
        tokenContainer->AddIntToken((int)eCeMenuTimersIT::isremote, epgTimer->isRemote());
        stringstream state;
        state << epgTimer->State();
        tokenContainer->AddStringToken((int)eCeMenuTimersST::state, state.str().c_str());
        tokenContainer->AddStringToken((int)eCeMenuTimersST::stateinfo, epgTimer->StateInfo());
        tokenContainer->AddStringToken((int)eCeMenuTimersST::vdrname, epgTimer->VdrName());
        stringstream action;
        action << epgTimer->Action();
        tokenContainer->AddStringToken((int)eCeMenuTimersST::action, action.str().c_str());
    }

    return true;
}

/******************************************************************
* cLeMenuRecordings
******************************************************************/
cLeMenuRecordings::cLeMenuRecordings(void) {
    currentRecording = NULL;
    recording = NULL;
    level = 0;
    total = 0;
    New = 0;
}

cLeMenuRecordings::cLeMenuRecordings(const cLeMenuRecordings &other) : cListElement(other) {
    currentRecording = NULL;
    recording = NULL;
    level = 0;
    total = 0;
    New = 0;
}

cLeMenuRecordings::~cLeMenuRecordings(void) {
    delete recording;
}

void cLeMenuRecordings::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eLeMenuRecordingsST::name);
    tokenContainer->DefineStringToken("{epgname}", (int)eLeMenuRecordingsST::epgname);
    tokenContainer->DefineStringToken("{shorttext}", (int)eLeMenuRecordingsST::shorttext);
    tokenContainer->DefineStringToken("{description}", (int)eLeMenuRecordingsST::description);
    tokenContainer->DefineStringToken("{date}", (int)eLeMenuRecordingsST::date);
    tokenContainer->DefineStringToken("{time}", (int)eLeMenuRecordingsST::time);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eLeMenuRecordingsST::durationminutes);
    tokenContainer->DefineStringToken("{durationeventminutes}", (int)eLeMenuRecordingsST::durationeventminutes);
    tokenContainer->DefineStringToken("{thumbnailpath}", (int)eLeMenuRecordingsST::thumbnailpath);
    tokenContainer->DefineStringToken("{posterpath}", (int)eLeMenuRecordingsST::posterpath);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeMenuRecordingsIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeMenuRecordingsIT::current);
    tokenContainer->DefineIntToken("{new}", (int)eLeMenuRecordingsIT::isnew);
    tokenContainer->DefineIntToken("{separator}", (int)eLeMenuRecordingsIT::separator);
    tokenContainer->DefineIntToken("{percentseen}", (int)eLeMenuRecordingsIT::percentseen);
    tokenContainer->DefineIntToken("{watched}", (int)eLeMenuRecordingsIT::watched);
    tokenContainer->DefineIntToken("{cutted}", (int)eLeMenuRecordingsIT::cutted);
    tokenContainer->DefineIntToken("{folder}", (int)eLeMenuRecordingsIT::folder);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eLeMenuRecordingsIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eLeMenuRecordingsIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eLeMenuRecordingsIT::year);
    tokenContainer->DefineIntToken("{duration}", (int)eLeMenuRecordingsIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eLeMenuRecordingsIT::durationhours);
    tokenContainer->DefineIntToken("{durationevent}", (int)eLeMenuRecordingsIT::durationevent);
    tokenContainer->DefineIntToken("{durationeventhours}", (int)eLeMenuRecordingsIT::durationeventhours);
    tokenContainer->DefineIntToken("{numrecordingsfolder}", (int)eLeMenuRecordingsIT::numrecordingsfolder);
    tokenContainer->DefineIntToken("{newrecordingsfolder}", (int)eLeMenuRecordingsIT::newrecordingsfolder);
    tokenContainer->DefineIntToken("{hasposterthumbnail}", (int)eLeMenuRecordingsIT::hasposterthumbnail);
    tokenContainer->DefineIntToken("{thumbnailwidth}", (int)eLeMenuRecordingsIT::thumbnailwidth);
    tokenContainer->DefineIntToken("{thumbnailheight}", (int)eLeMenuRecordingsIT::thumbnailheight);
    tokenContainer->DefineIntToken("{hasposter}", (int)eLeMenuRecordingsIT::hasposter);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eLeMenuRecordingsIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eLeMenuRecordingsIT::posterheight);
    InheritTokenContainer();
}

void cLeMenuRecordings::ClearCurrentElement(void) {
   if (wasCurrent && currentRecording) {
        currentRecording->Close();
    }
}

void cLeMenuRecordings::Set(const cRecording *recording, int level, int total, int New) {
    if (!this->recording) {
        this->recording = new cRecording(recording->FileName());
    }
    this->level = level;
    this->total = total;
    this->New = New;
    dirty = true;
}

bool cLeMenuRecordings::Parse(bool forced) {
    if (!dirty)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::current, current);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::separator, !selectable);
    bool isFolder = (total > 0) ? true : false;
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::folder, isFolder);

    char *recName = RecName(recording->Name(), level);
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::name, recName);

    const cRecording *usedRecording = recording;

    if (isFolder) {
        char *folderName = FolderName(recording->Name(), level);
        cRecordingsFolderInfo::cFolderInfo *folderInfo = recFolderInfo.Get(folderName);
        delete[] folderName;
        if (folderInfo) {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
            LOCK_RECORDINGS_READ;
            const cRecordings* recordings = Recordings;
#else
            cRecordings* recordings = &Recordings;
#endif
            const cRecording *newestRec = recordings->GetByName(*folderInfo->LatestFileName);
            if (newestRec) {
                usedRecording = newestRec;
            }
            delete folderInfo;
        }
    }
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::isnew, usedRecording->IsNew());

    int percSeen = 0;
#if APIVERSNUM < 20108
    percSeen = -1;
#else
    percSeen = 0;
    int framesSeen = usedRecording->GetResume();
    int framesTotal = usedRecording->NumFrames();
    if (framesTotal > 0) {
        percSeen = (double)framesSeen / (double)framesTotal * 100;
    }
#endif
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::percentseen, percSeen);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::newrecordingsfolder, New);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::numrecordingsfolder, total);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::cutted, usedRecording->IsEdited());

    int recDuration = usedRecording->LengthInSeconds();
    bool watched = false;
    if (usedRecording->IsEdited()) {
        if (percSeen >= 85)
            watched = true;
    } else {
        int watchedLimit = recDuration * 85 / 100 - (Setup.MarginStop + 5)*60;
        int watchedTime = percSeen * recDuration / 100;
        if (watchedLimit > 0 && watchedTime > 0 && (watchedTime > watchedLimit))
            watched = true;
    }
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::watched, watched);

    const cEvent *event = NULL;
    const cRecordingInfo *info = usedRecording->Info();
    if (!info) {
        delete[] recName;
        return true;
    }
    event = info->GetEvent();
    if (!event) {
        delete[] recName;
        return true;
    }
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::epgname, info->Title() ? info->Title() : recName);
    delete[] recName;

    cString recDate = event->GetDateString();
    cString recTime = event->GetTimeString();
    if (strstr(*recDate, "1970")) {
        time_t start = usedRecording->Start();
        recDate = DateString(start);
        recTime = TimeString(start);
    }

    time_t startTime = event->StartTime();
    if (!startTime)
        startTime = usedRecording->Start();
    struct tm * sStartTime = localtime(&startTime);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::year, sStartTime->tm_year + 1900);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::daynumeric, sStartTime->tm_mday);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::month, sStartTime->tm_mon+1);

    int duration = event->Duration() / 60;
    recDuration = (recDuration>0)?(recDuration / 60):0;
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::date, *recDate);
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::time, *recTime);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::duration, recDuration);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::durationhours, recDuration / 60);
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::durationminutes, *cString::sprintf("%.2d", recDuration%60));
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::durationevent, duration);
    tokenContainer->AddIntToken((int)eLeMenuRecordingsIT::durationeventhours, duration / 60);
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::durationeventminutes, *cString::sprintf("%.2d", duration%60));

    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::shorttext, info->ShortText());
    tokenContainer->AddStringToken((int)eLeMenuRecordingsST::description, info->Description());

    SetScraperRecordingPoster(tokenContainer, usedRecording, true);

    return true;
}

void cLeMenuRecordings::RenderCurrent(void) {
    if (!currentRecording)
        return;
    currentRecording->Set(recording, level, total, New);
    currentRecording->SetListPosition(container.X(), container.Y(), container.Width(), container.Height(), num);
    currentRecording->Parse();
}

void cLeMenuRecordings::Clear(bool forceClearBackground) {
    delete recording;
    recording = NULL;
    cListElement::Clear(forceClearBackground);
}

void cLeMenuRecordings::Close(void) {
    delete recording;
    recording = NULL;
    cListElement::Close();
}

char *cLeMenuRecordings::RecName(const char *path, int level) {
    char *recName;
    const char *startName = path;
    for (int i = 0; i < level; i++) {
        startName = strchr(startName, FOLDERDELIMCHAR);
        startName++;
    }
    if (*startName == '%')
        startName++;
    const char *endName = startName;
    if (endName = strchr(endName, FOLDERDELIMCHAR)) {
        int len = endName - startName;
        recName = new char[len+1];
        strncpy(recName, startName, len);
        recName[len] = 0;
    } else {
        recName = strdup(startName);
    }
    return recName;
}

char *cLeMenuRecordings::FolderName(const char *path, int level) {
    char *folderName;
    const char *endName = path;
    bool foundEnd = false;
    for (int i = 0; i <= level; i++) {
        endName = strchr(endName, FOLDERDELIMCHAR);
        if (!endName) {
            foundEnd = false;
        }
        foundEnd = true;
        endName++;
    }
    if (foundEnd) {
        int len = endName - path - 1;
        folderName = new char[len+1];
        strncpy(folderName, path, len);
        folderName[len] = 0;
    } else {
        folderName = strdup(path);
    }
    return folderName;
}

/******************************************************************
* cCeMenuRecordings
******************************************************************/
cCeMenuRecordings::cCeMenuRecordings(void) {
    recording = NULL;
    level = 0;
    total = 0;
    New = 0;
}

cCeMenuRecordings::~cCeMenuRecordings(void) {
}

void cCeMenuRecordings::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eCeMenuRecordingsST::name);
    tokenContainer->DefineStringToken("{epgname}", (int)eCeMenuRecordingsST::epgname);
    tokenContainer->DefineStringToken("{shorttext}", (int)eCeMenuRecordingsST::shorttext);
    tokenContainer->DefineStringToken("{description}", (int)eCeMenuRecordingsST::description);
    tokenContainer->DefineStringToken("{date}", (int)eCeMenuRecordingsST::date);
    tokenContainer->DefineStringToken("{time}", (int)eCeMenuRecordingsST::time);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eCeMenuRecordingsST::durationminutes);
    tokenContainer->DefineStringToken("{durationeventminutes}", (int)eCeMenuRecordingsST::durationeventminutes);
    tokenContainer->DefineStringToken("{thumbnailpath}", (int)eCeMenuRecordingsST::thumbnailpath);
    tokenContainer->DefineStringToken("{posterpath}", (int)eCeMenuRecordingsST::posterpath);
    tokenContainer->DefineIntToken("{menuitemx}", (int)eCeMenuRecordingsIT::menuitemx);
    tokenContainer->DefineIntToken("{menuitemy}", (int)eCeMenuRecordingsIT::menuitemy);
    tokenContainer->DefineIntToken("{menuitemwidth}", (int)eCeMenuRecordingsIT::menuitemwidth);
    tokenContainer->DefineIntToken("{menuitemheight}", (int)eCeMenuRecordingsIT::menuitemheight);
    tokenContainer->DefineIntToken("{numitem}", (int)eCeMenuRecordingsIT::numitem);
    tokenContainer->DefineIntToken("{new}", (int)eCeMenuRecordingsIT::isnew);
    tokenContainer->DefineIntToken("{percentseen}", (int)eCeMenuRecordingsIT::percentseen);
    tokenContainer->DefineIntToken("{watched}", (int)eCeMenuRecordingsIT::watched);
    tokenContainer->DefineIntToken("{cutted}", (int)eCeMenuRecordingsIT::cutted);
    tokenContainer->DefineIntToken("{folder}", (int)eCeMenuRecordingsIT::folder);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eCeMenuRecordingsIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eCeMenuRecordingsIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eCeMenuRecordingsIT::year);
    tokenContainer->DefineIntToken("{duration}", (int)eCeMenuRecordingsIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eCeMenuRecordingsIT::durationhours);
    tokenContainer->DefineIntToken("{durationevent}", (int)eCeMenuRecordingsIT::durationevent);
    tokenContainer->DefineIntToken("{durationeventhours}", (int)eCeMenuRecordingsIT::durationeventhours);
    tokenContainer->DefineIntToken("{numrecordingsfolder}", (int)eCeMenuRecordingsIT::numrecordingsfolder);
    tokenContainer->DefineIntToken("{newrecordingsfolder}", (int)eCeMenuRecordingsIT::newrecordingsfolder);
    tokenContainer->DefineIntToken("{hasposterthumbnail}", (int)eCeMenuRecordingsIT::hasposterthumbnail);
    tokenContainer->DefineIntToken("{thumbnailwidth}", (int)eCeMenuRecordingsIT::thumbnailwidth);
    tokenContainer->DefineIntToken("{thumbnailheight}", (int)eCeMenuRecordingsIT::thumbnailheight);
    tokenContainer->DefineIntToken("{hasposter}", (int)eCeMenuRecordingsIT::hasposter);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eCeMenuRecordingsIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eCeMenuRecordingsIT::posterheight);
    InheritTokenContainer();
}

void cCeMenuRecordings::Set(const cRecording *recording, int level, int total, int New) {
    this->recording = recording;
    this->level = level;
    this->total = total;
    this->New = New;
    dirty = true;
}

bool cCeMenuRecordings::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;

    tokenContainer->Clear();
    SetListTokens(tokenContainer);
    bool isFolder = (total > 0) ? true : false;
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::folder, isFolder);

    string name = recording->Name() ? recording->Name() : "";
    string buffer = "";
    stringstream folderName;
    try {
        vector<string> tokens;
        istringstream f(name.c_str());
        string s;
        int i=0;
        while (getline(f, s, FOLDERDELIMCHAR)) {
            tokens.push_back(s);
            if (isFolder && i <= level) {
                if (i > 0)
                    folderName << FOLDERDELIMCHAR;
                folderName << s;
                i++;
            }
        }
        buffer = tokens.at(level);
        if (!isFolder && recording->IsEdited()) {
            buffer = buffer.substr(1);
        }
    } catch (...) {
        buffer = name.c_str();
    }

    const cRecording *usedRecording = recording;

    if (isFolder) {
        cRecordingsFolderInfo::cFolderInfo *folderInfo = recFolderInfo.Get(folderName.str().c_str());
        if (folderInfo) {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
            LOCK_RECORDINGS_READ;
            const cRecordings* recordings = Recordings;
#else
            cRecordings* recordings = &Recordings;
#endif
            const cRecording *newestRec = recordings->GetByName(*folderInfo->LatestFileName);
            if (newestRec) {
                usedRecording = newestRec;
            }
            delete folderInfo;
        }
    }

    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::name, buffer.c_str());
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::isnew, usedRecording->IsNew());

    int percSeen = 0;
#if APIVERSNUM < 20108
    percSeen = -1;
#else
    percSeen = 0;
    int framesSeen = usedRecording->GetResume();
    int framesTotal = usedRecording->NumFrames();
    if (framesTotal > 0) {
        percSeen = (double)framesSeen / (double)framesTotal * 100;
    }
#endif
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::percentseen, percSeen);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::newrecordingsfolder, New);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::numrecordingsfolder, total);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::cutted, usedRecording->IsEdited());

    int recDuration = usedRecording->LengthInSeconds();
    bool watched = false;
    if (usedRecording->IsEdited()) {
        if (percSeen >= 85)
            watched = true;
    } else {
        int watchedLimit = recDuration * 85 / 100 - (Setup.MarginStop + 5)*60;
        int watchedTime = percSeen * recDuration / 100;
        if (watchedLimit > 0 && watchedTime > 0 && (watchedTime > watchedLimit))
            watched = true;
    }
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::watched, watched);

    const cEvent *event = NULL;
    const cRecordingInfo *info = usedRecording->Info();
    if (!info) return true;
    event = info->GetEvent();
    if (!event) return true;

    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::epgname, info->Title() ? info->Title() : buffer.c_str());
    cString recDate = event->GetDateString();
    cString recTime = event->GetTimeString();
    if (strstr(*recDate, "1970")) {
        time_t start = usedRecording->Start();
        recDate = DateString(start);
        recTime = TimeString(start);
    }

    time_t startTime = event->StartTime();
    if (!startTime)
        startTime = usedRecording->Start();
    struct tm * sStartTime = localtime(&startTime);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::year, sStartTime->tm_year + 1900);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::daynumeric, sStartTime->tm_mday);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::month, sStartTime->tm_mon+1);

    int duration = event->Duration() / 60;
    recDuration = (recDuration>0)?(recDuration / 60):0;
    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::date, *recDate);
    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::time, *recTime);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::duration, recDuration);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::durationhours, recDuration / 60);
    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::durationminutes, *cString::sprintf("%.2d", recDuration%60));
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::durationevent, duration);
    tokenContainer->AddIntToken((int)eCeMenuRecordingsIT::durationeventhours, duration / 60);
    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::durationeventminutes, *cString::sprintf("%.2d", duration%60));
    
    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::shorttext, info->ShortText());
    tokenContainer->AddStringToken((int)eCeMenuRecordingsST::description, info->Description());

    SetScraperRecordingPoster(tokenContainer, usedRecording, false);

    return true;
}

/******************************************************************
* cLeMenuPlugin
******************************************************************/
cLeMenuPlugin::cLeMenuPlugin(void) {
    currentPlugin = NULL;
    plugId = -1;
    plugMenuId = -1;
}

cLeMenuPlugin::cLeMenuPlugin(const cLeMenuPlugin &other) : cListElement(other) {
    currentPlugin = NULL;
    plugId = -1;
    plugMenuId = -1;
}

cLeMenuPlugin::~cLeMenuPlugin(void) {
}

void cLeMenuPlugin::SetTokenContainer(void) {
    if (plugId == -1 || plugMenuId == -1)
        return;
    skindesignerapi::cTokenContainer *tkPlugMenu = plgManager->GetTokenContainer(plugId, plugMenuId);
    tokenContainer = new skindesignerapi::cTokenContainer(*tkPlugMenu);
    InheritTokenContainer();
}

void cLeMenuPlugin::ClearCurrentElement(void) {
    if (wasCurrent && currentPlugin) {
        currentPlugin->Close();
    }
}

void cLeMenuPlugin::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
    dirty = true;
}

bool cLeMenuPlugin::Parse(bool forced) {
    if (!dirty)
        return false;
    tokenContainer->AddIntToken(0, current);
    return true;
}

void cLeMenuPlugin::RenderCurrent(void) {
    if (!currentPlugin)
        return;
    currentPlugin->Set(tokenContainer);
    currentPlugin->SetListPosition(container.X(), container.Y(), container.Width(), container.Height(), num);
    currentPlugin->Parse();
}

/******************************************************************
* cCeMenuPlugin
******************************************************************/
cCeMenuPlugin::cCeMenuPlugin(void) {
    plugId = -1;
    plugMenuId = -1;
}

cCeMenuPlugin::~cCeMenuPlugin(void) {
}

void cCeMenuPlugin::SetTokenContainer(void) {
    if (plugId == -1 || plugMenuId == -1)
        return;
    skindesignerapi::cTokenContainer *tkPlugMenu = plgManager->GetTokenContainer(plugId, plugMenuId);
    tokenContainer = new skindesignerapi::cTokenContainer(*tkPlugMenu);
    InheritTokenContainer();
}

void cCeMenuPlugin::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
    dirty = true;
}

bool cCeMenuPlugin::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;
    return true;
}

/******************************************************************
* cLeAudioTracks
******************************************************************/
cLeAudioTracks::cLeAudioTracks(void) {
    text = NULL;
}

cLeAudioTracks::~cLeAudioTracks(void) {
    free(text);
}

void cLeAudioTracks::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eLeDisplayTracksST::title);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeDisplayTracksIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeDisplayTracksIT::current);
    InheritTokenContainer();
}

void cLeAudioTracks::Set(const char *text) {
    if (!text)
        return;
    free(this->text);
    this->text = strdup(text);
    dirty = true;
}

bool cLeAudioTracks::Parse(bool forced) {
    if (!dirty)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeDisplayTracksIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeDisplayTracksIT::current, current);
    tokenContainer->AddStringToken((int)eLeDisplayTracksST::title, text);
    return true;
}

/******************************************************************
* cLeChannelList
******************************************************************/
cLeChannelList::cLeChannelList(void) {
    channel = NULL;
}

cLeChannelList::cLeChannelList(const cLeChannelList &other) : cListElement(other) {
    channel = NULL;
}

cLeChannelList::~cLeChannelList(void) {
}

void cLeChannelList::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eLeChannelListST::name);
    tokenContainer->DefineStringToken("{channelid}", (int)eLeChannelListST::channelid);
    tokenContainer->DefineStringToken("{presenteventtitle}", (int)eLeChannelListST::presenteventtitle);
    tokenContainer->DefineStringToken("{presenteventstart}", (int)eLeChannelListST::presenteventstart);
    tokenContainer->DefineStringToken("{presenteventstop}", (int)eLeChannelListST::presenteventstop);
    tokenContainer->DefineStringToken("{nexteventtitle}", (int)eLeChannelListST::nexteventtitle);
    tokenContainer->DefineStringToken("{nexteventstart}", (int)eLeChannelListST::nexteventstart);
    tokenContainer->DefineStringToken("{nexteventstop}", (int)eLeChannelListST::nexteventstop);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeChannelListIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeChannelListIT::current);
    tokenContainer->DefineIntToken("{number}", (int)eLeChannelListIT::number);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eLeChannelListIT::channellogoexists);
    tokenContainer->DefineIntToken("{presenteventelapsed}", (int)eLeChannelListIT::presenteventelapsed);
    tokenContainer->DefineIntToken("{presenteventremaining}", (int)eLeChannelListIT::presenteventremaining);
    InheritTokenContainer();    
}

void cLeChannelList::Set(const cChannel *channel) {
    dirty = true;
    this->channel = channel;
}

bool cLeChannelList::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeChannelListIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeChannelListIT::current, current);
    tokenContainer->AddIntToken((int)eLeChannelListIT::number, channel->Number());
    tokenContainer->AddStringToken((int)eLeChannelListST::name, channel->Name());
    cString channelID = channel->GetChannelID().ToString();
    tokenContainer->AddStringToken((int)eLeChannelListST::channelid, *channelID);
    tokenContainer->AddIntToken((int)eLeChannelListIT::channellogoexists, imgCache->LogoExists(*channelID));

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_SCHEDULES_READ;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif
    const cSchedule *schedule = schedules->GetSchedule(channel);
    if (schedule) {
        const cEvent *presentEvent = schedule->GetPresentEvent();
        if (presentEvent) {
            tokenContainer->AddStringToken((int)eLeChannelListST::presenteventtitle, presentEvent->Title());
            tokenContainer->AddStringToken((int)eLeChannelListST::presenteventstart, *presentEvent->GetTimeString());
            tokenContainer->AddStringToken((int)eLeChannelListST::presenteventstop, *presentEvent->GetEndTimeString());
            tokenContainer->AddIntToken((int)eLeChannelListIT::presenteventelapsed, (time(0) - presentEvent->StartTime())/60);
            tokenContainer->AddIntToken((int)eLeChannelListIT::presenteventremaining, presentEvent->Duration()/60 - (time(0) - presentEvent->StartTime())/60);
        }
        const cList<cEvent> *events = schedule->Events();
        if (events && presentEvent) {
            const cEvent *nextEvent = events->Next(presentEvent);
            if (nextEvent) {
                tokenContainer->AddStringToken((int)eLeChannelListST::nexteventtitle, nextEvent->Title());
                tokenContainer->AddStringToken((int)eLeChannelListST::nexteventstart, *nextEvent->GetTimeString());
                tokenContainer->AddStringToken((int)eLeChannelListST::nexteventstop, *nextEvent->GetEndTimeString());
            }
        }
    }
    
    return true;
}

/******************************************************************
* cLeGroupList
******************************************************************/
cLeGroupList::cLeGroupList(void) {
    group = NULL;
    numChannels = 0;
}

cLeGroupList::cLeGroupList(const cLeGroupList &other) : cListElement(other) {
    group = NULL;
    numChannels = 0;
}

cLeGroupList::~cLeGroupList(void) {
}

void cLeGroupList::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{groupname}", (int)eLeGroupListST::groupname);
    tokenContainer->DefineIntToken("{numchannels}", (int)eLeGroupListIT::numchannels);
    tokenContainer->DefineIntToken("{nummenuitem}", (int)eLeGroupListIT::nummenuitem);
    tokenContainer->DefineIntToken("{current}", (int)eLeGroupListIT::current);
    InheritTokenContainer();    
}

void cLeGroupList::Set(const char *group, int numChannels) {
    dirty = true;
    this->group = group;
    this->numChannels = numChannels;
}

bool cLeGroupList::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!dirty)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eLeGroupListIT::nummenuitem, num);
    tokenContainer->AddIntToken((int)eLeGroupListIT::current, current);
    tokenContainer->AddIntToken((int)eLeGroupListIT::numchannels, numChannels);
    tokenContainer->AddStringToken((int)eLeGroupListST::groupname, group);
    return true;
}
