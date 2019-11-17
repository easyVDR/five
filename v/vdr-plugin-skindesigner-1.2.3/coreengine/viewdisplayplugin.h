#ifndef __VIEWDISPLAYPLUGIN_H
#define __VIEWDISPLAYPLUGIN_H

#include "view.h"
#include "viewdetail.h"
#include "viewelementsdisplaymenu.h"
#include "viewgrid.h"
#include "../libskindesignerapi/skindesignerapi.h"

class cPluginTabView;
/***********************************************************
* cViewPlugin
***********************************************************/
class cViewPlugin : public cView, public skindesignerapi::ISkinDisplayPlugin {
private:
    int id;
    int plugId;
    int numViews;
    cViewPlugin **views;
    int numViewGrids;
    cViewGrid **viewGrids;
    map<string,int> gridNames;
    cPluginTabView *tabView;
    bool viewChanged;
    int newViewId;
    void SetViewElements(void);
    void SetViewGrids(void);
    int GridId(const char *name);
public:
    cViewPlugin(int id, int plugId);
    ~cViewPlugin(void);
    //Internal Interface
    bool ReadFromXML(const char *plugName, const char *tplName, cSdOsd *osd = NULL);
    bool ReadSubViews(const char *plugName);
    void AddViewElement(cVePlugin *viewElement);
    void AddViewGrid(cViewGrid *viewGrid);
    void AddTab(cArea *tab);
    void AddScrollbar(cVeDmScrollbar *scrollbar);
    void AddTablabels(cVeDmTablabels *tablabels);
    void SetGlobals(cGlobals *globals);
    void PreCache(void);
    cVePlugin *GetViewElement(int veId);
    cViewGrid *GetViewGrid(int gId);
    cPluginTabView *GetViewTab(void);
    void Hide(void);
    void Show(void);
    //libskindesigner api interface
    bool InitOsd(void);
    void CloseOsd(void);
    void Deactivate(int viewId, bool hide);
    void Activate(int viewId);
    void SetViewElementTokens(int veId, int viewId, skindesignerapi::cTokenContainer *tk);
    void ClearViewElement(int veId, int viewId);
    void DisplayViewElement(int veId, int viewId);
    void SetGrid(long gId, int viewId, int viewGridId,  double x, double y, double width, double height, skindesignerapi::cTokenContainer *tk);
    void SetGridCurrent(long gId, int viewId, int viewGridId, bool current);
    void DeleteGrid(long gId, int viewId, int viewGridId);
    void DisplayGrids(int viewId, int viewGridId);
    void ClearGrids(int viewId, int viewGridId);
    void SetTabTokens(int viewId, skindesignerapi::cTokenContainer *tk);
    void TabLeft(int viewId);
    void TabRight(int viewId);
    void TabUp(int viewId);
    void TabDown(int viewId);
    void DisplayTabs(int viewId);
    void ClearTab(int viewId);
    void Flush(void);
    bool ChannelLogoExists(string channelId);
    string GetEpgImagePath(void);
};
/***********************************************************
* cPluginTabView
***********************************************************/
class cPluginTabView {
private:
    cSdOsd *sdOsd;
    bool init;
    bool drawScrollbar;
    bool firstTab;
    cVeDmScrollbar *scrollbar;
    cVeDmTablabels *tablabels;
    cViewDetailAdvancedPlugin *detailView;
    void DrawScrollbar(void);
public:
    cPluginTabView(int viewId, int plugId);
    ~cPluginTabView(void);
    void SetGlobals(cGlobals *globals);
    void SetOsd(cSdOsd *osd) { sdOsd = osd; };
    void AddTab(cArea *tab);
    void AddScrollbar(cVeDmScrollbar *scrollbar);
    void AddTablabels(cVeDmTablabels *tablabels);
    void PreCache(int containerX, int containerY, int containerWidth, int containerHeight);
    void Set(skindesignerapi::cTokenContainer *tk);
    void Render(void);
    void Clear(void);
    void KeyLeft(void);
    void KeyRight(void);
    void KeyUp(void);
    void KeyDown(void);
};
#endif //__VIEWDISPLAYPLUGIN_H