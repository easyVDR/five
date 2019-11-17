#ifndef __ATTRIBUTE_H
#define __ATTRIBUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <vdr/skins.h>
#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"
#include "definitions.h"
#include "complextypes.h"

class cArea;
/******************************************************************
* cAttributes
******************************************************************/
class cAttributes {
private:
    map<string, int> commonAttribIDs;
    map<int, string> commonAttribNames;
    void SetCommonAttributesDefs(void);
protected:
    cGlobals *globals;
    skindesignerapi::cTokenContainer *tokenContainer;
    cRect container;
    int numAttribs;
    int *attribs;
    cNumericExpr **attribCtors;
    cCondition *cond;
    map<string, int> attribIDs;
    map<int, string> attribNames;
    int CommonAttributeId(const char *att);
    const char *CommonAttributeName(int id);
    int AttributeId(const char *att);
    const char *AttributeName(int id);
    bool SetCommon(int id, const char *val);
    virtual bool IdEqual(int id, int compId);
    void SetBool(int id, const char *val);
    void SetViewElementMode(int id, const char *val);
    void SetShiftType(int id, const char *val);
    void SetShiftMode(int id, const char *val);
    void SetScrollMode(int id, const char *val);
    void SetScrollSpeed(int id, const char *val);
    void SetOrientation(int id, const char *val);
    void SetDirection(int id, const char *val);
    void SetAlign(int id, const char *val);
    void SetButton(int id, const char *val);
public:
    cAttributes(int numAttributes);
    cAttributes(const cAttributes &other);
    virtual ~cAttributes(void);
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer);
    virtual void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    virtual void SetContainer(int x, int y, int width, int height);
    virtual void Set(vector<stringpair> &attributes) {};
    void SetX(int width);
    void SetY(int height);
    void SetWidth(int width);
    void SetHeight(int height);
    virtual void Cache(void);
    int GetValue(int id);
    int X(void);
    int Y(void);
    int Width(void);
    int Height(void);
    int DoDebug(void) { return attribs[(int)eCommonAttribs::debug] == 1 ? true : false; };
    bool DoExecute(void);
    virtual void Debug(void);
};
/******************************************************************
* cLoopInfo
******************************************************************/
class cLoopInfo {
public:
    int colWidth;
    int rowHeight;
    int index;
    int row;
    cLoopInfo(void) {
        colWidth = 0;
        rowHeight = 0;
        index = 0;
        row = 0;
    };
};
/******************************************************************
* cFunction
******************************************************************/
class cFunction : public cAttributes, public cListObject {
private:
    cArea *owningArea;
protected:
    const char *funcType;
    cColor *color;
    char *name;
    bool scrolling;
    void SetColor(const char *val);
    void SetAnimType(int id, const char *val);
    void SetOverflow(int id, const char *val);
public:
    cFunction(cArea *owner, int numAttributes);
    cFunction(const cFunction &other);
    virtual ~cFunction(void);
    virtual void SetLoopInfo(cLoopInfo *loopInfo);
    void SetOwner(cArea *owner) { owningArea = owner; };
    const char *Name(void) { return name; };
    virtual void Cache(void);
    void CacheFuncReferences(void);
    void Scrolling(bool scrolling) { this->scrolling = scrolling; };
    virtual void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0) {};
    virtual int FuncX(void) { return X(); };
    virtual int FuncY(void) { return Y(); };
    virtual int FuncWidth(void) { return Width(); };
    virtual int FuncHeight(void) { return Height(); };
    virtual int Align(void) { return (int)eAlign::left; };
    virtual int Valign(void) { return (int)eAlign::top; };
    int GetX(eAlign align, int x0, int colWidth);
    int GetY(eAlign valign, int y0, int rowHeight);
    virtual bool Blinking(void) { return false; };
    virtual int BlinkFreq(void) { return -1; };
    virtual void Debug(void);
};

#endif //__ATTRIBUTE_H