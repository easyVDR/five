#include "pluginstructure.h"

skindesignerapi::cPluginStructure::cPluginStructure(void) {
    name = "";
    libskindesignerAPIVersion = "undefined";
    id = -1;
    rootview = "";
};

skindesignerapi::cPluginStructure::~cPluginStructure(void) {
    for (map<int,sPlugMenu>::iterator it = menus.begin(); it != menus.end(); it++) {
        delete (it->second).tokenContainer;
    }
};

void skindesignerapi::cPluginStructure::RegisterMenu(int key, int type, string tpl, skindesignerapi::cTokenContainer *tk) {
    tk->CreateContainers();
    sPlugMenu s;
    s.type = type;
    s.tplname = tpl;
    s.tokenContainer = tk;
    menus.insert(pair<int, sPlugMenu>(key, s));
}

skindesignerapi::cTokenContainer *skindesignerapi::cPluginStructure::GetMenuTokenContainer(int key) {
    map<int, sPlugMenu>::iterator hit = menus.find(key);
    if (hit == menus.end())
        return NULL;
    return hit->second.tokenContainer;
}

void skindesignerapi::cPluginStructure::RegisterRootView(string templateName) {
    rootview = templateName;
}

void skindesignerapi::cPluginStructure::RegisterSubView(int subView, string templateName) {
    subviews.insert(pair<int, string>(subView, templateName));
}

void skindesignerapi::cPluginStructure::RegisterViewElement(int view, int viewElement, string name, skindesignerapi::cTokenContainer *tk) {
    tk->CreateContainers();
    sPlugViewElement ve;
    ve.id = viewElement;
    ve.viewId = view;
    ve.name = name;
    ve.tokenContainer = tk;
    viewelements.insert(pair<int, sPlugViewElement>(view, ve));
}

void skindesignerapi::cPluginStructure::RegisterViewGrid(int view, int viewGrid, string name, skindesignerapi::cTokenContainer *tk) {
    tk->CreateContainers();
    sPlugViewGrid vg;
    vg.id = viewGrid;
    vg.viewId = view;
    vg.name = name;
    vg.tokenContainer = tk;
    viewgrids.insert(pair<int, sPlugViewGrid>(view, vg));
}

void skindesignerapi::cPluginStructure::RegisterViewTab(int view, skindesignerapi::cTokenContainer *tk) {
    tk->CreateContainers();
    viewtabs.insert(pair<int, skindesignerapi::cTokenContainer*>(view, tk));
}

skindesignerapi::cTokenContainer *skindesignerapi::cPluginStructure::GetTokenContainerVE(int viewId, int veId) {
    pair<multimap<int, sPlugViewElement>::iterator, multimap<int, sPlugViewElement>::iterator> range;
    range = viewelements.equal_range(viewId);
    for (multimap<int, sPlugViewElement>::iterator it=range.first; it!=range.second; ++it) {
        sPlugViewElement *ve = &it->second;
        if (ve->id == veId)
            return ve->tokenContainer;
    }
    return NULL;
}

skindesignerapi::cTokenContainer *skindesignerapi::cPluginStructure::GetTokenContainerGE(int viewId, int geId) {
    pair<multimap<int, sPlugViewGrid>::iterator, multimap<int, sPlugViewGrid>::iterator> range;
    range = viewgrids.equal_range(viewId);
    for (multimap<int, sPlugViewGrid>::iterator it=range.first; it!=range.second; ++it) {
        sPlugViewGrid *ge = &it->second;
        if (ge->id == geId)
            return ge->tokenContainer;
    }
    return NULL;
}

skindesignerapi::cTokenContainer *skindesignerapi::cPluginStructure::GetTokenContainerTab(int viewId) {
    map<int,skindesignerapi::cTokenContainer*>::iterator hit = viewtabs.find(viewId);
    if (hit == viewtabs.end())
        return NULL;
    return hit->second;
}
