#ifndef __TVGUIDE_VIEW_H
#define __TVGUIDE_VIEW_H

#include <vector>
#include <string>
#include <sstream>
#include <vdr/skins.h>
#include "services/scraper2vdr.h"
#include "services/epgsearch.h"
#include "services/remotetimers.h"
#include "config.h"
#include "imagecache.h"
#include "imageloader.h"
#include "tools.h"

enum eEPGViewTabs {
    evtInfo = 0,
    evtAddInfo,
    evtImages,
    evtCount
};

enum eMediaViewTabs {
    mvtInfo = 0,
    mvtAddInfo,
    mvtCast,
    mvtOnlineInfo,
    mvtImages,
    mvtCount
};

class cView : public cThread {
protected:
    const cEvent *event;
    cPixmap *pixmapBackground;
    cStyledPixmap *pixmapHeader;
    cPixmap *pixmapHeaderLogo;
    cPixmap *pixmapContent;
    cPixmap *pixmapScrollbar;
    cPixmap *pixmapScrollbarBack;
    cPixmap *pixmapTabs;
    cFont *font, *fontSmall, *fontHeader, *fontHeaderLarge;
    cImage *imgScrollBar;
    int activeView;
    bool scrollable;
    bool tabbed;
    int x, y;
    int width, height;
    int border;
    int headerWidth, headerHeight;
    int contentHeight;
    int tabHeight;
    int scrollbarWidth;
    std::vector<std::string> tabs;
    std::string title;
    std::string subTitle;
    std::string dateTime;
    std::string infoText;
    std::string addInfoText;
    const cChannel *channel;
    int eventID;
    bool headerDrawn;
    void DrawHeader(void);
    void ClearContent(void);
    void CreateContent(int fullHeight);
    void DrawContent(std::string *text);
    void DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2 = NULL);
    void CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg);
    void DrawActors(std::vector<cActor> *actors);
    void ClearScrollbar(void);
    void ClearScrollbarImage(void);
    cImage *CreateScrollbarImage(int width, int height, tColor clrBgr, tColor clrBlend);
    virtual void SetTabs(void) {};
    void DrawTabs(void);
public:    
    cView(void);
    virtual ~cView(void);
    void SetTitle(const char *t) { title = t ? t : ""; };
    void SetSubTitle(const char *s) { subTitle = s ? s : ""; };
    void SetDateTime(const char *dt) { dateTime = dt; };
    void SetInfoText(const char *i) { infoText = i ? i : ""; };
    void SetAdditionalInfoText(std::string addInfo) { addInfoText = addInfo; };
    void SetChannel(const cChannel *c) { channel = c; };
    void SetEventID(int id) { eventID = id; };
    void SetEvent(const cEvent *event) { this->event = event; };
    virtual void LoadMedia(void) {};
    void SetGeometry(void);
    void SetFonts(void);
    virtual bool KeyUp(void);
    virtual bool KeyDown(void);
    virtual void KeyLeft(void) {};
    virtual void KeyRight(void) {};
    void DrawScrollbar(void);
    virtual void Action(void) {};
};

class cEPGView : public cView {
protected:
    std::vector<std::string> epgPics;
    int numEPGPics;
    int numTabs;
    void SetTabs(void);
    void CheckEPGImages(void);
    void DrawImages(void);
public:    
    cEPGView(void);
    virtual ~cEPGView(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

class cSeriesView : public cView {
protected:
    int seriesId;
    int episodeId;
    cSeries series;
    std::string tvdbInfo;
    void SetTabs(void);
    void CreateTVDBInfo(void);
    void DrawImages(void);
    int GetRandomPoster(void);
public:    
    cSeriesView(int seriesId, int episodeId);
    virtual ~cSeriesView(void);
    void LoadMedia(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

class cMovieView : public cView {
protected:
    int movieId;
    cMovie movie;
    std::string movieDBInfo;
    void SetTabs(void);
    void CreateMovieDBInfo(void);
    void DrawImages(void);
public:    
    cMovieView(int movieId);
    virtual ~cMovieView(void);
    void LoadMedia(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

#endif //__TVGUIDE_VIEW_H