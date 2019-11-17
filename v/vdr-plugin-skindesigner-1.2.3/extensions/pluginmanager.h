#ifndef __PLUGINMANAGER_H
#define __PLUGINMANAGER_H

#include <string>
#include <map>
#include "../libskindesignerapi/skindesignerapi.h"

using namespace std;

class cSDPluginManager {
private:
    int lastId;
    //plugin id --> plugin name
    map < int, string > registeredPlugins;
    //Basic Plugin Interface
    //plugin id --> plugin definition 
    map < int, map < int, skindesignerapi::sPlugMenu > > pluginMenus;
    map < int, map < int, skindesignerapi::sPlugMenu > >::iterator plugMenuIt;
    //plugin id - menuId --> subviewid
    map < int, map<int, int> > subViewMapping;
    
    //Advanced Plugin Interface
    //plugin id --> rootview templatename definition 
    map< int, string > rootviews;
    map< int, string >::iterator rootViewsIt;
    //plugin id --> subviewid /templatename definition 
    map< int, map< int, string > > subviews;
    map< int, string> subviewsCurrent;
    map< int, string>::iterator svIt;
    bool subviewsfound;
    //plugin id --> view id --> viewelement definition 
    map< int, multimap< int, skindesignerapi::sPlugViewElement > > viewelements;
    pair<multimap<int, skindesignerapi::sPlugViewElement>::iterator, multimap<int, skindesignerapi::sPlugViewElement>::iterator> veRange;
    multimap<int, skindesignerapi::sPlugViewElement>::iterator veIt;
    //plugin id --> view id --> viewgrid definition 
    map< int, multimap< int, skindesignerapi::sPlugViewGrid > > viewgrids;
    pair<multimap<int, skindesignerapi::sPlugViewGrid>::iterator, multimap<int, skindesignerapi::sPlugViewGrid>::iterator> gRange;
    multimap<int, skindesignerapi::sPlugViewGrid>::iterator gIt;
    //plugin id --> view id --> tokencontainer of detailedview definition
    map< int, map< int, skindesignerapi::cTokenContainer* > > viewtabs;
public:
    cSDPluginManager(void);
    ~cSDPluginManager(void);
    void Reset(void);
    //Basic Plugin Interface
    void RegisterBasicPlugin(skindesignerapi::cPluginStructure *plugStructure);
    int GetNumPluginMenus(void);
    void InitPluginMenuIterator(void);
    map <int,skindesignerapi::sPlugMenu> *GetPluginMenus(string &name, int &id);
    skindesignerapi::cTokenContainer *GetTokenContainer(int plugId, int plugMenuId);
    void AddSubviewMapping(int plugId, int plugMenuId, int subViewId);
    int GetSubviewId(int plugId, int plugMenuId);
    //Advanced Plugin Interface
    void RegisterAdvancedPlugin(skindesignerapi::cPluginStructure *plugStructure);
    void InitPluginViewIterator(void);
    bool GetNextPluginView(string &plugName, int &plugId, string &tplName);
    int GetNumSubviews(int plugId);
    void InitPluginSubviewIterator(int plugId);
    bool GetNextSubView(int &id, string &tplname);
    int GetNumViewElements(int plugId, int viewId);
    void InitViewElementIterator(int plugId, int viewId);
    bool GetNextViewElement(int &veId, string &veName);
    skindesignerapi::cTokenContainer *GetTokenContainerVE(int plugId, int viewId, int veId);
    int GetNumViewGrids(int plugId, int viewId);
    void InitViewGridIterator(int plugId, int viewId);
    bool GetNextViewGrid(int &gId, string &gName);
    skindesignerapi::cTokenContainer *GetTokenContainerGE(int plugId, int viewId, int gId);
    skindesignerapi::cTokenContainer *GetTokenContainerTab(int plugId, int viewId);
};

#endif //__PLUGINMANAGER_H
