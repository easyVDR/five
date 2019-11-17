#ifndef __XMLPARSER_H
#define __XMLPARSER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <vdr/plugin.h>
#include "../extensions/libxmlwrapper.h"
#include "osdwrapper.h"
#include "view.h"
#include "viewlist.h"
#include "../extensions/skinsetup.h"

using namespace std;

// --- cXmlParser -------------------------------------------------------------

class cXmlParser : public cLibXMLWrapper {
private:
    cSdOsd *sdOsd;
    cView *view;
    cGlobals *globals;
    cSkinSetup *skinSetup;
    //parsing views
    bool ParseSubView(string name);
    void ParseViewElement(string name, cView *subView = NULL);
    void ParsePluginViewElement(bool isScrollbar = false, bool isTabLabels = false);
    void ParseViewList(cView *subView = NULL, const char *listName = NULL);
    void ParseViewTab(cView *subView);
    void ParseViewTabPlugin(void);
    void ParseGrid(void);
    cArea *ParseArea(void);
    cAreaContainer *ParseAreaContainer(void);
    void ParseFunctionCalls(cArea *area);
    void ParseLoopFunctionCalls(cArea *area, cFuncLoop *loopFunc);
    //parsing globals
    void ParseGlobalColors(void);
    void InsertColor(string name, string value);
    void ParseGlobalVariables(void);
    void InsertVariable(string name, string type, string value);
    void ParseGlobalFonts(void);
    void ParseTranslations(void);
    //parsing skin setup
    void ParseSetupMenu(void);
    void ParseSetupParameter(void);
    //helpers
    string GetPath(string xmlFile);
public:
    cXmlParser(void);
    virtual ~cXmlParser(void);
    void SetOsd(cSdOsd *osd) { sdOsd = osd; };
    //reading views
    bool ReadView(cView *view, string xmlFile);
    bool ParseView(void);
    //Parse advanced plugin view
    bool ParsePluginView(void);
    //reading basic plugin menu views
    bool ReadPluginView(string templateName);
    bool ParsePluginView(string plugName, int plugId, int menuNumber, int menuType);
    //reading globals
    bool ReadGlobals(cGlobals *globals, string xmlFile);
    bool ParseGlobals(void);
    //reading skin setups
    bool ReadSkinSetup(cSkinSetup *skinSetup, string xmlFile);
    bool ParseSkinSetup(string skin);
};

#endif //__XMLPARSER_H
