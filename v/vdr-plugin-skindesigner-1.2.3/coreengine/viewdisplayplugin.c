#include "viewdisplayplugin.h"
#include "../config.h"
/***********************************************************
* cViewPlugin
***********************************************************/
cViewPlugin::cViewPlugin(int id, int plugId) {
    this->id = id;
    this->plugId = plugId;
    viewId = eViewType::DisplayPlugin;
    viewName = strdup("displayplugin");
    if (id == 0)
        numViews = plgManager->GetNumSubviews(plugId) + 1;
    else
        numViews = 1;
    views = new cViewPlugin*[numViews];
    views[0] = this;
    for (int i=1; i < numViews; i++) {
        views[i] = NULL;
    }
    numViewElements = plgManager->GetNumViewElements(plugId, id);
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    SetViewElements();
    numViewGrids = plgManager->GetNumViewGrids(plugId, id);
    viewGrids = new cViewGrid*[numViewGrids];
    for (int i=0; i < numViewGrids; i++) {
        viewGrids[i] = NULL;
    }
    SetViewGrids();
    tabView = NULL;
    viewChanged = true;
    newViewId = 0;
}

cViewPlugin::~cViewPlugin(void) {
    for (int i=0; i< numViewGrids; i++)
        delete viewGrids[i];
    delete[] viewGrids;
    delete tabView;
    for (int i=1; i < numViews; i++) {
        delete views[i];
    }
    delete[] views;
}

/***********************************************************
* Public Functions
***********************************************************/
bool cViewPlugin::ReadFromXML(const char *plugName, const char *tplName, cSdOsd *osd) {
    cString xmlFile = cString::sprintf("plug-%s-%s", plugName, tplName);
    cXmlParser parser;
    if (osd)
        parser.SetOsd(osd);
    else
        parser.SetOsd(&sdOsd);
    if (!parser.ReadView(this, *xmlFile)) {
        return false;
    }
    if (!parser.ParsePluginView()) {
        return false;
    }
    return true;
}

bool cViewPlugin::ReadSubViews(const char *plugName) {
    plgManager->InitPluginSubviewIterator(plugId);
    int svId = -1;
    string svTpl = "";
    while (plgManager->GetNextSubView(svId, svTpl)) {
        cViewPlugin *plugSubView = new cViewPlugin(svId, plugId);
        if (!plugSubView->ReadFromXML(plugName, svTpl.c_str(), &sdOsd))
            return false;
        views[svId] = plugSubView;
    }
    return true;
}


void cViewPlugin::SetGlobals(cGlobals *globals) {
    cView::SetGlobals(globals);
    for (int i=0; i < numViewGrids; i++) {
        if (viewGrids[i])
            viewGrids[i]->SetGlobals(globals);
    }
    for (int i=1; i < numViews; i++) {
        views[i]->SetGlobals(globals);
    }
    if (tabView) {
        tabView->SetGlobals(globals);
    }
}

void cViewPlugin::AddViewElement(cVePlugin *viewElement) {
    int veId = ViewElementId(viewElement->Name());
    if (veId == ATTR_UNKNOWN)
        return;
    viewElement->SetId(veId);
    viewElement->SetPluginId(plugId);
    viewElement->SetViewId(id);
    viewElement->SetTokenContainer();
    viewElements[veId] = viewElement;
}

void cViewPlugin::AddViewGrid(cViewGrid *viewGrid) {
    int gId = GridId(viewGrid->Name());
    if (gId == ATTR_UNKNOWN)
        return;
    viewGrid->SetId(gId);
    viewGrid->SetPluginId(plugId);
    viewGrid->SetViewId(id);
    viewGrid->SetTokenContainer();
    viewGrids[gId] = viewGrid;
}

void cViewPlugin::AddTab(cArea *tab) {
    if (!tabView) {
        tabView = new cPluginTabView(id, plugId);
        tabView->SetOsd(&sdOsd);
    }
    tabView->AddTab(tab);
}

void cViewPlugin::AddScrollbar(cVeDmScrollbar *scrollbar) {
    if (!tabView) {
        tabView = new cPluginTabView(id, plugId);
        tabView->SetOsd(&sdOsd);
    }
    int id = ViewElementId("scrollbar");
    scrollbar->SetId(id);
    scrollbar->SetTokenContainer();
    tabView->AddScrollbar(scrollbar);
}

void cViewPlugin::AddTablabels(cVeDmTablabels *tablabels) {
    if (!tabView) {
        tabView = new cPluginTabView(id, plugId);
        tabView->SetOsd(&sdOsd);
    }
    int id = ViewElementId("tablabels");
    tablabels->SetId(id);
    tablabels->SetTokenContainer();
    tabView->AddTablabels(tablabels);
}

void cViewPlugin::PreCache(void) {
    if (container.Width() > 0) {
        attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
        attribs->Cache();
    }
    cView::PreCache();
    for (int i=0; i < numViewGrids; i++) {
        if (viewGrids[i]) {
            viewGrids[i]->SetContainer(0, 0, attribs->Width(), attribs->Height());
            viewGrids[i]->PreCache();
        }
    }
    for (int i=1; i < numViews; i++) {
        views[i]->SetContainer(0, 0, attribs->Width(), attribs->Height());
        views[i]->PreCache();
    }
    if (tabView) {
        tabView->PreCache(0, 0, attribs->Width(), attribs->Height());
    }
}

cVePlugin *cViewPlugin::GetViewElement(int veId) {
    if (!viewElements[veId])
        return NULL;
    cVePlugin *ve = dynamic_cast<cVePlugin*>(viewElements[veId]);
    return ve;
}

cViewGrid *cViewPlugin::GetViewGrid(int gId) {
    return viewGrids[gId];
}

cPluginTabView *cViewPlugin::GetViewTab(void) {
    return tabView;    
}

void cViewPlugin::Hide(void) {
    for (int i=0; i < numViewGrids; i++) {
        if (viewGrids[i])
            viewGrids[i]->Hide();
    }
    for (int i=0; i < numViewElements; i++) {
        if (viewElements[i])
            viewElements[i]->Hide();
    }
}

void cViewPlugin::Show(void) {
    for (int i=0; i < numViewGrids; i++) {
        if (viewGrids[i])
            viewGrids[i]->Show();
    }
    for (int i=0; i < numViewElements; i++) {
        if (viewElements[i])
            viewElements[i]->Show();
    }
}
/***********************************************************
* libskindesigner api interface
***********************************************************/
bool cViewPlugin::InitOsd(void) {
    return Init();
}

void cViewPlugin::CloseOsd(void) {
    viewChanged = true;
    newViewId = 0;
    Close();
    for (int i=0 ; i < numViewGrids; i++) {
        if (viewGrids[i])
            viewGrids[i]->Close();
    }
    for (int i=1 ; i < numViews; i++) {
        if (views[i])
            views[i]->Close();
    }
}

void cViewPlugin::Deactivate(int viewId, bool hide) {
    for (int i=0; i < numViewElements; i++) {
        if (viewElements[i])
            viewElements[i]->StopScrolling(false);
    }
    if (hide)
        views[viewId]->Hide();
}

void cViewPlugin::Activate(int viewId) {
    viewChanged = true;
    newViewId = viewId;
    views[viewId]->Show();
}

void cViewPlugin::SetViewElementTokens(int veId, int viewId, skindesignerapi::cTokenContainer *tk) {
    cVePlugin *ve = views[viewId]->GetViewElement(veId);
    if (!ve)
        return;
    ve->Set(tk);
}

void cViewPlugin::ClearViewElement(int veId, int viewId) {
    cVePlugin *ve = views[viewId]->GetViewElement(veId);
    if (!ve)
        return;
    ve->StopScrolling();
    ve->Clear();
}

void cViewPlugin::DisplayViewElement(int veId, int viewId) {
    cVePlugin *ve = views[viewId]->GetViewElement(veId);
    if (!ve)
        return;
    if (ve->Parse())
        ve->Render();
}

void cViewPlugin::SetGrid(long gId, int viewId, int viewGridId,  double x, double y, double width, double height, skindesignerapi::cTokenContainer *tk){
    cViewGrid *grid = views[viewId]->GetViewGrid(viewGridId);
    if (!grid)
        return;
    grid->CheckSize(gId);
    grid->PositionGrid(gId, x, y, width, height);
    if (tk) {
        //only set tokens new if necessary
        grid->SetTokens(gId, tk);
    }
}

void cViewPlugin::SetGridCurrent(long gId, int viewId, int viewGridId, bool current) {
    if (gId < 0)
        return;
    cViewGrid *grid = views[viewId]->GetViewGrid(viewGridId);
    if (!grid)
        return;
    grid->SetCurrentGrid(gId, current);    
}

void cViewPlugin::DeleteGrid(long gId, int viewId, int viewGridId) {
    cViewGrid *grid = views[viewId]->GetViewGrid(viewGridId);
    if (!grid)
        return;
    grid->DeleteGrid(gId);
}

void cViewPlugin::DisplayGrids(int viewId, int viewGridId) {
    cViewGrid *grid = views[viewId]->GetViewGrid(viewGridId);
    if (!grid)
        return;
    grid->Render();
}

void cViewPlugin::ClearGrids(int viewId, int viewGridId) {
    cViewGrid *grid = views[viewId]->GetViewGrid(viewGridId);
    if (!grid)
        return;
    grid->ClearGrids();
}

void cViewPlugin::SetTabTokens(int viewId, skindesignerapi::cTokenContainer *tk) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->SetOsd(&sdOsd);
    tab->Set(tk);
}

void cViewPlugin::TabLeft(int viewId) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->KeyLeft(); 
}

void cViewPlugin::TabRight(int viewId) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->KeyRight(); 
}

void cViewPlugin::TabUp(int viewId) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->KeyUp(); 
}

void cViewPlugin::TabDown(int viewId) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->KeyDown(); 
}

void cViewPlugin::DisplayTabs(int viewId) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->Render();  
}

void cViewPlugin::ClearTab(int viewId) {
    cPluginTabView *tab = views[viewId]->GetViewTab();
    if (!tab)
        return;
    tab->Clear(); 
}

void cViewPlugin::Flush(void) {
    if (viewChanged) {
        viewChanged = false;
        newTvFrame = views[newViewId]->GetTvFrame();
        menuInit = true;
    }
    cView::Flush();
}

bool cViewPlugin::ChannelLogoExists(string channelId) {
    return imgCache->LogoExists(channelId);
}

string cViewPlugin::GetEpgImagePath(void) {
    return *config.epgImagePath;
}

/***********************************************************
* Private Functions
***********************************************************/
void cViewPlugin::SetViewElements(void) {
    plgManager->InitViewElementIterator(plugId, id);
    int veId = -1;
    string veName = "";
    while (plgManager->GetNextViewElement(veId, veName)) {
        viewElementNames.insert(pair<string, int>(veName, veId));
    }
}

void cViewPlugin::SetViewGrids(void) {
    plgManager->InitViewGridIterator(plugId, id);
    int gId = -1;
    string gName = "";
    while (plgManager->GetNextViewGrid(gId, gName)) {
        gridNames.insert(pair<string, int>(gName, gId));
    }
}

int cViewPlugin::GridId(const char *name) {
    map<string, int>::iterator hit = gridNames.find(name);
    if (hit != gridNames.end())
        return (int)hit->second;
    return ATTR_UNKNOWN;    
}

/***********************************************************
* cPluginTabView
***********************************************************/
cPluginTabView::cPluginTabView(int viewId, int plugId) {
    sdOsd = NULL;
    init = true;
    drawScrollbar = true;
    scrollbar = NULL;
    tablabels = NULL;
    detailView = new cViewDetailAdvancedPlugin(viewId, plugId);
    firstTab = true;
}

cPluginTabView::~cPluginTabView(void) {
    delete scrollbar;
    delete tablabels;
    delete detailView;
}

void cPluginTabView::SetGlobals(cGlobals *globals) {
    detailView->SetGlobals(globals);
    if (scrollbar)
        scrollbar->SetGlobals(globals);
    if (tablabels)
        tablabels->SetGlobals(globals);
}

void cPluginTabView::AddTab(cArea *tab) {
    if (firstTab) {
        tab->SetActiveTab(true);
        firstTab = false;
    }
    detailView->AddArea(tab);
}

void cPluginTabView::AddScrollbar(cVeDmScrollbar *scrollbar) {
    this->scrollbar = scrollbar;
}

void cPluginTabView::AddTablabels(cVeDmTablabels *tablabels) {
    this->tablabels = tablabels;
}

void cPluginTabView::PreCache(int containerX, int containerY, int containerWidth, int containerHeight) {
    detailView->SetOsd(sdOsd);
    detailView->SetTokenContainer();
    detailView->SetContainer(containerX, containerY, containerWidth, containerHeight);
    detailView->Cache();
    if (scrollbar) {
        scrollbar->SetContainer(containerX, containerY, containerWidth, containerHeight);
        scrollbar->Cache();
    }
    if (tablabels) {
        tablabels->SetContainer(containerX, containerY, containerWidth, containerHeight);
        tablabels->Cache();        
    }
}

void cPluginTabView::Set(skindesignerapi::cTokenContainer *tk) {
    detailView->Set(tk);
}

void cPluginTabView::Render(void) {
    detailView->Render();
    if (drawScrollbar) {
        drawScrollbar = false;
        DrawScrollbar();
    }
    if (!init)
        return;
    init = false;
    vector<const char*> tabs;
    int activeTab = detailView->GetTabs(tabs);
    if (tablabels) {        
        tablabels->SetTabs(tabs);
        tablabels->SetActiveTab(activeTab);
        if (tablabels->Parse())
            tablabels->Render();
    }
}

void cPluginTabView::Clear(void) {
    init = true;
    drawScrollbar = true;
    detailView->ResetTabs();
    detailView->Close();
    if (tablabels)
        tablabels->Close();
    if (scrollbar)
        scrollbar->Close();
}

void cPluginTabView::KeyLeft(void) {
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
        sdOsd->Flush();
        drawScrollbar = true;
    } else {
        //scroll page
        if (detailView->ScrollUp(true)) {
            drawScrollbar = true;
        }
    }
}

void cPluginTabView::KeyRight(void) {
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
        sdOsd->Flush();
        drawScrollbar = true;
    } else {
        //scroll page
        if (detailView->ScrollDown(true)) {
            drawScrollbar = true;
        }
    }
}

void cPluginTabView::KeyUp(void) {
    if (detailView->ScrollUp()) {
        drawScrollbar = true;
    }
}

void cPluginTabView::KeyDown(void) {
    if (detailView->ScrollDown()) {
        drawScrollbar = true;
    }
}

void cPluginTabView::DrawScrollbar(void) {
    if (!scrollbar)
        return;
    int barheight = 0;
    int offset = 0;
    bool end = true;
    detailView->Scrollbar(barheight, offset, end);
    scrollbar->SetDetail(barheight, offset, end);
    scrollbar->Render();    
}

