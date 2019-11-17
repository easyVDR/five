#include "xmlparser.h"
#include "../config.h"
#include "../extensions/helpers.h"

using namespace std;

cXmlParser::cXmlParser(void) {
    sdOsd = NULL;
    view = NULL;
    globals = NULL;
    skinSetup = NULL;
}

cXmlParser::~cXmlParser() {
}

/*********************************************************************
* PUBLIC Functions
*********************************************************************/
bool cXmlParser::ReadView(cView *view, string xmlFile) {
    if (!view)
        return false;
    this->view = view;
    string xmlPath = GetPath(xmlFile);
    if (! ReadXMLFile(xmlPath.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    if (! CheckNodeName( view->GetViewName() ))
        return false;
    return true;
}

bool cXmlParser::ParseView(void) {
    if (!view)
        return false;
    vector<stringpair> rootAttribs = ParseAttributes();
    view->SetAttributes(rootAttribs);

    cViewMenu *menuView = dynamic_cast<cViewMenu*>(view);
    cViewChannel *channelView = dynamic_cast<cViewChannel*>(view);
    cViewTracks *tracksView = dynamic_cast<cViewTracks*>(view);

    if (!LevelDown())
        return false;

    do {
        if (view->ValidViewElement(NodeName())) {
            ParseViewElement(NodeName());
        } else if (channelView) {
            if (view->ValidViewList(NodeName())) {
                ParseViewList(NULL, NodeName());
            } else {
                esyslog("skindesigner: unknown node %s", NodeName());
                return false;                
            }
        } else if (menuView) {
            if (menuView->ValidSubView(NodeName())) {
                ParseSubView(NodeName());
            } else if (view->ValidViewList(NodeName())) {
                ParseViewList();
            } else {
                esyslog("skindesigner: unknown node %s", NodeName());
                return false;
            }
        } else if (tracksView) {
            if (view->ValidViewList(NodeName())) {
                ParseViewList();
            } else {
                esyslog("skindesigner: unknown node %s", NodeName());
                return false;
            }
        } else {
            esyslog("skindesigner: unknown view %s", NodeName());
            return false;
        }
    } while (NextNode());

    return true;
}

bool cXmlParser::ParsePluginView(void) {
    if (!view)
        return false;
    vector<stringpair> rootAttribs = ParseAttributes();
    view->SetAttributes(rootAttribs);

    if (!LevelDown())
        return false;

    do {
        if (CheckNodeName("viewelement")) {
            ParsePluginViewElement();
        } else if (CheckNodeName("grid")) {
            ParseGrid();
        } else if (CheckNodeName("tab")) {
            ParseViewTabPlugin();
        } else if (CheckNodeName("scrollbar")) {
            ParsePluginViewElement(true, false);
        } else if (CheckNodeName("tablabels")) {
            ParsePluginViewElement(false, true);
        } else {
            esyslog("skindesigner: unknown node %s in plugin view", NodeName());
        }
    } while (NextNode());

    return true;
}

bool cXmlParser::ReadPluginView(string templateName) {
    string xmlPath = GetPath(templateName);
    DeleteDocument();
    if (! ReadXMLFile(xmlPath.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    return true;
}

bool cXmlParser::ParsePluginView(string plugName, int plugId, int menuNumber, int menuType) {
    if (!view)
        return false;
    cViewMenu *menuView = dynamic_cast<cViewMenu*>(view);
    if (!menuView) {
        return false;
    }
    vector<pair<string, string> > attribs = ParseAttributes();

    if (!LevelDown())
        return false;

    cSubView *plugView = cViewMenu::CreatePluginview(plugName.c_str(), plugId, menuNumber, menuType);
    plugView->SetAttributes(attribs);
    menuView->AddPluginview(plugView);
    do {
        if (plugView->ValidViewElement(NodeName())) {
            ParseViewElement(NodeName(), plugView);
        } else if (plugView->ValidViewList(NodeName())) {
            ParseViewList(plugView);
        } else if (CheckNodeName("tab")) {
            ParseViewTab(plugView);
        } else {
            esyslog("skindesigner: error - unknown %s", NodeName());
            return false;
        }
    } while (NextNode());
    LevelUp();
    return true;
}

bool cXmlParser::ReadGlobals(cGlobals *globals, string xmlFile) {
    this->globals = globals;
    string xmlPath = GetPath(xmlFile);
    DeleteDocument();
    if (! ReadXMLFile(xmlPath.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    if (! CheckNodeName("globals") )
        return false;
    return true;
}

bool cXmlParser::ParseGlobals(void) {
    if (!LevelDown())
        return false;
    do {
        if (CheckNodeName("colors")) {
            ParseGlobalColors();
        } else if (CheckNodeName("variables")) {
            ParseGlobalVariables();
        } else if (CheckNodeName("fonts")) {
            ParseGlobalFonts();
        } else if (CheckNodeName("translations")) {
            ParseTranslations();
        }
    } while (NextNode());
    return true;
}

bool cXmlParser::ReadSkinSetup(cSkinSetup *skinSetup, string xmlFile) {
    this->skinSetup = skinSetup;
    if (! ReadXMLFile(xmlFile.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    if (! CheckNodeName("setup") )
        return false;
    return true;
}

bool cXmlParser::ParseSkinSetup(string skin) {
    if (!LevelDown())
        return false;
    do {
        if (CheckNodeName("menu")) {
            ParseSetupMenu();
        } else if (CheckNodeName("translations")) {
            ParseTranslations();
        }
    } while (NextNode());
    return true;
}
/*********************************************************************
* PRIVATE Functions
*********************************************************************/
bool cXmlParser::ParseSubView(string name) {
    if (!view)
        return false;
    cViewMenu *menuView = dynamic_cast<cViewMenu*>(view);
    if (!menuView) {
        return false;
    }
    vector<pair<string, string> > attribs = ParseAttributes();

    if (!LevelDown())
        return false;

    cSubView *subView = cViewMenu::CreateSubview(name.c_str());
    subView->SetAttributes(attribs);
    menuView->AddSubview(name.c_str(), subView);

    do {
        if (subView->ValidViewElement(NodeName())) {
            ParseViewElement(NodeName(), subView);
        } else if (subView->ValidViewList(NodeName())) {
            ParseViewList(subView);
        } else if (CheckNodeName("tab")) {
            ParseViewTab(subView);           
        } else {
            return false;
        }
    } while (NextNode());
    LevelUp();
    return true;
}

void cXmlParser::ParseViewElement(string name, cView *subView) {
    if (!view)
        return;
    const char *viewElementName = NodeName();
    vector<stringpair> attributes = ParseAttributes();

    if (!LevelDown())
        return;

    //correct detailheader name
    if (subView) {
        if (!strcmp(subView->GetViewName(), "menudetailedepg") && !name.compare("detailheader")) {
            name = "detailheaderepg";
        } else if (!strcmp(subView->GetViewName(), "menudetailedrecording") && !name.compare("detailheader")) {
            name = "detailheaderrec";
        } else if (startswith(subView->GetViewName(), "menuplugin") && !name.compare("detailheader")) {
            name = "detailheaderplugin";
        }
    }

    cViewElement *viewElement = cViewElement::CreateViewElement(name.c_str(), view->GetViewName());
    if (!viewElement) {
        LevelUp();
        return;
    }

    viewElement->SetOsd(sdOsd);
    viewElement->SetAttributes(attributes);
    do {
        if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
            continue;
        }
        cAreaNode *area = NULL;
        if (CheckNodeName("area") || CheckNodeName("areascroll")) {
            area = ParseArea();
        } else {
            area = ParseAreaContainer();
        }
        viewElement->AddArea(area);
    } while (NextNode());

    LevelUp();

    if (subView)
        subView->AddViewElement(viewElementName, viewElement);
    else
        view->AddViewElement(viewElementName, viewElement);

}

void cXmlParser::ParsePluginViewElement(bool isScrollbar, bool isTabLabels) {
    if (!view)
        return;
    cViewPlugin *pluginView = dynamic_cast<cViewPlugin*>(view);
    if (!pluginView)
        return;

    vector<stringpair> attributes = ParseAttributes();

    if (!LevelDown())
        return;

    cViewElement *viewElement = NULL;
    if (isScrollbar) {
        viewElement = new cVeDmScrollbar();
    } else if (isTabLabels) {
        viewElement = new cVeDmTablabels();
    } else {
        viewElement = new cVePlugin();        
    }

    viewElement->SetOsd(sdOsd);
    viewElement->SetAttributes(attributes);
    do {
        if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
            continue;
        }
        cAreaNode *area = NULL;
        if (CheckNodeName("area") || CheckNodeName("areascroll")) {
            area = ParseArea();
        } else {
            area = ParseAreaContainer();
        }
        viewElement->AddArea(area);
    } while (NextNode());

    LevelUp();
    
    if (isScrollbar) {
        pluginView->AddScrollbar(dynamic_cast<cVeDmScrollbar*>(viewElement));
    } else if (isTabLabels) {
        pluginView->AddTablabels(dynamic_cast<cVeDmTablabels*>(viewElement));
    } else {
        pluginView->AddViewElement(dynamic_cast<cVePlugin*>(viewElement));
    }
}

void cXmlParser::ParseViewList(cView *subView, const char *listName) {
    if (!view)
        return;

    const char *name = NULL;
    if (subView)
        name = subView->GetViewName();
    else
        name = view->GetViewName();

    vector<stringpair> attribs = ParseAttributes();
    cViewList *viewList = NULL;
    if (!listName)
        viewList = cViewList::CreateViewList(name);
    else
        viewList = cViewList::CreateViewList(listName);
    viewList->SetAttributes(attribs);

    if (!LevelDown())
        return;

    do {
        if (CheckNodeName("currentelement")) {
            cViewElement *currentElement = NULL;
            if (!listName)
                currentElement = cViewList::CreateCurrentElement(name);
            else
                currentElement = cViewList::CreateCurrentElement(listName);
            currentElement->SetOsd(sdOsd);
            vector<stringpair> attribsList = ParseAttributes();
            currentElement->SetAttributes(attribsList);
            if (!LevelDown())
                continue;
            do {
                if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
                    esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
                    continue;
                }
                cAreaNode *area = NULL;
                if (CheckNodeName("area") || CheckNodeName("areascroll")) {
                    area = ParseArea();
                } else {
                    area = ParseAreaContainer();
                }
                currentElement->AddArea(area);
            } while (NextNode());
            LevelUp();
            viewList->AddCurrentElement(currentElement);
        } else if (CheckNodeName("listelement")) {
            cViewElement *listElement = NULL;
            if (!listName)
                listElement = cViewList::CreateListElement(name);
            else
                listElement = cViewList::CreateListElement(listName);
            listElement->SetOsd(sdOsd);
            vector<stringpair> attribsList = ParseAttributes();
            listElement->SetAttributes(attribsList);            
            if (!LevelDown())
                return;
            do {
                if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
                    esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
                    continue;
                }
                cAreaNode *area = NULL;
                if (CheckNodeName("area") || CheckNodeName("areascroll")) {
                    area = ParseArea();
                } else {
                    area = ParseAreaContainer();
                }
                listElement->AddArea(area);
            } while (NextNode());
            LevelUp();
            viewList->AddListElement(listElement);
        }
    } while (NextNode());
    LevelUp();
    
    if (subView)
        subView->AddViewList(viewList);
    else if (listName) {
        cViewChannel *channelView = dynamic_cast<cViewChannel*>(view);
        channelView->AddChannelViewList(listName, viewList);
    } else {
        view->AddViewList(viewList);        
    }
    
}

void cXmlParser::ParseViewTab(cView *subView) {
    if (!view || !subView)
        return;
    cSubView *menuSubView = dynamic_cast<cSubView*>(subView);
    if (!menuSubView) {
        return;
    }
    vector<stringpair> attribs = ParseAttributes();
    cArea *tab = new cArea();
    tab->SetOsd(sdOsd);
    tab->SetTab();
    tab->SetAttributes(attribs);
    ParseFunctionCalls(tab);
    menuSubView->AddTab(tab);
}

void cXmlParser::ParseViewTabPlugin(void) {
    if (!view)
        return;
    cViewPlugin *plugView = dynamic_cast<cViewPlugin*>(view);
    if (!plugView) {
        return;
    }
    vector<stringpair> attribs = ParseAttributes();
    cArea *tab = new cArea();
    tab->SetOsd(sdOsd);
    tab->SetTab();
    tab->SetAttributes(attribs);
    ParseFunctionCalls(tab);
    plugView->AddTab(tab);
}

void cXmlParser::ParseGrid(void) {
    if (!view)
        return;
    cViewPlugin *pluginView = dynamic_cast<cViewPlugin*>(view);
    if (!pluginView)
        return;

    vector<stringpair> attributes = ParseAttributes();

    if (!LevelDown())
        return;

    cViewGrid *viewGrid = new cViewGrid();
    viewGrid->SetAttributes(attributes);
    cGridElement *gridElement = new cGridElement();
    gridElement->SetOsd(sdOsd);

    do {
        if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in grid", NodeName());
            continue;
        }
        cAreaNode *area = NULL;
        if (CheckNodeName("area") || CheckNodeName("areascroll")) {
            area = ParseArea();
        } else {
            area = ParseAreaContainer();
        }
        gridElement->AddArea(area);
    } while (NextNode());
    LevelUp();
    
    viewGrid->AddGridElement(gridElement);
    pluginView->AddViewGrid(viewGrid);
}

cArea *cXmlParser::ParseArea(void) {
    vector<stringpair> attribs = ParseAttributes();
    cArea *area = new cArea();
    area->SetOsd(sdOsd);
    if (CheckNodeName("areascroll")) {
        area->SetScrolling();
    }
    area->SetAttributes(attribs);
    ParseFunctionCalls(area);
    return area;    
}

cAreaContainer *cXmlParser::ParseAreaContainer(void) {
    vector<stringpair> attribs = ParseAttributes();

    cAreaContainer *areaContainer = new cAreaContainer();
    areaContainer->SetAttributes(attribs);

    if (!LevelDown())
        return areaContainer;
    do {
        if (!CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in areacontainer", NodeName());
            continue;
        }
        cArea *area = ParseArea();
        areaContainer->AddArea(area);
    } while (NextNode());
    LevelUp();
    return areaContainer;
}

void cXmlParser::ParseFunctionCalls(cArea *area) {
    if (!view)
        return;
    if (!LevelDown())
        return;
    do {
        if (CheckNodeName("loop")) {
            vector<stringpair> attribs = ParseAttributes();
            cFunction *f = area->AddFunction(NodeName(), attribs);
            cFuncLoop *loopFunc = dynamic_cast<cFuncLoop*>(f);
            if (!loopFunc)
                continue;
            ParseLoopFunctionCalls(area, loopFunc);
        } else if (area->ValidFunction(NodeName())) {
            vector<stringpair> attribs = ParseAttributes();
            area->AddFunction(NodeName(), attribs);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseLoopFunctionCalls(cArea *area, cFuncLoop *loopFunc) {
    if (!view)
        return;
    if (!LevelDown())
        return;
    do {
        if (area->ValidFunction(NodeName())) {
            vector<stringpair> attribs = ParseAttributes();
            area->AddFunction(NodeName(), attribs, loopFunc);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseGlobalColors(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("color")) {
            continue;
        }
        string attributeName = "name";
        string colorName  = "";
        string colorValue = "";
        bool ok = GetAttribute(attributeName, colorName);
        if (ok) {
            ok = GetNodeValue(colorValue);
            if (ok)
                InsertColor(colorName, colorValue);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::InsertColor(string name, string value) {
    if (value.size() != 8)
        return;
    std::stringstream str;
    str << value;
    tColor colVal;
    str >> std::hex >> colVal;
    globals->AddColor(name, colVal);
}

void cXmlParser::ParseGlobalVariables(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("var")) {
            continue;
        }
        string attributeName = "name";
        string attributeType = "type";
        string varName  = "";
        string varType  = "";
        string varValue = "";

        bool ok1 = GetAttribute(attributeName, varName);
        bool ok2 = GetAttribute(attributeType, varType);

        if (ok1 && ok2) {
            bool ok = GetNodeValue(varValue);
            if (ok)
                InsertVariable(varName, varType, varValue);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::InsertVariable(string name, string type, string value) {
    if (!type.compare("int")) {
        int val = atoi(value.c_str());
        globals->AddInt(name, val);
    } else if (!type.compare("double")) {
        globals->AddDouble(name, value);
    } else if (!type.compare("string")) {
        globals->AddString(name, value);
    }
}

void cXmlParser::ParseGlobalFonts(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("font")) {
            continue;
        }
        string attributeName = "name";
        string fontName = "";
        string fontValue = "";

        bool ok = GetAttribute(attributeName, fontName);
        if (ok) {
            ok = GetNodeValue(fontValue);
            if (ok) {
                globals->AddFont(fontName, fontValue);
            }
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseTranslations(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("token")) {
            continue;
        }
        string attributeName = "name";
        string tokenName = "";

        if (!GetAttribute(attributeName, tokenName))
            continue;

        if (!LevelDown())
            continue;

        stringmap tokenTranslations;
        do {
            if (!CheckNodeName("trans")) {
                continue;
            }
            string attributeName = "lang";
            string language = "";
            if (!GetAttribute(attributeName, language))
                continue;
            string translation = "";
            if (!GetNodeValue(translation))
                continue;
            tokenTranslations.insert(stringpair(language, translation));
        } while (NextNode());
        LevelUp();

        if (globals) {
            globals->AddTranslation(tokenName, tokenTranslations);
        } else if (skinSetup) {
            skinSetup->SetTranslation(tokenName, tokenTranslations);
        }

    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseSetupMenu(void) {
    if (!skinSetup)
        return;
    if (!LevelDown())
        return;
    do {
        if (CheckNodeName("parameter")) {
            ParseSetupParameter();
        } else if (CheckNodeName("submenu")) {
            string attributeName = "name";
            string subMenuName = "";
            string attributeDisplayText = "displaytext";
            string subDisplayText = "";
            GetAttribute(attributeName, subMenuName);
            GetAttribute(attributeDisplayText, subDisplayText);
            skinSetup->SetSubMenu(subMenuName, subDisplayText);
            ParseSetupMenu();
        }
    } while (NextNode());
    skinSetup->SubMenuDone();
    LevelUp();
}

void cXmlParser::ParseSetupParameter(void) {
    if (!skinSetup)
        return;
    string attributeType = "type";
    string paramType = "";
    string attributeName = "name";
    string paramName = "";
    string attributeDisplayText = "displaytext";
    string paramDisplayText = "";
    string attributeHelpText = "helptext";
    string paramHelpText = "";
    string attributeMin = "min";
    string paramMin = "";
    string attributeMax = "max";
    string paramMax = "";
    string paramValue = "";
    string attributeOptions = "options";
    string paramOptions="";

    GetAttribute(attributeType, paramType);
    GetAttribute(attributeName, paramName);
    GetAttribute(attributeDisplayText, paramDisplayText);
    GetAttribute(attributeHelpText, paramHelpText);
    GetAttribute(attributeMin, paramMin);
    GetAttribute(attributeMax, paramMax);
    GetAttribute(attributeOptions, paramOptions);
    GetNodeValue(paramValue);

    skinSetup->SetParameter(paramType, paramName, paramDisplayText, paramHelpText, paramMin, paramMax, paramValue, paramOptions);
}

string cXmlParser::GetPath(string xmlFile) {
    string activeSkin = Setup.OSDSkin;
    string activeTheme = Setup.OSDTheme;
    string path = "";
    if (!xmlFile.compare("globals.xml")) {
        path = *cString::sprintf("%s%s/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), xmlFile.c_str());
    } else if (!xmlFile.compare("theme.xml")) {
        path = *cString::sprintf("%s%s/themes/%s/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), activeTheme.c_str(), xmlFile.c_str());
    } else if (!xmlFile.compare("setup.xml")) {
        path = *cString::sprintf("%s%s/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), xmlFile.c_str());
    } else {
        path = *cString::sprintf("%s%s/xmlfiles/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), xmlFile.c_str());
    }
    return path;
}

