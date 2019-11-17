#include "attribute.h"
#include "../config.h"

/***************************************************************************
* cAttributes
***************************************************************************/
cAttributes::cAttributes(int numAttributes) {
    globals = NULL;
    tokenContainer = NULL;
    numAttribs = (int)eCommonAttribs::count + numAttributes;
    attribs = new int[numAttribs];
    for (int i=0; i < numAttribs; i++)
        attribs[i] = ATTR_UNKNOWN;
    attribCtors = new cNumericExpr*[numAttribs];
    for (int i=0; i < numAttribs; i++)
        attribCtors[i] = NULL;
    cond = NULL;
    SetCommonAttributesDefs();
}

cAttributes::cAttributes(const cAttributes &other) : cAttributes(other.numAttribs - (int)eCommonAttribs::count){
    globals = other.globals;
    for (int i=0; i < numAttribs; i++) {
        attribs[i] = other.attribs[i];
        if (other.attribCtors[i]) {
            attribCtors[i] = new cNumericExpr(*other.attribCtors[i]);
            attribCtors[i]->SetContainer(&container);    
        }
    }
    cond = NULL;
    if (other.cond) {
        cond = new cCondition(*other.cond);
    }
    attribIDs = other.attribIDs;
    attribNames = other.attribNames;
}

cAttributes::~cAttributes(void) {
    delete[] attribs;
    for (int i=0; i < numAttribs; i++) {
        delete attribCtors[i];
    }
    delete[] attribCtors;
    delete cond;
}

void cAttributes::SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) {
    this->tokenContainer = tokenContainer;
}

void cAttributes::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    this->tokenContainer = tokenContainer;
    if (cond) {
        cond->SetTokenContainer(tokenContainer);
    }
    for (int i=0; i < numAttribs; i++) {
        if (!attribCtors[i])
            continue;
        attribCtors[i]->SetTokenContainer(tokenContainer);
    }
}

void cAttributes::SetContainer(int x, int y, int width, int height) {
    container.SetX(x);
    container.SetY(y);
    container.SetWidth(width);
    container.SetHeight(height);
}

void cAttributes::SetX(int x) {
    attribs[(int)eCommonAttribs::x] = x;
}

void cAttributes::SetY(int y) {
    attribs[(int)eCommonAttribs::y] = y;
}

void cAttributes::SetWidth(int width) {
    attribs[(int)eCommonAttribs::width] = width;
}

void cAttributes::SetHeight(int height) {
    attribs[(int)eCommonAttribs::height] = height;
}

void cAttributes::Cache(void) {
    if (cond) {
        cond->SetGlobals(globals);
        cond->SetTokenContainer(tokenContainer);
        cond->Prepare();
    }
    for (int i=0; i < numAttribs; i++) {
        if (!attribCtors[i])
            continue;
        attribCtors[i]->SetContainer(&container);
        attribCtors[i]->SetGlobals(globals);
        attribCtors[i]->SetTokenContainer(tokenContainer);
        if (attribCtors[i]->CacheStatic()) {
            int val = attribCtors[i]->GetValue();
            attribs[i] = val;
            delete attribCtors[i];
            attribCtors[i] = NULL;
        } else {
            attribCtors[i]->PrepareTokens();
        }
    }
}

int cAttributes::GetValue(int id) {
    if (!attribCtors[id + (int)eCommonAttribs::count])
        return attribs[(int)id + (int)eCommonAttribs::count];
    return attribCtors[id + (int)eCommonAttribs::count]->Calculate();
}


int cAttributes::X(void) { 
    int x = 0;
    if (!attribCtors[(int)eCommonAttribs::x])
        x = attribs[(int)eCommonAttribs::x];
    else
        x = attribCtors[(int)eCommonAttribs::x]->Calculate();
    x += container.X();
    return x;
}

int cAttributes::Y(void) { 
    int y = 0;
    if (!attribCtors[(int)eCommonAttribs::y])
        y = attribs[(int)eCommonAttribs::y];
    else
        y = attribCtors[(int)eCommonAttribs::y]->Calculate();
    y += container.Y();
    return y;
}

int cAttributes::Width(void) {
    if (!attribCtors[(int)eCommonAttribs::width])
        return attribs[(int)eCommonAttribs::width];
    return attribCtors[(int)eCommonAttribs::width]->Calculate();
}

int cAttributes::Height(void) { 
    if (!attribCtors[(int)eCommonAttribs::height])
        return attribs[(int)eCommonAttribs::height];
    return attribCtors[(int)eCommonAttribs::height]->Calculate();
}

bool cAttributes::DoExecute(void) {
    if (!cond)
        return true;
    return cond->True();
}


void cAttributes::Debug(void) {
    esyslog("skindesigner:            container %d %d %dx%d", container.X(), container.Y(), container.Width(), container.Height());
    for (int i=0; i < numAttribs; i++) {
        if (attribs[i] != ATTR_UNKNOWN) {
            if (i == (int)eCommonAttribs::debug)
                continue;
            const char *attName = "attribute";
            if (i < (int)eCommonAttribs::count)
                attName = CommonAttributeName(i);
            else
                attName = AttributeName(i - (int)eCommonAttribs::count);
            dsyslog("skindesigner:            fixed Value %s = %d", attName, attribs[i]);
        }
        if (attribCtors[i]) {
            const char *attName = "attribute";
            if (i < (int)eCommonAttribs::count)
                attName = CommonAttributeName(i);
            else
                attName = AttributeName(i - (int)eCommonAttribs::count);
            dsyslog("skindesigner:            %s constructor:", attName);
            attribCtors[i]->Debug();
        }
    }
    if (cond) {
        cond->Debug();
    }
}

/***************************************************************************
* Protected Functions
***************************************************************************/
int cAttributes::CommonAttributeId(const char *att) {
    if (!strcmp(att, "condition"))
        return ATTR_COND;
    map<string, int>::iterator hit = commonAttribIDs.find(att);
    if (hit != commonAttribIDs.end())
        return hit->second;
    return ATTR_UNKNOWN;
}

const char *cAttributes::CommonAttributeName(int id) {
    if (id < 0 || id >= (int)eCommonAttribs::count)
        return "";
    map<int, string>::iterator hit = commonAttribNames.find(id);
    if (hit != commonAttribNames.end())
        return hit->second.c_str();
    return "";
}

int cAttributes::AttributeId(const char *att) {
    int id = CommonAttributeId(att);
    if (id != ATTR_UNKNOWN)
        return id;
    map<string, int>::iterator hit = attribIDs.find(att);
    if (hit != attribIDs.end())
        id = (int)hit->second + (int)eCommonAttribs::count;
    return id;
}

const char *cAttributes::AttributeName(int id) {
    map<int, string>::iterator hit = attribNames.find(id);
    if (hit != attribNames.end())
        return hit->second.c_str();
    return "";    
}

bool cAttributes::SetCommon(int id, const char *val) {
    if (id == ATTR_COND) {
        cond = new cCondition(val);
        return true;
    }
    if (id == (int)eCommonAttribs::debug) {
        SetBool(id, val);
        return true;    
    } else if (id == (int)eCommonAttribs::x || id == (int)eCommonAttribs::width) {
        attribCtors[id] = new cNumericExpr(val);
        return true;
    } else if (id == (int)eCommonAttribs::y || id == (int)eCommonAttribs::height) {
        attribCtors[id] = new cNumericExpr(val);
        attribCtors[id]->SetVertical();
        return true;
    }
    return false;
}

bool cAttributes::IdEqual(int id, int compId) {
    if (compId + (int)eCommonAttribs::count == id)
        return true;
    return false;
}


void cAttributes::SetBool(int id, const char *val) {
    if (!strcmp(val, "true")) {
        attribs[id] = 1;
    } else {
        attribs[id] = 0;
    }
}

void cAttributes::SetViewElementMode(int id, const char *val) {
    eViewElementMode mode = eViewElementMode::regular;
    if (!strcmp(val, "light"))
        mode = eViewElementMode::light;
    attribs[id] = (int)mode;
}

void cAttributes::SetShiftType(int id, const char *val) {
    eShiftType shiftType = eShiftType::none;
    if (!strcmp(val, "left"))
        shiftType = eShiftType::left;
    else if (!strcmp(val, "right"))
        shiftType = eShiftType::right;
    else if (!strcmp(val, "top"))
        shiftType = eShiftType::top;
    else if (!strcmp(val, "bottom"))
        shiftType = eShiftType::bottom;
    else {
        esyslog("skindesigner: unknown shift type \"%s\"", val);
        return;
    }
    attribs[id] = (int)shiftType;
}

void cAttributes::SetShiftMode(int id, const char *val) {
    eShiftMode shiftMode = eShiftMode::linear;
    if (!strcmp(val, "slowed"))
        shiftMode = eShiftMode::slowedDown;
    attribs[id] = (int)shiftMode;
}

void cAttributes::SetScrollMode(int id, const char *val) {
    eScrollMode mode = eScrollMode::none;
    if (!strcmp(val, "forthandback"))
        mode = eScrollMode::forthandback;
    else if (!strcmp(val, "carriagereturn"))
        mode = eScrollMode::carriagereturn;
    attribs[id] = (int)mode;
}

void cAttributes::SetScrollSpeed(int id, const char *val) {
    eScrollSpeed speed = eScrollSpeed::medium;
    if (!strcmp(val, "slow"))
        speed = eScrollSpeed::slow;
    else if (!strcmp(val, "fast"))
        speed = eScrollSpeed::fast;
    else if (!strcmp(val, "medium"))
        speed = eScrollSpeed::medium;
    attribs[id] = (int)speed;
}

void cAttributes::SetOrientation(int id, const char *val) {
    eOrientation orientation = eOrientation::none;
    if (!strcmp(val, "horizontal"))
        orientation = eOrientation::horizontal;
    else if (!strcmp(val, "vertical"))
        orientation = eOrientation::vertical;
    else if (!strcmp(val, "absolute"))
        orientation = eOrientation::absolute;
    attribs[id] = (int)orientation;
}

void cAttributes::SetAlign(int id, const char *val) {
    eAlign align = eAlign::left;
    if (!strcmp(val, "center")) {
        align = eAlign::center;
    } else if (!strcmp(val, "right")) {
        align = eAlign::right;
    } else if (!strcmp(val, "top")) {
        align = eAlign::top;
    } else if (!strcmp(val, "bottom")) {
        align = eAlign::bottom;
    } else if (!strcmp(val, "left")) {
        align = eAlign::left;
    }
    attribs[id] = (int)align;
}

void cAttributes::SetDirection(int id, const char *val) {
    eDirection direction = eDirection::none;
    if (!strcmp(val, "bottomup"))
        direction = eDirection::bottomup;
    else if (!strcmp(val, "topdown"))
        direction = eDirection::topdown;
    attribs[id] = (int)direction;
}

void cAttributes::SetButton(int id, const char *val) {
    eButtonType button = eButtonType::none;
    if (!strcmp(val, "left"))
        button = eButtonType::left;
    else if (!strcmp(val, "right"))
        button = eButtonType::right;
    attribs[id] = (int)button;
}
/***************************************************************************
* Private Functions
***************************************************************************/
void cAttributes::SetCommonAttributesDefs(void) {
    commonAttribIDs.insert(pair<string, int>("x", (int)eCommonAttribs::x));
    commonAttribIDs.insert(pair<string, int>("y", (int)eCommonAttribs::y));
    commonAttribIDs.insert(pair<string, int>("width", (int)eCommonAttribs::width));
    commonAttribIDs.insert(pair<string, int>("height", (int)eCommonAttribs::height));
    commonAttribIDs.insert(pair<string, int>("debug", (int)eCommonAttribs::debug));
    commonAttribNames.insert(pair<int, string>((int)eCommonAttribs::x, "x"));
    commonAttribNames.insert(pair<int, string>((int)eCommonAttribs::y, "y"));
    commonAttribNames.insert(pair<int, string>((int)eCommonAttribs::width, "width"));
    commonAttribNames.insert(pair<int, string>((int)eCommonAttribs::height, "height"));
    commonAttribNames.insert(pair<int, string>((int)eCommonAttribs::debug, "debug"));
}

/***************************************************************************
* cFunction
***************************************************************************/
cFunction::cFunction(cArea *owner, int numAttributes) : cAttributes(numAttributes) {
    funcType = "Unknown";
    owningArea = owner;
    color = NULL;
    name = NULL;
    scrolling = false;
}

cFunction::cFunction(const cFunction &other) : cAttributes(other) {
    funcType = other.funcType;
    owningArea = NULL;
    color = NULL;
    if (other.color)
        color = new cColor(*other.color);
    name = NULL;
    if (other.name)
        name = strdup(other.name);
    scrolling = other.scrolling;
}

cFunction::~cFunction(void) {
    delete color;
    free(name);
}

void cFunction::SetLoopInfo(cLoopInfo *loopInfo) {
    for (int i=0; i < numAttribs; i++) {
        if (!attribCtors[i])
            continue;
        attribCtors[i]->SetLoopInfo(loopInfo);
    }
    if (cond)
        cond->SetLoopInfo(loopInfo);
}

void cFunction::Cache(void) {
    if (color) {
        color->SetGlobals(globals);
        color->Cache();
    }
    cAttributes::Cache();
}

void cFunction::CacheFuncReferences(void) {
    for (int i=0; i < numAttribs; i++) {
        if (!attribCtors[i])
            continue;
        vector<cFactor*> refFactors = attribCtors[i]->GetRefFactors();
        for (vector<cFactor*>::iterator it = refFactors.begin(); it != refFactors.end(); it++) {
            cFactor *f = *it;
            if (!f->funcRefName)
                continue;
            cFunction *fRef = owningArea->GetFunction(f->funcRefName);
            if (fRef) {
                f->funcRef = fRef;
            }
        }
    }
}

int cFunction::GetX(eAlign align, int x0, int colWidth) {
    int containerWidth = colWidth > 0 ? colWidth : container.Width();
    int x = x0 + X();
    if (align == eAlign::right) {
        x = x0 + containerWidth - FuncWidth();
    } else if (align == eAlign::center) {
        x = x0 + (containerWidth - FuncWidth()) / 2;
    }
    return x;
}

int cFunction::GetY(eAlign valign, int y0, int rowHeight) {
    int containerHeight = rowHeight > 0 ? rowHeight : container.Height();
    int y = y0 + Y();
    if (valign == eAlign::bottom) {
        y = y0 + containerHeight - FuncHeight();
    } else if (valign == eAlign::center) {
        y = y0 + (containerHeight - FuncHeight()) / 2;
    }
    return y;
}

void cFunction::Debug(void) {
    esyslog("skindesigner:        ---> Function %s", funcType);
    cAttributes::Debug();
    if (name) {
        esyslog("skindesigner:            name %s", name);
    }
    if (color) {
        color->Debug();
    }
}

/***************************************************************************
* Protected Functions
***************************************************************************/

void cFunction::SetColor(const char *val) {
    color = new cColor(val);
}

void cFunction::SetAnimType(int id, const char *val) {
    eAnimType animType = eAnimType::none;
    if (!strcmp(val, "blink"))
        animType = eAnimType::blink;
    else if (!strcmp(val, "animated"))
        animType = eAnimType::animated;
    attribs[id] = (int)animType; 
}

void cFunction::SetOverflow(int id, const char *val) {
    eOverflowType overflowType = eOverflowType::none;
    if (!strcmp(val, "linewrap"))
        overflowType = eOverflowType::wrap;
    else if (!strcmp(val, "cut"))
        overflowType = eOverflowType::cut;
    attribs[id] = (int)overflowType;
}
