#ifndef __TVGUIDE_IMAGECACHE_H
#define __TVGUIDE_IMAGECACHE_H

#define X_DISPLAY_MISSING

#include <map>
#include <vector>
#include "imagemagickwrapper.h"

enum eCacheType {
    ctOsdElement = 0,
    ctGrid,
    ctLogo,
    ctChannelGroup,
    ctIcon,
};

enum eOsdElementType {
    oeNone = -1,
    oeStatusHeaderContentFull,
    oeStatusHeaderContentWindowed,
    oeStatusHeaderTVFrame,
    oeButtonRed,
    oeButtonGreen,
    oeButtonYellow,
    oeButtonBlue,
    oeLogoBack,
    oeTimeline1,
    oeTimeline2,
    oeDateViewer,
    oeClock,
    oeChannelJump,
};

class cImageCache : public cImageMagickWrapper {
public:
    cImageCache();
    ~cImageCache();
    void CreateCache(void);
    cImage *GetOsdElement(eOsdElementType type);
    cImage *GetGrid(int width, int height, bool active);
    cImage *GetChannelGroup(int width, int height);
    cImage *GetLogo(const cChannel *channel);
    cImage *GetIcon(std::string name, int width, int height);
    std::string GetCacheSize(eCacheType type);
    void Clear(void);
private:
    cImage *tempStaticLogo;
    Image bufferGrid;
    Image bufferGridActive;
    bool gridsAvailable;
    int cornerWidth;
    int cornerHeight;
    cImage *imgLeft;
    cImage *imgLeftActive;
    cImage *imgRight;
    cImage *imgRightActive;
    cImage *imgHead;
    cImage *imgHeadActive;
    cImage *imgBottom;
    cImage *imgBottomActive;
    cImage *imgChannelgroupHead;
    cImage *imgChannelgroupBottom;
    cImage *groupsHead;
    cImage *groupsBottom;
    cImage *groupsLeft;
    cImage *groupsRight;
	std::map<eOsdElementType, cImage*> osdElementCache;
    std::map<std::string, cImage*> gridCache;
    std::map<std::string, cImage*> groupsCache;
    std::map<std::string, cImage*> logoCache;
    std::map<std::string, cImage*> iconCache;
    void CreateOsdIconCache(void);
    void PrepareGridIconCache(void);
    void CreateGridIconCache(void);
    void CreateChannelGroupCache(void);
    void CreateLogoCache(void);
	bool LoadIcon(std::string name);
	void InsertIntoOsdElementCache(eOsdElementType type, int width=0, int height=0);
    void InsertIntoGridCache(std::string name, int width, int height, bool active);
    cImage *CreateGrid(int width, int height, bool active);
    void AddCornersHorizontal(cImage *img, bool active);
    void AddCornersVertical(cImage *img, bool active);
    void InsertIntoGroupsCacheHorizontal(int size);
    void InsertIntoGroupsCacheVertical(int size);
    void AddCornersGroupHorizontal(cImage *img);
    void AddCornersGroupVertical(cImage *img);
    bool LoadLogo(const cChannel *channel);
    void InsertIntoLogoCache(std::string channelID);
};

#endif //__TVGUIDE_IMAGECACHE_H