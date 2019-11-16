#include "osdelements.h"

/**********************************************************************
* cOsdElement
**********************************************************************/
skindesignerapi::cOsdElement::cOsdElement(skindesignerapi::ISkinDisplayPlugin *view, int viewId) {
    this->view = view;
    this->viewId = viewId;
    tk = NULL;
}

skindesignerapi::cOsdElement::~cOsdElement() {
}

void skindesignerapi::cOsdElement::ClearTokens(void) {
    tk->Clear();
}

int skindesignerapi::cOsdElement::GetLoopIndex(const char *loop) {
    return tk->LoopIndex(loop);
}

void skindesignerapi::cOsdElement::SetLoop(vector<int> loopInfo) {
    tk->CreateLoopTokenContainer(&loopInfo);
}

void skindesignerapi::cOsdElement::AddStringToken(int index, const char *value) {
    tk->AddStringToken(index, value);
}

void skindesignerapi::cOsdElement::AddIntToken(int index, int value) {
    tk->AddIntToken(index, value);
}

void skindesignerapi::cOsdElement::AddLoopToken(int loopIndex, int row, int index, const char *value) {
    tk->AddLoopToken(loopIndex, row, index, value);
}

bool skindesignerapi::cOsdElement::ChannelLogoExists(string channelId) {
    return view->ChannelLogoExists(channelId);
}

string skindesignerapi::cOsdElement::GetEpgImagePath(void) {
    return view->GetEpgImagePath();
}

void skindesignerapi::cOsdElement::DebugTokenContainer(void) {
    tk->Debug();
}

/**********************************************************************
* cViewElement
**********************************************************************/
skindesignerapi::cViewElement::cViewElement(skindesignerapi::ISkinDisplayPlugin *view, int viewId, int viewElementId) : cOsdElement(view, viewId) {
    this->viewElementId = viewElementId;
}

skindesignerapi::cViewElement::~cViewElement() {
    view->ClearViewElement(viewElementId, viewId);
}

void skindesignerapi::cViewElement::Clear(void) {
    view->ClearViewElement(viewElementId, viewId);
}

void skindesignerapi::cViewElement::Display(void) {
    view->SetViewElementTokens(viewElementId, viewId, tk);
    view->DisplayViewElement(viewElementId, viewId);
}

/**********************************************************************
* cViewGrid
**********************************************************************/
skindesignerapi::cViewGrid::cViewGrid(skindesignerapi::ISkinDisplayPlugin *view, int viewId, int viewGridId) : cOsdElement(view, viewId) {
    this->viewGridId = viewGridId;
}

skindesignerapi::cViewGrid::~cViewGrid() {
    view->ClearGrids(viewId, viewGridId);
}

void skindesignerapi::cViewGrid::SetGrid(long gridId, double x, double y, double width, double height) {
    view->SetGrid(gridId, viewId, viewGridId, x, y, width, height, tk);
}

void skindesignerapi::cViewGrid::SetCurrent(long gridId, bool current) {
    view->SetGridCurrent(gridId, viewId, viewGridId, current);
}

void skindesignerapi::cViewGrid::MoveGrid(long gridId, double x, double y, double width, double height) {
    view->SetGrid(gridId, viewId, viewGridId, x, y, width, height, NULL);
}

void skindesignerapi::cViewGrid::Delete(long gridId) {
    view->DeleteGrid(gridId, viewId, viewGridId);
}

void skindesignerapi::cViewGrid::Clear(void) {
    view->ClearGrids(viewId, viewGridId);
}

void skindesignerapi::cViewGrid::Display(void) {
    view->DisplayGrids(viewId, viewGridId);
}

/**********************************************************************
* cViewTab
**********************************************************************/
skindesignerapi::cViewTab::cViewTab(skindesignerapi::ISkinDisplayPlugin *view, int viewId) : cOsdElement(view, viewId) {
}

skindesignerapi::cViewTab::~cViewTab() {
    view->ClearTab(viewId);
}

void skindesignerapi::cViewTab::Init(void) {
    view->SetTabTokens(viewId, tk);
}

void skindesignerapi::cViewTab::Left(void) {
    view->TabLeft(viewId);
}

void skindesignerapi::cViewTab::Right(void) {
    view->TabRight(viewId);
}

void skindesignerapi::cViewTab::Up(void) {
    view->TabUp(viewId);
}

void skindesignerapi::cViewTab::Down(void) {
    view->TabDown(viewId);
}

void skindesignerapi::cViewTab::Display(void) {
    view->DisplayTabs(viewId);
}

/**********************************************************************
* cOsdView
**********************************************************************/
skindesignerapi::cOsdView::cOsdView(skindesignerapi::cPluginStructure *plugStruct, skindesignerapi::ISkinDisplayPlugin *displayPlugin, int viewId) {
    this->plugStruct = plugStruct;
    this->displayPlugin = displayPlugin;
    this->viewId = viewId;
}

skindesignerapi::cOsdView::~cOsdView() {
    if (displayPlugin && (viewId == 0))
        displayPlugin->CloseOsd();
}

void skindesignerapi::cOsdView::Deactivate(bool hide) {
    if (!displayPlugin)
        return;
    displayPlugin->Deactivate(viewId, hide);
}

void skindesignerapi::cOsdView::Activate(void) {
    if (!displayPlugin)
        return;
    displayPlugin->Activate(viewId);
}

skindesignerapi::cViewElement *skindesignerapi::cOsdView::GetViewElement(int viewElementId) {
    if (!displayPlugin)
        return NULL;
    cTokenContainer *tk = plugStruct->GetTokenContainerVE(viewId, viewElementId);
    if (!tk)
        return NULL;
    skindesignerapi::cViewElement *ve = new cViewElement(displayPlugin, viewId, viewElementId);
    ve->SetTokenContainer(tk);
    return ve;
}

skindesignerapi::cViewGrid *skindesignerapi::cOsdView::GetViewGrid(int viewGridId) {
    if (!displayPlugin)
        return NULL;
    cTokenContainer *tk = plugStruct->GetTokenContainerGE(viewId, viewGridId);
    if (!tk)
        return NULL;
    skindesignerapi::cViewGrid *ge = new cViewGrid(displayPlugin, viewId, viewGridId);
    ge->SetTokenContainer(tk);
    return ge;
}

skindesignerapi::cViewTab *skindesignerapi::cOsdView::GetViewTabs(void) {
    if (!displayPlugin)
        return NULL;
    cTokenContainer *tk = plugStruct->GetTokenContainerTab(viewId);
    if (!tk)
        return NULL;
    skindesignerapi::cViewTab *tab = new cViewTab(displayPlugin, viewId);
    tab->SetTokenContainer(tk);
    return tab;
}

void skindesignerapi::cOsdView::Display(void) {
    if (displayPlugin)
        displayPlugin->Flush();
}
