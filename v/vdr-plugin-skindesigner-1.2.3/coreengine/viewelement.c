#include "viewelement.h"
#include "../config.h"

/******************************************************************
* cViewElement
******************************************************************/
cViewElement::cViewElement(void) {
    sdOsd = NULL;
    id = -1;
    dirty = true;
    init = true;
    drawn = false;
    scrollingStarted = false;
    blocked = false;
    detached = false;
    doAnimOut = false;
    doStartAnim = true;
    waitOnWakeup = true;
    startAnimation = true;
    restartAnimation = false;
    globals = NULL;
    tokenContainer = NULL;
    attribs = new cViewElementAttribs((int)eViewElementAttribs::count);
    clearAll = false;
    detacher = NULL;
    shifter = NULL;
    fader = NULL;
}

cViewElement::cViewElement(const cViewElement &other) {
    sdOsd = other.sdOsd;
    id = other.id;
    dirty = other.dirty; 
    init = other.init;
    drawn = false;
    scrollingStarted = false;
    blocked = false;
    detached = false;
    doAnimOut = other.doAnimOut;
    doStartAnim = other.doStartAnim;
    waitOnWakeup = true;
    startAnimation = true;
    restartAnimation = false;
    globals = other.globals;
    container.Set(other.container.X(), other.container.Y(), other.container.Width(), other.container.Height());
    tokenContainer = NULL;
    attribs = new cViewElementAttribs(*other.attribs);
    clearAll = false;

    for (const cAreaNode *node = other.areaNodes.First(); node; node = other.areaNodes.Next(node)) {
        if (cArea *a = dynamic_cast<cArea*>((cAreaNode*)node)) {
            areaNodes.Add(new cArea(*a));
        } else if (cAreaContainer *ac = dynamic_cast<cAreaContainer*>((cAreaNode*)node)) {
            areaNodes.Add(new cAreaContainer(*ac));
        } 
    }
    
    detacher = NULL;
    shifter = NULL;
    fader = NULL;
}

cViewElement::~cViewElement(void) {
    delete attribs;
    delete detacher;
    delete tokenContainer;
}

/******************************************************************
* Public Functions
******************************************************************/
cViewElement *cViewElement::CreateViewElement(const char *name, const char *viewname) {
    cViewElement *e = NULL;
    
    //common view elements
    if (!strcmp(name, "background") && strcmp(viewname, "displayaudiotracks"))
        e = new cViewElement();
    else if (!strcmp(name, "datetime"))
        e = new cVeDateTime();
    else if (!strcmp(name, "time"))
        e = new cVeTime();
    else if (!strcmp(name, "message"))
        e = new cVeMessage();
    else if (!strcmp(name, "devices"))
        e = new cVeDevices();
    else if (!strcmp(name, "currentweather"))
        e = new cVeCurrentWeather();
    else if (!strcmp(name, "customtokens"))
        e = new cVeCustomTokens();
    
    //displaychannel viewelements
    else if (!strcmp(name, "channelinfo"))
        e = new cVeDcChannelInfo();
    else if (!strcmp(name, "channelgroup"))
        e = new cVeDcChannelGroup();
    else if (!strcmp(name, "epginfo"))
        e = new cVeDcEpgInfo();
    else if (!strcmp(name, "progressbar") && !strcmp(viewname, "displaychannel"))
        e = new cVeDcProgressBar();
    else if (!strcmp(name, "statusinfo"))
        e = new cVeDcStatusInfo();
    else if (!strcmp(name, "audioinfo"))
        e = new cVeDcAudioInfo();
    else if (!strcmp(name, "screenresolution"))
        e = new cVeDcScreenResolution();
    else if (!strcmp(name, "signalquality"))
        e = new cVeDcSignalQuality();
    else if (!strcmp(name, "scrapercontent") && !strcmp(viewname, "displaychannel"))
        e = new cVeDcScraperContent();
    else if (!strcmp(name, "channelhints"))
        e = new cVeDcChannelHints();
    else if (!strcmp(name, "channeldetail"))
        e = new cVeDcChannelDetail();
    else if (!strcmp(name, "channellistback"))
        e = new cViewElement();
    else if (!strcmp(name, "grouplistback"))
        e = new cViewElement();
    else if (!strcmp(name, "groupchannellistback"))
        e = new cViewElement();
    else if (!strcmp(name, "channellistdetail"))
        e = new cVeDcChannelListDetail();
    else if (!strcmp(name, "groupchannellistdetail"))
        e = new cVeDcGroupChannelListDetail();
    else if (!strcmp(name, "ecminfo"))
        e = new cVeDcEcmInfo();
    
    //displaymenu viewelements
    else if (!strcmp(name, "header") && strcmp(viewname, "displayaudiotracks"))
        e = new cVeDmHeader();
    else if (!strcmp(name, "sortmode"))
        e = new cVeDmSortmode();
    else if (!strcmp(name, "colorbuttons"))
        e = new cVeDmColorbuttons();
    else if (!strcmp(name, "scrollbar"))
        e = new cVeDmScrollbar();
    else if (!strcmp(name, "timers"))
        e = new cVeDmTimers();
    else if (!strcmp(name, "currentschedule"))
        e = new cVeDmCurrentschedule();
    else if (!strcmp(name, "discusage"))
        e = new cVeDmDiscusage();
    else if (!strcmp(name, "systemload"))
        e = new cVeDmSystemload();
    else if (!strcmp(name, "systemmemory"))
        e = new cVeDmSystemmemory();
    else if (!strcmp(name, "temperatures"))
        e = new cVeDmTemperatures();
    else if (!strcmp(name, "vdrstatistics"))
        e = new cVeDmVdrstatistics();
    else if (!strcmp(name, "lastrecordings"))
        e = new cVeDmLastrecordings();
    else if (!strcmp(name, "detailheaderepg"))
        e = new cVeDmDetailheaderEpg();
    else if (!strcmp(name, "detailheaderrec"))
        e = new cVeDmDetailheaderRec();
    else if (!strcmp(name, "detailheaderplugin"))
        e = new cVeDmDetailheaderPlugin();
    else if (!strcmp(name, "tablabels"))
        e = new cVeDmTablabels();

    //displayreplay viewelements
    else if (!strcmp(name, "backgroundmodeonly"))
        e = new cViewElement();
    else if (!strcmp(name, "rectitle"))
        e = new cVeDrRecTitle();
    else if (!strcmp(name, "recinfo"))
        e = new cVeDrRecInfo();
    else if (!strcmp(name, "currenttime"))
        e = new cVeDrCurrentTime();
    else if (!strcmp(name, "totaltime"))
        e = new cVeDrTotalTime();
    else if (!strcmp(name, "endtime"))
        e = new cVeDrEndTime();
    else if (!strcmp(name, "progressbar") && !strcmp(viewname, "displayreplay"))
        e = new cVeDrProgressBar();
    else if (!strcmp(name, "cutmarks"))
        e = new cVeDrCutMarks();
    else if (!strcmp(name, "controlicons"))
        e = new cVeDrControlIcons();
    else if (!strcmp(name, "controliconsmodeonly"))
        e = new cVeDrControlIcons();
    else if (!strcmp(name, "progressmodeonly"))
        e = new cVeDrProgressModeonly();
    else if (!strcmp(name, "jump"))
        e = new cVeDrJump();
    else if (!strcmp(name, "onpause"))
        e = new cVeDrOnPause();
    else if (!strcmp(name, "onpausemodeonly"))
        e = new cVeDrOnPause();
    else if (!strcmp(name, "scrapercontent") && !strcmp(viewname, "displayreplay"))
        e = new cVeDrScraperContent();

    //displayvolume viewelements
    else if (!strcmp(name, "volume"))
        e = new cVeVolume();

    //displayvolume viewelements
    else if (!strcmp(name, "background") && !strcmp(viewname, "displayaudiotracks"))
        e = new cVeDtBackground();
    else if (!strcmp(name, "header") && !strcmp(viewname, "displayaudiotracks"))
        e = new cVeDtHeader();

    //default
    else {
        dsyslog("skindesigner: unknown view element %s", name);
        e = new cViewElement();
    }

    return e;
}

void cViewElement::SetGlobals(cGlobals *globals) {
    this->globals = globals;
    attribs->SetGlobals(globals);
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetGlobals(globals);
    }
}

void cViewElement::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    InheritTokenContainer();
}

bool cViewElement::Detached(void) {
    return detached;
}

void cViewElement::SetContainer(int x, int y, int width, int height) {
    container.SetX(x);
    container.SetY(y);
    container.SetWidth(width);
    container.SetHeight(height);
}

void cViewElement::SetAttributes(vector<stringpair> &attributes) { 
    attribs->Set(attributes); 
}

void cViewElement::AddArea(cAreaNode *area) {
    areaNodes.Add(area);
}

void cViewElement::SetAreaX(int x) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetX(x);
    }
}

void cViewElement::SetAreaY(int y) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetY(y);
    }
}

void cViewElement::SetAreaWidth(int width) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetWidth(width);
    }
}

void cViewElement::SetAreaHeight(int height) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetHeight(height);
    }
}

void cViewElement::SetPosition(int newX, int newY, int newWidth, int newHeight) {
    cRect newPos(newX, newY, newWidth, newHeight);
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetViewPort(newPos);
    }    
}

void cViewElement::Cache(void) {
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->Cache();
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
        node->Cache();
    }
}

bool cViewElement::Execute(void) {
    return attribs->DoExecute();
}

void cViewElement::Clear(bool forceClearBackground) {
    if (scrollingStarted)
        StopScrolling();
    tokenContainer->Clear();
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->StopBlinkers();
        sdOsd->Lock();
        node->Clear(forceClearBackground);
        sdOsd->Unlock();
    }
    dirty = false;
    drawn = false;
    scrollingStarted = false;
}

void cViewElement::Hide(void) {
    StopAnimation();
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        sdOsd->Lock();
        node->Hide();
        sdOsd->Unlock();
    }
    init = true;
    StopScrolling();
}

void cViewElement::Show(void) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        sdOsd->Lock();
        node->Show();
        sdOsd->Unlock();
    }
}

void cViewElement::WakeUp(void) {
    if (!detacher || !waitOnWakeup) {
        return;
    }
    detacher->WakeUp();
}

void cViewElement::Close(void) {
    StopAnimation();
    StopScrolling();
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->StopBlinkers();
        sdOsd->Lock();
        node->Close();
        sdOsd->Unlock();
    }
    dirty = true;
    init = true;
    startAnimation = true;
    restartAnimation = false;
    drawn = false;
    scrollingStarted = false;
    blocked = false;
}

void cViewElement::StopBlinking(void) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->StopBlinkers();
    }
}

void cViewElement::Render(void) {
    if (!dirty || blocked)
        return;

    if (attribs->DoDebug())
        Debug();
    
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        //Check redraw of already scrolling list element
        if (drawn && scrollingStarted && node->Scrolling()) {
            if (DoScroll()) {
                //current list element
                continue;
            } else {
                //not current list element anymore
                scrollingStarted = false;
            }
        }
        sdOsd->Lock();
        node->Clear();
        sdOsd->Unlock();
        if (!node->Execute())
            continue;
        if (node->BackgroundArea() && drawn)
            continue;
        sdOsd->Lock();
        node->Render();
        sdOsd->Unlock();
        
        if (DoScroll() && node->Scrolling()) {
            cArea *scrollArea = node->ScrollingArea();
            if (scrollArea) {
                scrollingStarted = true;
                cScroller *scroller = new cScroller(scrollArea);
                scrollers.push_back(scroller);
                cView::AddAnimation(scroller);                
            }
        }     
    }
    dirty = false;
    drawn = true;
    if (startAnimation || restartAnimation) {
        startAnimation = false;
        restartAnimation = false;
        StartAnimation();
    }
}

void cViewElement::StopScrolling(bool deletePixmaps) {
    for (list<cScroller*>::iterator it = scrollers.begin(); it != scrollers.end(); it++) {
        cView::RemoveAnimation(*it);
    }
    scrollers.clear();
}

void cViewElement::ParseDetached(void) {
    Parse(true);
}

void cViewElement::RenderDetached(void) {
    blocked = false;
    Render();
}

bool cViewElement::Shifting(void) {
    if (attribs->ShiftTime() > 0) {
        return true;
    }
    return false;
}

bool cViewElement::Fading(void) {
    if (attribs->FadeTime() > 0) {
        return true;
    }
    return false;
}

int cViewElement::FadeTime(void) {
    return attribs->FadeTime();
}

int cViewElement::ShiftTime(void) {
    return attribs->ShiftTime();
}

int cViewElement::ShiftMode(void) {
    int mode = attribs->ShiftMode();
    if (mode < 0) mode = 0;
    return mode;
}

void cViewElement::ShiftPositions(cPoint *start, cPoint *end) {
    cRect shiftbox = CoveredArea();
    cPoint startPoint = ShiftStart(shiftbox);
    start->Set(startPoint);
    end->Set(shiftbox.X(), shiftbox.Y());
}

void cViewElement::StartAnimation(void) {
    shifter = NULL;
    fader = NULL;
    if (ShiftTime() > 0) {
        shifter = new cShifter((cShiftable*)this);
        if (doAnimOut)
            shifter->SetPersistent();
        cView::AddAnimation(shifter, doStartAnim);
    } else if (FadeTime() > 0) {
        fader = new cFader((cFadable*)this);
        if (doAnimOut)
            fader->SetPersistent();
        cView::AddAnimation(fader, doStartAnim);
    }
}

void cViewElement::SetTransparency(int transparency, bool force) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        sdOsd->Lock();
        node->SetTransparency(transparency);
        sdOsd->Unlock();
    }
}

void cViewElement::SetPosition(cPoint &position, cPoint &reference, bool force) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        sdOsd->Lock();
        node->SetPosition(position, reference);
        sdOsd->Unlock();
    }
}

cRect cViewElement::CoveredArea(void) {
    cRect unionArea;
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        unionArea.Combine(node->CoveringArea());
    }
    return unionArea;
}

void cViewElement::Flush(void) {
    sdOsd->Flush();
}

bool cViewElement::Parse(bool forced) { 
    if (blocked && !forced) {
        return false;
    }
    if (!Detached() || !init) {
        return true;
    }
    delete detacher;
    bool isAnimated = (FadeTime() > 0) || (ShiftTime() > 0);
    detacher = new cDetacher((cDetachable*)this, waitOnWakeup, startAnimation && isAnimated);
    detacher->Start();
    startAnimation = false;
    init = false;
    blocked = true;
    return false;
}

cFunction *cViewElement::GetFunction(const char *name) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        if (cFunction *f = node->GetFunction(name))
            return f;
    }
    return NULL;
}

void cViewElement::Debug(bool full) {
    esyslog("skindesigner: ---> viewElement %d", id);
    tokenContainer->Debug();
    esyslog("skindesigner: container %d %d %dx%d", container.X(), container.Y(), container.Width(), container.Height());
    attribs->Debug();
    if (!full)
        return;
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->Debug(full);
    }    
}

/******************************************************************
* protected Functions
******************************************************************/
void cViewElement::InheritTokenContainer(void) {
    tokenContainer->CreateContainers();
    attribs->SetTokenContainer(tokenContainer);
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetTokenContainer(tokenContainer);
    }
}

void cViewElement::InheritTokenContainerDeep(void) {
    tokenContainer->CreateContainers();
    attribs->SetTokenContainerDeep(tokenContainer);
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        node->SetTokenContainerDeep(tokenContainer);
    }    
}

cPoint cViewElement::ShiftStart(cRect &shiftbox) {
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

void cViewElement::StopAnimation(void) {
    delete detacher;
    detacher = NULL;
    if (shifter)
        cView::RemoveAnimation(shifter);
    if (fader)
        cView::RemoveAnimation(fader);
}