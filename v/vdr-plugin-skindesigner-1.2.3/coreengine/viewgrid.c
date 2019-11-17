#include "viewgrid.h"
#include "../extensions/helpers.h"

cViewGrid::cViewGrid(void) {
    id = -1;
    plugId = -1;
    viewId = -1;
    globals = NULL;
    attribs = new cViewElementAttribs((int)eViewElementAttribs::count);
    gridTpl = NULL;
    gridsize = GRIDSIZE;
    grid = new cGridElement*[gridsize];
    for (int i=0; i < gridsize; i++) {
        grid[i] = NULL;
    }
    gridMin = 0;
    gridMax = -1;
}

cViewGrid::~cViewGrid(void) {
    delete attribs;
    delete gridTpl;
}

void cViewGrid::SetGlobals(cGlobals *globals) {
    this->globals = globals;
}

void cViewGrid::SetContainer(int x, int y, int width, int height) {
    container.SetX(x);
    container.SetY(y);
    container.SetWidth(width);
    container.SetHeight(height);
}

void cViewGrid::SetAttributes(vector<stringpair> &attributes) {
    attribs->Set(attributes);
}

void cViewGrid::AddGridElement(cGridElement *gridElement) {
    gridTpl = gridElement;
}

const char *cViewGrid::Name(void) {
    return attribs->Name();
}

void cViewGrid::SetTokenContainer(void) {
    if (!gridTpl)
        return;
    gridTpl->SetId(id);
    gridTpl->SetPluginId(plugId);
    gridTpl->SetViewId(viewId);
    gridTpl->SetTokenContainer();
}

void cViewGrid::PreCache(void) {
    attribs->SetContainer(container.X(), container.Y(), container.Width(), container.Height());
    attribs->SetGlobals(globals);
    attribs->Cache();
    gridTpl->SetGlobals(globals);
    gridTpl->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
    gridTpl->Cache();
    
}

void cViewGrid::CheckSize(int id) {
    if (id < gridsize)
        return;
    int newgridsize = gridsize + GRIDSIZE;
    while (newgridsize < id)
        newgridsize += gridsize;
    cGridElement **gridNew = new cGridElement*[newgridsize];
    int i=0;
    bool foundFirst = false;
    for (; i < gridsize; i++) {
        if (!foundFirst && grid[i]) {
            foundFirst = true;
            gridMin = i;
        }
        gridNew[i] = grid[i];
    }
    gridsize = newgridsize;
    for (; i < gridsize; i++) {
        gridNew[i] = NULL;
    }
    delete[] grid;
    grid = gridNew;
}

void cViewGrid::SetTokens(int gId, skindesignerapi::cTokenContainer *tk) {
    if (!grid[gId]) {
        return;
    }
    grid[gId]->Set(tk);
}

void cViewGrid::PositionGrid(int gId, double x, double y, double width, double height) {
    int gridX = attribs->X() + x * attribs->Width();
    int gridY = attribs->Y() + y * attribs->Height();
    int gridWidth = width * attribs->Width();
    int gridHeight = height * attribs->Height();
    if (!grid[gId]) {
        if (gId >= gridMax)
            gridMax = gId+1;
        grid[gId] = CreateGrid(gridX, gridY, gridWidth, gridHeight);
    } else {
        if (grid[gId]->Width() == gridWidth && grid[gId]->Height() == gridHeight) {
            grid[gId]->SetPosition(gridX, gridY, gridWidth, gridHeight);
        } else {
            cGridElement *ge = CreateGrid(gridX, gridY, gridWidth, gridHeight);
            ge->Set(grid[gId]->GetTokenContainer());
            grid[gId]->Close();
            delete grid[gId];
            grid[gId] = ge;
        }
        grid[gId]->SetDirty();
    }
}

void cViewGrid::SetCurrentGrid(int gId, bool current) {
    if (gId >= 0 && grid[gId]) {
        grid[gId]->SetCurrent(current);
    }
}

void cViewGrid::DeleteGrid(int gId) {
    if (!grid[gId])
        return;
    grid[gId]->Close();
    delete grid[gId];
    grid[gId] = NULL;
}

void cViewGrid::ClearGrids(void) {
    for (int i = 0; i < gridsize; i++) {
        if (!grid[i])
            continue;
        grid[i]->StopBlinking();
        grid[i]->Close();
        delete grid[i];
        grid[i] = NULL;
    }
}

void cViewGrid::Render(void) {
    for (int i = gridMin; i < gridMax; i++) {
        if (grid[i] && grid[i]->Parse()) {
            grid[i]->Render();
        }
    }
}

cGridElement *cViewGrid::CreateGrid(int x, int y, int width, int height) {
    cGridElement *ge = new cGridElement(*gridTpl);
    ge->SetAreaX(x);
    ge->SetAreaY(y);
    ge->SetAreaWidth(width);
    ge->SetAreaHeight(height);
    return ge;
}

void cViewGrid::Close(void) {
    ClearGrids();
    gridsize = GRIDSIZE;
    delete[] grid;
    grid = new cGridElement*[gridsize];
    for (int i=0; i < gridsize; i++) {
        grid[i] = NULL;
    }
    gridMin = 0;
    gridMax = -1;
}

void cViewGrid::Hide(void) {
    for (int i = 0; i < gridsize; i++) {
        if (grid[i]) {
            grid[i]->Hide();
        }
    }
}

void cViewGrid::Show(void) {
    for (int i = 0; i < gridsize; i++) {
        if (grid[i]) {
            grid[i]->Show();
        }
    }
}
