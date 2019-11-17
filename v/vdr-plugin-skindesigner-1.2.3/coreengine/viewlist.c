#include "viewlist.h"
#include "view.h"

cViewList::cViewList(void) {
    globals = NULL;
    attribs = new cViewListAttribs((int)eViewListAttribs::count);
    numElements = 0;
    orientation = eOrientation::vertical;
    cleared = true;
    itemCount = 0;
    listElement = NULL;
    currentElement = NULL;
    listElements = NULL;
    plugId = -1;
    plugMenuId = -1;
    fader = NULL;
    shifter = NULL;
}

cViewList::~cViewList(void) {
    delete attribs;
    delete listElement;
    delete currentElement;
    if (listElements) {
        for (int i=0; i < numElements; ++i) {
            if (listElements[i]) {
                delete listElements[i];
                listElements[i] = NULL;
            }
        }
    }
    delete[] listElements;
    delete tokenContainer;
}

void cViewList::SetGlobals(cGlobals *globals) {
    this->globals = globals;
    if (listElement)
        listElement->SetGlobals(globals);
    if (currentElement)
        currentElement->SetGlobals(globals);
}

void cViewList::SetContainer(int x, int y, int width, int height) {
    container.SetX(x);
    container.SetY(y);
    container.SetWidth(width);
    container.SetHeight(height);
}

void cViewList::SetAttributes(vector<stringpair> &attributes) {
    attribs->Set(attributes);
}

cViewList *cViewList::CreateViewList(const char *name) {
    cViewList *l = NULL;
    if (!strcmp(name, "menudefault"))
        l = new cViewListDefault();
    else if (!strcmp(name, "menumain"))
        l = new cViewListMain();
    else if (!strcmp(name, "menusetup"))
        l = new cViewListMain();
    else if (!strcmp(name, "menuschedules"))
        l = new cViewListSchedules();
    else if (!strcmp(name, "menutimers"))
        l = new cViewListTimers();
    else if (!strcmp(name, "menuchannels"))
        l = new cViewListChannels();
    else if (!strcmp(name, "menurecordings"))
        l = new cViewListRecordings();
    else if (!strcmp(name, "displayaudiotracks"))
        l = new cViewListAudioTracks();
    else if (startswith(name, "menuplugin"))
        l = new cViewListPlugin();
    else if (startswith(name, "channellist"))
        l = new cViewListChannelList();
    else if (startswith(name, "grouplist"))
        l = new cViewListGroupList();
    else if (startswith(name, "groupchannellist"))
        l = new cViewListChannelList();
    else
        esyslog("skindesigner: unknown viewlist %s", name);
    return l;
}

cViewElement *cViewList::CreateListElement(const char *name) {
    cViewElement *le = NULL;
    if (!strcmp(name, "menudefault"))
        le = new cLeMenuDefault();
    else if (!strcmp(name, "menumain"))
        le = new cLeMenuMain();
    else if (!strcmp(name, "menusetup"))
        le = new cLeMenuMain();
    else if (!strcmp(name, "menuschedules"))
        le = new cLeMenuSchedules();
    else if (!strcmp(name, "menutimers"))
        le = new cLeMenuTimers();
    else if (!strcmp(name, "menuchannels"))
        le = new cLeMenuChannels();
    else if (!strcmp(name, "menurecordings"))
        le = new cLeMenuRecordings();
    else if (!strcmp(name, "displayaudiotracks"))
        le = new cLeAudioTracks();
    else if (startswith(name, "menuplugin"))
        le = new cLeMenuPlugin();
    else if (startswith(name, "channellist"))
        le = new cLeChannelList();
    else if (startswith(name, "grouplist"))
        le = new cLeGroupList();
    else if (startswith(name, "groupchannellist"))
        le = new cLeChannelList();
    else
        esyslog("skindesigner: unknown viewlist %s", name);
    return le;
}

cViewElement *cViewList::CreateCurrentElement(const char *name) {
    cViewElement *ce = NULL;
    if (!strcmp(name, "menumain"))
        ce = new cCeMenuMain();
    else if (!strcmp(name, "menusetup"))
        ce = new cCeMenuMain();
    else if (!strcmp(name, "menuschedules"))
        ce = new cCeMenuSchedules();
    else if (!strcmp(name, "menutimers"))
        ce = new cCeMenuTimers();
    else if (!strcmp(name, "menuchannels"))
        ce = new cCeMenuChannels();
    else if (!strcmp(name, "menurecordings"))
        ce = new cCeMenuRecordings();
    else if (startswith(name, "menuplugin"))
        ce = new cCeMenuPlugin();
    else
        esyslog("skindesigner: unknown viewlist %s", name);
    return ce;
}

void cViewList::AddListElement(cViewElement *listElement) {
    this->listElement = listElement;
    listElement->SetTokenContainer();
}

void cViewList::AddCurrentElement(cViewElement *currentElement) {
    this->currentElement = currentElement;
    currentElement->SetTokenContainer();
}

void cViewList::PreCache(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->CreateContainers();
    attribs->SetTokenContainer(tokenContainer);
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->SetGlobals(globals);
    attribs->Cache();
    numElements = attribs->NumListElements();
    orientation = attribs->Orientation();
    eAlign align = attribs->Align();
    int x = attribs->X();
    int y = attribs->Y();
    int width = attribs->Width();
    int height = attribs->Height();

    int step = 0;
    if (orientation == eOrientation::vertical) {
        step = height / numElements;
    } else {
        step = width / numElements;
    }

    int start = y;
    if (align == eAlign::center) {
        if (orientation == eOrientation::vertical) {
            start = y + (height - numElements * step) / 2;
        } else {
            start = x + (width - numElements * step) / 2;
        }
    } else if (align == eAlign::bottom) {
        start = y + height - numElements * step;
    } else if (align == eAlign::right) {
        start = x + width - numElements * step;
    }
    Prepare(start, step);
}

int cViewList::NumItems(void) {
    return numElements;
}

bool cViewList::Execute(void) {
    return attribs->DoExecute();
}

eOrientation cViewList::Orientation(void) {
    return attribs->Orientation();
}

void cViewList::Draw(eMenuCategory menuCat) {
    int current = -1;
    int numCalls = 0;
    for (int i = 0; i < numElements; i++) {
        listElements[i]->SetMenuCategory(menuCat);
        if (listElements[i]->Parse()) {
            if (listElements[i]->Shifting())
                SetShiftParameters(i, numCalls);
            if (listElements[i]->Fading() && itemCount == 1) {
                listElements[i]->SetSuppressAnimation(true);
            }
            listElements[i]->Render();
            if (listElements[i]->Current()) {
                listElements[i]->RenderCurrent();
                current = i;
            }
        }
    }
    if (current >= 0 && listElements[current]) {
        listElements[current]->WakeCurrent();
    }
    cleared = false;
}

void cViewList::Clear(void) {
    cleared = true;
    if (!listElements)
        return;
    for (int i = 0; i < numElements; i++) {
        listElements[i]->StopBlinking();
        listElements[i]->StopScrolling();
    }
    for (int i = 0; i < numElements; i++) {
        listElements[i]->Clear();
    }
}

void cViewList::Close(void) {
    if (shifter)
        cView::RemoveAnimation(shifter);
    if (fader)
        cView::RemoveAnimation(fader);
    if (!listElements)
        return;
    for (int i = 0; i < numElements; i++) {
        listElements[i]->StopBlinking();
        listElements[i]->StopScrolling();
    }
    for (int i = 0; i < numElements; i++)
        listElements[i]->Close();
}

void cViewList::SetTransparency(int transparency, bool force) {
    for (int i = 0; i < numElements; i++) {
        if (listElements[i])
            listElements[i]->SetTransparency(transparency, force);
    }
}

cRect cViewList::CoveredArea(void) {
    cRect unionArea;
    for (int i = 0; i < numElements; i++) {
        if (listElements[i])
            unionArea.Combine(listElements[i]->CoveredArea());
    }
    return unionArea;
}

void cViewList::SetPosition(cPoint &position, cPoint &reference, bool force) {
    for (int i = 0; i < numElements; i++) {
        if (listElements[i])
            listElements[i]->SetPosition(position, reference, force);
    }
}

void cViewList::ShiftPositions(cPoint *start, cPoint *end) {
    cRect shiftbox = CoveredArea();
    cPoint startPoint = ShiftStart(shiftbox);
    start->Set(startPoint);
    end->Set(shiftbox.X(), shiftbox.Y());
}

void cViewList::StartAnimation(bool animOut) {
    shifter = NULL;
    fader = NULL;
    if (ShiftTime() > 0) {
        shifter = new cShifter((cShiftable*)this);
        if (animOut)
            shifter->SetPersistent();
        cView::AddAnimation(shifter);
    } else if (FadeTime() > 0) {
        fader = new cFader((cFadable*)this);
        if (animOut)
            fader->SetPersistent();
        cView::AddAnimation(fader);
    }
}

void cViewList::Debug(void) {
    esyslog("skindesigner: --- debug viewlist");
    attribs->Debug();
}

cPoint cViewList::ShiftStart(cRect &shiftbox) {
    eShiftType type = (eShiftType)attribs->ShiftType();
    cPoint start;
    if (type == eShiftType::none) {
        start = attribs->ShiftStartpoint();
    } else if (type == eShiftType::left) {
        start.SetX(-shiftbox.Width());
        start.SetY(shiftbox.Y());
    } else if (type == eShiftType::right) {
        start.SetX(cOsd::OsdWidth());
        start.SetY(shiftbox.Y());        
    } else if (type == eShiftType::top) {
        start.SetX(shiftbox.X());
        start.SetY(-shiftbox.Height());        
    } else if (type == eShiftType::bottom) {
        start.SetX(shiftbox.X());
        start.SetY(cOsd::OsdHeight());
    }
    return start;
}

void cViewList::SetShiftParameters(int index, int &call) {
    if (itemCount == 1) {
        listElements[index]->SetSuppressAnimation(true);
        return;
    }
    listElements[index]->SetSuppressAnimation(cleared);
    if (listElements[index]->WasCurrent()) {
        if (call == 0) {
            call++;
            listElements[index]->SetSelectedFromTop();
        } else if (call == 1) {                    
            call++;
            listElements[index]->SetSelectedFromBottom();
        }
    } else if (listElements[index]->Current()) {
        if (call == 0) {
            call++;
            listElements[index]->SetSelectedFromBottom();
        } else if (call == 1) {                    
            call++;
            listElements[index]->SetSelectedFromTop();
        }
    }
}

void cViewList::CheckListAnimation(int index) {
    itemCount++;
    if (listElements[index])
        listElements[index]->StopListAnimation();
}

/******************************************************************
* cViewListDefault
******************************************************************/
cViewListDefault::cViewListDefault(void) {
    listDefault = NULL;
    colX = NULL;
    colWidths = NULL;
    avrgFontWidth = 0;
    listFont = NULL;
    plugName = NULL;
}

cViewListDefault::~cViewListDefault(void) {
    delete[] listDefault;
    delete[] colX;
    delete[] colWidths;
}

void cViewListDefault::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeMenuDefault *tpl = dynamic_cast<cLeMenuDefault*>(listElement);
    if (!tpl) return;
    listDefault = new cLeMenuDefault*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listDefault[i] = new cLeMenuDefault(*tpl);
        listElements[i] = listDefault[i];
        listDefault[i]->SetNumber(i);
        listDefault[i]->SetOrientation(orientation);
        listDefault[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listDefault[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listDefault[i]->SetAreaWidth(width);
        }
        listDefault[i]->SetContainer(x, y, width, height);
        listDefault[i]->Cache();
        pos += step;
    }

    //Setting average font width
    avrgFontWidth = 20;
    const char *determinateFont = attribs->DeterminateFont();
    if (!determinateFont) {
        return;
    }
    cFunction *f = listDefault[0]->GetFunction(determinateFont);
    if (!f)
        return;
    cFuncDrawText *fdt = dynamic_cast<cFuncDrawText*>(f);
    if (!fdt) {
        return;
    }
    avrgFontWidth = fdt->AvrgFontWidth();
    listFont = fdt->GetFont();
    colX = new int[MAX_TABS];
    colWidths = new int[MAX_TABS];
    for (int i=0; i < MAX_TABS; i++) {
        colX[i] = 0;
        colWidths[i] = 0;
    }
    for (int i = 0; i < numElements; i++) {
        listDefault[i]->SetListInfo(colX, colWidths);
    }
}

void cViewListDefault::SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5) {
    int itemWidth = attribs->MenuItemWidth();
    if (!tab1) {
        colX[0] = 0;
        colWidths[0] = itemWidth;
        for (int i = 1; i < MAX_TABS; i++) {
                colX[i] = 0;
                colWidths[i] = 0;
        }
    } else {
        colX[0] = 0;
        colX[1] = tab1 ? colX[0] + tab1 : 0;
        colX[2] = tab2 ? colX[1] + tab2 : 0;
        colX[3] = tab3 ? colX[2] + tab3 : 0;
        colX[4] = tab4 ? colX[3] + tab4 : 0;
        colX[5] = tab5 ? colX[4] + tab5 : 0;
        for (int i = 1; i < MAX_TABS; i++)
            colX[i] *= avrgFontWidth;

        bool end = false;
        for (int i = 0; i < MAX_TABS; i++) {
            if ( i < MAX_TABS -1 && colX[i+1] > 0) {
                colWidths[i] = colX[i+1] - colX[i];
            } else if (!end) {
                end = true;
                colWidths[i] = itemWidth - colX[i];
            } else {
                colWidths[i] = 0;
            }
        }
    }
}

void cViewListDefault::Set(const char *text, int index, bool current, bool selectable) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listDefault[index]->StopScrolling();
    listDefault[index]->SetCurrent(current);
    listDefault[index]->SetSelectable(selectable);
    listDefault[index]->SetPlugin(plugName);
    listDefault[index]->SetText(text);
}

const cFont *cViewListDefault::GetListFont(void) {
    return listFont;
};

int cViewListDefault::GetListWidth(void) {
    return attribs->MenuItemWidth();
}

/******************************************************************
* cViewListMain
******************************************************************/
cViewListMain::cViewListMain(void) {
    listMain = NULL;
    currentMain = NULL;
}

cViewListMain::~cViewListMain(void) {
    delete[] listMain;
}

void cViewListMain::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeMenuMain *tpl = dynamic_cast<cLeMenuMain*>(listElement);
    if (!tpl) return;

    listMain = new cLeMenuMain*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listMain[i] = new cLeMenuMain(*tpl);
        listElements[i] = listMain[i];
        listMain[i]->SetNumber(i);
        listMain[i]->SetOrientation(orientation);
        listMain[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listMain[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listMain[i]->SetAreaWidth(width);
        }
        listMain[i]->SetContainer(x, y, width, height);
        listMain[i]->Cache();
        pos += step;
    }

    if (!currentElement) return;
    currentMain = dynamic_cast<cCeMenuMain*>(currentElement);
    if (!currentMain) return;

    currentMain->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    currentMain->SetDetached();
    currentMain->Cache();
    for (int i = 0; i < numElements; i++) {
        listMain[i]->SetCurrentElement(currentMain);
    }
}

void cViewListMain::Set(const char *text, int index, bool current, bool selectable) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listMain[index]->StopScrolling();
    listMain[index]->SetCurrent(current);
    listMain[index]->ClearCurrentElement();
    listMain[index]->SetSelectable(selectable);
    listMain[index]->SetText(text);
}

const char *cViewListMain::GetPlugin(void) {
    for (int i = 0; i < numElements; i++) {
        if (listMain[i] && listMain[i]->Current()) {
            return listMain[i]->PluginName();
        }
    }
    return NULL;
}
/******************************************************************
* cViewListSchedules
******************************************************************/
cViewListSchedules::cViewListSchedules(void) {
    listSchedules = NULL;
    currentSchedules = NULL;
    epgSearchFav = false;
}

cViewListSchedules::~cViewListSchedules(void) {
    delete[] listSchedules;
}

void cViewListSchedules::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeMenuSchedules *tpl = dynamic_cast<cLeMenuSchedules*>(listElement);
    if (!tpl) return;

    listSchedules = new cLeMenuSchedules*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listSchedules[i] = new cLeMenuSchedules(*tpl);
        listElements[i] = listSchedules[i];
        listSchedules[i]->SetNumber(i);
        listSchedules[i]->SetOrientation(orientation);
        listSchedules[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listSchedules[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listSchedules[i]->SetAreaWidth(width);
        }
        listSchedules[i]->SetContainer(x, y, width, height);
        listSchedules[i]->Cache();
        pos += step;
    }

    if (!currentElement) return;
    currentSchedules = dynamic_cast<cCeMenuSchedules*>(currentElement);
    if (!currentSchedules) return;

    currentSchedules->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    currentSchedules->SetDetached();
    currentSchedules->Cache();
    for (int i = 0; i < numElements; i++) {
        listSchedules[i]->SetCurrentElement(currentSchedules);
    }
}

void cViewListSchedules::Set(const cEvent *event, int index, bool current, bool selectable,
                             const cChannel *channel, bool withDate, eTimerMatch timerMatch) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listSchedules[index]->StopScrolling();
    listSchedules[index]->SetCurrent(current);
    listSchedules[index]->ClearCurrentElement();
    listSchedules[index]->SetSelectable(selectable);
    listSchedules[index]->SetEpgSearchFav(epgSearchFav);
    listSchedules[index]->Set(event, channel, withDate, timerMatch);
}

/******************************************************************
* cViewListTimers
******************************************************************/
cViewListTimers::cViewListTimers(void) {
    listTimers = NULL;
    currentTimer = NULL;
}

cViewListTimers::~cViewListTimers(void) {
    delete[] listTimers;
}

void cViewListTimers::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeMenuTimers *tpl = dynamic_cast<cLeMenuTimers*>(listElement);
    if (!tpl) return;

    listTimers = new cLeMenuTimers*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listTimers[i] = new cLeMenuTimers(*tpl);
        listElements[i] = listTimers[i];
        listTimers[i]->SetNumber(i);
        listTimers[i]->SetOrientation(orientation);
        listTimers[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listTimers[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listTimers[i]->SetAreaWidth(width);
        }
        listTimers[i]->SetContainer(x, y, width, height);
        listTimers[i]->Cache();
        pos += step;
    }

    if (!currentElement) return;
    currentTimer = dynamic_cast<cCeMenuTimers*>(currentElement);
    if (!currentTimer) return;

    currentTimer->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    currentTimer->SetDetached();
    currentTimer->Cache();
    for (int i = 0; i < numElements; i++) {
        listTimers[i]->SetCurrentElement(currentTimer);
    }
}

void cViewListTimers::Set(const cTimer *timer, int index, bool current, bool selectable) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listTimers[index]->StopScrolling();
    listTimers[index]->SetCurrent(current);
    listTimers[index]->ClearCurrentElement();
    listTimers[index]->SetSelectable(selectable);
    listTimers[index]->Set(timer);
}

/******************************************************************
* cViewListChannels
******************************************************************/
cViewListChannels::cViewListChannels(void) {
    listChannels = NULL;
    currentChannel = NULL;
}

cViewListChannels::~cViewListChannels(void) {
    delete[] listChannels;
}

void cViewListChannels::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeMenuChannels *tpl = dynamic_cast<cLeMenuChannels*>(listElement);
    if (!tpl) return;

    listChannels = new cLeMenuChannels*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listChannels[i] = new cLeMenuChannels(*tpl);
        listElements[i] = listChannels[i];
        listChannels[i]->SetNumber(i);
        listChannels[i]->SetOrientation(orientation);
        listChannels[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listChannels[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listChannels[i]->SetAreaWidth(width);
        }
        listChannels[i]->SetContainer(x, y, width, height);
        listChannels[i]->Cache();
        pos += step;
    }

    if (!currentElement) return;
    currentChannel = dynamic_cast<cCeMenuChannels*>(currentElement);
    if (!currentChannel) return;

    currentChannel->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    currentChannel->SetDetached();
    currentChannel->Cache();
    for (int i = 0; i < numElements; i++) {
        listChannels[i]->SetCurrentElement(currentChannel);
    }
}

void cViewListChannels::Set(const cChannel *channel, int index, bool current, bool selectable, bool withProvider) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listChannels[index]->StopScrolling();
    listChannels[index]->SetCurrent(current);
    listChannels[index]->ClearCurrentElement();
    listChannels[index]->SetSelectable(selectable);
    listChannels[index]->Set(channel, withProvider);
}

/******************************************************************
* cViewListRecordings
******************************************************************/
cViewListRecordings::cViewListRecordings(void) {
    listRecordings = NULL;
    currentRecording = NULL;
}

cViewListRecordings::~cViewListRecordings(void) {
    delete[] listRecordings;
}

void cViewListRecordings::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeMenuRecordings *tpl = dynamic_cast<cLeMenuRecordings*>(listElement);
    if (!tpl) return;

    listRecordings = new cLeMenuRecordings*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listRecordings[i] = new cLeMenuRecordings(*tpl);
        listElements[i] = listRecordings[i];
        listRecordings[i]->SetNumber(i);
        listRecordings[i]->SetOrientation(orientation);
        listRecordings[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listRecordings[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listRecordings[i]->SetAreaWidth(width);
        }
        listRecordings[i]->SetContainer(x, y, width, height);
        listRecordings[i]->Cache();
        pos += step;
    }

    if (!currentElement) return;
    currentRecording = dynamic_cast<cCeMenuRecordings*>(currentElement);
    if (!currentRecording) return;

    currentRecording->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    currentRecording->SetDetached();
    currentRecording->Cache();
    for (int i = 0; i < numElements; i++) {
        listRecordings[i]->SetCurrentElement(currentRecording);
    }
}

void cViewListRecordings::Set(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listRecordings[index]->StopScrolling();
    listRecordings[index]->SetCurrent(current);
    listRecordings[index]->ClearCurrentElement();
    listRecordings[index]->SetSelectable(selectable);
    listRecordings[index]->Set(recording, level, total, New);
}

/******************************************************************
* cViewListPlugin
******************************************************************/
cViewListPlugin::cViewListPlugin(void) {
    listPlugin = NULL;
    currentPlugin = NULL;
}

cViewListPlugin::~cViewListPlugin(void) {
    delete[] listPlugin;
}

void cViewListPlugin::Prepare(int start, int step) {
    if (!listElement)
        return;

    if (!listElement)
        return;

    cLeMenuPlugin *tpl = dynamic_cast<cLeMenuPlugin*>(listElement);
    if (!tpl) return;

    listPlugin = new cLeMenuPlugin*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listPlugin[i] = new cLeMenuPlugin(*tpl);
        listElements[i] = listPlugin[i];
        listPlugin[i]->SetNumber(i);
        listPlugin[i]->SetOrientation(orientation);
        listPlugin[i]->SetPlugId(plugId);
        listPlugin[i]->SetPlugMenuId(plugMenuId);
        listPlugin[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listPlugin[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listPlugin[i]->SetAreaWidth(width);
        }
        listPlugin[i]->SetContainer(x, y, width, height);
        listPlugin[i]->Cache();
        pos += step;
    }

    if (!currentElement) return;
    currentPlugin = dynamic_cast<cCeMenuPlugin*>(currentElement);
    if (!currentPlugin) return;

    currentPlugin->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    currentPlugin->SetPlugId(plugId);
    currentPlugin->SetPlugMenuId(plugMenuId);
    currentPlugin->SetTokenContainer();
    currentPlugin->SetDetached();
    currentPlugin->Cache();
    for (int i = 0; i < numElements; i++) {
        listPlugin[i]->SetCurrentElement(currentPlugin);
    }
}

void cViewListPlugin::Set(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listPlugin[index]->StopScrolling();
    listPlugin[index]->SetCurrent(current);
    listPlugin[index]->ClearCurrentElement();
    listPlugin[index]->SetSelectable(selectable);
    listPlugin[index]->Set(tk);
}

/******************************************************************
* cViewListAudioTracks
******************************************************************/
cViewListAudioTracks::cViewListAudioTracks(void) {
    listAudioTracks = NULL;
    numTracks = 0;
    tokenContainer = NULL;
}

cViewListAudioTracks::~cViewListAudioTracks(void) {
    delete[] listAudioTracks;
    delete tokenContainer;
}

void cViewListAudioTracks::Close(void) {
    if (!listElements)
        return;
    for (int i = 0; i < numElements; i++) {
        delete listAudioTracks[i];
    }
    delete[] listAudioTracks;
    listAudioTracks = NULL;
 }

void cViewListAudioTracks::PreCache(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{numelements}", 0);
    tokenContainer->CreateContainers();
    attribs->SetTokenContainer(tokenContainer);
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->SetGlobals(globals);
    attribs->Cache();
}

void cViewListAudioTracks::SetNumtracks(int numTracks) {
    this->numTracks = numTracks;
    this->numElements = numTracks;
    tokenContainer->Clear();
    tokenContainer->AddIntToken(0, numTracks);
}

void cViewListAudioTracks::SetTracks(const char * const *tracks) {
    if (numTracks <= 0)
        return;
    orientation = attribs->Orientation();
    eAlign align = attribs->Align();
    int x = attribs->X();
    int y = attribs->Y();
    int width = attribs->Width();
    int height = attribs->Height();

    int step = 0;
    if (orientation == eOrientation::vertical) {
        step = height / numTracks;
    } else {
        step = width / numTracks;
    }

    int start = 0;
    if (align == eAlign::center) {
        if (orientation == eOrientation::vertical) {
            start = y + (height - numTracks * step) / 2;
        } else {
            start = x + (width - numTracks * step) / 2;
        }
    } else if (align == eAlign::bottom) {
        start = y + height - numTracks * step;
    } else if (align == eAlign::right) {
        start = x + width - numTracks * step;
    } else if (align == eAlign::top) {
        start = y;
    }

    cLeAudioTracks *tpl = dynamic_cast<cLeAudioTracks*>(listElement);
    if (!tpl) return;

    listAudioTracks = new cLeAudioTracks*[numTracks];
    listElements = new cListElement*[numTracks];
    int pos = start;

    for (int i = 0; i < numTracks; i++) {
        listAudioTracks[i] = new cLeAudioTracks(*tpl);
        listElements[i] = listAudioTracks[i];
        listAudioTracks[i]->SetNumber(i);
        listAudioTracks[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listAudioTracks[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listAudioTracks[i]->SetAreaWidth(width);
        }
        listAudioTracks[i]->SetContainer(x, y, width, height);
        listAudioTracks[i]->Cache();
        pos += step;
    }
    for (int i=0; i < numTracks; i++) {
        listAudioTracks[i]->Set(tracks[i]);
        listAudioTracks[i]->SetSelectable(true);
    }
}

void cViewListAudioTracks::SetCurrentTrack(int index) {
    for (int i = 0; i < numTracks; i++) {
        if (i != index) {
            listAudioTracks[i]->SetCurrent(false);
            listAudioTracks[i]->SetDirty();
            listAudioTracks[i]->StopScrolling();
        }
    }
    listAudioTracks[index]->SetCurrent(true);
    listAudioTracks[index]->SetDirty();
}

void cViewListAudioTracks::Draw(void) {
    for (int i = 0; i < numTracks; i++) {
        if (listAudioTracks[i]->Parse()) {
            listAudioTracks[i]->Render();
        }
    }
}

/******************************************************************
* cViewListChannelList
******************************************************************/
cViewListChannelList::cViewListChannelList(void) {
    listChannelList = NULL;
}

cViewListChannelList::~cViewListChannelList(void) {
    delete[] listChannelList;
}

void cViewListChannelList::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeChannelList *tpl = dynamic_cast<cLeChannelList*>(listElement);
    if (!tpl) return;

    listChannelList = new cLeChannelList*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listChannelList[i] = new cLeChannelList(*tpl);
        listElements[i] = listChannelList[i];
        listChannelList[i]->SetNumber(i);
        listChannelList[i]->SetOrientation(orientation);
        listChannelList[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listChannelList[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listChannelList[i]->SetAreaWidth(width);
        }
        listChannelList[i]->SetContainer(x, y, width, height);
        listChannelList[i]->Cache();
        pos += step;
    }
}

void cViewListChannelList::Set(const cChannel *channel, int index, bool current) {
    if (index < 0 || index  >= numElements)
        return;
    itemCount++;
    if (!current)
        listChannelList[index]->StopScrolling();
    listChannelList[index]->SetCurrent(current);
    listChannelList[index]->SetSelectable(true);
    listChannelList[index]->Set(channel);
}

/******************************************************************
* cViewListGroupList
******************************************************************/
cViewListGroupList::cViewListGroupList(void) {
    listGroupList = NULL;
}

cViewListGroupList::~cViewListGroupList(void) {
    delete[] listGroupList;
}

void cViewListGroupList::Prepare(int start, int step) {
    if (!listElement)
        return;

    cLeGroupList *tpl = dynamic_cast<cLeGroupList*>(listElement);
    if (!tpl) return;

    listGroupList = new cLeGroupList*[numElements];
    listElements = new cListElement*[numElements];
    int pos = start;

    for (int i = 0; i < numElements; i++) {
        listGroupList[i] = new cLeGroupList(*tpl);
        listElements[i] = listGroupList[i];
        listGroupList[i]->SetNumber(i);
        listGroupList[i]->SetOrientation(orientation);
        listGroupList[i]->SetTokenContainer();
        int x, y, width, height;
        if (orientation == eOrientation::vertical) {
            x = attribs->X();
            y = pos;
            width = attribs->Width();
            height = step;
            listGroupList[i]->SetAreaHeight(height);
        } else {
            x = pos;
            y = attribs->Y();
            width = step;
            height = attribs->Height();
            listGroupList[i]->SetAreaWidth(width);
        }
        listGroupList[i]->SetContainer(x, y, width, height);
        listGroupList[i]->Cache();
        pos += step;
    }
}

void cViewListGroupList::Set(const char *group, int numChannels, int index, bool current) {
    if (index < 0 || index  >= numElements)
        return;
    CheckListAnimation(index);
    if (!current)
        listGroupList[index]->StopScrolling();
    listGroupList[index]->SetCurrent(current);
    listGroupList[index]->SetSelectable(true);
    listGroupList[index]->Set(group, numChannels);
}
