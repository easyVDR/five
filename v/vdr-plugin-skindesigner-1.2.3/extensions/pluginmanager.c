#include "pluginmanager.h"

cSDPluginManager::cSDPluginManager(void) {
    lastId = 0;
    subviewsfound = false;
}

cSDPluginManager::~cSDPluginManager(void) {
}

void cSDPluginManager::Reset(void) {
    subViewMapping.clear();
}

void cSDPluginManager::RegisterBasicPlugin(skindesignerapi::cPluginStructure *plugStructure) {
    dsyslog("skindesigner: plugin %s uses libskindesigner API Version %s", 
                                                            plugStructure->name.c_str(), 
                                                            plugStructure->libskindesignerAPIVersion.c_str());
    plugStructure->id = lastId;
    registeredPlugins.insert(pair<int,string>(lastId, plugStructure->name));
    lastId++;
    map< int, skindesignerapi::sPlugMenu > menusNew;
    for (map< int, skindesignerapi::sPlugMenu >::iterator it = plugStructure->menus.begin(); it != plugStructure->menus.end(); it++) {
        int key = it->first;
        skindesignerapi::sPlugMenu menu = it->second;
        skindesignerapi::sPlugMenu menuNew;
        menuNew.type = menu.type;
        menuNew.tplname = menu.tplname;
        menuNew.tokenContainer = new skindesignerapi::cTokenContainer(*menu.tokenContainer);
        menusNew.insert(pair<int, skindesignerapi::sPlugMenu>(key, menuNew));
    }
    pluginMenus.insert(pair< int, map < int, skindesignerapi::sPlugMenu > >(plugStructure->id, menusNew));

    if (plugStructure->menus.size() > 0)
        dsyslog("skindesigner: plugin %s has registered %ld menus", plugStructure->name.c_str(), plugStructure->menus.size());

}

int cSDPluginManager::GetNumPluginMenus(void) {
    int numMenusTotal = 0;
    for (map < int, map < int, skindesignerapi::sPlugMenu > >::iterator it = pluginMenus.begin(); it != pluginMenus.end(); it++) {
        numMenusTotal += (it->second).size();
    }
    return numMenusTotal;
}

void cSDPluginManager::InitPluginMenuIterator(void) {
    plugMenuIt = pluginMenus.begin();
}

map <int,skindesignerapi::sPlugMenu> *cSDPluginManager::GetPluginMenus(string &name, int &id) {
    if (plugMenuIt == pluginMenus.end())
        return NULL;
    id = plugMenuIt->first;
    map<int,string>::iterator hit = registeredPlugins.find(id);
    if (hit != registeredPlugins.end())
        name = hit->second; 
    map <int,skindesignerapi::sPlugMenu> *templates = &plugMenuIt->second;
    plugMenuIt++;
    return templates; 
}

skindesignerapi::cTokenContainer *cSDPluginManager::GetTokenContainer(int plugId, int plugMenuId) {
    map <int, map<int, skindesignerapi::sPlugMenu> >::iterator hit = pluginMenus.find(plugId);
    if (hit == pluginMenus.end())
        return NULL;
    map<int, skindesignerapi::sPlugMenu>::iterator hit2 = (hit->second).find(plugMenuId);
    if (hit2 == (hit->second).end())
        return NULL;
    skindesignerapi::cTokenContainer *tk = hit2->second.tokenContainer;
    return tk;    
}

void cSDPluginManager::AddSubviewMapping(int plugId, int plugMenuId, int subViewId) {
    map <int, map<int,int> >::iterator hit = subViewMapping.find(plugId);
    if (hit == subViewMapping.end()) {
        map<int,int> menus;
        menus.insert(pair<int,int>(plugMenuId, subViewId));
        subViewMapping.insert(pair<int, map<int,int> >(plugId, menus));
    } else {
        (hit->second).insert(pair<int,int>(plugMenuId, subViewId));
    }
}

int cSDPluginManager::GetSubviewId(int plugId, int plugMenuId) {
    map <int, map<int,int> >::iterator hit = subViewMapping.find(plugId);
    if (hit == subViewMapping.end())
        return -1;
    map<int,int>::iterator hit2 = (hit->second).find(plugMenuId);
    if (hit2 == (hit->second).end())
        return -1;
    return hit2->second;
}

void cSDPluginManager::RegisterAdvancedPlugin(skindesignerapi::cPluginStructure *plugStructure) {
    dsyslog("skindesigner: plugin %s uses libskindesigner API Version %s", 
                                                            plugStructure->name.c_str(), 
                                                            plugStructure->libskindesignerAPIVersion.c_str());
    plugStructure->id = lastId;
    registeredPlugins.insert(pair<int,string>(lastId, plugStructure->name));
    lastId++;

    rootviews.insert(pair<int,string>(plugStructure->id, plugStructure->rootview));
    subviews.insert(pair<int,map<int,string> >(plugStructure->id, plugStructure->subviews));

    multimap< int, skindesignerapi::sPlugViewElement > viewelementsNew;
    for (map< int, skindesignerapi::sPlugViewElement >::iterator it = plugStructure->viewelements.begin(); it != plugStructure->viewelements.end(); it++) {
        int key = it->first;
        skindesignerapi::sPlugViewElement ve = it->second;
        skindesignerapi::sPlugViewElement veNew;
        veNew.id = ve.id;
        veNew.viewId = ve.viewId;
        veNew.name = ve.name;        
        veNew.tokenContainer = new skindesignerapi::cTokenContainer(*ve.tokenContainer);
        viewelementsNew.insert(pair<int, skindesignerapi::sPlugViewElement>(key, veNew));
    }
    viewelements.insert(pair< int, multimap < int, skindesignerapi::sPlugViewElement > >(plugStructure->id, viewelementsNew));

    multimap< int, skindesignerapi::sPlugViewGrid > viewgridsNew;
    for (map< int, skindesignerapi::sPlugViewGrid >::iterator it = plugStructure->viewgrids.begin(); it != plugStructure->viewgrids.end(); it++) {
        int key = it->first;
        skindesignerapi::sPlugViewGrid vg = it->second;
        skindesignerapi::sPlugViewGrid vgNew;
        vgNew.id = vg.id;
        vgNew.viewId = vg.viewId;
        vgNew.name = vg.name;        
        vgNew.tokenContainer = new skindesignerapi::cTokenContainer(*vg.tokenContainer);
        viewgridsNew.insert(pair<int, skindesignerapi::sPlugViewGrid>(key, vgNew));
    }
    viewgrids.insert(pair< int, multimap < int, skindesignerapi::sPlugViewGrid > >(plugStructure->id, viewgridsNew));

    map< int, skindesignerapi::cTokenContainer* > viewtabsNew;
    for (map<int,skindesignerapi::cTokenContainer*>::iterator it = plugStructure->viewtabs.begin(); it != plugStructure->viewtabs.end(); it++) {
        int id = it->first;
        skindesignerapi::cTokenContainer *tk = it->second;
        viewtabsNew.insert(pair<int,skindesignerapi::cTokenContainer*>(id, new skindesignerapi::cTokenContainer(*tk)));
    }
    viewtabs.insert(pair< int, map<int,skindesignerapi::cTokenContainer*> >(plugStructure->id, viewtabsNew));

    if (plugStructure->rootview.size() > 0)
        dsyslog("skindesigner: plugin %s has registered %ld views with %ld viewelements and %ld viewgrids", 
                 plugStructure->name.c_str(), 
                 1 + plugStructure->subviews.size(),
                 plugStructure->viewelements.size(),
                 plugStructure->viewgrids.size());
}

void cSDPluginManager::InitPluginViewIterator(void) {
    rootViewsIt = rootviews.begin();
}

bool cSDPluginManager::GetNextPluginView(string &plugName, int &plugId, string &tplName) {
    if (rootViewsIt == rootviews.end())
        return false;
    plugId = rootViewsIt->first;
    tplName = rootViewsIt->second;
    map<int,string>::iterator hit = registeredPlugins.find(plugId);
    if (hit != registeredPlugins.end())
        plugName = hit->second; 
    rootViewsIt++;
    return true; 
} 

int cSDPluginManager::GetNumSubviews(int plugId) {
    map< int, map< int, string > >::iterator hit = subviews.find(plugId);
    if (hit == subviews.end())
        return 0;
    return (hit->second).size();
}

void cSDPluginManager::InitPluginSubviewIterator(int plugId) {
    map< int, map< int, string > >::iterator hit = subviews.find(plugId);
    if (hit == subviews.end()) {
        subviewsfound = false;
        return;
    }
    subviewsCurrent = hit->second;
    subviewsfound = true;
    svIt = subviewsCurrent.begin();
}

bool cSDPluginManager::GetNextSubView(int &id, string &tplname) {
    if (!subviewsfound)
        return false;
    if( svIt == subviewsCurrent.end() ) {
        return false;    
    }
    id = svIt->first;
    tplname = svIt->second;
    svIt++;
    return true;
}

int cSDPluginManager::GetNumViewElements(int plugId, int viewId) {
    map< int, multimap< int, skindesignerapi::sPlugViewElement > >::iterator hit = viewelements.find(plugId);
    if (hit == viewelements.end())
        return 0;
    multimap<int, skindesignerapi::sPlugViewElement> *plugVEs = &hit->second;
    pair<multimap<int, skindesignerapi::sPlugViewElement>::iterator, multimap<int, skindesignerapi::sPlugViewElement>::iterator> range;
    range = plugVEs->equal_range(viewId);
    int numVEs = 0;
    for (multimap<int, skindesignerapi::sPlugViewElement>::iterator it=range.first; it!=range.second; ++it) {
        numVEs++;
    }
    return numVEs;
}

void cSDPluginManager::InitViewElementIterator(int plugId, int viewId) {
    map< int, multimap< int, skindesignerapi::sPlugViewElement > >::iterator hit = viewelements.find(plugId);
    if (hit == viewelements.end())
        return;
    multimap<int, skindesignerapi::sPlugViewElement> *plugVEs = &hit->second;
    veRange = plugVEs->equal_range(viewId);
    veIt = veRange.first;
}

bool cSDPluginManager::GetNextViewElement(int &veId, string &veName) {
    if (veIt == veRange.second)
        return false;
    skindesignerapi::sPlugViewElement *ve = &veIt->second;
    veId = ve->id;
    veName = ve->name;
    veIt++;
    return true;
}

skindesignerapi::cTokenContainer *cSDPluginManager::GetTokenContainerVE(int plugId, int viewId, int veId) {
    map< int, multimap< int, skindesignerapi::sPlugViewElement > >::iterator hit = viewelements.find(plugId);
    if (hit == viewelements.end())
        return NULL;
    multimap<int, skindesignerapi::sPlugViewElement> *plugVEs = &hit->second;
    for (multimap<int, skindesignerapi::sPlugViewElement>::iterator it = plugVEs->begin(); it != plugVEs->end(); it++) {
        int view = it->first;
        if (view != viewId)
            continue;
        skindesignerapi::sPlugViewElement *ve = &it->second;
        if (ve->id == veId)
            return ve->tokenContainer;
    }
    return NULL;
}

int cSDPluginManager::GetNumViewGrids(int plugId, int viewId) {
    map< int, multimap< int, skindesignerapi::sPlugViewGrid > >::iterator hit = viewgrids.find(plugId);
    if (hit == viewgrids.end())
        return 0;
    multimap<int, skindesignerapi::sPlugViewGrid> *plugVGs = &hit->second;
    pair<multimap<int, skindesignerapi::sPlugViewGrid>::iterator, multimap<int, skindesignerapi::sPlugViewGrid>::iterator> range;
    range = plugVGs->equal_range(viewId);
    int numVGs = 0;
    for (multimap<int, skindesignerapi::sPlugViewGrid>::iterator it=range.first; it!=range.second; ++it) {
        numVGs++;
    }
    return numVGs;
}

void cSDPluginManager::InitViewGridIterator(int plugId, int viewId) {
    map< int, multimap< int, skindesignerapi::sPlugViewGrid > >::iterator hit = viewgrids.find(plugId);
    if (hit == viewgrids.end())
        return;
    multimap<int, skindesignerapi::sPlugViewGrid> *plugGEs = &hit->second;
    gRange = plugGEs->equal_range(viewId);
    gIt = gRange.first;
}

bool cSDPluginManager::GetNextViewGrid(int &gId, string &gName) {
    if (gIt == gRange.second)
        return false;
    skindesignerapi::sPlugViewGrid *ge = &gIt->second;
    gId = ge->id;
    gName = ge->name;
    gIt++;
    return true;
}

skindesignerapi::cTokenContainer *cSDPluginManager::GetTokenContainerGE(int plugId, int viewId, int gId) {
    map< int, multimap< int, skindesignerapi::sPlugViewGrid > >::iterator hit = viewgrids.find(plugId);
    if (hit == viewgrids.end())
        return NULL;
    multimap<int, skindesignerapi::sPlugViewGrid> *plugGEs = &hit->second;
    for (multimap<int, skindesignerapi::sPlugViewGrid>::iterator it = plugGEs->begin(); it != plugGEs->end(); it++) {
        int view = it->first;
        if (view != viewId)
            continue;
        skindesignerapi::sPlugViewGrid *g = &it->second;
        if (g->id == gId)
            return g->tokenContainer;
    }
    return NULL;
}

skindesignerapi::cTokenContainer *cSDPluginManager::GetTokenContainerTab(int plugId, int viewId) {
    map< int, map< int, skindesignerapi::cTokenContainer* > >::iterator hit = viewtabs.find(plugId);
    if (hit == viewtabs.end())
        return NULL;
    map< int, skindesignerapi::cTokenContainer* > *tabs = &hit->second;
    map< int, skindesignerapi::cTokenContainer* >::iterator hit2 = tabs->find(viewId);
    if (hit2 == tabs->end())
        return NULL;
    return (hit2->second);
}
