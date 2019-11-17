#ifndef __VIEWGRID_H
#define __VIEWGRID_H

#define GRIDSIZE 500

#include "gridelement.h"

class cViewGrid {
protected:
    cRect container;
    cGlobals *globals;
    cViewElementAttribs *attribs;
    cGridElement *gridTpl;
    int gridsize;
    cGridElement **grid;
    int gridMin;
    int gridMax;
    int id;
    int viewId;
    int plugId;
    cGridElement *CreateGrid(int x, int y, int width, int height);
public:
    cViewGrid(void);
    virtual ~cViewGrid(void);
    void SetGlobals(cGlobals *globals);
    void SetContainer(int x, int y, int width, int height);
    void SetAttributes(vector<stringpair> &attributes);
    void SetId(int id) { this->id = id; };
    void SetPluginId(int plugId) { this->plugId = plugId; };
    void SetViewId(int viewId) { this->viewId = viewId; };
    void AddGridElement(cGridElement *gridElement);
    const char *Name(void);
    void SetTokenContainer(void);
    void PreCache(void);
    void CheckSize(int id);
    void SetTokens(int gId, skindesignerapi::cTokenContainer *tk);
    void PositionGrid(int gId, double x, double y, double width, double height);
    void SetCurrentGrid(int gId, bool current);
    void DeleteGrid(int gId);
    void ClearGrids(void);
    void Render(void);
    void Close(void);
    void Hide(void);
    void Show(void);
};

#endif //__VIEWGRID_H