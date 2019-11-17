#include "complextypes.h"
#include "../config.h"

/******************************************************************
* helpers
******************************************************************/
char *RemoveSpace(char *e) {
    if (!e)
        return e;
    int numSpaces = 0;
    int exprLen = strlen(e);
    for (int i = 0; i < exprLen; ++i) {
        if (isspace(e[i]))
            ++numSpaces;
    }
    if (numSpaces == 0)
        return e;

    char *replaced = (char*)malloc(exprLen - numSpaces + 1);
    replaced[exprLen - numSpaces] = '\0';
    int j = 0;
    for (int i = 0; i < exprLen; ++i) {
        if (!isspace(e[i])){
            replaced[j++] = e[i];
        }
    }
    free(e);
    return replaced;
}

void ReplaceDecimalpoint(char *e) {
    int size = (int)strlen(e);
    for ( int i = 0; i < size; ++i ) {
       if (e[i] == '.')
           e[i] = config.decPoint;
    }
}

void ReplaceStart(char *e, int num) {
    int size = (int)strlen(e);
    if (size <= num)
        return;
    for ( int i = 0; i < size; ++i ) {
       if (i < size - num)
           e[i] = e[i+num];
       else e[i] = 0;
    }
}

void ReplaceEnd(char *e, int num) {
    int size = (int)strlen(e) - 1;
    if (size <= num)
        return;
    for ( int i = size; i > size-num; --i ) {
        e[i] = 0;
    }
}
/******************************************************************
* cCond
******************************************************************/
cCond::cCond(const char *expression) {
    this->expr = strdup(expression);
    operation = eCondOp::tAnd;
    type = eCondType::token;
    constant = false;
    isTrue = false;
    tokenIndex = -1;
    compareValue = -1;
    compareStrValue = NULL;
}

cCond::cCond(const cCond &other) {
    expr = strdup(other.expr);
    operation = other.operation;
    type = other.type;
    constant = other.constant;
    tokenType = other.tokenType;
    isTrue = other.isTrue;
    tokenIndex = other.tokenIndex;
    compareValue = other.compareValue;
    compareStrValue = NULL;
    if (other.compareStrValue)
        compareStrValue = strdup(other.compareStrValue);
}

cCond::~cCond(void) { 
    free(expr); 
    free(compareStrValue);
}

void cCond::Debug(void) {
    esyslog("skindesigner:              cond %s, operation %s, type %d", expr, 
                                                            (operation == eCondOp::tAnd) ? "++" : "||",
                                                            (int)type);
    if (constant)
        esyslog("skindesigner:              constant cond: %s", isTrue ? "TRUE" : "FALSE");
    if (tokenIndex >= 0)
        esyslog("skindesigner:              token index: %d", tokenIndex);
    if (compareValue >= 0)
        esyslog("skindesigner:              compare value: %d", compareValue);
    if (compareStrValue)
        esyslog("skindesigner:              compare string value: %s", compareStrValue);
}

/******************************************************************
* cCondition
******************************************************************/
cCondition::cCondition(const char *expression) {
    expr = strdup(expression);
    globals = NULL;
    tokenContainer = NULL;
    loopInfo = NULL;
}

cCondition::cCondition(const cCondition &other) {
    expr = strdup(other.expr);
    globals = NULL;
    tokenContainer = NULL;
    loopInfo = NULL;
    for (const cCond *cond = other.conds.First(); cond; cond = other.conds.Next(cond))
        conds.Add(new cCond(*cond));
}

cCondition::~cCondition(void) {
    free(expr);
}

void cCondition::Prepare(void) {
    expr = RemoveSpace(expr);
    Tokenize();
    PrepareTokens();
}

bool cCondition::True(void) {
    if (conds.Count() == 0)
        return true;
    bool ok = true;
    for (cCond *c = conds.First(); c; c = conds.Next(c)) {
        bool condTrue = true;
        //evaluate condition
        if (c->constant) 
        {
            condTrue = c->isTrue;
        } 
        else if (c->type == eCondType::token) 
        {
            if (c->tokenType == eCondTokenType::inttoken) {
                int tokenVal = tokenContainer->IntToken(c->tokenIndex);
                condTrue = (tokenVal > 0) ? true : false;                
            } else if (c->tokenType == eCondTokenType::stringtoken) {
                char *tokenVal = tokenContainer->StringToken(c->tokenIndex);
                if (tokenVal)
                    condTrue = (!strcmp(tokenVal, "1")) ? true : false;
            } else if (c->tokenType == eCondTokenType::looptoken) {
                if (loopInfo && loopInfo->row >= 0) {
                    char *tokenVal = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, c->tokenIndex);
                    if (tokenVal)
                        condTrue = (!strcmp(tokenVal, "1")) ? true : false;
                }
            }

        } 
        else if (c->type == eCondType::negtoken) 
        {
            if (c->tokenType == eCondTokenType::inttoken) {
                int tokenVal = tokenContainer->IntToken(c->tokenIndex);
                condTrue = (tokenVal > 0) ? false : true;
            } else if (c->tokenType == eCondTokenType::stringtoken) {
                char *tokenVal = tokenContainer->StringToken(c->tokenIndex);
                if (tokenVal)
                    condTrue = (!strcmp(tokenVal, "1")) ? false : true;
            } else if (c->tokenType == eCondTokenType::looptoken) {
                if (loopInfo && loopInfo->row >= 0) {
                    char *tokenVal = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, c->tokenIndex);
                    if (tokenVal)
                        condTrue = (!strcmp(tokenVal, "1")) ? false : true;
                }
            }
        }
        else if (c->type == eCondType::lowerInt || c->type == eCondType::equalInt || c->type == eCondType::notequalInt || c->type == eCondType::greaterInt) 
        {
            if (c->tokenType == eCondTokenType::inttoken) {
                int tokenVal = tokenContainer->IntToken(c->tokenIndex);
                if (c->type == eCondType::lowerInt)
                    condTrue = (tokenVal < c->compareValue) ? true : false;
                else if (c->type == eCondType::equalInt)
                    condTrue = (tokenVal == c->compareValue) ? true : false;
                else if (c->type == eCondType::notequalInt)
                    condTrue = (tokenVal != c->compareValue) ? true : false;
                else if (c->type == eCondType::greaterInt)
                    condTrue = (tokenVal > c->compareValue) ? true : false;
            } else if (c->tokenType == eCondTokenType::stringtoken) {
                char *tokenVal = tokenContainer->StringToken(c->tokenIndex);
                if (tokenVal) {
                    int intVal = atoi(tokenVal);
                    if (c->type == eCondType::lowerInt)
                        condTrue = (intVal < c->compareValue) ? true : false;
                    else if (c->type == eCondType::equalInt)
                        condTrue = (intVal == c->compareValue) ? true : false;
                    else if (c->type == eCondType::notequalInt)
                        condTrue = (intVal != c->compareValue) ? true : false;
                    else if (c->type == eCondType::greaterInt)
                        condTrue = (intVal > c->compareValue) ? true : false;
                }
            } else if (c->tokenType == eCondTokenType::looptoken) {
                if (loopInfo && loopInfo->row >= 0) {
                    char *tokenVal = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, c->tokenIndex);
                    if (tokenVal) {
                        int intVal = atoi(tokenVal);
                        if (c->type == eCondType::lowerInt)
                            condTrue = (intVal < c->compareValue) ? true : false;
                        else if (c->type == eCondType::equalInt)
                            condTrue = (intVal == c->compareValue) ? true : false;
                        else if (c->type == eCondType::notequalInt)
                            condTrue = (intVal != c->compareValue) ? true : false;
                        else if (c->type == eCondType::greaterInt)
                            condTrue = (intVal > c->compareValue) ? true : false;
                    }
                }
            }
        }
        else if (c->type == eCondType::isset || c->type == eCondType::empty) 
        {
            if (c->tokenType == eCondTokenType::stringtoken) {
                char *tokenVal = tokenContainer->StringToken(c->tokenIndex);
                if (tokenVal) {
                    if (c->type == eCondType::isset)
                        condTrue = strlen(tokenVal) > 0 ? true : false;
                    else if (c->type == eCondType::empty)
                        condTrue = strlen(tokenVal) == 0 ? true : false;
                } else {
                    if (c->type == eCondType::isset)
                        condTrue = false;
                    else if (c->type == eCondType::empty)
                        condTrue = true;
                }
            } else if (c->tokenType == eCondTokenType::looptoken) {
                if (loopInfo && loopInfo->row >= 0) {
                    char *tokenVal = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, c->tokenIndex);
                    if (tokenVal) {
                        if (c->type == eCondType::isset)
                            condTrue = strlen(tokenVal) > 0 ? true : false;
                        else if (c->type == eCondType::empty)
                            condTrue = strlen(tokenVal) == 0 ? true : false;
                    } else {
                        if (c->type == eCondType::isset)
                            condTrue = false;
                        else if (c->type == eCondType::empty)
                            condTrue = true;
                    }
                }
            }
        }
        else if (c->type == eCondType::equalString || c->type == eCondType::notEqualString || c->type == eCondType::contains || c->type == eCondType::notContains) 
        {
            if (c->tokenType == eCondTokenType::stringtoken) {
                char *tokenVal = tokenContainer->StringToken(c->tokenIndex);
                if (tokenVal) {
                    if (c->type == eCondType::equalString)
                        condTrue = !strcmp(tokenVal, c->compareStrValue) ? true : false;
                    else if (c->type == eCondType::notEqualString)
                        condTrue = strcmp(tokenVal, c->compareStrValue) ? true : false;
                    else if (c->type == eCondType::contains)
                        condTrue = strstr(tokenVal, c->compareStrValue) ? true : false;
                    else if (c->type == eCondType::notContains)
                        condTrue = !strstr(tokenVal, c->compareStrValue) ? true : false;
                }
            } else if (c->tokenType == eCondTokenType::looptoken) {
                if (loopInfo && loopInfo->row >= 0) {
                    char *tokenVal = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, c->tokenIndex);
                    if (tokenVal) {
                        if (c->type == eCondType::equalString)
                            condTrue = !strcmp(tokenVal, c->compareStrValue) ? true : false;
                        else if (c->type == eCondType::notEqualString)
                            condTrue = strcmp(tokenVal, c->compareStrValue) ? true : false;
                        else if (c->type == eCondType::contains)
                            condTrue = strstr(tokenVal, c->compareStrValue) ? true : false;
                        else if (c->type == eCondType::notContains)
                            condTrue = !strstr(tokenVal, c->compareStrValue) ? true : false;
                    }
                }
            }
        }        //link
        if (c->operation == eCondOp::tAnd) {
            ok = ok && condTrue;
        } else if (c->operation == eCondOp::tOr) {
            ok = ok || condTrue;                
        }
    }
    return ok;
}

void cCondition::Debug(void) {
    esyslog("skindesigner:            condition \"%s\"", expr);
    for (cCond *c = conds.First(); c; c = conds.Next(c)) {
        c->Debug();
    }
    esyslog("skindesigner:            condition is %s", True() ? "TRUE" : "FALSE");
}

void cCondition::Tokenize(void) {
    char *condition = strdup(expr);
    char delimiter[] = "+|";
    eCondOp operation = eCondOp::tAnd;

    char *cond = strtok(condition, delimiter);
    while (cond) {
        eCondType type = eCondType::token;
        if (startswith(cond, "{") && endswith(cond, "}")) {
            type = eCondType::token;
        } else if (startswith(cond, "not{") && endswith(cond, "}")) {
            type = eCondType::negtoken;
        } else if (startswith(cond, "lt({") && endswith(cond, ")")) {
            type = eCondType::lowerInt;
        } else if (startswith(cond, "eq({") && endswith(cond, ")")) {
            type = eCondType::equalInt;
        } else if (startswith(cond, "noteq({") && endswith(cond, ")")) {
            type = eCondType::notequalInt;
        } else if (startswith(cond, "gt({") && endswith(cond, ")")) {
            type = eCondType::greaterInt;
        } else if (startswith(cond, "isset{") && endswith(cond, "}")) {
            type = eCondType::isset;
        } else if (startswith(cond, "empty{") && endswith(cond, "}")) {
            type = eCondType::empty;
        } else if (startswith(cond, "strequal({") && endswith(cond, ")")) {
            type = eCondType::equalString;
        } else if (startswith(cond, "strnotequal({") && endswith(cond, ")")) {
            type = eCondType::notEqualString;
        } else if (startswith(cond, "strcontains({") && endswith(cond, ")")) {
            type = eCondType::contains;
        } else if (startswith(cond, "strnotcontains({") && endswith(cond, ")")) {
            type = eCondType::notContains;
        } else {
            esyslog("skindesigner: invalid condition term %s", cond);
            cond = strtok(NULL, delimiter);
            continue;
        }
        cCond *c = new cCond(cond);
        c->operation = operation;
        c->type = type;
        conds.Add(c);
        if (expr[cond - condition + strlen(cond)] == '+')
            operation = eCondOp::tAnd;
        else
            operation = eCondOp::tOr;
        cond = strtok(NULL, delimiter);
    }
    free(condition);
}

void cCondition::PrepareTokens(void) {
    for (cCond *c = conds.First(); c; c = conds.Next(c)) {
        switch (c->type) {
            case eCondType::token:
            case eCondType::negtoken:
                SetTokenCond(c);
                break;
            case eCondType::lowerInt:
            case eCondType::equalInt:
            case eCondType::notequalInt:
            case eCondType::greaterInt:
                SetIntegerCond(c);
                break;
            case eCondType::isset:
            case eCondType::empty:
                SetStringCond(c);
                break;
            case eCondType::equalString:
            case eCondType::notEqualString:
            case eCondType::contains:
            case eCondType::notContains:
                SetStringCompareCond(c);
                break;
            default:
                break;
        }
    }
}

void cCondition::SetTokenCond(cCond *c) {
    if (c->type == eCondType::negtoken) {
        ReplaceStart(c->expr, 3);
    }
    //check globals
    int result = 0;
    string tmp = c->expr;
    tmp = tmp.substr(1, tmp.size()-2);
    if (globals->GetInt(tmp, result)) {
        c->constant = true;
        if (result == 1 && c->type == eCondType::token)
            c->isTrue = true;
        if (result == 0 && c->type == eCondType::negtoken)
            c->isTrue = true;
        return;
    }
    SetTokenIndex(c, c->expr);
}

void cCondition::SetIntegerCond(cCond *c) {
    char *tokenStart = strchr(c->expr, '{');
    char *tokenEnd = strchr(c->expr, '}');
    if (!tokenStart || !tokenEnd)
        return;
    char token[200] = "";
    strncpy(token, tokenStart, tokenEnd - tokenStart + 1);

    char *condStart = strchr(c->expr, ',');
    char *condEnd = strchr(c->expr, ')');
    char strCond[100] = "";
    strncpy(strCond, condStart + 1, condEnd - condStart - 1);
    c->compareValue = atoi(strCond);

    //check globals
    int result = 0;
    if (globals->GetInt(token, result)) {
        c->constant = true;
        if (c->type == eCondType::lowerInt && result < c->compareValue)
            c->isTrue = true;
        else if (c->type == eCondType::equalInt && result == c->compareValue)
            c->isTrue = true;
        else if (c->type == eCondType::notequalInt && result != c->compareValue)
            c->isTrue = true;
        else if (c->type == eCondType::greaterInt && result > c->compareValue)
            c->isTrue = true;
        return;
    }
    SetTokenIndex(c, token);
}

void cCondition::SetStringCond(cCond *c) {
    ReplaceStart(c->expr, 5);
    SetTokenIndex(c, c->expr);
}

void cCondition::SetStringCompareCond(cCond *c) {
    char *tokenStart = strchr(c->expr, '{');
    char *tokenEnd = strchr(c->expr, '}');
    if (!tokenStart || !tokenEnd)
        return;
    char token[200] = "";
    strncpy(token, tokenStart, tokenEnd - tokenStart + 1);

    char *condStart = strstr(c->expr, ",'");
    char *condEnd = strstr(c->expr, "')");
    char strCond[100] = "";
    strncpy(strCond, condStart + 2, condEnd - condStart - 2);
    c->compareStrValue = strdup(strCond);
    SetTokenIndex(c, token);
}

void cCondition::SetTokenIndex(cCond *c, const char *token) {
    int tokenIndex = tokenContainer->IntTokenIndex(token);
    if (tokenIndex >= 0) {
        c->tokenIndex = tokenIndex;
        c->tokenType = eCondTokenType::inttoken;
        return;
    }
    tokenIndex = tokenContainer->StringTokenIndex(token);
    if (tokenIndex >= 0) {
        c->tokenIndex = tokenIndex;
        c->tokenType = eCondTokenType::stringtoken;
        return;
    }
    tokenIndex = tokenContainer->LoopTokenIndex(token);
    if (tokenIndex >= 0) {
        c->tokenIndex = tokenIndex;
        c->tokenType = eCondTokenType::looptoken;
    }
}
/******************************************************************
* cSummand
******************************************************************/
cSummand::cSummand(const char *summand) { 
    this->summand = strdup(summand); 
}

cSummand::cSummand(const cSummand &other) {
    summand = NULL;
    if (other.summand)
        summand = strdup(other.summand);
    positive = other.positive;
    for (const cFactor *fac = other.factors.First(); fac; fac = other.factors.Next(fac)) {
        factors.Add(new cFactor(*fac));
    }
}

cSummand::~cSummand(void) {
    free(summand); 
}

void cSummand::Debug(void) {
    esyslog("skindesigner:              summand %s, positive: %d", summand, positive);
    for (cFactor *f = factors.First(); f; f = factors.Next(f)) {
        const char *link = f->multiplication ? "multiplication" : "division";
        if (f->type == eFactorType::constant)
            esyslog("skindesigner:              constant factor %f, %s", f->constValue, link);
        else if (f->type == eFactorType::inttoken)
            esyslog("skindesigner:              IntToken factor, index %d, %s", f->tokenIndex, link);
        else if (f->type == eFactorType::stringtoken)
            esyslog("skindesigner:              StringToken factor, index %d, %s", f->tokenIndex, link);
        else if (f->type == eFactorType::looptoken)
            esyslog("skindesigner:              LoopToken factor, index %d, %s", f->tokenIndex, link);
        else if (f->type == eFactorType::xref)
            esyslog("skindesigner:              posx reference factor, %s, %p, %s, result: %d", f->funcRefName, f->funcRef, link, f->funcRef->FuncX());
        else if (f->type == eFactorType::yref)
            esyslog("skindesigner:              posy reference factor, %s, %p, %s, result: %d", f->funcRefName, f->funcRef, link, f->funcRef->FuncY());
        else if (f->type == eFactorType::widthref)
            esyslog("skindesigner:              width reference factor, %s, %p, %s, result: %d", f->funcRefName, f->funcRef, link, f->funcRef->FuncWidth());
        else if (f->type == eFactorType::heightref)
            esyslog("skindesigner:              height reference factor, %s, %p %s, result: %d", f->funcRefName, f->funcRef, link, f->funcRef->FuncHeight());
        else if (f->type == eFactorType::areawidth)
            esyslog("skindesigner:              {areawidth} factor, %s", link);
        else if (f->type == eFactorType::areaheight)
            esyslog("skindesigner:              {areaheight} factor, %s", link);
        else if (f->type == eFactorType::columnwidth)
            esyslog("skindesigner:              {columnwidth} factor, %s", link);
        else if (f->type == eFactorType::rowheight)
            esyslog("skindesigner:              {rowheight} factor, %s", link);
    }
}

/******************************************************************
* cNumericExpr
******************************************************************/
cNumericExpr::cNumericExpr(const char *expression) {
    expr = strdup(expression);
    globals = NULL;
    container = NULL;
    tokenContainer = NULL;
    loopInfo = NULL;
    horizontal = true;
    value = 0;
    dynamic = false;
}

cNumericExpr::cNumericExpr(const cNumericExpr &other) {
    expr = strdup(other.expr);
    globals = other.globals;
    container = NULL;
    tokenContainer = NULL;
    loopInfo = NULL;
    horizontal = other.horizontal;
    value = other.value;
    dynamic = other.dynamic;
    for (const cSummand *s = other.summands.First(); s; s = other.summands.Next(s)) {
        summands.Add(new cSummand(*s));
    }
}

cNumericExpr::~cNumericExpr(void) {
    free(expr);
}

/******************************************************************
* Public Functions
******************************************************************/
bool cNumericExpr::CacheStatic(void) {
    expr = RemoveSpace(expr);
    if (config.replaceDecPoint)
        ReplaceDecimalpoint(expr);

    //first check if expression is already an valid integer
    if (IsNumeric(expr)) {
        value = atoi(expr);
        return true;
    }
    
    //check if expression is a percent expression
    if (PercentValue(expr)) {
        return true;
    }
    //if areawidth or height unknown, percent values have to be replaced
    expr = ReplacePercentValue(expr);
    //replace {areawidth} and {areaheight}
    if (container->Width() >= 0) {
        expr = ReplaceTokens(expr, "{areawidth}", container->Width());
    }
    if (container->Height() >= 0)
        expr = ReplaceTokens(expr, "{areaheight}", container->Height());

    //replace globals
    string tmp = expr;
    globals->ReplaceIntVars(tmp);
    globals->ReplaceDoubleVars(tmp);
    free(expr);
    expr = strdup(tmp.c_str());
    if (IsNumeric(expr)) {
        value = atoi(expr);
        return true;
    }
    //check if all variables are eliminated
    if (IsNumericExpression(expr)) {
        value = EvaluateExpression(expr);
        return true;
    }
    //now we have a expression with runtime variables
    dynamic = true;
    return false;
}

void cNumericExpr::PrepareTokens(void) {
    CreateSummands();
    CreateFactors();
    ConsolidateFactors();
    ConsolidateSummand();
}

vector<cFactor*> cNumericExpr::GetRefFactors(void) {
    vector<cFactor*> refFactors;
    for (cSummand *s = summands.First(); s; s = summands.Next(s)) {
        for (cFactor* f = s->factors.First(); f; f = s->factors.Next(f)) {
            if (f->type >= eFactorType::xref) {
                refFactors.push_back(f);
            }
        }
    }
    return refFactors;
}

int cNumericExpr::Calculate(void) {
    double result = 0.0f;
    for (cSummand *s = summands.First(); s; s = summands.Next(s)) {
        double factor = 1.0f;
        for (cFactor *f = s->factors.First(); f; f = s->factors.Next(f)) {
            double fac = 0.0f;
            switch( f->type ) {
                case eFactorType::constant:
                    fac = f->constValue;
                    break;
                case eFactorType::stringtoken: {
                    char *val = tokenContainer->StringToken(f->tokenIndex);
                    if (val)
                        fac = atoi(val);
                    break; }
                case eFactorType::inttoken:
                    fac = tokenContainer->IntToken(f->tokenIndex);
                    break;
                case eFactorType::looptoken:
                    if (loopInfo && loopInfo->row >= 0) {
                        char *val = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, f->tokenIndex);
                        if (val)
                            fac = atoi(val);
                    }
                    break;
                case eFactorType::xref:
                    if (f->funcRef)
                        fac = f->funcRef->FuncX();
                    break;
                case eFactorType::yref:
                    if (f->funcRef)
                        fac = f->funcRef->FuncY();
                    break;
                case eFactorType::widthref:
                    if (f->funcRef)
                        fac = f->funcRef->FuncWidth();
                    break;
                case eFactorType::heightref:
                    if (f->funcRef)
                        fac = f->funcRef->FuncHeight();
                    break;
                case eFactorType::areawidth:
                    fac = container->Width();
                    break;
                case eFactorType::areaheight:
                    fac = container->Height();
                    break;
                case eFactorType::columnwidth:
                    if (loopInfo)
                        fac = loopInfo->colWidth;
                    break;
                case eFactorType::rowheight:
                    if (loopInfo)
                        fac = loopInfo->rowHeight;
                    break;
            }
            if (f->multiplication)
                factor *= fac;
            else if (fac)
                factor /= fac;            
        }
        if (s->positive)
            result += factor;
        else
            result -= factor;
    }
    return (int)result;
}


void cNumericExpr::Debug(void) {
    esyslog("skindesigner:            Numeric Expression \"%s\", Result: %d", expr, Calculate());
    for (cSummand *s = summands.First(); s; s = summands.Next(s)) {
        s->Debug();
    }
}

/******************************************************************
* Private Functions
******************************************************************/
bool cNumericExpr::IsNumeric(const char *e) {
    //negativ numbers
    if (*e == '-')
        ++e;
    //empty
    if (!*e)
        return false;
    while (*e) {
        if (isdigit(*e) || *e == '.' || *e == ',')
            ++e;
        else
            return false;
    }
    return true;
}

bool cNumericExpr::IsNumericExpression(const char *e) {
    if (!*e)
        return false;
    while (*e) {
        if (isdigit(*e)  || *e == '.' || *e == ',' || *e == '+' || *e == '-' || *e == '*' || *e == '/')
            ++e;
        else
            return false;
    }
    return true;    
}

bool cNumericExpr::PercentValue(const char *e) {
    const char *hit = strchr(e, '%');
    if (!hit)
        return false;
    char buffer[20] = "";
    if (strlen(e) > 20)
        return false;
    strncpy(buffer, e, strlen(e)-1);
    buffer[strlen(e)-1] = '\0';
    int val = atoi(buffer);
    bool ok = false;
    if (horizontal && container->Width() > 0) {
        value = container->Width() * val / 100;
        ok = true;
    } else if (!horizontal && container->Height() > 0){
        value = container->Height() * val / 100;
        ok = true;
    }
    return ok;
}

char *cNumericExpr::ReplacePercentValue(char *e) {
    const char *hit = strchr(e, '%');
    if (!hit)
        return e;
    char buffer[20] = "";
    if (strlen(e) > 20)
        return e;
    strncpy(buffer, e, strlen(e)-1);
    buffer[strlen(e)-1] = '\0';
    int val = atoi(buffer);
    double percentVal = (double)val/100.0f;

    char replacement[50] = "";
    if (horizontal) {
        sprintf(replacement, "%.5f*{areawidth}", percentVal);
    } else {
        sprintf(replacement, "%.5f*{areaheight}", percentVal);        
    }

    int len = strlen(replacement) + 1;
    char *replaced = (char*)malloc(len);
    memset(replaced, 0, len);
    strcpy(replaced, replacement);
    free(expr);
    return replaced;
}

char *cNumericExpr::ReplaceToken(char *e, const char* token, int val) {
    char *tokenStart = strstr(e, token);
    if (!tokenStart) {
        return e;
    }
    char buffer[20] = "";
    sprintf(buffer, "%d", val);
    size_t newSize = strlen(e) - strlen(token) + strlen(buffer) + 1;
    char *replaced = (char*)malloc(newSize);
    memset(replaced, 0, newSize);
    size_t beginning = strlen(e) - strlen(tokenStart);
    if (beginning > 0)
        strncpy(replaced, e, beginning);
    strcat(replaced, buffer);
    strcat(replaced, tokenStart + strlen(token));
    free(e);
    return replaced;
}

char *cNumericExpr::ReplaceTokens(char *e, const char* token, int val) {
    if (!e)
        return e;
    while (true) {
        char *tokenStart = strstr(e, token);
        if (!tokenStart)
            break;
        e = ReplaceToken(e, token, val);
    }
    return e;
}

int cNumericExpr::EvaluateExpression(char* e) {
    return round(ParseSummands(e));
}

double cNumericExpr::EvaluateExpressionDouble(char *e) {
    return ParseSummands(e);
}

double cNumericExpr::ParseAtom(char*& e) {
    // Read the number from string
    char* end_ptr;
    double res = strtod(e, &end_ptr);
    // Advance the pointer and return the result
    e = end_ptr;
    return res;
}

// Parse multiplication and division
double cNumericExpr::ParseFactors(char*& e) {
    double num1 = ParseAtom(e);
    for(;;) {
        // Save the operation
        char op = *e;
        if(op != '/' && op != '*')
            return num1;
        e++;
        double num2 = ParseAtom(e);
        // Perform the saved operation
        if(op == '/') {
            if (num2 != 0) {
                num1 /= num2;
            }
        } else
            num1 *= num2;
    }
}

// Parse addition and subtraction
double cNumericExpr::ParseSummands(char*& e) {
    double num1 = ParseFactors(e);
    for(;;) {
        char op = *e;
        if(op != '-' && op != '+')
            return num1;
        e++;
        double num2 = ParseFactors(e);
        if(op == '-')
            num1 -= num2;
        else
            num1 += num2;
    }
}

void cNumericExpr::CreateSummands(void) {
    char *sum = strdup(expr);
    char delimiter[] = "+-";
    bool positive = true;
    if (sum && *sum && *sum == '-') {
        positive = false;
    }
    char *summand = strtok(sum, delimiter);
    while (summand) {
        cSummand *s = new cSummand(summand);
        s->positive = positive;
        summands.Add(s);
        //check next sign
        if (expr[summand - sum + strlen(summand)] == '-')
            positive = false;
        else
            positive = true;
        summand = strtok(NULL, delimiter);
    }
    free(sum);
}

void cNumericExpr::CreateFactors(void) {
    char delimiterFac[] = "*/";
    for (cSummand *s = summands.First(); s; s = summands.Next(s)) {
        char *sum = strdup(s->summand);
        if (IsNumericExpression(sum)) {
            cFactor *f = new cFactor();
            f->constValue = EvaluateExpressionDouble(sum);
            s->factors.Add(f);
            free(sum);
            continue;            
        }
        bool multiplication = true;
        char *fac = strtok(sum, delimiterFac);
        while (fac) {
            cFactor *f = new cFactor();
            if (IsNumeric(fac)) {
                f->constValue = atof(fac);
                f->multiplication = multiplication;
                s->factors.Add(f);
            } else {
                //factor has to be a token or a function reference now
                if (SetTokenFactor(f, fac)) {
                    f->multiplication = multiplication;
                    s->factors.Add(f);
                } else if (SetReferenceFactor(f, fac)) {
                    f->multiplication = multiplication;
                    s->factors.Add(f);
                } else if (SetGeometryFactor(f, fac)) { 
                    f->multiplication = multiplication;
                    s->factors.Add(f);
                } else {
                    esyslog("skindesigner: invalid factor %s in expression \"%s\"", fac, expr);
                    delete f;
                }
            }
            //check next sign
            if (s->summand[fac - sum + strlen(fac)] == '/')
                multiplication = false;
            else
                multiplication = true;
            fac = strtok(NULL, delimiterFac);
        }
        free(sum);
    }
}

bool cNumericExpr::SetTokenFactor(cFactor *f, char *tokenName) {
    int tokenIndex = tokenContainer->IntTokenIndex(tokenName);
    if (tokenIndex >= 0) {
        f->tokenIndex = tokenIndex;
        f->type = eFactorType::inttoken;
        return true;
    }
    tokenIndex = tokenContainer->StringTokenIndex(tokenName);
    if (tokenIndex >= 0) {
        f->tokenIndex = tokenIndex;
        f->type = eFactorType::stringtoken;
        return true;
    }
    tokenIndex = tokenContainer->LoopTokenIndex(tokenName);
    if (tokenIndex >= 0) {
        f->tokenIndex = tokenIndex;
        f->type = eFactorType::looptoken;
        return true;
    }
    return false;
}


bool cNumericExpr::SetReferenceFactor(cFactor *f, char *tokenName) {
    int start = 0;
    if (startswith(tokenName, "{posx(") && endswith(tokenName, ")}")) {
        start = 6;
        f->type = eFactorType::xref;
    } else if (startswith(tokenName, "{posy(") && endswith(tokenName, ")}")) {
        start = 6;
        f->type = eFactorType::yref;
    } else if (startswith(tokenName, "{width(") && endswith(tokenName, ")}")) {
        start = 7;
        f->type = eFactorType::widthref;
    } else if (startswith(tokenName, "{height(") && endswith(tokenName, ")}")) {
        start = 8;
        f->type = eFactorType::heightref;    
    }

    if (start == 0)
        return false;

    tokenName += start;
    f->funcRefName = strdup(tokenName);
    ReplaceEnd(f->funcRefName, 2);

    return true;
}

bool cNumericExpr::SetGeometryFactor(cFactor *f, char *tokenName) {
    bool ok = false;
    if (!strcmp(tokenName, "{areawidth}")) {
        f->type = eFactorType::areawidth;
        ok = true;
    } else if (!strcmp(tokenName, "{areaheight}")) {
        f->type = eFactorType::areaheight;
        ok = true;
    } else if (!strcmp(tokenName, "{columnwidth}")) {
        f->type = eFactorType::columnwidth;
        ok = true;
    } else if (!strcmp(tokenName, "{rowheight}")) {
        f->type = eFactorType::rowheight;        
        ok = true;
    }
    return ok;
}

void cNumericExpr::ConsolidateSummand(void) {
    cSummand *constSummand = NULL;
    for (cSummand *s = summands.First(); s; s = summands.Next(s)) {
        if (s->factors.Count() == 1 && s->factors.First()->type == eFactorType::constant) {
            if (!constSummand) {
                constSummand = s;
            } else {
                if (s->positive)
                    constSummand->factors.First()->constValue += s->factors.First()->constValue;
                else
                    constSummand->factors.First()->constValue -= s->factors.First()->constValue;
                summands.Del(s);
                s = constSummand;
            }
        }
    }    
}

void cNumericExpr::ConsolidateFactors(void) {
    for (cSummand *s = summands.First(); s; s = summands.Next(s)) {
        cFactor *constFactor = NULL;
        for (cFactor *f = s->factors.First(); f; f = s->factors.Next(f)) {
            if (f->type == eFactorType::constant) {
                if (!constFactor) {
                    constFactor = f;
                } else {
                    if (f->multiplication)
                        constFactor->constValue *= f->constValue;
                    else 
                        constFactor->constValue /= f->constValue;
                    s->factors.Del(f);
                    f = constFactor;
                }
            }
        }
    }
}

/******************************************************************
* cColor
******************************************************************/
cColor::cColor(const char *expression) {
    globals = NULL;
    expr = strdup(expression);
    value = 0x00;
}

cColor::cColor(const cColor &other) {
    globals = other.globals;
    expr = strdup(other.expr);
    value = other.value;    
}

cColor::~cColor(void) {
    free(expr);
}

void cColor::Cache(void) {
    tColor colVal = 0x00;
    string tmp = expr;
    if (globals->GetColor(tmp, colVal)) {
        value = colVal;
        return;
    }
    if (strlen(expr) != 8)
        return;
    std::stringstream str;
    str << tmp;
    colVal = 0x00;
    str >> std::hex >> colVal;
    value = colVal;
}

tColor cColor::Color(void) {
    return value;
}

void cColor::Debug(void) {
    esyslog("skindesigner:            Color \"%s\", Value %x", expr, value);
}

/******************************************************************
* cTextExpr
******************************************************************/
cTextExpr::cTextExpr(const char *expression) {
    globals = NULL;
    expr = strdup(expression);
    tokenContainer = NULL;
    loopInfo = NULL;
}

cTextExpr::cTextExpr(const cTextExpr &other) {
    globals = other.globals;
    expr = strdup(other.expr);
    tokenContainer = NULL;
    loopInfo = NULL;
    for (const cTextToken* t = other.textTokens.First(); t; t = other.textTokens.Next(t)) {
        textTokens.Add(new cTextToken(*t));
    }
}

cTextExpr::~cTextExpr(void) {
    free(expr);
}

void cTextExpr::CorrectImagePath(void) {
    //no absolute pathes allowed
    if (!startswith(expr, "{")) {
        esyslog("skindesigner: no absolute pathes allowed for images - %s", expr);
    }
    if (startswith(expr, "{ressourcedir}")) {
        string tmp = expr;
        tmp = tmp.replace(0, 14, *config.GetSkinRessourcePath());
        free(expr);
        expr = strdup(tmp.c_str());
    }
}


void cTextExpr::Cache(void) {
    Translate();
    Tokenize();
    PrepareTokens();
}

char *cTextExpr::DeterminateText(void) {
    //calculate length of complete new string
    int textLength = 0;
    for (cTextToken* t = textTokens.First(); t; t = textTokens.Next(t)) {
        if (t->type == eTexttokenType::constant) {
            textLength += strlen(t->constValue);
        } else if (t->type == eTexttokenType::stringtoken) {
            char *str = tokenContainer->StringToken(t->tokenIndex);
            if (str)
                textLength += strlen(str);
        } else if (t->type == eTexttokenType::inttoken) {
            int value  = tokenContainer->IntToken(t->tokenIndex);
            if (value >= 0) {
                cString str = cString::sprintf("%d", value);
                textLength += strlen(*str);
            }
        } else if (t->type == eTexttokenType::looptoken && loopInfo && loopInfo->row >= 0) {
            char *str = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, t->tokenIndex);
            if (str)
                textLength += strlen(str);
        } else if (t->type == eTexttokenType::printftoken) {
            DeterminatePrintfToken(t);
            if (t->printfResult)
                textLength += strlen(t->printfResult);
        } else if (t->type == eTexttokenType::condstringtoken) {
            char *str = tokenContainer->StringToken(t->tokenIndex);
            if (str) {
                textLength += strlen(str);
                if (t->condStart)
                    textLength += strlen(t->condStart);
                if (t->condEnd)
                    textLength += strlen(t->condEnd);
            }
        }  else if (t->type == eTexttokenType::condinttoken) {
            int value  = tokenContainer->IntToken(t->tokenIndex);
            if (value >= 0) {
                cString str = cString::sprintf("%d", value);
                textLength += strlen(*str);
                if (t->condStart)
                    textLength += strlen(t->condStart);
                if (t->condEnd)
                    textLength += strlen(t->condEnd);
            }
        }
    }

    if (textLength <= 0)
        return NULL;

    char *retVal = (char*)malloc(textLength+1);
    memset(retVal, 0, textLength);
    bool first = true;
    for (cTextToken* t = textTokens.First(); t; t = textTokens.Next(t)) {
        if (first) {
            first = false;
            if (t->type == eTexttokenType::constant) {
                strcpy(retVal, t->constValue);
            } else if (t->type == eTexttokenType::stringtoken) {
                char *str = tokenContainer->StringToken(t->tokenIndex);
                if (str)
                    strcpy(retVal, str);
            } else if (t->type == eTexttokenType::inttoken) {
                int value  = tokenContainer->IntToken(t->tokenIndex);
                if (value >= 0) {
                    cString str = cString::sprintf("%d", value);
                    strcpy(retVal, *str);
                }
            } else if (t->type == eTexttokenType::looptoken && loopInfo && loopInfo->row >= 0) {
                char *str = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, t->tokenIndex);
                if (str)
                    strcpy(retVal, str);
            }  else if (t->type == eTexttokenType::printftoken) {
                if (t->printfResult) {
                    strcpy(retVal, t->printfResult);
                    free(t->printfResult);
                    t->printfResult = NULL;
                }
            } else if (t->type == eTexttokenType::condstringtoken) {
                char *str = tokenContainer->StringToken(t->tokenIndex);
                if (str) {
                    if (t->condStart) {
                        strcpy(retVal, t->condStart);
                        strcat(retVal, str);
                    } else {
                        strcpy(retVal, str);
                    }
                    if (t->condEnd)
                        strcat(retVal, t->condEnd);
                }
            }  else if (t->type == eTexttokenType::condinttoken) {
                int value  = tokenContainer->IntToken(t->tokenIndex);
                if (value >= 0) {
                    cString str = cString::sprintf("%d", value);
                    if (t->condStart) {
                        strcpy(retVal, t->condStart);
                        strcat(retVal, *str);
                    } else {
                        strcpy(retVal, *str);
                    }
                    if (t->condEnd)
                        strcat(retVal, t->condEnd);
                }
            }
        } else {
            if (t->type == eTexttokenType::constant) {
                strcat(retVal, t->constValue);
            } else if (t->type == eTexttokenType::stringtoken) {
                char *str = tokenContainer->StringToken(t->tokenIndex);
                if (str)
                    strcat(retVal, str);
            } else if (t->type == eTexttokenType::inttoken) {
                int value  = tokenContainer->IntToken(t->tokenIndex);
                if (value >= 0) {
                    cString str = cString::sprintf("%d", value);
                    strcat(retVal, *str);
                }
            } else if (t->type == eTexttokenType::looptoken && loopInfo && loopInfo->row >= 0) {
                char *str = tokenContainer->LoopToken(loopInfo->index, loopInfo->row, t->tokenIndex);
                if (str)
                    strcat(retVal, str);
            }  else if (t->type == eTexttokenType::printftoken) {
                if (t->printfResult) {
                    strcat(retVal, t->printfResult);
                    free(t->printfResult);
                    t->printfResult = NULL;
                }
            } else if (t->type == eTexttokenType::condstringtoken) {
                char *str = tokenContainer->StringToken(t->tokenIndex);
                if (str) {
                    if (t->condStart)
                        strcat(retVal, t->condStart);
                    strcat(retVal, str);
                    if (t->condEnd)
                        strcat(retVal, t->condEnd);
                }
            } else if (t->type == eTexttokenType::condinttoken) {
                int value  = tokenContainer->IntToken(t->tokenIndex);
                if (value >= 0) {
                    cString str = cString::sprintf("%d", value);
                    if (t->condStart)
                        strcat(retVal, t->condStart);
                    strcat(retVal, *str);
                    if (t->condEnd)
                        strcat(retVal, t->condEnd);
                }
            }
        }
    }
    return retVal;
}


void cTextExpr::Debug(const char *exprName) {
    esyslog("skindesigner:            TextExpr %s: \"%s\"", exprName, expr);
    char *res = DeterminateText();
    esyslog("skindesigner:            Result: \"%s\"", res);
    free(res);
    for (cTextToken *t = textTokens.First(); t; t = textTokens.Next(t)) {
        if (t->type == eTexttokenType::constant)
            esyslog("skindesigner:            constant token \"%s\"", t->constValue);
        else if (t->type == eTexttokenType::stringtoken)
            esyslog("skindesigner:            string token %s, index %d", t->constValue, t->tokenIndex);
        else if (t->type == eTexttokenType::inttoken)
            esyslog("skindesigner:            int token %s, index %d", t->constValue, t->tokenIndex);
        else if (t->type == eTexttokenType::looptoken)
            esyslog("skindesigner:            loop token %s, index %d", t->constValue, t->tokenIndex);
        else if (t->type == eTexttokenType::printftoken)
            esyslog("skindesigner:            printf token %s", t->constValue);
        else if (t->type == eTexttokenType::condstringtoken || t->type == eTexttokenType::condinttoken)
            esyslog("skindesigner:            conditional token %s", t->constValue);
    }
}

void cTextExpr::Translate(void) {
    string translated = "";
    if (!globals->Translate(expr, translated))
        return;
    free(expr);
    expr = strdup(translated.c_str());
}

void cTextExpr::Tokenize(void) {
    char *head = expr;
    char *tail = expr;
    while (*tail) {
        if (*tail == '|') {
            char *begin = CopyTextPart(head, tail, false);
            if (begin) {
                cTextToken *t = new cTextToken();
                t->constValue = begin;
                textTokens.Add(t);
            }
            head = tail;
            tail++;
            while (*tail) {
                if (*tail == '|') {
                    char *token = CopyTextPart(head, tail);
                    cTextToken *t = new cTextToken();
                    t->type = eTexttokenType::condstringtoken;
                    t->constValue = token;
                    textTokens.Add(t);
                    head = tail+1;
                    break;
                }
                tail++;
            }
        } else if (*tail == '{') {
            char *begin = CopyTextPart(head, tail, false);
            if (begin) {
                cTextToken *t = new cTextToken();
                t->constValue = begin;
                textTokens.Add(t);
            }
            head = tail;
            tail++;
            while (*tail) {
                if (*tail == '}') {
                    char *token = CopyTextPart(head, tail);
                    cTextToken *t = new cTextToken();
                    t->type = eTexttokenType::stringtoken;
                    t->constValue = token;
                    textTokens.Add(t);
                    head = tail+1;
                    break;
                }
                tail++;
            }
        }
        tail++;
    }
    char *end = CopyTextPart(head, tail, false);
    if (end) {
        cTextToken *t = new cTextToken();
        t->constValue = end;
        textTokens.Add(t);
    }
}

void cTextExpr::PrepareTokens(void) {
    bool error = false;
    for (cTextToken *t = textTokens.First(); t; t = textTokens.Next(t)) {
        if (t->type == eTexttokenType::condstringtoken) {
            ParseCondToken(t);
            continue;
        } else if (t->type != eTexttokenType::stringtoken)
            continue;
        if (ParsePrintfToken(t)) {
            continue;
        }
        if (CheckGlobals(t)) {
            continue;
        }
        int tokenIndex = tokenContainer->IntTokenIndex(t->constValue);
        if (tokenIndex >= 0) {
            t->tokenIndex = tokenIndex;
            t->type = eTexttokenType::inttoken;
            continue;
        }
        tokenIndex = tokenContainer->StringTokenIndex(t->constValue);
        if (tokenIndex >= 0) {
            t->tokenIndex = tokenIndex;
            t->type = eTexttokenType::stringtoken;
            continue;
        }
        tokenIndex = tokenContainer->LoopTokenIndex(t->constValue);
        if (tokenIndex >= 0) {
            t->tokenIndex = tokenIndex;
            t->type = eTexttokenType::looptoken;
            continue;
        }
        esyslog("skindesigner: invalid text token %s in expression \"%s\"", t->constValue, expr);
        error = true;
        break;
    }
    if (error) {
        textTokens.Clear();
    }
}

bool cTextExpr::CheckGlobals(cTextToken *t) {
    if (!globals || !t->constValue)
        return false;

    string name = t->constValue;
    name = name.substr(1, name.size()-2);
    string replacement = "";
    if (globals->GetString(name, replacement)) {
        free(t->constValue);
        t->constValue = strdup(replacement.c_str());
        t->type = eTexttokenType::constant;
        return true;
    }
    int numReplacement = 0;
    if (globals->GetInt(name, numReplacement)) {
        free(t->constValue);
        cString repl = cString::sprintf("%d", numReplacement);
        t->constValue = strdup(*repl);
        t->type = eTexttokenType::constant;
        return true;
    }
    return false;
}

bool cTextExpr::ParsePrintfToken(cTextToken *t) {
    //check valid printftoken
    char *start = strstr(t->constValue, "printf(");
    if (!start)
        return false;
    char *end =  strchr(start, ')');
    if (!end)
        return false;
    t->type = eTexttokenType::printftoken;

    //find printf expression
    char *startExpr = strchr(t->constValue, '\'');
    if (!startExpr)
        return false;
    char *endExpr = strchr(startExpr + 1, '\'');
    if (!endExpr)
        return false;
    int expLen = endExpr - startExpr - 1;
    char buffer[100];
    strncpy(buffer, startExpr+1, expLen);
    buffer[expLen] = '\0';
    t->printfExpr = strdup(buffer);
    
    //find variables
    char *startVar = strchr(t->constValue, ',');
    if (!startVar)
        return false;
    startVar++;
    vector<string> varTokens;
    char *nextVar = NULL;
    while (nextVar = strchr(startVar, ',')) {
        while(isspace(*startVar))
            startVar++;
        int varLen = nextVar - startVar;
        buffer[0] = '{';
        strncpy((char*)buffer + 1, startVar, varLen);
        buffer[varLen+1] = '}';
        buffer[varLen+2] = '\0';
        int i = 1;
        while(isspace(buffer[varLen-i])) {
            buffer[varLen-i] = '}';
            buffer[varLen-i+1] = '\0';
            i++;
        }
        varTokens.push_back(buffer);
        startVar = nextVar + 1;
    }
    while (isspace(*startVar))
        startVar++;
    if (startVar+1) {
        int varLen = end - startVar + 1;
        buffer[0] = '{';
        strncpy((char*)buffer + 1, startVar, varLen);
        buffer[varLen] = '}';
        buffer[varLen+1] = '\0';
        int i = 1;
        while(isspace(buffer[varLen-i])) {
            buffer[varLen-i] = '}';
            buffer[varLen-i+1] = '\0';
            i++;
        }
        varTokens.push_back(buffer);
    }
    //evaluate variables
    bool ok = true;
    for (size_t i=0; i < varTokens.size(); i++) {
        sPrintfInfo info;
        int tokenIndex = tokenContainer->IntTokenIndex(varTokens[i].c_str());
        if (tokenIndex >= 0) {
            info.type = ePrintfVarType::inttoken;
            info.index = tokenIndex;
            t->printfVarIndices.push_back(info);
            continue;
        }
        tokenIndex = tokenContainer->StringTokenIndex(varTokens[i].c_str());
        if (tokenIndex >= 0) {
            info.type = ePrintfVarType::stringtoken;
            info.index = tokenIndex;
            t->printfVarIndices.push_back(info);
            continue;
        }
        tokenIndex = tokenContainer->LoopTokenIndex(varTokens[i].c_str());
        if (tokenIndex >= 0) {
            info.type = ePrintfVarType::looptoken;
            info.index = tokenIndex;
            t->printfVarIndices.push_back(info);
            continue;
        }
        ok = false;
    }
    return ok;
}

void cTextExpr::DeterminatePrintfToken(cTextToken *t) {
    int numVars = t->printfVarIndices.size();
    vector<int> results;
    for (int i=0; i < numVars; i++) {
        if (t->printfVarIndices[i].type == ePrintfVarType::inttoken) {
            results.push_back(tokenContainer->IntToken(t->printfVarIndices[i].index));
        } else if (t->printfVarIndices[i].type == ePrintfVarType::stringtoken) {
            if (tokenContainer->StringToken(t->printfVarIndices[i].index)) {
                results.push_back(atoi(tokenContainer->StringToken(t->printfVarIndices[i].index)));
            }
        } else if (t->printfVarIndices[i].type == ePrintfVarType::looptoken  && loopInfo && loopInfo->row >= 0) {
            if (tokenContainer->LoopToken(loopInfo->index, loopInfo->row, t->printfVarIndices[i].index)) {
                results.push_back(atoi(tokenContainer->LoopToken(loopInfo->index, loopInfo->row, t->printfVarIndices[i].index)));                
            }
        }
    }

    switch (numVars) {
        case 1:
            t->printfResult = strdup(*cString::sprintf(t->printfExpr, results[0]));
            break;
        case 2:
            t->printfResult = strdup(*cString::sprintf(t->printfExpr, results[0], results[1]));
            break;
        case 3:
            t->printfResult = strdup(*cString::sprintf(t->printfExpr, results[0], results[1], results[2]));
            break;
        case 4:
            t->printfResult = strdup(*cString::sprintf(t->printfExpr, results[0], results[1], results[2], results[3]));
            break;
        case 5:
            t->printfResult = strdup(*cString::sprintf(t->printfExpr, results[0], results[1], results[2], results[3], results[4]));
            break;
        case 6:
            t->printfResult = strdup(*cString::sprintf(t->printfExpr, results[0], results[1], results[2], results[3], results[4], results[5]));
            break;
        default:
            break;
    }
}

void cTextExpr::ParseCondToken(cTextToken *t) {
    char *head = t->constValue;
    char *tail = t->constValue;
    head++;
    tail++;
    char *token = NULL;
    while (*tail) {
        if (*tail == '{') {
            t->condStart = CopyTextPart(head, tail, false);
            head = tail;
            tail++;
            while (*tail) {
                if (*tail == '}') {
                    token = CopyTextPart(head, tail);
                    head = tail+1;
                    break;
                }
                tail++;
            }
        }
        tail++;
    }
    tail--;
    t->condEnd = CopyTextPart(head, tail, false);
    if (!token) {
        esyslog("skindesigner: invalid conditional texttoken %s in expression \"%s\"", t->constValue, expr);
        return;
    }

    int tokenIndex = tokenContainer->StringTokenIndex(token);
    if (tokenIndex >= 0) {
        t->tokenIndex = tokenIndex;
        return;
    }

    tokenIndex = tokenContainer->IntTokenIndex(token);
    if (tokenIndex >= 0) {
        t->tokenIndex = tokenIndex;
        t->type = eTexttokenType::condinttoken;
        return;
    }
    esyslog("skindesigner: invalid token %s in expression\"%s\"", token, expr);
}

char *cTextExpr::CopyTextPart(char *start, char *stop, bool incLastChar) {
    if (!start)
        return NULL;
    if (start >= stop)
        return NULL;
    char *val = NULL;
    size_t len = 0;
    if (stop) {
        //defined end
        len = stop - start + 1;
    } else {
        //search end of text
        char *p = start;
        while (*p)
            len++; p++;
        len++;
    }
    val = (char*)malloc(len+1);
    memset(val, 0, len+1);
    if (!incLastChar)
        len--;
    memcpy(val, start, len);
    return val;
}
