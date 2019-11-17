#include "area.h"
#include "../config.h"

/******************************************************************
* cAreaNode
******************************************************************/
cAreaNode::cAreaNode(void) {
    globals = NULL;
    isTab = false;
    activeTab = false;
}

cAreaNode::~cAreaNode(void) {
}

void cAreaNode::SetContainer(int x, int y, int width, int height) {
    container.SetX(x);
    container.SetY(y);
    container.SetWidth(width);
    container.SetHeight(height);
}

/******************************************************************
* cArea
******************************************************************/
cArea::cArea(void) {
    sdOsd = NULL;
    init = true;
    isBackgroundArea = false;
    attribs = new cAreaAttribs((int)eAreaAttribs::count);
    scrolling = false;
    isScrolling = false;
    scrollingStarted = false;
    scrollFunc = NULL;
    blinking = false;
    areaContainer = NULL;
    pix = NULL;
}

cArea::cArea(const cArea &other) {
    sdOsd = other.sdOsd;
    init = true;
    isBackgroundArea = false;
    pix = NULL;
    globals = other.globals;
    attribs = new cAreaAttribs(*other.attribs);
    //area container is set from outside during cloning of areacontainer
    areaContainer = NULL;
    scrollingStarted = false;
    //scrolling is set from outside by ScrollFunc(), see below
    scrolling = other.scrolling;
    isScrolling = false;
    blinking = false;
    scrollFunc = NULL;

    for (const cFunction *func = other.functions.First(); func; func = other.functions.Next(func)) {
        if (cFuncFill *f = dynamic_cast<cFuncFill*>((cFunction*)func)) {
            cFuncFill *fFill = new cFuncFill(*f);
            fFill->SetOwner(this);
            functions.Add(fFill);
        } else if (cFuncDrawRectangle *f = dynamic_cast<cFuncDrawRectangle*>((cFunction*)func)) {
            cFuncDrawRectangle *fDrawRect = new cFuncDrawRectangle(*f);
            fDrawRect->SetOwner(this);
            functions.Add(fDrawRect);
        } else if (cFuncDrawEllipse *f = dynamic_cast<cFuncDrawEllipse*>((cFunction*)func)) {
            cFuncDrawEllipse *fDrawEllipse = new cFuncDrawEllipse(*f);
            fDrawEllipse->SetOwner(this);
            functions.Add(fDrawEllipse);
        } else if (cFuncDrawSlope *f = dynamic_cast<cFuncDrawSlope*>((cFunction*)func)) {
            cFuncDrawSlope *fDrawSlope = new cFuncDrawSlope(*f);
            fDrawSlope->SetOwner(this);
            functions.Add(fDrawSlope);
        } else if (cFuncDrawText *f = dynamic_cast<cFuncDrawText*>((cFunction*)func)) {
            cFuncDrawText *fDrawText = new cFuncDrawText(*f);
            fDrawText->SetOwner(this);
            functions.Add(fDrawText);
        } else if (cFuncDrawTextVertical *f = dynamic_cast<cFuncDrawTextVertical*>((cFunction*)func)) {
            cFuncDrawTextVertical *fDrawTextVertical = new cFuncDrawTextVertical(*f);
            fDrawTextVertical->SetOwner(this);
            functions.Add(fDrawTextVertical);
        } else if (cFuncDrawTextBox *f = dynamic_cast<cFuncDrawTextBox*>((cFunction*)func)) {
            cFuncDrawTextBox *fDrawTextBox = new cFuncDrawTextBox(*f);
            fDrawTextBox->SetOwner(this);
            functions.Add(fDrawTextBox);
        } else if (cFuncDrawImage *f = dynamic_cast<cFuncDrawImage*>((cFunction*)func)) {
            cFuncDrawImage *fDrawImage = new cFuncDrawImage(*f);
            fDrawImage->SetOwner(this);
            functions.Add(fDrawImage);
        } else {
            esyslog("skindesigner: ERROR: unknown function in area cloning!!!");
        }
    }
    
    //func references have to be set from outside if already cached clone is wanted
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->CacheFuncReferences();
    }
    if (scrolling) {
        SetScrollFunc();
    }
}

cArea::~cArea(void) {
    StopBlinkers();
    delete attribs;
}

void cArea::SetGlobals(cGlobals *globals) {
    this->globals = globals;
    attribs->SetGlobals(globals);
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetGlobals(globals);
    }
}

void cArea::SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) {
    attribs->SetTokenContainer(tokenContainer);
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetTokenContainer(tokenContainer);
    }
}

void cArea::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    attribs->SetTokenContainerDeep(tokenContainer);
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetTokenContainerDeep(tokenContainer);
    }
}

void cArea::SetAttributes(vector<stringpair> &attributes) {
    attribs->Set(attributes); 
}

bool cArea::ValidFunction(const char *func) {
    if (!strcmp(func, "fill")) return true;
    if (!strcmp(func, "drawtext")) return true;
    if (!strcmp(func, "drawtextbox")) return true;
    if (!strcmp(func, "drawtextvertical")) return true;
    if (!strcmp(func, "drawimage")) return true;
    if (!strcmp(func, "drawrectangle")) return true;
    if (!strcmp(func, "drawellipse")) return true;
    if (!strcmp(func, "drawslope")) return true;
    esyslog("skindesigner: unknown function \"%s\"", func);
    return false;
}

cFunction *cArea::AddFunction(const char *name, vector<stringpair> attribs, cFuncLoop *loopFunc) {
    cFunction *f = NULL;
    if (!strcmp(name, "fill")) {
        f = new cFuncFill(this, (int)eFillAttribs::count);
    } else if (!strcmp(name, "drawrectangle")) {
        f = new cFuncDrawRectangle(this, (int)eDrawRectangleAttribs::count);
    } else if (!strcmp(name, "drawellipse")) {
        f = new cFuncDrawEllipse(this, (int)eDrawEllipseAttribs::count);
    } else if (!strcmp(name, "drawslope")) {
        f = new cFuncDrawSlope(this, (int)eDrawSlopeAttribs::count);
    } else if (!strcmp(name, "drawtext")) {
        f = new cFuncDrawText(this, (int)eDrawTextAttribs::count);        
    } else if (!strcmp(name, "drawtextvertical")) {
        f = new cFuncDrawTextVertical(this, (int)eDrawTextAttribs::count);        
    } else if (!strcmp(name, "drawtextbox")) {
        f = new cFuncDrawTextBox(this, (int)eDrawTextBoxAttribs::count);        
    } else if (!strcmp(name, "drawimage")) {
        f = new cFuncDrawImage(this, (int)eDrawImageAttribs::count);        
    } else if (!strcmp(name, "loop")) {
        f = new cFuncLoop(this, (int)eLoopAttribs::count);
    }
    if (!f) {
        esyslog("skindesigner: TODO: function \"%s\" not implemented", name);
        return NULL;
    }
    f->Set(attribs);
    if (!loopFunc)
        functions.Add(f);
    else
        loopFunc->AddFunction(f);
    return f;
}

cFunction *cArea::GetFunction(const char *name) {
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        const char *funcName = f->Name();
        if (funcName && !strcmp(funcName, name)) {
            return f;
        }
        cFuncLoop *loopFunc = dynamic_cast<cFuncLoop*>(f);
        if (loopFunc) {
            cFunction *lf = loopFunc->GetFunction(name);
            if (lf)
                return lf;
        }
    }
    if (!areaContainer)
        return NULL;
    return areaContainer->GetFunction(name);
}

void cArea::SetX(int x) {
    attribs->SetX(x);
}

void cArea::SetY(int y) {
    attribs->SetY(y);
}

void cArea::SetWidth(int width) {
    attribs->SetWidth(width);
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetContainer(0, 0, attribs->Width(), attribs->Height());
    }
}

void cArea::SetHeight(int height) {
    attribs->SetHeight(height);
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetContainer(0, 0, attribs->Width(), attribs->Height());
    }
}

void cArea::Cache(void) {
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->Cache();
    attribs->CheckDynamic();
    isBackgroundArea = attribs->BackgroundArea();

    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (!attribs->Dynamic()) {
            f->SetContainer(0, 0, attribs->Width(), attribs->Height());
        } else {
            f->SetContainer(0, 0, -1, -1);            
        }
        f->Cache();
    }
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->CacheFuncReferences();
    }
    if (scrolling) {
        SetScrollFunc();
    }
}

void cArea::Close(void) {
    StopBlinkers();
    if (pix) {
        sdOsd->DestroyPixmap(pix);
        pix = NULL;
    }
    init = true;
    scrollingStarted = false;
}

void cArea::Clear(bool forceClearBackground) {
    if (!init && isBackgroundArea && !forceClearBackground) {
        return;
    }
    StopBlinkers();
    if (pix) {
        pix->SetDrawPortPoint(cPoint(0,0));
        pix->Fill(clrTransparent);
    }
    scrollingStarted = false;
}

void cArea::ClearWithoutIndicators(void) {
    if (attribs->IndicatorArea()) {
        return;
    }
    StopBlinkers();
    if (pix) {
        pix->Fill(clrTransparent);
    }
    scrollingStarted = false;
}

void cArea::Hide(void) {
    StopBlinkers();
    if (pix) {
        pix->SetLayer(-1);
    }
}

void cArea::Show(void) {
    StartBlinkers();
    if (pix) {
        pix->SetLayer(attribs->Layer());
    }
}

void cArea::Render(void) {
    if (attribs->DoDebug())
        Debug();

    if (init) {
        InitFunctions();
        init = false;    
    }

    if (!isScrolling && scrollFunc && attribs->Orientation() == (int)eOrientation::horizontal) {
        scrollFunc->Scrolling(false);
    } else if (isScrolling && scrollFunc && attribs->Orientation() == (int)eOrientation::horizontal) {
        scrollFunc->Scrolling(true);
    }

    if (!pix) {
        if (!IsTab())
            CreatePixmap();
        else {
            int overlap = ScrollHeight();
            if (overlap > 0) {
                cRect drawport;
                drawport.SetX(0);
                drawport.SetY(0);
                drawport.SetWidth(attribs->Width());
                drawport.SetHeight(overlap + attribs->Height());
                CreatePixmap(drawport);
            } else {
                CreatePixmap();
            }
        }
    }

    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (f->DoDebug())
            f->Debug();
        if (!f->DoExecute())
            continue;
        if (pix) {
            f->Render(pix);
        }
    }

    StartBlinkers();
}

bool cArea::Execute(void) { 
    return attribs->DoExecute(); 
}

void cArea::SetTransparency(int transparency, bool absolute) {
    if (transparency < 0 || transparency > 100)
        return;
    int alpha = (100 - transparency)*255/100;
    if (!absolute) {
        int pixTransparency = attribs->Transparency();
        if (pixTransparency > 0) {
            alpha = (100 - pixTransparency) * alpha / 100;            
        }
    }
    if (pix) {
        pix->SetAlpha(alpha);
    }
}

void cArea::SetIndicatorTransparency(int transparency) {
    if (!attribs->IndicatorArea())
        return;
    if (transparency < 0 || transparency > 100)
        return;
    int alpha = (100 - transparency)*255/100;
    if (pix) {
        pix->SetAlpha(alpha);
    }
}

void cArea::SetIndicatorPosition(cPoint &pos) {
    if (!attribs->IndicatorArea())
        return;
    SetDrawPort(pos);
}

bool cArea::Scrolling(void) { 
    if (!scrolling)
        return false;
    if (!Execute())
        return false;
    if (ScrollOrientation() == eOrientation::horizontal) {
        if (!scrollFunc)
            return false;
        if (scrollFunc->X() + scrollFunc->FuncWidth() > attribs->Width())
            return true;
    } else if (ScrollOrientation() == eOrientation::vertical) {
        int maxHeight = 0;
        for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
            int funcHeight = f->FuncY() + f->FuncHeight();
            if (funcHeight > maxHeight)
                maxHeight = funcHeight;
        }
        if (maxHeight > attribs->Height())
            return true;
    }
    return false;
}

int cArea::ScrollWidth(void) {
    if (!scrollFunc)
        return 0;
    return scrollFunc->X() + scrollFunc->FuncWidth() + 10 - attribs->Width();
}

int cArea::ScrollHeight(void) {
    int maxHeight = 0;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (!f->DoExecute())
            continue;
        int funcHeight = f->FuncY() + f->FuncHeight();
        if (funcHeight > maxHeight)
            maxHeight = funcHeight;
    }
    return maxHeight - attribs->Height();
}
  
int cArea::ScrollDelay(void) { 
    return attribs->Delay(); 
}

eScrollMode cArea::ScrollMode(void) {
    return (eScrollMode)attribs->Mode(); 
}

eScrollSpeed cArea::ScrollSpeed(void) {
    return (eScrollSpeed)attribs->ScrollSpeed();
}

eOrientation cArea::ScrollOrientation(void) {
    return (eOrientation)attribs->Orientation();
}

void cArea::StartScrolling(void) {
    cRect drawport;
    drawport.SetX(0);
    drawport.SetY(0);
    if (ScrollOrientation() == eOrientation::horizontal) {
        drawport.SetWidth(ScrollWidth() + attribs->Width());
        drawport.SetHeight(attribs->Height());
    } else if (ScrollOrientation() == eOrientation::vertical) {
        drawport.SetWidth(attribs->Width());
        drawport.SetHeight(ScrollHeight() + attribs->Height() + 10);
    }
    isScrolling = true;
    CreatePixmap(drawport);
    Render();
}

void cArea::StopScrolling(void) {
    isScrolling = false;
    if (pix && !(pix->ViewPort().Size() == pix->DrawPort().Size())) {
        sdOsd->DestroyPixmap(pix);
        pix = NULL;        
    }
}

void cArea::SetViewPort(cRect &vp) {
    if (!pix)
        return;
    pix->SetViewPort(vp);
}

void cArea::SetPosition(cPoint &pos, cPoint &ref) {
    if (!pix)
        return;
    int x0 = attribs->X() == -1 ? 0 : attribs->X();
    int y0 = attribs->Y() == -1 ? 0 : attribs->Y();    
    int x = (x0 - ref.X()) + pos.X();
    int y = (y0 - ref.Y()) + pos.Y();
/* Enable for xineliboutput OSD Bug
    if (x < 0) x = 0;
    if (y < 0) y = 0;
*/
    pix->SetViewPort(cRect(x, y, pix->ViewPort().Width(), pix->ViewPort().Height()));
}

void cArea::SetDrawPort(cPoint &point) {
    if (!pix)
        return;
    pix->SetDrawPortPoint(point);
}

cRect cArea::ViewPort(void) {
    if (!pix)
        return cRect::Null;
    cRect vp = pix->ViewPort();
    return vp;
}

cRect cArea::CoveringArea(void) {
    return ViewPort();
}

cRect cArea::DrawPort(void) {
    if (!pix)
        return cRect::Null;
    cRect dp = pix->DrawPort();
    return dp;
}

int cArea::BlinkFreq(int func) {
    cFunction *blinkFunc = functions.Get(func);
    if (!blinkFunc)
        return -1;
    return blinkFunc->BlinkFreq();
}

void cArea::DoBlink(int func, bool on) {
    cFunction *blinker = functions.Get(func);
    if (!blinker)
        return;
    if (on) {
        if (pix) {
            blinker->Render(pix);
        }
    } else {
        cRect blinkRect = cRect(blinker->GetX((eAlign)blinker->Align(), 0, 0), 
                                blinker->GetY((eAlign)blinker->Valign(), 0, 0), 
                                blinker->FuncWidth(), 
                                blinker->FuncHeight());
        if (pix) {
            pix->DrawRectangle(blinkRect, clrTransparent);
        }
    }
}

void cArea::Debug(bool full) {
    esyslog("skindesigner:    --> area");
    esyslog("skindesigner:    container %d %d %dx%d", container.X(), container.Y(), container.Width(), container.Height());
    attribs->Debug();
    if (!full)
        return;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->Debug();
    }
}

/******************************************************************
* Private Functions
******************************************************************/
void cArea::InitFunctions(void) {
    if (!attribs->Dynamic())
        return;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetContainer(0, 0, attribs->Width(), attribs->Height());
    }
}

void cArea::CreatePixmap(cRect drawPort) {
    if (pix) {
        sdOsd->DestroyPixmap(pix);
        pix = NULL;
    }
    if (attribs->Width() <=0 || attribs->Height() <= 0) {
        return;
    }

    int layer = attribs->Layer();
    cRect viewPort(attribs->X() == -1 ? 0 : attribs->X(), attribs->Y() == -1 ? 0 : attribs->Y(), attribs->Width(), attribs->Height());
    pix = sdOsd->CreatePixmap(layer, viewPort, drawPort);
    if (pix)
        pix->Clear();
    int pixTransparency = attribs->Transparency();
    if (pixTransparency > 0) {
        SetTransparency(pixTransparency, true);
    }
}

void cArea::SetScrollFunc(void) {
    //if area has only one function, take this anyway
    if (functions.Count() == 1) {
        scrollFunc = functions.First();
        return;
    }
    //else use scrollelement name
    const char *scrollFuncName = attribs->GetScrollElement();
    if (!scrollFuncName)
        return;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (!f->Name())
            continue;
        if (!strcmp(f->Name(), scrollFuncName)) {
            scrollFunc = f;
            return;
        }
    }
}

void cArea::StartBlinkers(void) {
    if (blinking)
        return;
    blinking = true;
    int func = 0;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (!f->DoExecute()) {
            func++;
            continue;
        }
        if (f->Blinking()) {
            cBlinker *blinker = new cBlinker((cBlinkable*)this, func);
            blinkers.push_back(blinker);
            cView::AddAnimation(blinker);
        }
        func++;
    }
}

void cArea::StopBlinkers(void) {
    blinking = false;
    for (list<cBlinker*>::iterator it = blinkers.begin(); it != blinkers.end(); it++) {
        cView::RemoveAnimation(*it);
    }
    blinkers.clear();
}

/******************************************************************
* cAreaContainer
******************************************************************/
cAreaContainer::cAreaContainer(void) {
    attribs = new cAreaContainerAttribs((int)eAreaContainerAttribs::count);
}

cAreaContainer::cAreaContainer(const cAreaContainer &other) {
    globals = other.globals;
    attribs = new cAreaContainerAttribs(*other.attribs);
    for (const cArea *area = other.areas.First(); area; area = other.areas.Next(area)) {
        cArea *a = new cArea(*area);
        a->SetAreaContainer(this);
        areas.Add(a);
    }
}

cAreaContainer::~cAreaContainer(void) {
    delete attribs;
}
    
void cAreaContainer::SetAttributes(vector<stringpair> &attributes) {
    attribs->Set(attributes); 
}

void cAreaContainer::SetGlobals(cGlobals *globals) {
    this->globals = globals;
    attribs->SetGlobals(globals);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetGlobals(globals);
    }
}

void cAreaContainer::SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) {
    attribs->SetTokenContainer(tokenContainer);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetTokenContainer(tokenContainer);
    }
}

void cAreaContainer::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    attribs->SetTokenContainerDeep(tokenContainer);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetTokenContainerDeep(tokenContainer);
    }
}

void cAreaContainer::AddArea(cArea *area) {
    area->SetAreaContainer(this);
    areas.Add(area);
}

cFunction *cAreaContainer::GetFunction(const char *name) {
    cFunction *fRef = NULL;
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        fRef = area->GetFunction(name);
        if (fRef)
            return fRef;
    }
    return NULL;
}

void cAreaContainer::SetX(int x) {
    attribs->SetX(x);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetX(x);
    }
}

void cAreaContainer::SetY(int y) {
    attribs->SetY(y);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetY(y);
    }
}

void cAreaContainer::SetWidth(int width) {
    attribs->SetWidth(width);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetWidth(width);
    }
}

void cAreaContainer::SetHeight(int height) {
    attribs->SetHeight(height);
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetHeight(height);
    }
}

void cAreaContainer::Cache(void) {
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->Cache();
    int x = attribs->X() > -1 ? attribs->X() : container.X();
    int y = attribs->Y() > -1 ? attribs->Y() : container.Y();
    int width = attribs->Width() > -1 ? attribs->Width() : container.Width();
    int height = attribs->Height() > -1 ? attribs->Height() : container.Height();
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetContainer(x, y, width, height);
        if (attribs->Width() > -1) {
            area->SetWidth(width);
        }
        if (attribs->Height() > -1) {
            area->SetHeight(height);
        }
        area->Cache();
    }
}

void cAreaContainer::Close(void) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->Close();
    }
}

void cAreaContainer::Clear(bool forceClearBackground) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->Clear(forceClearBackground);
    }
}

void cAreaContainer::ClearWithoutIndicators(void) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->ClearWithoutIndicators();
    }
}

void cAreaContainer::Hide(void) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->Hide();
    }
}

void cAreaContainer::Show(void) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->Show();
    }
}

void cAreaContainer::Render(void) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        if (area->Execute())
            area->Render();
    }
}

bool cAreaContainer::Execute(void) { 
    return attribs->DoExecute(); 
}

void cAreaContainer::SetTransparency(int transparency, bool absolute) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetTransparency(transparency, absolute);
    }
}

void cAreaContainer::SetIndicatorTransparency(int transparency) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetIndicatorTransparency(transparency);
    }    
}

void cAreaContainer::SetIndicatorPosition(cPoint &pos) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetIndicatorPosition(pos);
    }    
}

void cAreaContainer::SetViewPort(cRect &vp) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetViewPort(vp);
    }    
}

void cAreaContainer::SetPosition(cPoint &pos, cPoint &ref) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->SetPosition(pos, ref);
    }    
}

cRect cAreaContainer::CoveringArea(void) {
    cRect unionArea;
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        unionArea.Combine(area->CoveringArea());
    }
    return unionArea;
}

bool cAreaContainer::Scrolling(void) {
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        if (area->Scrolling())
            return true;
    }
    return false;
}

cArea *cAreaContainer::ScrollingArea(void) { 
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        if (area->Execute() && area->Scrolling())
            return area;
    }
    return NULL;
}

void cAreaContainer::Debug(bool full) {
    esyslog("skindesigner:    --> area container");
    esyslog("skindesigner:    container %d %d %dx%d", container.X(), container.Y(), container.Width(), container.Height());
    attribs->Debug();
    if (!full)
        return;
    for (cArea *area = areas.First(); area; area = areas.Next(area)) {
        area->Debug(full);
    }
}
