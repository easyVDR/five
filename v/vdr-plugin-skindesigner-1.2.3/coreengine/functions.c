#include "attribute.h"
#include "../config.h"

/***************************************************************************
* cFuncFill
***************************************************************************/
cFuncFill::cFuncFill(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "Fill";
    SetAttributesDefs();
}

cFuncFill::cFuncFill(const cFuncFill &other) : cFunction(other) {
    funcType = other.funcType;
}

cFuncFill::~cFuncFill(void) {
}

void cFuncFill::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func Fill attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eFillAttribs::color)) {
            SetColor(attVal);
        }
    }
}

void cFuncFill::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("color", (int)eFillAttribs::color));
    attribNames.insert(pair<int, string>((int)eFillAttribs::color, "color"));
}

void cFuncFill::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    if (!color)
        return;
    p->Fill(color->Color());
}

/***************************************************************************
* cFuncDrawRectangle
***************************************************************************/
cFuncDrawRectangle::cFuncDrawRectangle(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawRectangle";
    SetAttributesDefs();
}

cFuncDrawRectangle::cFuncDrawRectangle(const cFuncDrawRectangle &other) : cFunction(other) {
    funcType = other.funcType;
}

cFuncDrawRectangle::~cFuncDrawRectangle(void) {
}

void cFuncDrawRectangle::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawRectangle attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawRectangleAttribs::color)) {
            SetColor(attVal);
        } else if (IdEqual(id, (int)eDrawRectangleAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawRectangleAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawRectangleAttribs::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawRectangleAttribs::animtype)) {
            SetAnimType(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawRectangle::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawRectangleAttribs::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawRectangleAttribs::valign));
    attribIDs.insert(pair<string, int>("color", (int)eDrawRectangleAttribs::color));
    attribIDs.insert(pair<string, int>("name", (int)eDrawRectangleAttribs::name));
    attribIDs.insert(pair<string, int>("animtype", (int)eDrawRectangleAttribs::animtype));
    attribIDs.insert(pair<string, int>("animfreq", (int)eDrawRectangleAttribs::animfreq));
    attribNames.insert(pair<int, string>((int)eDrawRectangleAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawRectangleAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawRectangleAttribs::color, "color"));
    attribNames.insert(pair<int, string>((int)eDrawRectangleAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawRectangleAttribs::animtype, "animtype"));
    attribNames.insert(pair<int, string>((int)eDrawRectangleAttribs::animfreq, "animfreq"));
}

void cFuncDrawRectangle::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    eAlign align = (eAlign)GetValue((int)eDrawRectangleAttribs::align);
    eAlign valign = (eAlign)GetValue((int)eDrawRectangleAttribs::valign);
    int x = GetX(align, x0, colWidth);
    int y = GetY(valign, y0, rowHeight);
    cRect rect(x, y, Width(), Height());
    p->DrawRectangle(rect, color->Color());
}

/***************************************************************************
* cFuncDrawEllipse
***************************************************************************/
cFuncDrawEllipse::cFuncDrawEllipse(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawEllipse";
    SetAttributesDefs();
}

cFuncDrawEllipse::cFuncDrawEllipse(const cFuncDrawEllipse &other) : cFunction(other) {
    funcType = other.funcType;
}

cFuncDrawEllipse::~cFuncDrawEllipse(void) {
}

void cFuncDrawEllipse::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawEllipse attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawEllipseAttribs::color)) {
            SetColor(attVal);
        } else if (IdEqual(id, (int)eDrawEllipseAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawEllipseAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawEllipseAttribs::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawEllipseAttribs::animtype)) {
            SetAnimType(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawEllipse::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawEllipseAttribs::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawEllipseAttribs::valign));
    attribIDs.insert(pair<string, int>("color", (int)eDrawEllipseAttribs::color));
    attribIDs.insert(pair<string, int>("name", (int)eDrawEllipseAttribs::name));
    attribIDs.insert(pair<string, int>("quadrant", (int)eDrawEllipseAttribs::quadrant));
    attribIDs.insert(pair<string, int>("animtype", (int)eDrawEllipseAttribs::animtype));
    attribIDs.insert(pair<string, int>("animfreq", (int)eDrawEllipseAttribs::animfreq));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::color, "color"));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::quadrant, "quadrant"));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::animtype, "animtype"));
    attribNames.insert(pair<int, string>((int)eDrawEllipseAttribs::animfreq, "animfreq"));
}

void cFuncDrawEllipse::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    eAlign align = (eAlign)GetValue((int)eDrawEllipseAttribs::align);
    eAlign valign = (eAlign)GetValue((int)eDrawEllipseAttribs::valign);
    int x = GetX(align, x0, colWidth);
    int y = GetY(valign, y0, rowHeight);
    cRect rect(x, y, Width(), Height());
    int quadrant = GetValue((int)eDrawEllipseAttribs::quadrant);
    p->DrawEllipse(rect, color->Color(), quadrant);
}

/***************************************************************************
* cFuncDrawSlope
***************************************************************************/
cFuncDrawSlope::cFuncDrawSlope(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawSlope";
    SetAttributesDefs();
}

cFuncDrawSlope::cFuncDrawSlope(const cFuncDrawSlope &other) : cFunction(other) {
    funcType = other.funcType;
}

cFuncDrawSlope::~cFuncDrawSlope(void) {
}

void cFuncDrawSlope::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawSlope attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawSlopeAttribs::color)) {
            SetColor(attVal);
        } else if (IdEqual(id, (int)eDrawSlopeAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawSlopeAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawSlopeAttribs::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawSlopeAttribs::animtype)) {
            SetAnimType(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawSlope::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawSlopeAttribs::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawSlopeAttribs::valign));
    attribIDs.insert(pair<string, int>("color", (int)eDrawSlopeAttribs::color));
    attribIDs.insert(pair<string, int>("name", (int)eDrawSlopeAttribs::name));
    attribIDs.insert(pair<string, int>("type", (int)eDrawSlopeAttribs::type));
    attribIDs.insert(pair<string, int>("animtype", (int)eDrawSlopeAttribs::animtype));
    attribIDs.insert(pair<string, int>("animfreq", (int)eDrawSlopeAttribs::animfreq));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::color, "color"));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::type, "type"));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::animtype, "animtype"));
    attribNames.insert(pair<int, string>((int)eDrawSlopeAttribs::animfreq, "animfreq"));
}

void cFuncDrawSlope::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    eAlign align = (eAlign)GetValue((int)eDrawSlopeAttribs::align);
    eAlign valign = (eAlign)GetValue((int)eDrawSlopeAttribs::valign);
    int x = GetX(align, x0, colWidth);
    int y = GetY(valign, y0, rowHeight);
    cRect rect(x, y, Width(), Height());
    int type = GetValue((int)eDrawSlopeAttribs::type);
    p->DrawSlope(rect, color->Color(), type);
}

/***************************************************************************
* cTextDrawer
***************************************************************************/
cMutex cTextDrawer::fontLock;

cTextDrawer::cTextDrawer(void) {
    font = NULL;
    fontName = NULL;
    fontSize = 0;
}

cTextDrawer::~cTextDrawer(void) {
    free(fontName);
}

void cTextDrawer::CacheFont(cGlobals *globals, int size) {
    //check if font name is a global token
    if (startswith(fontName, "{") && endswith(fontName, "}")) {
        string tmpFontName = "";
        if (globals->GetFont(fontName, tmpFontName)) {
            free(fontName);
            fontName = strdup(tmpFontName.c_str());
        } else {
            esyslog("skindesigner: unknown font %s", fontName);
            return;
        }
    }
    if (size > 0)
        LoadFont(size);
}

void cTextDrawer::LoadFont(int size) {
    if (!fontName)
        return; 
    if (size <= 0)
        return;
    font = fontManager->Font(fontName, size);
    if (font)
        fontSize = size;
}
    
int cTextDrawer::TextWidth(const char *text) {
    int textWidth = 0;
    fontLock.Lock();
    textWidth = font->Width(text);
    fontLock.Unlock();
    return textWidth;
}

int cTextDrawer::FontHeight(void) {
    int fontHeight = 0;
    fontLock.Lock();
    fontHeight = font->Height();
    fontLock.Unlock();
    return fontHeight;
}
/***************************************************************************
* cFuncDrawText
***************************************************************************/
cFuncDrawText::cFuncDrawText(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawText";
    text = NULL;
    SetAttributesDefs();
}

cFuncDrawText::cFuncDrawText(const cFuncDrawText &other) : cFunction(other) {
    funcType = other.funcType;
    fontName = NULL;
    if (other.fontName)
        fontName = strdup(other.fontName);
    font = other.font;
    text = NULL;
    if (other.text)
        text = new cTextExpr(*other.text);
}

cFuncDrawText::~cFuncDrawText(void) {
    delete text;
}

void cFuncDrawText::SetLoopInfo(cLoopInfo *loopInfo) {
    cFunction::SetLoopInfo(loopInfo);
    if (text)
        text->SetLoopInfo(loopInfo);
}

void cFuncDrawText::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    cAttributes::SetTokenContainerDeep(tokenContainer);
    if (text) {
        text->SetTokenContainer(tokenContainer);
    }
}

void cFuncDrawText::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawText attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawTextAttribs::color)) {
            SetColor(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::animtype)) {
            SetAnimType(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::font)) {
            fontName = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::text)) {
            text = new cTextExpr(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribs::fontsize)) {
            attribCtors[id] = new cNumericExpr(attVal);
            attribCtors[id]->SetVertical();
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawText::Cache(void) {
    cFunction::Cache();
    if (text) {
        text->SetTokenContainer(tokenContainer);
        text->SetGlobals(globals);
        text->Cache();
    }
    if (fontName) {
        CacheFont(globals, GetValue((int)eDrawTextAttribs::fontsize));
    }
}

void cFuncDrawText::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    if (!font) {
        LoadFont(GetValue((int)eDrawTextAttribs::fontsize));
        if (!font)
            return;
    }
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    if (!funcText)
        return;
    int maxTextWidth = Width();
    if (maxTextWidth > 0 && !scrolling) {
        if (TextWidth(funcText) > maxTextWidth) {
            funcText = Cut(funcText, maxTextWidth);
        }
    } else if (!scrolling) {
        maxTextWidth = container.Width() - X();
        if (TextWidth(funcText) > maxTextWidth) {
            funcText = Cut(funcText, maxTextWidth);
        }
    }

    eAlign horAlign = (eAlign)GetValue((int)eDrawTextAttribs::align);
    eAlign verAlign = (eAlign)GetValue((int)eDrawTextAttribs::valign);

    int contWidth = colWidth > 0 ? colWidth : container.Width();
    int x = X() + x0;
    if (horAlign == eAlign::right) {
        x = x0 + contWidth - TextWidth(funcText);
    } else if (horAlign == eAlign::center) {
        x = x0 + (contWidth - TextWidth(funcText)) / 2;
    }
    if (x < 0) x = 0;

    int contHeight = rowHeight > 0 ? rowHeight : container.Height();
    int y = Y() + y0;
    if (verAlign == eAlign::bottom) {
        y = y0 + contHeight - FontHeight();
    } else if (verAlign == eAlign::center) {
        y = y0 + (contHeight - FontHeight()) / 2;
    }

    p->DrawText(cPoint(x, y), funcText, color->Color(), clrTransparent, font);
    free(funcText);
}

int cFuncDrawText::FuncX(void) {
    if (!font)
        return 0;
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    if (!funcText)
        return 0;
    eAlign horAlign = (eAlign)GetValue((int)eDrawTextAttribs::align);
    int x = X();
    if (horAlign == eAlign::right) {
        x = container.Width() - TextWidth(funcText);
    } else if (horAlign == eAlign::center) {
        x = (container.Width() - TextWidth(funcText)) / 2;
    }
    return (x >= 0) ? x : 0 ;
}

int cFuncDrawText::FuncY(void) {
    if (!font)
        return 0;
    eAlign verAlign = (eAlign)GetValue((int)eDrawTextAttribs::valign);
    int y = Y();
    if (verAlign == eAlign::bottom) {
        y = container.Height() - FontHeight();
    } else if (verAlign == eAlign::center) {
        y = (container.Height() - FontHeight()) / 2;
    }
    return y;
}

int cFuncDrawText::FuncWidth(void) {
    if (!font)
        return 0;
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    if (!funcText)
        return 0;
    int textWidth = TextWidth(funcText);
    free(funcText);
    return textWidth;
}

int cFuncDrawText::FuncHeight(void) {
    if (!font)
        return 0;
    return FontHeight();
}

int cFuncDrawText::AvrgFontWidth(void) {
    if (!font)
        return 20;
    return TextWidth("x")+3;
}

const cFont *cFuncDrawText::GetFont(void) {
    return font;
}

void cFuncDrawText::Debug(void) {
    cFunction::Debug();
    if (fontName)
        esyslog("skindesigner:            fontname: \"%s\"", fontName);
    if (font)
        esyslog("skindesigner:            cached font name: \"%s\", size %d", font->FontName(), font->Height());
    if (text)
        text->Debug("draw text");
}

void cFuncDrawText::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawTextAttribs::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawTextAttribs::valign));
    attribIDs.insert(pair<string, int>("color", (int)eDrawTextAttribs::color));
    attribIDs.insert(pair<string, int>("font", (int)eDrawTextAttribs::font));
    attribIDs.insert(pair<string, int>("fontsize", (int)eDrawTextAttribs::fontsize));
    attribIDs.insert(pair<string, int>("name", (int)eDrawTextAttribs::name));
    attribIDs.insert(pair<string, int>("text", (int)eDrawTextAttribs::text));
    attribIDs.insert(pair<string, int>("animtype", (int)eDrawTextAttribs::animtype));
    attribIDs.insert(pair<string, int>("animfreq", (int)eDrawTextAttribs::animfreq));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::color, "color"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::font, "font"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::fontsize, "fontsize"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::text, "text"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::animtype, "animtype"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribs::animfreq, "animfreq"));
}

char *cFuncDrawText::Cut(char *expr, int width) {
    char *cutted = NULL;
    int w = 3 * font->Width(".");
    for (char *p = expr; *p; ) {
        int sl = Utf8CharLen(p);
        uint sym = Utf8CharGet(p, sl);
        w += font->Width(sym);
        if( w >= width ) {
            cutted = (char*)malloc(p - expr + 4);
            memset(cutted, 0, p - expr + 4);
            strncpy(cutted, expr, p - expr);
            cutted[p - expr] = '.';
            cutted[p - expr + 1] = '.';
            cutted[p - expr + 2] = '.';
            break;
        }
        p += sl;
    }
    if (cutted) {
        free(expr);
        return cutted;
    }
    return expr;
}

/***************************************************************************
* cFuncDrawTextVertical
***************************************************************************/
cFuncDrawTextVertical::cFuncDrawTextVertical(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawTextVertical";
    text = NULL;
    SetAttributesDefs();
}

cFuncDrawTextVertical::cFuncDrawTextVertical(const cFuncDrawTextVertical &other) : cFunction(other) {
    funcType = other.funcType;
    fontName = NULL;
    if (other.fontName)
        fontName = strdup(other.fontName);
    text = NULL;
    if (other.text)
        text = new cTextExpr(*other.text);
}

cFuncDrawTextVertical::~cFuncDrawTextVertical(void) {
    delete text;
}

void cFuncDrawTextVertical::SetLoopInfo(cLoopInfo *loopInfo) {
    cFunction::SetLoopInfo(loopInfo);
    if (text)
        text->SetLoopInfo(loopInfo);
}

void cFuncDrawTextVertical::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    cAttributes::SetTokenContainerDeep(tokenContainer);
    if (text) {
        text->SetTokenContainer(tokenContainer);
    }
}

void cFuncDrawTextVertical::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawTextVertical attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawTextAttribsVertical::color)) {
            SetColor(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::direction)) {
            SetDirection(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::animtype)) {
            SetAnimType(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::font)) {
            fontName = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawTextAttribsVertical::text)) {
            text = new cTextExpr(attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawTextVertical::Cache(void) {
    cFunction::Cache();
    if (text) {
        text->SetTokenContainer(tokenContainer);
        text->SetGlobals(globals);
        text->Cache();
    }
    if (fontName) {
        CacheFont(globals, 0);
    }
}

void cFuncDrawTextVertical::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    if (!funcText)
        return;
    int fontSize = GetValue((int)eDrawTextAttribsVertical::fontsize);
    int direction = GetValue((int)eDrawTextAttribsVertical::direction);
    tColor clr = color->Color();
    cImage *textVertical = imgCache->GetVerticalText(funcText, clr, fontName, fontSize, direction);
    if (!textVertical)
        return;

    eAlign horAlign = (eAlign)GetValue((int)eDrawTextAttribsVertical::align);
    eAlign verAlign = (eAlign)GetValue((int)eDrawTextAttribsVertical::valign);
    int x = 0;
    int y = 0;
    if (horAlign == eAlign::center) {
        x = (container.Width() - textVertical->Width()) / 2;
    } else if (horAlign == eAlign::right) {
        x = container.Width() - textVertical->Width();
    }
    if (verAlign == eAlign::center) {
        y = (container.Height() - textVertical->Height()) / 2;
    } else if (horAlign == eAlign::bottom) {
        y = container.Height() - textVertical->Height();
    }
    cPoint pos(x, y);
    p->DrawImage(pos, *textVertical);
    free(funcText);
}

int cFuncDrawTextVertical::FuncWidth(void) {
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    if (!funcText)
        return 0;
    int fontSize = GetValue((int)eDrawTextAttribsVertical::fontsize);
    int direction = GetValue((int)eDrawTextAttribsVertical::direction);
    tColor clr = color->Color();
    cImage *textVertical = imgCache->GetVerticalText(funcText, clr, fontName, fontSize, direction);
    if (!textVertical)
        return 0;
    return textVertical->Width();
}

int cFuncDrawTextVertical::FuncHeight(void) {
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    if (!funcText)
        return 0;
    int fontSize = GetValue((int)eDrawTextAttribsVertical::fontsize);
    int direction = GetValue((int)eDrawTextAttribsVertical::direction);
    tColor clr = color->Color();
    cImage *textVertical = imgCache->GetVerticalText(funcText, clr, fontName, fontSize, direction);
    if (!textVertical)
        return 0;
    return textVertical->Height();
}

void cFuncDrawTextVertical::Debug(void) {
    cFunction::Debug();
    if (fontName)
        esyslog("skindesigner:            fontname: \"%s\"", fontName);
    if (text)
        text->Debug("draw text");
}

void cFuncDrawTextVertical::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawTextAttribsVertical::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawTextAttribsVertical::valign));
    attribIDs.insert(pair<string, int>("direction", (int)eDrawTextAttribsVertical::direction));
    attribIDs.insert(pair<string, int>("color", (int)eDrawTextAttribsVertical::color));
    attribIDs.insert(pair<string, int>("font", (int)eDrawTextAttribsVertical::font));
    attribIDs.insert(pair<string, int>("fontsize", (int)eDrawTextAttribsVertical::fontsize));
    attribIDs.insert(pair<string, int>("name", (int)eDrawTextAttribsVertical::name));
    attribIDs.insert(pair<string, int>("text", (int)eDrawTextAttribsVertical::text));
    attribIDs.insert(pair<string, int>("animtype", (int)eDrawTextAttribsVertical::animtype));
    attribIDs.insert(pair<string, int>("animfreq", (int)eDrawTextAttribsVertical::animfreq));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::direction, "direction"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::color, "color"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::font, "font"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::fontsize, "fontsize"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::text, "text"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::animtype, "animtype"));
    attribNames.insert(pair<int, string>((int)eDrawTextAttribsVertical::animfreq, "animfreq"));
}

/***************************************************************************
* cFuncDrawTextBox
***************************************************************************/
cFuncDrawTextBox::cFuncDrawTextBox(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawTextBox";
    text = NULL;
    floater = NULL;
    SetAttributesDefs();
}

cFuncDrawTextBox::cFuncDrawTextBox(const cFuncDrawTextBox &other) : cFunction(other) {
    funcType = other.funcType;
    fontName = NULL;
    if (other.fontName)
        fontName = strdup(other.fontName);
    font = other.font;
    text = NULL;
    if (other.text)
        text = new cTextExpr(*other.text);
    floater = NULL;
}

cFuncDrawTextBox::~cFuncDrawTextBox(void) {
    delete text;
    delete floater;
}

void cFuncDrawTextBox::SetLoopInfo(cLoopInfo *loopInfo) {
    cFunction::SetLoopInfo(loopInfo);
    if (text)
        text->SetLoopInfo(loopInfo);
}

void cFuncDrawTextBox::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    cAttributes::SetTokenContainerDeep(tokenContainer);
    if (text) {
        text->SetTokenContainer(tokenContainer);
    }
}

void cFuncDrawTextBox::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawTextBox attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawTextBoxAttribs::color)) {
            SetColor(attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::floatmode)) {
            SetFloatMode(id, attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::floatheight)) {
            attribCtors[id] = new cNumericExpr(attVal);
            attribCtors[id]->SetVertical();
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::font)) {
            fontName = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::text)) {
            text = new cTextExpr(attVal);
        } else if (IdEqual(id, (int)eDrawTextBoxAttribs::fontsize)) {
            attribCtors[id] = new cNumericExpr(attVal);
            attribCtors[id]->SetVertical();
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawTextBox::Cache(void) {
    cFunction::Cache();
    if (text) {
        text->SetTokenContainer(tokenContainer);
        text->SetGlobals(globals);
        text->Cache();
    }
    if (fontName) {
        CacheFont(globals, GetValue((int)eDrawTextBoxAttribs::fontsize));
    }
}

void cFuncDrawTextBox::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    if (!font) {
        LoadFont(GetValue((int)eDrawTextBoxAttribs::fontsize));
        if (!font)
            return;
    }
    SetFloater();
    eFloatMode mode = (eFloatMode)GetValue((int)eDrawTextBoxAttribs::floatmode);
    int fontHeight = FontHeight();
    int lines = floater->Lines();
    eAlign align = (eAlign)GetValue((int)eDrawTextBoxAttribs::align);
    int boxX = X() + x0;
    int floatHeight = 0;
    if ( mode == eFloatMode::topleft ) {
        boxX += GetValue((int)eDrawTextBoxAttribs::floatwidth);
        floatHeight = GetValue((int)eDrawTextBoxAttribs::floatheight);
    }
    eAlign valign = (eAlign)GetValue((int)eDrawTextBoxAttribs::valign);
    int height = Height();
    if (height <= 0)
        height = container.Height();
    int y = Y() + y0;
    if (valign == eAlign::center) {
        y = y0 + (height - fontHeight * lines) / 2;
    } else if (valign == eAlign::bottom) {
        y = y0 + height - fontHeight * lines;
    }
    int x = boxX;
    for (int line=0; line < lines; line++) {
        const char *lineText = floater->GetLine(line);
        if (!lineText)
            break;
        if (align == eAlign::center) {
            x = boxX + (Width() - TextWidth(lineText)) / 2;
        } else if (align == eAlign::right) {
            x = boxX + Width() - TextWidth(lineText);
        }
        p->DrawText(cPoint(x, y), lineText, color->Color(), clrTransparent, font);
        y += fontHeight;
        if ( mode == eFloatMode::topleft ) {
            if ((line+1) * fontHeight >= floatHeight)
                x = X() + x0;
        }
    }
}

int cFuncDrawTextBox::FuncWidth(void) {
    return Width();
}

int cFuncDrawTextBox::FuncHeight(void) {
    if (!font)
        return 0;
    int boxHeight = Height();
    if (boxHeight > 0)
        return boxHeight;
    SetFloater();
    int lines = floater->Lines();
    return lines * FontHeight();
}

void cFuncDrawTextBox::Debug(void) {
    cFunction::Debug();
    if (fontName)
        esyslog("skindesigner:            fontname: \"%s\"", fontName);
    if (font)
        esyslog("skindesigner:            cached font name: \"%s\", size %d", font->FontName(), font->Height());
    if (text)
        text->Debug("draw textbox");
}

void cFuncDrawTextBox::SetFloater(void) {
    char *funcText = NULL;
    if (text)
        funcText = text->DeterminateText();
    int boxWidth = Width();
    int boxHeight  = Height();
    int floatWidth = GetValue((int)eDrawTextBoxAttribs::floatwidth);
    int floatHeight = GetValue((int)eDrawTextBoxAttribs::floatheight);
    int maxLines = GetValue((int)eDrawTextBoxAttribs::maxlines);
    delete floater;
    floater = new cTextFloater();
    floater->Set(funcText, font, boxWidth, boxHeight, floatWidth, floatHeight, maxLines);
    free(funcText);
}

void cFuncDrawTextBox::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawTextBoxAttribs::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawTextBoxAttribs::valign));
    attribIDs.insert(pair<string, int>("maxlines", (int)eDrawTextBoxAttribs::maxlines));
    attribIDs.insert(pair<string, int>("floatwidth", (int)eDrawTextBoxAttribs::floatwidth));
    attribIDs.insert(pair<string, int>("floatheight", (int)eDrawTextBoxAttribs::floatheight));
    attribIDs.insert(pair<string, int>("float", (int)eDrawTextBoxAttribs::floatmode));
    attribIDs.insert(pair<string, int>("color", (int)eDrawTextBoxAttribs::color));
    attribIDs.insert(pair<string, int>("font", (int)eDrawTextBoxAttribs::font));
    attribIDs.insert(pair<string, int>("fontsize", (int)eDrawTextBoxAttribs::fontsize));
    attribIDs.insert(pair<string, int>("name", (int)eDrawTextBoxAttribs::name));
    attribIDs.insert(pair<string, int>("text", (int)eDrawTextBoxAttribs::text));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::maxlines, "maxlines"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::floatwidth, "floatwidth"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::floatheight, "floatheight"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::floatmode, "float"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::color, "color"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::font, "font"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::fontsize, "fontsize"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawTextBoxAttribs::text, "text"));
}

void cFuncDrawTextBox::SetFloatMode(int id, const char *val) {
    eFloatMode mode = eFloatMode::none;
    if (!strcmp(val, "topleft"))
        mode = eFloatMode::topleft;
    else if (!strcmp(val, "topright"))
        mode = eFloatMode::topright;
    attribs[id] = (int)mode;
}

/***************************************************************************
* cFuncDrawImage
***************************************************************************/
cFuncDrawImage::cFuncDrawImage(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "DrawImage";
    path = NULL;
    SetAttributesDefs();
}

cFuncDrawImage::cFuncDrawImage(const cFuncDrawImage &other) : cFunction(other) {
    funcType = other.funcType;
    path = NULL;
    if (other.path)
        path = new cTextExpr(*other.path);
}

cFuncDrawImage::~cFuncDrawImage(void) {
    delete path;
}

void cFuncDrawImage::SetLoopInfo(cLoopInfo *loopInfo) {
    cFunction::SetLoopInfo(loopInfo);
    if (path)
        path->SetLoopInfo(loopInfo);
}

void cFuncDrawImage::SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer) {
    cAttributes::SetTokenContainerDeep(tokenContainer);
    if (path) {
        path->SetTokenContainer(tokenContainer);
    }
}

void cFuncDrawImage::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown func DrawText attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eDrawImageAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eDrawImageAttribs::path)) {
            path = new cTextExpr(attVal);
        } else if (IdEqual(id, (int)eDrawImageAttribs::align)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawImageAttribs::valign)) {
            SetAlign(id, attVal);
        } else if (IdEqual(id, (int)eDrawImageAttribs::imagetype)) {
            SetImageType(id, attVal);
        } else if (IdEqual(id, (int)eDrawImageAttribs::animtype)) {
            SetAnimType(id, attVal);
        } else if (IdEqual(id, (int)eDrawImageAttribs::cache)) {
            SetBool(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncDrawImage::Debug(void) {
    cFunction::Debug();
    if (path)
        path->Debug("image path");
}

void cFuncDrawImage::SetImageType(int id, const char *val) {
    eImageType imgType = eImageType::image;
    if (!strcmp(val, "channellogo"))
        imgType = eImageType::channellogo;
    else if (!strcmp(val, "seplogo"))
        imgType = eImageType::seplogo;
    else if (!strcmp(val, "skinpart"))
        imgType = eImageType::skinpart;
    else if (!strcmp(val, "menuicon"))
        imgType = eImageType::menuicon;
    else if (!strcmp(val, "icon"))
        imgType = eImageType::icon;
    attribs[id] = (int)imgType;
}

void cFuncDrawImage::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("align", (int)eDrawImageAttribs::align));
    attribIDs.insert(pair<string, int>("valign", (int)eDrawImageAttribs::valign));
    attribIDs.insert(pair<string, int>("imagetype", (int)eDrawImageAttribs::imagetype));
    attribIDs.insert(pair<string, int>("name", (int)eDrawImageAttribs::name));
    attribIDs.insert(pair<string, int>("cache", (int)eDrawImageAttribs::cache));
    attribIDs.insert(pair<string, int>("path", (int)eDrawImageAttribs::path));
    attribIDs.insert(pair<string, int>("animtype", (int)eDrawImageAttribs::animtype));
    attribIDs.insert(pair<string, int>("animfreq", (int)eDrawImageAttribs::animfreq));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::align, "align"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::imagetype, "imagetype"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::cache, "cache"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::path, "path"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::animtype, "animtype"));
    attribNames.insert(pair<int, string>((int)eDrawImageAttribs::animfreq, "animfreq"));
}

void cFuncDrawImage::Cache(void) {
    cFunction::Cache();
    if (path) {
        int type = GetValue((int)eDrawImageAttribs::imagetype);
        if (type == (int)eImageType::image) {
            path->CorrectImagePath();
        }
        path->SetGlobals(globals);
        path->SetTokenContainer(tokenContainer);
        path->Cache();
    }

    if (config.cacheImagesInitial)
        PreCacheImage();
}

void cFuncDrawImage::PreCacheImage(void) {
    int imgWidth = Width();
    int imgHeight = Height();
    char *imgPath = path->DeterminateText();
    if (!(imgWidth > 0 && imgHeight > 0))
        return;

    eImageType type = (eImageType)GetValue((int)eDrawImageAttribs::imagetype);
    switch (type) {
        case eImageType::channellogo:
            imgCache->CacheLogo(imgWidth, imgHeight);
            break;
        case eImageType::skinpart:
            if (imgPath)
                imgCache->GetSkinpart(imgPath, imgWidth, imgHeight);
            break;
        case eImageType::icon: 
        case eImageType::menuicon:
            if (imgPath)
                imgCache->GetIcon(type, imgPath, imgWidth, imgHeight);
            break;
        default: 
            break;
    }
    free(imgPath);
}

void cFuncDrawImage::Render(cPixmap *p, int x0, int y0, int colWidth, int rowHeight) {
    if (!path)
        return;
    eAlign align = (eAlign)GetValue((int)eDrawImageAttribs::align);
    eAlign valign = (eAlign)GetValue((int)eDrawImageAttribs::valign);
    int x = GetX(align, x0, colWidth);
    int y = GetY(valign, y0, rowHeight);
    cPoint pos(x, y);
    eImageType type = (eImageType)GetValue((int)eDrawImageAttribs::imagetype);
    char *imgPath = path->DeterminateText();
    if (!imgPath)
        return;
    
    switch (type) {
        case eImageType::channellogo: {
            cImage *logo = imgCache->GetLogo(imgPath, Width(), Height());
            if (logo) {
                p->DrawImage(pos, *logo);
            }
            break; }
        case eImageType::seplogo: {
            cImage *sep = imgCache->GetSeparatorLogo(imgPath, Width(), Height());
            if (sep) {
                p->DrawImage(pos, *sep);
            }
            break; }
        case eImageType::skinpart: {
            cCachedImage *img = imgCache->GetSkinpart(imgPath, Width(), Height());
            if (!img) break;
            if (img->handle) {
                p->DrawImage(pos, img->handle);
            } else if (img->image) {
                p->DrawImage(pos, *(img->image));                
            }
            break; }
        case eImageType::icon: 
        case eImageType::menuicon: {
            cCachedImage *img = imgCache->GetIcon(type, imgPath, Width(), Height());
            if (!img) break;
            if (img->handle) {
                p->DrawImage(pos, img->handle);
            } else if (img->image) {
                p->DrawImage(pos, *(img->image));                
            }
            break; }
        case eImageType::image: {
            cImageLoader imgLoader;
            if (imgLoader.LoadImage(imgPath)) {
                cImage *img = imgLoader.CreateImage(Width(), Height());
                if (!img) break;
                p->DrawImage(pos, *img);
                delete(img);
            }
            break; }
    }
    free(imgPath);
}

/***************************************************************************
* cFuncLoop
***************************************************************************/
cFuncLoop::cFuncLoop(cArea *owner, int numAttributes) : cFunction(owner, numAttributes) {
    funcType = "LoopFunc";
    SetAttributesDefs();
}

cFuncLoop::~cFuncLoop(void) {
}

void cFuncLoop::Set(vector<stringpair> &attributes) {
    for (vector<stringpair>::iterator att = attributes.begin(); att != attributes.end(); att++) {
        const char *attName = (*att).first.c_str();
        const char *attVal = (*att).second.c_str();
        int id = AttributeId(attName);
        if (id == ATTR_UNKNOWN) {
            esyslog("skindesigner: unknown loopfunc attribute \"%s\" = \"%s\"", attName, attVal);
            continue;
        }
        
        if (SetCommon(id, attVal))
            continue;
        if (IdEqual(id, (int)eLoopAttribs::name)) {
            name = strdup(attVal);
        } else if (IdEqual(id, (int)eLoopAttribs::orientation)) {
            SetOrientation(id, attVal);
        } else if (IdEqual(id, (int)eLoopAttribs::overflow)) {
            SetOverflow(id, attVal);
        } else if (IdEqual(id, (int)eLoopAttribs::valign)) {
            SetAlign(id, attVal);
        } else {
            attribCtors[id] = new cNumericExpr(attVal);
        }
    }
}

void cFuncLoop::Cache(void) {
    cFunction::Cache();
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetGlobals(globals);
        f->SetTokenContainer(tokenContainer);
        f->Cache();
    }
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetLoopInfo(&loopInfo);
        f->CacheFuncReferences();
    }
}

void cFuncLoop::SetContainer(int x, int y, int width, int height) {
    cAttributes::SetContainer(x, y, width, height);
    int funcX = X();
    int funcY = Y();
    int funcWidth = Width();
    int funcHeight = Height();
    
    int funcContainerX = (funcX > 0) ? x + funcX : x;
    int funcContainerY = (funcY > 0) ? y + funcY : y;
    int funcContainerWidth = (funcWidth > 0) ? funcWidth : width;
    int funcContainerHeight = (funcHeight > 0) ? funcHeight : height;

    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->SetContainer(funcContainerX, funcContainerY, funcContainerWidth, funcContainerHeight);
    }
}


void cFuncLoop::AddFunction(cFunction *f) {
    functions.Add(f);
}

cFunction *cFuncLoop::GetFunction(const char *name) {
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        const char *funcName = f->Name();
        if (funcName && !strcmp(funcName, name)) {
            return f;
        }
    }
    return NULL;
}

void cFuncLoop::Render(cPixmap *p, int x0, int y0, int cw, int rh) {
    int loopIndex = tokenContainer->LoopIndex(Name());
    if (loopIndex < 0) {
        esyslog("skindesigner: unknown loop function \"%s\"", Name());
        return;
    }
    int numRows = tokenContainer->NumLoops(loopIndex);
    if (numRows < 1)
        return;
    int columnWidth = GetValue((int)eLoopAttribs::columnwidth);
    int rowHeight = GetValue((int)eLoopAttribs::rowheight);
    int maxItems = GetValue((int)eLoopAttribs::maxitems);
    eOrientation orientation = (eOrientation)GetValue((int)eLoopAttribs::orientation);
    eOverflowType overflow = (eOverflowType)GetValue((int)eLoopAttribs::overflow);
    eAlign vAlign = (eAlign)GetValue((int)eLoopAttribs::valign);

    int loopWidth = Width();
    if (loopWidth <= 0)
        loopWidth = container.Width();
    int loopHeight = Height();
    if (loopHeight <= 0)
        loopHeight = container.Height();

    loopInfo.colWidth = columnWidth;
    loopInfo.rowHeight = rowHeight;
    loopInfo.index = loopIndex;

    int loopX = X();
    int loopY = Y();
    int x = (loopX > 0) ? loopX : 0;
    int y = (loopY > 0) ? loopY : 0;

    //set y position for vertical loop with valign bottom
    if (orientation == eOrientation::vertical && vAlign == eAlign::bottom) {
        int actRowheight = (rowHeight > 0) ? rowHeight : RowHeight();
        if (actRowheight > 0) {
            float fTotalItems = (float)loopHeight / (float)actRowheight;
            int totalItems = (int)fTotalItems;
            if (fTotalItems - (float)totalItems > 0.5f)
                totalItems++;
            if (maxItems > 0 && maxItems < totalItems)
                totalItems = maxItems;
            if (numRows < totalItems)
                y += (totalItems - numRows) * actRowheight;
        }
    }

    for (int row = 0; row < numRows; ++row) {
        if (maxItems > 0 && row >= maxItems) {
            break;
        }
        loopInfo.row = row;
        //check overflow cut
        if (overflow == eOverflowType::cut) {
            if (orientation == eOrientation::horizontal && (row * columnWidth > loopWidth)) {
                return;
            } else if (orientation == eOrientation::vertical && (row * rowHeight > loopHeight)) {
                return;
            }
        }
        for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
            if (f->DoDebug())
                f->Debug();
            if (!f->DoExecute())
                continue;
            f->Render(p, x, y, columnWidth, rowHeight);
        }        
        if (orientation == eOrientation::horizontal) {
            if (columnWidth > 0) {
                x += columnWidth;
            } else {
                x += ColumnWidth();
            }
            //check overflow wrap
            if (overflow == eOverflowType::wrap && orientation == eOrientation::horizontal) {
                if (x + columnWidth - 10 > loopWidth) {
                    x = X();
                    if (x < 0) x = 0;
                    y += rowHeight;
                }
            }
        } else if (orientation == eOrientation::vertical) {
            if (rowHeight > 0) {
                y += rowHeight;
            } else {
                y += RowHeight();
            }
        }
    }
}

int cFuncLoop::FuncWidth(void) {
    loopInfo.row = 0;
    int loopIndex = tokenContainer->LoopIndex(Name());
    if (loopIndex < 0) {
        esyslog("skindesigner: unknown loop function \"%s\"", Name());
        return 0;
    }
    int numLoops = tokenContainer->NumLoops(loopIndex);
    if (numLoops < 1)
        return 0;

    int columnWidth = GetValue((int)eLoopAttribs::columnwidth);
    if (columnWidth <=0)
        columnWidth = ColumnWidth();
    eOverflowType overflow = (eOverflowType)GetValue((int)eLoopAttribs::overflow);
    if (overflow == eOverflowType::cut) {
        int maxItems = GetValue((int)eLoopAttribs::maxitems);
        numLoops = min(numLoops, maxItems);
    }
    if (numLoops > 0)
        return numLoops * columnWidth;
    return 0;
}

int cFuncLoop::FuncHeight(void) {
    loopInfo.row = 0;
    int loopIndex = tokenContainer->LoopIndex(Name());
    if (loopIndex < 0) {
        esyslog("skindesigner: unknown loop function \"%s\"", Name());
        return 0;
    }
    int numLoops = tokenContainer->NumLoops(loopIndex);
    if (numLoops < 1)
        return 0;

    int rowHeight = GetValue((int)eLoopAttribs::rowheight);
    if (rowHeight <=0)
        rowHeight = RowHeight();
    
    eOverflowType overflow = (eOverflowType)GetValue((int)eLoopAttribs::overflow);
    if (overflow == eOverflowType::cut) {
        int maxItems = GetValue((int)eLoopAttribs::maxitems);
        numLoops = min(numLoops, maxItems);
    } else if (overflow == eOverflowType::wrap) {
        int loopWidth = Width();
        if (loopWidth <= 0)
            loopWidth = container.Width();
        loopWidth++;
        int columnWidth = GetValue((int)eLoopAttribs::columnwidth);
        if (columnWidth <=0)
            columnWidth = ColumnWidth();
        int itemsPerRow = loopWidth / columnWidth;
        int rows = numLoops / itemsPerRow;
        if (numLoops % itemsPerRow > 0)
            rows++;
        return rows * rowHeight;
    }
    if (numLoops > 0)
        return numLoops * rowHeight;
    return 0;
}

void cFuncLoop::Debug(void) {
    loopInfo.row = 0;
    cFunction::Debug();
    dsyslog("skindesigner:        loopfunc Functions:");
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        f->Debug();
    }
}

void cFuncLoop::SetAttributesDefs(void) {
    attribIDs.insert(pair<string, int>("columnwidth", (int)eLoopAttribs::columnwidth));
    attribIDs.insert(pair<string, int>("rowheight", (int)eLoopAttribs::rowheight));
    attribIDs.insert(pair<string, int>("name", (int)eLoopAttribs::name));
    attribIDs.insert(pair<string, int>("orientation", (int)eLoopAttribs::orientation));
    attribIDs.insert(pair<string, int>("valign", (int)eLoopAttribs::valign));
    attribIDs.insert(pair<string, int>("overflow", (int)eLoopAttribs::overflow));
    attribIDs.insert(pair<string, int>("maxitems", (int)eLoopAttribs::maxitems));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::columnwidth, "columnwidth"));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::rowheight, "rowheight"));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::name, "name"));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::orientation, "orientation"));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::valign, "valign"));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::overflow, "overflow"));
    attribNames.insert(pair<int, string>((int)eLoopAttribs::maxitems, "maxitems"));
}

int cFuncLoop::ColumnWidth(void) {
    int width = 0;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (!f->DoExecute())
            continue;
        int funcWidth = f->X() + f->FuncWidth();
        if (funcWidth > width)
            width = funcWidth;
    }
    return width;
}

int cFuncLoop::RowHeight(void) {
    int height = 0;
    for (cFunction *f = functions.First(); f; f = functions.Next(f)) {
        if (!f->DoExecute())
            continue;
        int funcHeight = f->Y() + f->FuncHeight();
        if (funcHeight > height)
            height = funcHeight;
    }
    return height;
}

// --- cTextFloater ----------------------------------------------------------

cTextFloater::cTextFloater(void) {
    text = eol = NULL;
    lines = 0;
    lastLine = -1;
}

cTextFloater::cTextFloater(const char *fext, const cFont *font, int width, int height, int floatWidth, int floatHeight, int maxLines) {
    text = NULL;
    Set(text, font, width, height, floatWidth, floatHeight, maxLines);
}

cTextFloater::~cTextFloater() {
    free(text);
}

void cTextFloater::Set(const char *Text, const cFont *font, int width, int height, int floatWidth, int floatHeight, int maxLines) {
    free(text);
    text = Text ? strdup(Text) : NULL;
    eol = NULL;
    lines = 0;
    lastLine = -1;
    if (!text)
        return;
    lines = 1;
    if (width <= 0)
        return;
    int lineHeight = font->Height();
    bool cut = false;
    if (height > 0)
        cut = true;
    bool doFloat = false;
    if (floatWidth > 0 && floatHeight > 0)
        doFloat = true;
    int textWidth = width;
    if (doFloat) {
        textWidth = width - floatWidth;
    }

    char *Blank = NULL;
    char *Delim = NULL;
    int w = 0;

    stripspace(text); // strips trailing newlines

    for (char *p = text; *p; ) {
        int sl = Utf8CharLen(p);
        uint sym = Utf8CharGet(p, sl);
        if (sym == '\n') {
            if ((maxLines > 0) && (lines == maxLines)) {
                while (*p) {
                    *p = 0;
                    p++;
                }
                return;
            }
            lines++;
            if (cut) {
                if (lines * lineHeight >= height) {
                    //remove last line, find last linebreak
                    p--;
                    int max = 100;
                    int i = 0;
                    while (*p) {
                        int sl = Utf8CharLen(p);
                        uint sym = Utf8CharGet(p, sl);
                        if (sym == '\n')
                            break;
                        p -= sl;
                        i++;
                        if (i > max)
                            break;
                    }
                    p++; 
                    if (*p) {
                        *p = '.'; p++; 
                        if (*p) { 
                            *p = '.'; p++; 
                            if (*p) { 
                                *p = '.'; p++; 
                                *p = 0;
                            }
                        }
                    }
                    break;
                }
            }
            if (doFloat) {
                if ((lines-1) * lineHeight >= floatHeight) {
                    textWidth = width;
                }
            }
            w = 0;
            Blank = Delim = NULL;
            p++;
            continue;
        } else if (sl == 1 && isspace(sym))
            Blank = p;

        int cw = font->Width(sym);
        if (w + cw > textWidth) {
            if (Blank) {
                *Blank = '\n';
                p = Blank;
                continue;
            } else if (w > 0) { // there has to be at least one character before the newline
                // Here's the ugly part, where we don't have any whitespace to
                // punch in a newline, so we need to make room for it:
                if (Delim)
                    p = Delim + 1; // let's fall back to the most recent delimiter
                char *s = MALLOC(char, strlen(text) + 2); // The additional '\n' plus the terminating '\0'
                int l = p - text;
                strncpy(s, text, l);
                s[l] = '\n';
                strcpy(s + l + 1, p);
                free(text);
                text = s;
                p = text + l;
                continue;
            }
        }
        w += cw;
        if (strchr("-.,:;!?_", *p)) {
            Delim = p;
            Blank = NULL;
        }
        p += sl;
    }
}

const char *cTextFloater::Text(void) {
    if (eol) {
        *eol = '\n';
        eol = NULL;
    }
    return text;
}

const char *cTextFloater::GetLine(int line) {
    char *s = NULL;
    if (line < lines) {
        if (eol) {
            *eol = '\n';
            if (line == lastLine + 1)
                s = eol + 1;
            eol = NULL;
        }
        if (!s) {
            s = text;
            for (int i = 0; i < line; i++) {
                s = strchr(s, '\n');
                if (s)
                    s++;
                else
                    break;
            }
        }
        if (s) {
            if ((eol = strchr(s, '\n')) != NULL)
                *eol = 0;
        }
        lastLine = line;
    }
    return s;
}
