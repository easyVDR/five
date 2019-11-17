#ifndef __XMLGLOBALS_H
#define __XMLGLOBALS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <vdr/plugin.h>

using namespace std;

typedef uint32_t tColor;

// --- cGlobals -------------------------------------------------------------

class cGlobals {
private:
    time_t customTokenChange;
    time_t lastCustomTokenQuery;
    string language;
    string DoTranslate(string token);
    void DeleteCurledBrackets(string &token);
    map <string, tColor> colors;
    map <string, string> fonts;
    map <string, int> intVars;
    map <string, double> doubleVars;
    map <string, string> stringVars;
    map <string, map< string, string > > translations;
    map <int, string> customStringTokens;
    map <int, int> customIntTokens;
public:
    cGlobals(void);
    virtual ~cGlobals(void) {};
    bool ReadFromXML(void);
    void AddColor(string &name, tColor &col);
    bool GetColor(string &name, tColor &col);
    void AddFont(string &name, string &font);
    bool GetFont(string name, string &font);
    void AddInt(string &name, int value);
    void ReplaceIntVars(string &value);
    bool GetInt(string name, int &val);
    void AddDouble(string &name, string &value);
    void ReplaceDoubleVars(string &value);
    void AddString(string &name, string &value);
    void ReplaceStringVars(string &value);
    bool GetString(string name, string &value);
    bool AddTranslation(string name,  map < string, string > transl);
    bool Translate(string text, string &translation);
    void AddCustomInt(int num, int value);
    void AddCustomString(int num, string &value);
    bool GetCustomInt(int num, int &val);
    bool GetCustomString(int num, string &val);
    map <int, string> GetCustomStringTokens(void);
    map <int, int> GetCustomIntTokens(void);
    bool CustomTokenChange(void);
    void ResetCustomTokenChange(void);
    void ListCustomTokens(void);
    void Debug(void);
};

#endif //__XMLGLOBALS_H