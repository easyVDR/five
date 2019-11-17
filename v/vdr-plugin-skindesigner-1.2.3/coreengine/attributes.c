#include "attributes.h"
#include "../config.h"

/***************************************************************************
* cViewAttribs
***************************************************************************/
cViewAttribs::cViewAttribs(int numAttributes) : cAttributes(numAttributes) {
    orientation = NULL;
    SetAttributesDefs();
}

cViewAttribs::~cViewAttribs(void) {
    delete orientation;
}

void cViewAttribs::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown view attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eViewAttribs::shifttype)) {
            SetShiftType(id, attVal);
        } else if (IdEqual(id, (int)eViewAttribs::shiftmode)) {
            SetShiftMode(id, attVal);
        } else if (IdEqual(id, (int)eViewAttribs::orientation)) {
            SetOrientationDynamic(id, attVal);
        } else if (IdEqual(id, (int)eViewAttribs::hideroot)) {
            SetBool(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
            if ( (id == (int)eViewAttribs::starty + (int)eCommonAttribs::count) ||
                 (id == (int)eViewAttribs::scaletvy + (int)eCommonAttribs::count) ||
                 (id == (int)eViewAttribs::scaletvheight + (int)eCommonAttribs::count) ) {
                attribCtors[id]->SetVertical();                
            }
        }   
    }
}

void cViewAttribs::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("fadetime", (int)eViewAttribs::fadetime));
    attribIDs.insert(pair<string, int>("shifttime", (int)eViewAttribs::shifttime));
    attribIDs.insert(pair<string, int>("shifttype", (int)eViewAttribs::shifttype));
    attribIDs.insert(pair<string, int>("shiftmode", (int)eViewAttribs::shiftmode));
    attribIDs.insert(pair<string, int>("startx", (int)eViewAttribs::startx));
    attribIDs.insert(pair<string, int>("starty", (int)eViewAttribs::starty));
    attribIDs.insert(pair<string, int>("scaletvx", (int)eViewAttribs::scaletvx));
    attribIDs.insert(pair<string, int>("scaletvy", (int)eViewAttribs::scaletvy));
    attribIDs.insert(pair<string, int>("scaletvwidth", (int)eViewAttribs::scaletvwidth));
    attribIDs.insert(pair<string, int>("scaletvheight", (int)eViewAttribs::scaletvheight));
    attribIDs.insert(pair<string, int>("orientation", (int)eViewAttribs::orientation));
    attribIDs.insert(pair<string, int>("debuggrid", (int)eViewAttribs::debuggrid));
    attribIDs.insert(pair<string, int>("hideroot", (int)eViewAttribs::hideroot));
    attribNames.insert(pair<int, string>((int)eViewAttribs::fadetime, "fadetime"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::shifttime, "shifttime"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::shifttype, "shifttype"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::shiftmode, "shiftmode"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::startx, "startx"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::starty, "starty"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::scaletvx, "scaletvx"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::scaletvy, "scaletvy"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::scaletvwidth, "scaletvwidth"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::scaletvheight, "scaletvheight"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::orientation, "orientation"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::debuggrid, "debuggrid"));
    attribNames.insert(pair<int, string>((int)eViewAttribs::hideroot, "hideroot"));
}

void cViewAttribs::Cache(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    cAttributes::Cache();
    if (orientation) {
        orientation->SetGlobals(globals);
        orientation->SetTokenContainer(tokenContainer);
        orientation->Cache();
        char *res = orientation->DeterminateText();
        if (res) {
            SetOrientation((int)eViewAttribs::orientation + (int)eCommonAttribs::count, res);
        }
        free(res);
    }
}

void cViewAttribs::Debug(void) {
    esyslog("skindesigner:        --> View Attribs");
    cAttributes::Debug();
}

eOrientation cViewAttribs::Orientation(void) {
    int orientation = GetValue((int)eViewAttribs::orientation);
    if (orientation == -1)
        return eOrientation::vertical;
    if (orientation == (int)eOrientation::none)
        return eOrientation::vertical;
    return (eOrientation)orientation;
}

cRect cViewAttribs::TvFrame(void) {
    int frameX = GetValue((int)eViewAttribs::scaletvx);
    int frameY = GetValue((int)eViewAttribs::scaletvy);
    int frameWidth = GetValue((int)eViewAttribs::scaletvwidth);
    int frameHeight = GetValue((int)eViewAttribs::scaletvheight);
    if (frameX < 0 || frameY < 0 || frameWidth <= 0 || frameHeight <= 0)
        return cRect::Null;
    frameX += cOsd::OsdLeft();
    frameY += cOsd::OsdTop();
    return cRect(frameX, frameY, frameWidth, frameHeight);
}

void cViewAttribs::SetOrientationDynamic(int id, const char *val) {
    if (strchr(val, '{') && strchr(val, '}')) {
        orientation = new cTextExpr(val);
    } else {
        SetOrientation(id, val);
    }
}

/***************************************************************************
* cViewElementAttribs
***************************************************************************/
cViewElementAttribs::cViewElementAttribs(int numAttributes) : cAttributes(numAttributes) {
    name = NULL;
    clearOnDisplay = NULL;
    SetAttributesDefs();
}

cViewElementAttribs::cViewElementAttribs(const cViewElementAttribs &other) : cAttributes(other) {
    name = NULL;
    clearOnDisplay = NULL;
}

cViewElementAttribs::~cViewElementAttribs(void) {
    free(name);
    free(clearOnDisplay);
}

void cViewElementAttribs::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown view element attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eViewElementAttribs::mode)) {
            SetViewElementMode(id, attVal);
        } else if (IdEqual(id, (int)eViewElementAttribs::shifttype)) {
            SetShiftType(id, attVal);
        } else if (IdEqual(id, (int)eViewElementAttribs::shiftmode)) {
            SetShiftMode(id, attVal);
        } else if (IdEqual(id, (int)eViewElementAttribs::orientation)) {
            SetOrientation(id, attVal);
        } else if (IdEqual(id, (int)eViewElementAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eViewElementAttribs::clearondisplay)) {
            clearOnDisplay = strdup(attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
            if (id == (int)eViewElementAttribs::starty + (int)eCommonAttribs::count) {
                attribCtors[id]->SetVertical();                
            }
        }
    }
}

void cViewElementAttribs::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("delay", (int)eViewElementAttribs::delay));
    attribIDs.insert(pair<string, int>("fadetime", (int)eViewElementAttribs::fadetime));
    attribIDs.insert(pair<string, int>("shifttime", (int)eViewElementAttribs::shifttime));
    attribIDs.insert(pair<string, int>("shifttype", (int)eViewElementAttribs::shifttype));
    attribIDs.insert(pair<string, int>("shiftmode", (int)eViewElementAttribs::shiftmode));
    attribIDs.insert(pair<string, int>("startx", (int)eViewElementAttribs::startx));
    attribIDs.insert(pair<string, int>("starty", (int)eViewElementAttribs::starty));
    attribIDs.insert(pair<string, int>("orientation", (int)eViewElementAttribs::orientation));
    attribIDs.insert(pair<string, int>("mode", (int)eViewElementAttribs::mode));
    attribIDs.insert(pair<string, int>("name", (int)eViewElementAttribs::name));
    attribIDs.insert(pair<string, int>("clearondisplay", (int)eViewElementAttribs::clearondisplay));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::delay, "delay"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::fadetime, "fadetime"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::shifttime, "shifttime"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::shifttype, "shifttype"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::shiftmode, "shiftmode"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::startx, "startx"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::starty, "starty"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::orientation, "orientation"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::mode, "mode"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eViewElementAttribs::clearondisplay, "clearondisplay"));
}

eOrientation cViewElementAttribs::Orientation(void) {
    int orientation = GetValue((int)eViewElementAttribs::orientation);
    if (orientation == -1)
        return eOrientation::vertical;
    if (orientation == (int)eOrientation::none)
        return eOrientation::vertical;
    return (eOrientation)orientation;
}

void cViewElementAttribs::Debug(void) {
    esyslog("skindesigner:   ---> View Element Attribs");
    cAttributes::Debug();
}

/***************************************************************************
* cViewListAttribs
***************************************************************************/
cViewListAttribs::cViewListAttribs(int numAttributes) : cAttributes(numAttributes) {
    determinateFont = NULL;
    SetAttributesDefs();
}

cViewListAttribs::~cViewListAttribs(void) {
    free(determinateFont);
}

void cViewListAttribs::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown view list attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eViewListAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eViewListAttribs::determinatefont)) {
            determinateFont = strdup(attVal);
        } else if (IdEqual(id, (int)eViewListAttribs::orientation)) {
            SetOrientation(id, attVal);
        } else if (IdEqual(id, (int)eViewListAttribs::shifttype)) {
            SetShiftType(id, attVal);
        } else if (IdEqual(id, (int)eViewListAttribs::shiftmode)) {
            SetShiftMode(id, attVal);
        } else if (IdEqual(id, (int)eViewListAttribs::button)) {
            SetButton(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

int cViewListAttribs::NumListElements(void) { 
    return GetValue((int)eViewListAttribs::numlistelements); 
}

int cViewListAttribs::MenuItemWidth(void) {
    return GetValue((int)eViewListAttribs::menuitemwidth); 
}

const char *cViewListAttribs::DeterminateFont(void) {
    return determinateFont; 
}

eAlign cViewListAttribs::Align(void) { 
    int align = GetValue((int)eViewListAttribs::align);
    if (align < 0)
        return eAlign::top;
    return (eAlign)align; 
}

eOrientation cViewListAttribs::Orientation(void) {
    int orientation = GetValue((int)eViewListAttribs::orientation);
    if (orientation < 0)
        return eOrientation::vertical;
    return (eOrientation)orientation; 
}

void cViewListAttribs::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eViewListAttribs::align));
    attribIDs.insert(pair<string, int>("menuitemwidth", (int)eViewListAttribs::menuitemwidth));
    attribIDs.insert(pair<string, int>("determinatefont", (int)eViewListAttribs::determinatefont));
    attribIDs.insert(pair<string, int>("numlistelements", (int)eViewListAttribs::numlistelements));
    attribIDs.insert(pair<string, int>("orientation", (int)eViewListAttribs::orientation));
    attribIDs.insert(pair<string, int>("fadetime", (int)eViewListAttribs::fadetime));
    attribIDs.insert(pair<string, int>("shifttime", (int)eViewListAttribs::shifttime));
    attribIDs.insert(pair<string, int>("shifttype", (int)eViewListAttribs::shifttype));
    attribIDs.insert(pair<string, int>("shiftmode", (int)eViewListAttribs::shiftmode));
    attribIDs.insert(pair<string, int>("startx", (int)eViewListAttribs::startx));
    attribIDs.insert(pair<string, int>("starty", (int)eViewListAttribs::starty));
    attribIDs.insert(pair<string, int>("condition", (int)eViewListAttribs::condition));
    attribIDs.insert(pair<string, int>("button", (int)eViewListAttribs::button));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::menuitemwidth, "menuitemwidth"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::determinatefont, "determinatefont"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::numlistelements, "numlistelements"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::orientation, "orientation"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::fadetime, "fadetime"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::shifttime, "shifttime"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::shifttype, "shifttype"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::shiftmode, "shiftmode"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::startx, "startx"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::starty, "starty"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::condition, "condition"));
    attribNames.insert(pair<int, string>((int)eViewListAttribs::button, "button"));
}

void cViewListAttribs::Debug(void) {
    esyslog("skindesigner:   ---> View List Attribs");
    esyslog("skindesigner:        DeterminateFont %s", determinateFont);
    cAttributes::Debug();
}

/***************************************************************************
* cAreaAttribs
***************************************************************************/
cAreaAttribs::cAreaAttribs(int numAttributes) : cAttributes(numAttributes) {
    name = NULL;
    scrollElement = NULL;
    dynamic = false;
    SetAttributesDefs();
}

cAreaAttribs::cAreaAttribs(const cAreaAttribs &other) : cAttributes(other) {
    name = NULL;
    if (other.name)
        name = new cTextExpr(*other.name);
    scrollElement = NULL;
    if (other.scrollElement)
        scrollElement = strdup(other.scrollElement);
    dynamic = false;
}

cAreaAttribs::~cAreaAttribs(void) {
    delete name;
    free(scrollElement);
}

void cAreaAttribs::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown area attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eAreaAttribs::scrollelement)) {
            scrollElement = strdup(attVal);
        } else if (IdEqual(id, (int)eAreaAttribs::mode)) {
            SetScrollMode(id, attVal);
        } else if (IdEqual(id, (int)eAreaAttribs::orientation)) {
            SetOrientation(id, attVal);
        } else if (IdEqual(id, (int)eAreaAttribs::scrollspeed)) {
            SetScrollSpeed(id, attVal);
        } else if (IdEqual(id, (int)eAreaAttribs::background)) {
            SetBool(id, attVal);
        } else if (IdEqual(id, (int)eAreaAttribs::indicator)) {
            SetBool(id, attVal);
        } else if (IdEqual(id, (int)eAreaAttribs::name)) {
            name = new cTextExpr(attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);    
        }
    }
}

int cAreaAttribs::Layer(void) {
    if (GetValue((int)eAreaAttribs::layer) > 0) {
        return GetValue((int)eAreaAttribs::layer);
    }
    return 1;
}

bool cAreaAttribs::BackgroundArea(void) {
    int isBackground = GetValue((int)eAreaAttribs::background);
    if (isBackground == 1)
        return true;
    return false; 
}

bool cAreaAttribs::IndicatorArea(void) {
    int isIndicator = GetValue((int)eAreaAttribs::indicator);
    if (isIndicator == 1)
        return true;
    return false; 
}

void cAreaAttribs::CheckDynamic(void) {
    for (int i = (int)eCommonAttribs::x; i <= (int)eCommonAttribs::height; ++i ) {
        if (attribCtors[i] && attribCtors[i]->Dynamic()) {
            dynamic = true;
            return;
        }
    }
}

const char *cAreaAttribs::Name(void) { 
    if (name)
        return name->DeterminateText(); 
    return NULL;
}

void cAreaAttribs::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("layer", (int)eAreaAttribs::layer));
    attribIDs.insert(pair<string, int>("transparency", (int)eAreaAttribs::transparency));
    attribIDs.insert(pair<string, int>("mode", (int)eAreaAttribs::mode));
    attribIDs.insert(pair<string, int>("orientation", (int)eAreaAttribs::orientation));
    attribIDs.insert(pair<string, int>("scrollelement", (int)eAreaAttribs::scrollelement));
    attribIDs.insert(pair<string, int>("scrollspeed", (int)eAreaAttribs::scrollspeed));
    attribIDs.insert(pair<string, int>("delay", (int)eAreaAttribs::delay));
    attribIDs.insert(pair<string, int>("background", (int)eAreaAttribs::background));
    attribIDs.insert(pair<string, int>("indicator", (int)eAreaAttribs::indicator));
    attribIDs.insert(pair<string, int>("name", (int)eAreaAttribs::name));
    attribIDs.insert(pair<string, int>("scrollheight", (int)eAreaAttribs::scrollheight));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::layer, "layer"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::transparency, "transparency"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::mode, "mode"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::orientation, "orientation"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::scrollelement, "scrollelement"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::scrollspeed, "scrollspeed"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::delay, "delay"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::background, "background"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eAreaAttribs::scrollheight, "scrollheight"));
}

void cAreaAttribs::Cache(void) {
    cAttributes::Cache();
    if (name) {
        name->SetGlobals(globals);
        name->SetTokenContainer(tokenContainer);
        name->Cache();
    }
}

void cAreaAttribs::Debug(void) {
    if (!name) {
        esyslog("skindesigner:        ---> Area Attribs");
    } else {
        esyslog("skindesigner:        ---> Tab %s Attribs", name->DeterminateText());        
    }
    cAttributes::Debug();
}

/***************************************************************************
* cAreaContainerAttribs
***************************************************************************/
cAreaContainerAttribs::cAreaContainerAttribs(int numAttributes) : cAttributes(numAttributes) {
    SetAttributesDefs();
}

cAreaContainerAttribs::cAreaContainerAttribs(const cAreaContainerAttribs &other) : cAttributes(other) {
}

cAreaContainerAttribs::~cAreaContainerAttribs(void) {
}

void cAreaContainerAttribs::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown area container attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
    }
}

void cAreaContainerAttribs::SetAttributesDefs(void) {
}

void cAreaContainerAttribs::Debug(void) {
    esyslog("skindesigner:            ---> Area Container Attribs");
    cAttributes::Debug();
}

