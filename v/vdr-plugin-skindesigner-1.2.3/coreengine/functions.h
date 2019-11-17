#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include "functions.h"

class cFuncFill : public cFunction {
private:
    void SetAttributesDefs(void);
public:
    cFuncFill(cArea *owner, int numAttributes);
    cFuncFill(const cFuncFill &other);
    virtual ~cFuncFill(void);
    void Set(vector<stringpair> &attributes);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
};

class cFuncDrawRectangle : public cFunction {
private:
    void SetAttributesDefs(void);
public:
    cFuncDrawRectangle(cArea *owner, int numAttributes);
    cFuncDrawRectangle(const cFuncDrawRectangle &other);
    virtual ~cFuncDrawRectangle(void);
    void Set(vector<stringpair> &attributes);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    bool Blinking(void) { return GetValue((int)eDrawRectangleAttribs::animtype) == (int)eAnimType::blink; };
    int BlinkFreq(void) { return GetValue((int)eDrawRectangleAttribs::animfreq); };
    int Align(void) { return GetValue((int)eDrawRectangleAttribs::align); };
    int Valign(void) { return GetValue((int)eDrawRectangleAttribs::valign); };
};

class cFuncDrawEllipse : public cFunction {
private:
    void SetAttributesDefs(void);
public:
    cFuncDrawEllipse(cArea *owner, int numAttributes);
    cFuncDrawEllipse(const cFuncDrawEllipse &other);
    virtual ~cFuncDrawEllipse(void);
    void Set(vector<stringpair> &attributes);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    bool Blinking(void) { return GetValue((int)eDrawEllipseAttribs::animtype) == (int)eAnimType::blink; };
    int BlinkFreq(void) { return GetValue((int)eDrawEllipseAttribs::animfreq); };
    int Align(void) { return GetValue((int)eDrawEllipseAttribs::align); };
    int Valign(void) { return GetValue((int)eDrawEllipseAttribs::valign); };
};

class cFuncDrawSlope : public cFunction {
private:
    void SetAttributesDefs(void);
public:
    cFuncDrawSlope(cArea *owner, int numAttributes);
    cFuncDrawSlope(const cFuncDrawSlope &other);
    virtual ~cFuncDrawSlope(void);
    void Set(vector<stringpair> &attributes);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    bool Blinking(void) { return GetValue((int)eDrawSlopeAttribs::animtype) == (int)eAnimType::blink; };
    int BlinkFreq(void) { return GetValue((int)eDrawSlopeAttribs::animfreq); };
    int Align(void) { return GetValue((int)eDrawSlopeAttribs::align); };
    int Valign(void) { return GetValue((int)eDrawSlopeAttribs::valign); };
};

class cTextDrawer {
private:
    static cMutex fontLock;
protected:
    const cFont *font;
    char *fontName;
    int fontSize;
    void CacheFont(cGlobals *globals, int size);
    void LoadFont(int size);
    int TextWidth(const char *text);
    int FontHeight(void);
public:
    cTextDrawer(void);
    virtual ~cTextDrawer(void);
};

class cFuncDrawText : public cFunction, public cTextDrawer {
private:
    cTextExpr *text;
    void SetAttributesDefs(void);
    char *Cut(char *expr, int width);
public:
    cFuncDrawText(cArea *owner, int numAttributes);
    cFuncDrawText(const cFuncDrawText &other);
    virtual ~cFuncDrawText(void);
    void SetLoopInfo(cLoopInfo *loopInfo);
    void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    void Set(vector<stringpair> &attributes);
    void Cache(void);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    int FuncX(void);
    int FuncY(void);
    int FuncWidth(void);
    int FuncHeight(void);
    int AvrgFontWidth(void);
    const cFont *GetFont(void);
    bool Blinking(void) { return GetValue((int)eDrawTextAttribs::animtype) == (int)eAnimType::blink; };
    int BlinkFreq(void) { return GetValue((int)eDrawTextAttribs::animfreq); };
    int Align(void) { return GetValue((int)eDrawTextAttribs::align); };
    int Valign(void) { return GetValue((int)eDrawTextAttribs::valign); };
    void Debug(void);
};

class cFuncDrawTextVertical : public cFunction, public cTextDrawer {
private:
    cTextExpr *text;
    void SetAttributesDefs(void);
public:
    cFuncDrawTextVertical(cArea *owner, int numAttributes);
    cFuncDrawTextVertical(const cFuncDrawTextVertical &other);
    virtual ~cFuncDrawTextVertical(void);
    void SetLoopInfo(cLoopInfo *loopInfo);
    void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    void Set(vector<stringpair> &attributes);
    void Cache(void);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    int FuncWidth(void);
    int FuncHeight(void);
    bool Blinking(void) { return GetValue((int)eDrawTextAttribsVertical::animtype) == (int)eAnimType::blink; };
    int BlinkFreq(void) { return GetValue((int)eDrawTextAttribsVertical::animfreq); };
    int Align(void) { return GetValue((int)eDrawTextAttribsVertical::align); };
    int Valign(void) { return GetValue((int)eDrawTextAttribsVertical::valign); };
    void Debug(void);
};

class cTextFloater;

class cFuncDrawTextBox : public cFunction, public cTextDrawer {
private:
    cTextExpr *text;
    cTextFloater *floater;
    void SetFloater(void);
    void SetAttributesDefs(void);
    void SetFloatMode(int id, const char *val);
public:
    cFuncDrawTextBox(cArea *owner, int numAttributes);
    cFuncDrawTextBox(const cFuncDrawTextBox &other);
    virtual ~cFuncDrawTextBox(void);
    void SetLoopInfo(cLoopInfo *loopInfo);
    void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    void Set(vector<stringpair> &attributes);
    void Cache(void);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    int FuncWidth(void);
    int FuncHeight(void);
    void Debug(void);
};

class cFuncDrawImage : public cFunction {
private:
    cTextExpr *path;
    void SetAttributesDefs(void);
    void SetImageType(int id, const char *val);
    void PreCacheImage(void);
public:
    cFuncDrawImage(cArea *owner, int numAttributes);
    cFuncDrawImage(const cFuncDrawImage &other);
    virtual ~cFuncDrawImage(void);
    void SetLoopInfo(cLoopInfo *loopInfo);
    void SetTokenContainerDeep(skindesignerapi::cTokenContainer *tokenContainer);
    void Set(vector<stringpair> &attributes);
    void Cache(void);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    bool Blinking(void) { return GetValue((int)eDrawImageAttribs::animtype) == (int)eAnimType::blink; };
    int BlinkFreq(void) { return GetValue((int)eDrawImageAttribs::animfreq); };
    int Align(void) { return GetValue((int)eDrawImageAttribs::align); };
    int Valign(void) { return GetValue((int)eDrawImageAttribs::valign); };
    void Debug(void);
};

class cFuncLoop : public cFunction {
private:
    cLoopInfo loopInfo;
    cList<cFunction> functions;
    void SetAttributesDefs(void);
    int ColumnWidth(void);
    int RowHeight(void);
public:
    cFuncLoop(cArea *owner, int numAttributes);
    virtual ~cFuncLoop(void);
    void Set(vector<stringpair> &attributes);
    void SetContainer(int x, int y, int width, int height);
    void Cache(void);
    void AddFunction(cFunction *f);
    cFunction *GetFunction(const char *name);
    void Render(cPixmap *p, int x0 = 0, int y0 = 0, int colWidth = 0, int rowHeight = 0);
    int FuncWidth(void);
    int FuncHeight(void);
    int Valign(void) { return GetValue((int)eLoopAttribs::valign); };
    void Debug(void);
};

class cTextFloater {
private:
    char *text;
    char *eol;
    int lines;
    int lastLine;
public:
    cTextFloater(void);
    cTextFloater(const char *text, const cFont *font, int width, int height = 0, int floatWidth = 0, int floatHeight = 0, int maxLines = 0);
    ~cTextFloater();
    void Set(const char *Text, const cFont *font, int width, int height = 0, int floatWidth = 0, int floatHeight = 0, int maxLines = 0);
    ///< Wraps the Text to make it fit into the area defined by the given Width
    ///< when displayed with the given Font.
    ///< Wrapping is done by inserting the necessary number of newline
    ///< characters into the string.
    ///< if height is set, new lines are only set till height is reached
    ///< if floatwidth and floatheight are set, the first lines (depending on 
    ///< size of floatheight) are set to floatwidth
    const char *Text(void);
    ///< Returns the full wrapped text.
    int Lines(void) { return lines; }
    ///< Returns the actual number of lines needed to display the full wrapped text.
    const char *GetLine(int line);
    ///< Returns the given Line. The first line is numbered 0.
};


#endif //__FUNCTIONS_H