#ifndef __VIEWLIST_H
#define __VIEWLIST_H

#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"
#include "listelements.h"
#include "area.h"

class cViewList : public cFadable, public cShiftable {
protected:
    int plugId;
    int plugMenuId;
    cViewListAttribs *attribs;
    cRect container;
    cGlobals *globals;
    skindesignerapi::cTokenContainer *tokenContainer;
    int numElements;
    eOrientation orientation;
    bool cleared;
    int itemCount;
    cViewElement *listElement;
    cViewElement *currentElement;
    cListElement **listElements;
    cFader *fader;
    cShifter *shifter;
    virtual void Prepare(int start, int step) {};
    cPoint ShiftStart(cRect &shiftbox);
    void SetShiftParameters(int index, int &call);
    void CheckListAnimation(int index);
public:
    cViewList(void);
    virtual ~cViewList(void);
    void SetGlobals(cGlobals *globals);    
    void SetContainer(int x, int y, int width, int height);
    void SetAttributes(vector<stringpair> &attributes);
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    static cViewList *CreateViewList(const char *name);
    static cViewElement *CreateListElement(const char *name);
    static cViewElement *CreateCurrentElement(const char *name);
    void AddListElement(cViewElement *listElement);
    void AddCurrentElement(cViewElement *currentElement);
    virtual void PreCache(void);
    int NumItems(void);
    bool Execute(void);
    eOrientation Orientation(void);
    void Draw(eMenuCategory menuCat);
    void Clear(void);
    void ResetItemCount(void) { itemCount = 0; };
    virtual void Close(void);
    eButtonType Button(void) { return attribs->Button(); };
    //Fadable
    bool Detached(void) { return false; };
    int Delay(void) { return 0; };
    int FadeTime(void) { return attribs->FadeTime(); };
    void SetTransparency(int transparency, bool force = false);
    //Shiftable
    int ShiftTime(void) { return attribs->ShiftTime(); };
    int ShiftMode(void) { return attribs->ShiftMode(); };
    void ShiftPositions(cPoint *start, cPoint *end);
    void SetPosition(cPoint &position, cPoint &reference, bool force = false);
    void SetStartShifting(void) { };
    void SetEndShifting(void) { };
    cRect CoveredArea(void);
    void StartAnimation(bool animOut = false);
    void Debug(void);
};

class cViewListDefault : public cViewList {
private:
    cLeMenuDefault **listDefault;
    int avrgFontWidth;
    const cFont *listFont;
    int *colX;
    int *colWidths;
    const char *plugName;
protected:
    void Prepare(int start, int step);
public:
    cViewListDefault(void);
    virtual ~cViewListDefault(void);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    void SetPlugin(const char *plugName) { this->plugName = plugName; };
    void Set(const char *text, int index, bool current, bool selectable);
    const cFont *GetListFont(void);
    int GetListWidth(void);
};

class cViewListMain : public cViewList {
private:
    cLeMenuMain **listMain;
    cCeMenuMain *currentMain;
protected:
    void Prepare(int start, int step);
public:
    cViewListMain(void);
    virtual ~cViewListMain(void);
    void Set(const char *text, int index, bool current, bool selectable);
    const char *GetPlugin(void);
};

class cViewListSchedules : public cViewList {
private:
    cLeMenuSchedules **listSchedules;
    cCeMenuSchedules *currentSchedules;
    bool epgSearchFav;
protected:
    void Prepare(int start, int step);
public:
    cViewListSchedules(void);
    virtual ~cViewListSchedules(void);
    void IsEpgSearchFav(bool isFav) { epgSearchFav = isFav; };
    void Set(const cEvent *event, int index, bool current, bool selectable, const cChannel *channel, bool withDate, eTimerMatch timerMatch);
};

class cViewListTimers : public cViewList {
private:
    cLeMenuTimers **listTimers;
    cCeMenuTimers *currentTimer;
protected:
    void Prepare(int start, int step);
public:
    cViewListTimers(void);
    virtual ~cViewListTimers(void);
    void Set(const cTimer *timer, int index, bool current, bool selectable);
};

class cViewListChannels : public cViewList {
private:
    cLeMenuChannels **listChannels;
    cCeMenuChannels *currentChannel;
protected:
    void Prepare(int start, int step);
public:
    cViewListChannels(void);
    virtual ~cViewListChannels(void);
    void Set(const cChannel *channel, int index, bool current, bool selectable, bool withProvider);
};

class cViewListRecordings : public cViewList {
private:
    cLeMenuRecordings **listRecordings;
    cCeMenuRecordings *currentRecording;
protected:
    void Prepare(int start, int step);
public:
    cViewListRecordings(void);
    virtual ~cViewListRecordings(void);
    void Set(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New);
};

class cViewListPlugin : public cViewList {
private:
    cLeMenuPlugin **listPlugin;
    cCeMenuPlugin *currentPlugin;
protected:
    void Prepare(int start, int step);
public:
    cViewListPlugin(void);
    virtual ~cViewListPlugin(void);
    void Set(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable);
};

class cViewListAudioTracks : public cViewList {
private:
    skindesignerapi::cTokenContainer *tokenContainer;
    int numTracks;
    cLeAudioTracks **listAudioTracks;
public:
    cViewListAudioTracks(void);
    virtual ~cViewListAudioTracks(void);
    void Close(void);
    void PreCache(void);
    void SetNumtracks(int numTracks);
    void SetTracks(const char * const *tracks);
    void SetCurrentTrack(int index);
    void Draw(void);
};

class cViewListChannelList : public cViewList {
private:
    cLeChannelList **listChannelList;
protected:
    void Prepare(int start, int step);
public:
    cViewListChannelList(void);
    virtual ~cViewListChannelList(void);
    void Set(const cChannel *channel, int index, bool current);
};

class cViewListGroupList : public cViewList {
private:
    cLeGroupList **listGroupList;
protected:
    void Prepare(int start, int step);
public:
    cViewListGroupList(void);
    virtual ~cViewListGroupList(void);
    void Set(const char *group, int numChannels, int index, bool current);
};
#endif //__VIEWLIST_H