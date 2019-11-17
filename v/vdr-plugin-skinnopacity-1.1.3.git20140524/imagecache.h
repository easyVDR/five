#ifndef __NOPACITY_IMAGECACHE_H
#define __NOPACITY_IMAGECACHE_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>
#include <vector>
#include "imagemagickwrapper.h"

using namespace Magick;

enum eCacheType {
    ctMenuIcon = 0,
    ctSkinIcon,
    ctLogo,
    ctLogoMenuItem,
    ctLogoTimer,
    ctSkinElement,
};

enum eSkinElementType {
    seNone = -1,
    seDefault = 0,
    seDefaultHigh,
    seMain,
    seMainHigh,
    seMainTop,
    seSchedules,
    seSchedulesHigh,
    seSchedulesTop,
    seChannels,
    seChannelsHigh,
    seChannelsTop,
    seRecordings,
    seRecordingsHigh,
    seRecordingsTop,
    seTimers,
    seTimersHigh,
    seTimersTop,
    seSetup,
    seSetupHigh,
    seSetupTop,
    seTracks,
    seTracksHigh,
    seTracksTop,
    seButtonRed,
    seButtonGreen,
    seButtonYellow,
    seButtonBlue,
    seMenuHeader,
    seMenuHeaderTop,
    seChannelBackground,
    seChannelTop,
    seChannelLogoBack,
    seReplayBackground,
    seReplayTop,
    seMessageStatus,
    seMessageInfo,
    seMessageWarning,
    seMessageError,
    seMessageMenuStatus,
    seMessageMenuInfo,
    seMessageMenuWarning,
    seMessageMenuError,
    seVolumeBackground,
    seScrollbar,
};

struct sImgProperties {
    int width;
    int height;
    bool preserveAspect;
};

class cImageCache : public cImageMagickWrapper {
public:
    cImageCache();
    ~cImageCache();
    void CreateCache(void);
    void CreateCacheDelayed(void);
    void Reload(void);
    bool ThemeChanged(void);
    cImage *GetMenuIcon(std::string name);
    cImage *GetSkinIcon(std::string name, int width=0, int height=0, bool preserveAspect = true);
    cImage *GetSkinElement(eSkinElementType type);
    cImage *GetLogo(eCacheType type, const cChannel *channel);
    cImage GetBackground(tColor back, tColor blend, int width, int height, bool flip = false);
    std::string GetCacheSize(eCacheType type);
private:
    cImage *tempStaticLogo;
    std::string osdTheme;
    std::map<std::string, cImage*> menuIconCache;
    std::map<std::string, cImage*> skinIconCache;
    std::map<eSkinElementType, cImage*> skinElementCache;
    std::map<std::string, cImage*> logoCache;
    std::map<std::string, cImage*> logoMenuItemCache;
    std::map<std::string, cImage*> logoTimerCache;
    std::vector<std::pair<std::string, cPoint> > GetMenuIcons(void);
    std::vector<std::pair<std::string, sImgProperties> > GetSkinIcons(void);
    bool LoadIcon(eCacheType type, std::string name);
    void InsertIntoIconCache(eCacheType type, std::string name, int width, int height, bool preserveAspect = true);
    bool LoadLogo(const cChannel *channel);
    void InsertIntoLogoCache(eCacheType type, std::string channelID);
    cPoint LogoSize(eCacheType type);
    void CreateSkinElementsBlended(void);
    void CreateSkinElementsGraphics(void);
    void InsertIntoSkinElementCache(eSkinElementType type, int width=0, int height=0);
    void Clear(void);
};

#endif //__NOPACITY_IMAGECACHE_H
