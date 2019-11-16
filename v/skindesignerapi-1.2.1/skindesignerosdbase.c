#include "skindesignerosdbase.h"

/**********************************************************************
* cSkindesignerOsdObject
**********************************************************************/

skindesignerapi::cSkindesignerOsdObject::cSkindesignerOsdObject(cPluginStructure *plugStruct) {
    this->plugStruct = plugStruct;
    init = true;
}

skindesignerapi::cSkindesignerOsdObject::~cSkindesignerOsdObject() {
}

skindesignerapi::cOsdView *skindesignerapi::cSkindesignerOsdObject::GetOsdView(int subViewId) {
    ISkinDisplayPlugin *displayPlugin = SkindesignerAPI::GetDisplayPlugin(plugStruct->id);
    if (!displayPlugin)
        return NULL;
    if (init) {
        init = false;
        if (!displayPlugin->InitOsd()) {
            esyslog("skindesignerapi: error opening osd");
            return NULL;
        }
    }
    cOsdView *view = NULL;
    if (subViewId > -1)
        view = new cOsdView(plugStruct, displayPlugin, subViewId);
    else {
        view = new cOsdView(plugStruct, displayPlugin, 0);
    }
    return view;
}

bool skindesignerapi::cSkindesignerOsdObject::SkindesignerAvailable(void) {
    return SkindesignerAPI::ServiceAvailable();
}


/**********************************************************************
* cSkindesignerOsdItem
**********************************************************************/
skindesignerapi::cSkindesignerOsdItem::cSkindesignerOsdItem(cTokenContainer *tk, eOSState State) : cOsdItem(State) {
    sdDisplayMenu = NULL;
    tokenContainer = new skindesignerapi::cTokenContainer(*tk);
    tokenContainer->CreateContainers();
}

skindesignerapi::cSkindesignerOsdItem::cSkindesignerOsdItem(cTokenContainer *tk, const char *Text, eOSState State, bool Selectable) : cOsdItem(Text, State, Selectable) {
    sdDisplayMenu = NULL;
    tokenContainer = new skindesignerapi::cTokenContainer(*tk);
    tokenContainer->CreateContainers();
}  

skindesignerapi::cSkindesignerOsdItem::~cSkindesignerOsdItem() {
    delete tokenContainer;
}

void skindesignerapi::cSkindesignerOsdItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable) {
    if (sdDisplayMenu) {
        if (!sdDisplayMenu->SetItemPlugin(tokenContainer, Index, Current, Selectable)) {
            DisplayMenu->SetItem(Text(), Index, Current, Selectable);
        }
    } else {
        DisplayMenu->SetItem(Text(), Index, Current, Selectable);
    }
}

int skindesignerapi::cSkindesignerOsdItem::GetLoopIndex(const char *loop) {
    return tokenContainer->LoopIndex(loop);
}

void skindesignerapi::cSkindesignerOsdItem::SetLoop(vector<int> loopInfo) {
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
}

void skindesignerapi::cSkindesignerOsdItem::AddStringToken(int index, const char *value) {
    tokenContainer->AddStringToken(index, value);
}

void skindesignerapi::cSkindesignerOsdItem::AddIntToken(int index, int value) {
    tokenContainer->AddIntToken(index, value);
}

void skindesignerapi::cSkindesignerOsdItem::AddLoopToken(int loopIndex, int row, int index, const char *value) {
    tokenContainer->AddLoopToken(loopIndex, row, index, value);
}


/**********************************************************************
* cSkindesignerOsdMenu
**********************************************************************/
skindesignerapi::cSkindesignerOsdMenu::cSkindesignerOsdMenu(skindesignerapi::cPluginStructure *plugStruct, const char *Title, int c0, int c1, int c2, int c3, int c4) : cOsdMenu(Title, c0, c1, c2, c3, c4) {
    this->plugStruct = plugStruct;
    tokenContainer = NULL;
    init = true;
    activeMenu = 0;
    firstCallCleared = false;
    secondCall = false;
    firstMenu = -1;
    firstType = mtList;
    displayText = false;
    sdDisplayMenu = NULL;
    SetMenuCategory(mcPlugin);
    SetSkinDesignerDisplayMenu();
}

skindesignerapi::cSkindesignerOsdMenu::~cSkindesignerOsdMenu() {
}

void skindesignerapi::cSkindesignerOsdMenu::SetPluginMenu(int menuId, eMenuType type) {
    activeMenu = menuId;
    if (firstCallCleared) {
        firstMenu = menuId;
        firstType = type;        
    }
    if (type == mtList)
        displayText = false;
    else if (type == mtText)
        displayText = true;
    if (sdDisplayMenu) {
        int plugId = plugStruct->id;
        sdDisplayMenu->SetPluginMenu(plugId, menuId, type, init);
    }
    init = false;
}

bool skindesignerapi::cSkindesignerOsdMenu::SetSkinDesignerDisplayMenu(void) {
    sdDisplayMenu = SkindesignerAPI::GetDisplayMenu();
    return (sdDisplayMenu != NULL);
}

void skindesignerapi::cSkindesignerOsdMenu::ClearTokens(void) {
    text = "";
    if (tokenContainer)
        tokenContainer->Clear();
}

int skindesignerapi::cSkindesignerOsdMenu::GetLoopIndex(const char *loop) {
    return tokenContainer->LoopIndex(loop);
}

void skindesignerapi::cSkindesignerOsdMenu::SetLoop(vector<int> loopInfo) {
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
}

void skindesignerapi::cSkindesignerOsdMenu::SetTokenContainer(cTokenContainer *tk) {
    tokenContainer = tk;
}

void skindesignerapi::cSkindesignerOsdMenu::AddStringToken(int index, const char *value) {
    tokenContainer->AddStringToken(index, value);
}

void skindesignerapi::cSkindesignerOsdMenu::AddIntToken(int index, int value) {
    tokenContainer->AddIntToken(index, value);
}

void skindesignerapi::cSkindesignerOsdMenu::AddLoopToken(int loopIndex, int row, int index, const char *value) {
    tokenContainer->AddLoopToken(loopIndex, row, index, value);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyLeft(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(true, true);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyRight(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(false, true);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyUp(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(true, false);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyDown(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(false, false);
}

skindesignerapi::cTokenContainer *skindesignerapi::cSkindesignerOsdMenu::GetTokenContainer(int menuId) {
    if (!plugStruct)
        return NULL;
    return plugStruct->GetMenuTokenContainer(menuId);
}

void skindesignerapi::cSkindesignerOsdMenu::Display(void) {
    if (firstCallCleared) {
        firstCallCleared = false;
        secondCall = true;
        return;
    }
    if (secondCall) {
        SetSkinDesignerDisplayMenu();
        SetPluginMenu(firstMenu, firstType);
    }
    if (displayText) {
        if (sdDisplayMenu) {
            sdDisplayMenu->SetMenuCategory(mcPlugin);
            sdDisplayMenu->SetTitle(Title());
            if (tokenContainer && sdDisplayMenu->SetPluginText(tokenContainer)) {
                sdDisplayMenu->Flush();
            } else {
                DisplayMenu()->Clear();
                DisplayMenu()->SetMenuCategory(mcText);
                DisplayMenu()->SetTitle(Title());
                DisplayMenu()->SetText(text.c_str(), false);
                DisplayMenu()->Flush();
            }
        } else {
            DisplayMenu()->Clear();
            DisplayMenu()->SetTitle(Title());
            DisplayMenu()->SetText(text.c_str(), false);
            DisplayMenu()->Flush();
        }
        return;
    }
    if (sdDisplayMenu) {
        //sdDisplayMenu->SetMenuCategory(mcPlugin);
        sdDisplayMenu->SetTitle(Title());
        for (cOsdItem *item = First(); item; item = Next(item)) {
            cSkindesignerOsdItem *sdItem = dynamic_cast<cSkindesignerOsdItem*>(item);
            if (sdItem) {
                sdItem->SetDisplayMenu(sdDisplayMenu);
            }
        }
    }
    cOsdMenu::Display();
}
