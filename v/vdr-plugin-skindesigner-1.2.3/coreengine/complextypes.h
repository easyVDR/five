#ifndef __COMPLEXTYPES_H
#define __COMPLEXTYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <vdr/skins.h>
#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"

class cLoopInfo;
class cFunction;
/******************************************************************
* helpers
******************************************************************/
char *RemoveSpace(char *e);
void ReplaceDecimalpoint(char *e);
void ReplaceStart(char *e, int num);
void ReplaceEnd(char *e, int num);

/******************************************************************
* cCondition
******************************************************************/
enum class eCondOp {
    tAnd,
    tOr
};

enum class eCondType {
    token,
    negtoken,
    lowerInt,
    equalInt,
    notequalInt,
    greaterInt,
    isset,
    empty,
    equalString,
    notEqualString,
    contains,
    notContains
};

enum class eCondTokenType {
    inttoken,
    stringtoken,
    looptoken
};

class cCond : public cListObject {
public:
    cCond(const char *expression);
    cCond(const cCond &other);
    virtual ~cCond(void);
    void Debug(void);
    char *expr;
    eCondOp operation;
    eCondType type;
    eCondTokenType tokenType;
    bool constant;
    bool isTrue;
    int tokenIndex;
    int compareValue;
    char *compareStrValue;
};

class cCondition {
private:
    char *expr;
    cGlobals *globals;
    skindesignerapi::cTokenContainer *tokenContainer;
    cLoopInfo *loopInfo;
    cList<cCond> conds;
    void Tokenize(void);
    void PrepareTokens(void);
    void SetTokenCond(cCond *c);
    void SetIntegerCond(cCond *c);
    void SetStringCond(cCond *c);
    void SetStringCompareCond(cCond *c);
    void SetTokenIndex(cCond *c, const char *token);
public:
    cCondition(const char *expression);
    cCondition(const cCondition &other);
    virtual ~cCondition(void);
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) {this->tokenContainer = tokenContainer; };
    void SetLoopInfo(cLoopInfo *loopInfo) { this->loopInfo = loopInfo; };
    void Prepare(void);
    bool True(void);
    void Debug(void);
};

/******************************************************************
* cNumericExpr
******************************************************************/
enum class eFactorType {
    constant = 0,
    stringtoken,
    inttoken,
    looptoken,
    xref,
    yref,
    widthref,
    heightref,
    areawidth,
    areaheight,
    columnwidth,
    rowheight
};

class cFactor: public cListObject {
public:
    cFactor(void) {
        multiplication = true;
        type = eFactorType::constant;
        constValue = 1.0f;
        tokenIndex = -1;
        funcRefName = NULL;
        funcRef = NULL;
    };
    cFactor(const cFactor &other) {
        multiplication = other.multiplication;
        type = other.type;
        constValue = other.constValue;
        tokenIndex = other.tokenIndex;
        funcRefName = NULL;
        if (other.funcRefName)
            funcRefName = strdup(other.funcRefName);
        funcRef = other.funcRef;
    }
    ~cFactor(void) {
        free(funcRefName);
    };
    bool multiplication;
    eFactorType type;
    double constValue;
    int tokenIndex;
    char *funcRefName;
    cFunction *funcRef;
};

class cSummand : public cListObject {
public:
    cSummand(const char *summand);
    cSummand(const cSummand &other);
    ~cSummand(void);
    void Debug(void);
    char *summand;
    bool positive;
    cList<cFactor> factors;
};

class cNumericExpr {
private:
    cGlobals *globals;
    cRect *container;
    skindesignerapi::cTokenContainer *tokenContainer;
    cLoopInfo *loopInfo;
    char *expr;
    cList<cSummand> summands;
    bool horizontal;
    int value;
    bool dynamic;
    //common string functions
    bool IsNumeric(const char *e);
    bool IsNumericExpression(const char *e);
    bool PercentValue(const char *e);
    char *ReplacePercentValue(char *e);
    char *ReplaceToken(char *e, const char* token, int value);
    char *ReplaceTokens(char *e, const char* token, int value);
    //calculate numeric expressions
    int EvaluateExpression(char *e);
    double EvaluateExpressionDouble(char *e);
    double ParseAtom(char*& e);
    double ParseFactors(char*& e);
    double ParseSummands(char*& e);
    //prepare expressions with tokens
    void CreateSummands(void);
    void CreateFactors(void);
    bool SetTokenFactor(cFactor *f, char *tokenName);
    bool SetReferenceFactor(cFactor *f, char *tokenName);
    bool SetGeometryFactor(cFactor *f, char *tokenName);
    void ConsolidateSummand(void);
    void ConsolidateFactors(void);
public:
    cNumericExpr(const char *expression);
    cNumericExpr(const cNumericExpr &other);
    virtual ~cNumericExpr(void);
    void SetContainer(cRect *container) { this->container = container; };
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) { this->tokenContainer = tokenContainer; };
    void SetLoopInfo(cLoopInfo *loopInfo) { this->loopInfo = loopInfo; };
    void SetVertical(void) { horizontal = false; };
    bool CacheStatic(void);
    void PrepareTokens(void);
    vector<cFactor*> GetRefFactors(void);
    int GetValue(void) { return value; };
    bool Dynamic(void) { return dynamic; };
    int Calculate(void);
    void Debug(void);
};

/******************************************************************
* cColor
******************************************************************/
class cColor {
private:
    cGlobals *globals;
    char *expr;
    tColor value;
public:
    cColor(const char *expression);
    cColor(const cColor &other);
    virtual ~cColor(void);
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void Cache(void);
    tColor Color(void);
    void Debug(void);
};

/******************************************************************
* cTextExpr
******************************************************************/
enum class eTexttokenType {
    constant = 0,
    stringtoken,
    inttoken,
    looptoken,
    printftoken,
    condstringtoken,
    condinttoken,
};

enum class ePrintfVarType {
    stringtoken,
    inttoken,
    looptoken
};

struct sPrintfInfo {
    ePrintfVarType type;
    int index;
};

class cTextToken: public cListObject {
public:
    cTextToken(void) {
        type = eTexttokenType::constant;
        constValue = NULL;
        printfExpr = NULL;
        printfResult = NULL;
        tokenIndex = -1;
        condStart = NULL;
        condEnd = NULL;
    };
    cTextToken(const cTextToken &other) {
        type = other.type;
        constValue = NULL;
        if (other.constValue)
            constValue = strdup(other.constValue);
        printfExpr = NULL;
        if (other.printfExpr)
            printfExpr = strdup(other.printfExpr);
        printfVarIndices = other.printfVarIndices;
        printfResult = NULL;
        if (other.printfResult)
            printfResult = strdup(other.printfResult);
        tokenIndex = other.tokenIndex;
        condStart = NULL;
        if (other.condStart)
            condStart = strdup(other.condStart);
        condEnd = NULL;
        if (other.condEnd)
            condEnd = strdup(other.condEnd);
    };
    ~cTextToken(void) {
        free(constValue);
        free(printfExpr);
    };
    eTexttokenType type;
    char *constValue;
    int tokenIndex;
    char *printfExpr;
    vector<sPrintfInfo> printfVarIndices;
    char *printfResult;
    char *condStart;
    char *condEnd;
};

class cTextExpr {
private:
    cGlobals *globals;
    skindesignerapi::cTokenContainer *tokenContainer;
    cLoopInfo *loopInfo;
    char *expr;
    cList<cTextToken> textTokens;
    void Translate(void);
    void Tokenize(void);
    void PrepareTokens(void);
    bool CheckGlobals(cTextToken *t);
    bool ParsePrintfToken(cTextToken *t);
    void DeterminatePrintfToken(cTextToken *t);
    void ParseCondToken(cTextToken *t);
    char *CopyTextPart(char *start, char *stop, bool incLastChar= true);
public:
    cTextExpr(const char *expression);
    cTextExpr(const cTextExpr &other);
    virtual ~cTextExpr(void);
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void SetTokenContainer(skindesignerapi::cTokenContainer *tokenContainer) { this->tokenContainer = tokenContainer; };
    void SetLoopInfo(cLoopInfo *loopInfo) { this->loopInfo = loopInfo; };
    void CorrectImagePath(void);
    void Cache(void);
    char *DeterminateText(void);
    void Debug(const char *exprName);
};

#endif //__COMPLEXTYPES_H