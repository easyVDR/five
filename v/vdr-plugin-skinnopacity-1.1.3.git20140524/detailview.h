#ifndef __NOPACITY_DETAILVIEW_H
#define __NOPACITY_DETAILVIEW_H

#include <vector>
#include <string>
#include <sstream>
#include <vdr/skins.h>
#include "services/scraper2vdr.h"
#include "services/epgsearch.h"
#include "config.h"
#include "imagecache.h"
#include "imageloader.h"
#include "helpers.h"

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

class cNopacityView : public cThread {
protected:
    cOsd *osd;
    cImageCache *imgCache;
    cPixmap *pixmapHeader;
    cPixmap *pixmapHeaderLogo;
    cPixmap *pixmapContent;
    cPixmap *pixmapContentBack;
    cPixmap *pixmapScrollbar;
    cPixmap *pixmapScrollbarBack;
    cPixmap *pixmapTabs;
    cFont *font, *fontSmall, *fontHeader, *fontHeaderLarge;
    int activeView;
    bool scrollable;
    bool tabbed;
    int x, y;
    int width, height;
    int border;
    int headerHeight, contentHeight, tabHeight;
    std::vector<std::string> tabs;
    std::string title;
    std::string subTitle;
    std::string dateTime;
    std::string infoText;
    std::string addInfoText;
    const cChannel *channel;
    int eventID;
    std::string recFileName;
    bool headerDrawn;
    void DrawHeader(void);
    void ClearContent(void);
    void CreateContent(int fullHeight);
    void DrawContent(std::string *text);
    void DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2 = NULL);
    void CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg);
    void DrawActors(std::vector<cActor> *actors);
    void ClearScrollbar(void);
    virtual void SetTabs(void) {};
    void DrawTabs(void);
public:    
    cNopacityView(cOsd *osd, cImageCache *imgCache);
    virtual ~cNopacityView(void);
    void SetTitle(const char *t) { title = t ? t : ""; };
    void SetSubTitle(const char *s) { subTitle = s ? s : ""; };
    void SetDateTime(const char *dt) { dateTime = dt; };
    void SetInfoText(const char *i) { infoText = i ? i : ""; };
    void SetAdditionalInfoText(std::string addInfo) { addInfoText = addInfo; };
    void SetChannel(const cChannel *c) { channel = c; };
    void SetEventID(int id) { eventID = id; };
    void SetRecFileName(const char *name) { recFileName = name ? name : ""; };
    virtual void LoadMedia(void) {};
    void SetGeometry(int x, int y, int width, int height, int border, int headerHeight);
    void SetFonts(void);
    void SetScrollbarPixmaps(cPixmap *s, cPixmap *sb) { pixmapScrollbar = s; pixmapScrollbarBack = sb; };
    virtual bool KeyUp(void);
    virtual bool KeyDown(void);
    virtual void KeyLeft(void) {};
    virtual void KeyRight(void) {};
    void DrawScrollbar(void);
    virtual void Action(void) {};
};

class cNopacityEPGView : public cNopacityView {
protected:
    cPixmap *pixmapHeaderEPGImage;
    std::vector<std::string> epgPics;
    int numEPGPics;
    int numTabs;
    void SetTabs(void);
    void DrawHeaderEPGImage(void);
    void CheckEPGImages(void);
    void DrawImages(void);
public:    
    cNopacityEPGView(cOsd *osd, cImageCache *imgCache);
    virtual ~cNopacityEPGView(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

class cNopacitySeriesView : public cNopacityView {
protected:
    cPixmap *pixmapHeaderBanner;
    int seriesId;
    int episodeId;
    cSeries series;
    std::string tvdbInfo;
    void SetTabs(void);
    void CreateTVDBInfo(void);
    void DrawHeaderBanner(void);
    void DrawImages(void);
    int GetRandomPoster(void);
public:    
    cNopacitySeriesView(cOsd *osd, cImageCache *imgCache, int seriesId, int episodeId);
    virtual ~cNopacitySeriesView(void);
    void LoadMedia(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

class cNopacityMovieView : public cNopacityView {
protected:
    cPixmap *pixmapHeaderPoster;
    int movieId;
    cMovie movie;
    std::string movieDBInfo;
    void SetTabs(void);
    void CreateMovieDBInfo(void);
    void DrawHeaderPoster(void);
    void DrawImages(void);
public:    
    cNopacityMovieView(cOsd *osd, cImageCache *imgCache, int movieId);
    virtual ~cNopacityMovieView(void);
    void LoadMedia(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

class cNopacityTextView : public cNopacityView {
protected:
public:    
    cNopacityTextView(cOsd *osd, cImageCache *imgCache);
    virtual ~cNopacityTextView(void);    
    void KeyLeft(void);
    void KeyRight(void);
    void Action(void);
};

#endif //__NOPACITY_DETAILVIEW_H