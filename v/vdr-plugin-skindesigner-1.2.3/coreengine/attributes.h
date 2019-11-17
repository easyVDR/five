#ifndef __ATTRIBUTES_H
#define __ATTRIBUTES_H

#include "attribute.h"
/******************************************************************
* cViewAttribs
******************************************************************/
class cViewAttribs : public cAttributes {
private:
    cTextExpr *orientation;
    void SetAttributesDefs(void);
    void SetOrientationDynamic(int id, const char *val);
public:
    cViewAttribs(int numAttributes);
    virtual ~cViewAttribs(void);
    void Set(vector<stringpair> &attributes);
    eOrientation Orientation(void);
    int FadeTime(void) { return GetValue((int)eViewAttribs::fadetime); };
    int ShiftTime(void) { return GetValue((int)eViewAttribs::shifttime); };
    cPoint ShiftStartpoint(void) { return cPoint(GetValue((int)eViewAttribs::startx), GetValue((int)eViewAttribs::starty)); };
    int ShiftType(void) { return GetValue((int)eViewAttribs::shifttype); };
    int ShiftMode(void) { return GetValue((int)eViewAttribs::shiftmode); };
    cRect TvFrame(void);
    void Cache(void);
    void Debug(void);
};
/******************************************************************
* cViewElementAttribs
******************************************************************/
class cViewElementAttribs : public cAttributes {
private:
    char *name;
    char *clearOnDisplay;
    void SetAttributesDefs(void);
public:
    cViewElementAttribs(int numAttributes);
    cViewElementAttribs(const cViewElementAttribs &other);
    virtual ~cViewElementAttribs(void);
    void Set(vector<stringpair> &attributes);
    int Mode(void) { return GetValue((int)eViewElementAttribs::mode); };
    int Delay(void) { return GetValue((int)eViewElementAttribs::delay); };
    eOrientation Orientation(void);
    int FadeTime(void) { return GetValue((int)eViewElementAttribs::fadetime); };
    int ShiftTime(void) { return GetValue((int)eViewElementAttribs::shifttime); };
    cPoint ShiftStartpoint(void) { return cPoint(GetValue((int)eViewElementAttribs::startx), GetValue((int)eViewElementAttribs::starty)); };
    int ShiftType(void) { return GetValue((int)eViewElementAttribs::shifttype); };
    int ShiftMode(void) { return GetValue((int)eViewElementAttribs::shiftmode); };
    const char *Name(void) { return name; };
    const char *ClearOnDisplay(void) { return clearOnDisplay; };
    void Debug(void);
};
/******************************************************************
* cViewListAttribs
******************************************************************/
class cViewListAttribs : public cAttributes {
private:
    char *determinateFont;
    void SetAttributesDefs(void);
public:
    cViewListAttribs(int numAttributes);
    virtual ~cViewListAttribs(void);
    void Set(vector<stringpair> &attributes);
    int NumListElements(void);
    int MenuItemWidth(void);
    const char *DeterminateFont(void);
    eAlign Align(void);
    eOrientation Orientation(void);
    int FadeTime(void) { return GetValue((int)eViewListAttribs::fadetime); };
    int ShiftTime(void) { return GetValue((int)eViewListAttribs::shifttime); };
    cPoint ShiftStartpoint(void) { return cPoint(GetValue((int)eViewListAttribs::startx), GetValue((int)eViewListAttribs::starty)); };
    int ShiftType(void) { return GetValue((int)eViewListAttribs::shifttype); };
    int ShiftMode(void) { return GetValue((int)eViewListAttribs::shiftmode); };
    eButtonType Button(void) { return (eButtonType)GetValue((int)eViewListAttribs::button); }
    void Debug(void);
};
/******************************************************************
* cAreaAttribs
******************************************************************/
class cAreaAttribs : public cAttributes {
private:
    cTextExpr *name;
    char *scrollElement;
    void SetAttributesDefs(void);
    bool dynamic;
public:
    cAreaAttribs(int numAttributes);
    cAreaAttribs(const cAreaAttribs &other);
    virtual ~cAreaAttribs(void);
    void Set(vector<stringpair> &attributes);
    const char *GetScrollElement(void) { return scrollElement; };
    int Orientation(void) { return GetValue((int)eAreaAttribs::orientation); };
    int Delay(void) { return GetValue((int)eAreaAttribs::delay); };
    int Mode(void) { return GetValue((int)eAreaAttribs::mode); };
    int ScrollSpeed(void) { return GetValue((int)eAreaAttribs::scrollspeed); };
    int Transparency(void) { return GetValue((int)eAreaAttribs::transparency); };    
    int Layer(void);
    int ScrollStep(void) { return GetValue((int)eAreaAttribs::scrollheight); };
    bool BackgroundArea(void);
    bool IndicatorArea(void);
    const char *Name(void);
    void CheckDynamic(void);
    bool Dynamic(void) {return dynamic; };
    void Cache(void);
    void Debug(void);
};
/******************************************************************
* cAreaContainerAttribs
******************************************************************/
class cAreaContainerAttribs : public cAttributes {
private:
    void SetAttributesDefs(void);
public:
    cAreaContainerAttribs(int numAttributes);
    cAreaContainerAttribs(const cAreaContainerAttribs &other);
    virtual ~cAreaContainerAttribs(void);
    void Set(vector<stringpair> &attributes);
    void Debug(void);
};

#endif //__ATTRIBUTES_H