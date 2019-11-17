#ifndef __TVGUIDE_CONFIG_H
#define __TVGUIDE_CONFIG_H

#include <vdr/themes.h>
#include <vdr/plugin.h>
#include "osdmanager.h"
#include "geometrymanager.h"
#include "fontmanager.h"
#include "imagecache.h"

enum {
    e12Hours,
    e24Hours
};

enum {
    eVertical,
    eHorizontal
};

enum {
    eNumJump,
    eGroupJump
};

enum {
    eStyleGraphical,
    eStyleBlendingMagick,
    eStyleBlendingDefault,
    eStyleFlat
};


enum eBlueKeyMode {
    eBlueKeySwitch = 0,
    eBlueKeyEPG,
    eBlueKeyFavorites
};

enum eInstRecFolderMode {
    eFolderRoot = 0,
    eFolderSelect,
    eFolderFixed
};

class cTvguideConfig {
    private:
        cString checkSlashAtEnd(std::string path);
    public:
        cTvguideConfig();
        ~cTvguideConfig();
        int debugImageLoading;
        int showMainMenuEntry;
        int replaceOriginalSchedule;
        int displayMode;
        int showTimeInGrid;
        int channelCols;
        int channelRows;
        int numGrids;
        int displayTime;
        int displayStatusHeader;
        int displayChannelGroups;
        int displayTimeBase;
        int headerHeightPercent;
        int channelGroupsPercent;
        int epgViewBorder;
        int scaleVideo;
        int decorateVideo;
        int timeLineWidthPercent;
        int timeLineHeightPercent;
        int displayChannelName;
        int channelHeaderWidthPercent;
        int channelHeaderHeightPercent;
        int footerHeightPercent;
        int stepMinutes;
        int bigStepHours;
        int hugeStepHours;
        int channelJumpMode;
        int jumpChannels;
        int blueKeyMode;
        int closeOnSwitch;
        int numkeyMode;
        int useRemoteTimers;
        int hideLastGroup;
        int hideChannelLogos;
        int logoWidthRatio;
        int logoHeightRatio;
        cString logoPath;
        int logoExtension;
        int hideEpgImages;
        int epgImageWidth;
        int epgImageHeight;
        int numAdditionalEPGPictures;
        int epgImageWidthLarge;
        int epgImageHeightLarge;
        int detailedViewScrollStep;
        cString epgImagePath;
        cString iconPath;
        cString logoPathDefault;
        cString iconPathDefault;
        cString epgImagePathDefault;
        int instRecFolderMode;
        std::string instRecFixedFolder;
        int favWhatsOnNow;
        int favWhatsOnNext;
        int favUseTime1;
        int favUseTime2;
        int favUseTime3;
        int favUseTime4;
        int favTime1;
        int favTime2;
        int favTime3;
        int favTime4;
        std::string descUser1;
        std::string descUser2;
        std::string descUser3;
        std::string descUser4;
        int favLimitChannels;
        int favStartChannel;
        int favStopChannel;
        int fontIndex;
        const char *fontNameDefault;
        int FontButtonDelta;
        int FontDetailViewDelta;
        int FontDetailViewSmallDelta;
        int FontDetailHeaderDelta;
        int FontMessageBoxDelta;
        int FontMessageBoxLargeDelta;
        int FontStatusHeaderDelta;
        int FontStatusHeaderLargeDelta;
        int FontChannelHeaderDelta;
        int FontChannelGroupsDelta;
        int FontGridDelta;
        int FontGridSmallDelta;
        int FontTimeLineWeekdayDelta;
        int FontTimeLineDateDelta;
        int FontTimeLineTimeDelta;
        int FontChannelHeaderHorizontalDelta;
        int FontChannelGroupsHorizontalDelta;
        int FontGridHorizontalDelta;
        int FontGridHorizontalSmallDelta;
        int FontTimeLineDateHorizontalDelta;
        int FontTimeLineTimeHorizontalDelta;
        int FontRecMenuItemDelta;
        int FontRecMenuItemSmallDelta;
        int FontRecMenuItemLargeDelta;
        int timeFormat;
        int useNopacityTheme;
        int themeIndex;
        int themeIndexCurrent;
        cString themeName;
        std::string nOpacityTheme;
        int style;
        int roundedCorners;
        int displayRerunsDetailEPGView;
        int numReruns;
        int useSubtitleRerun;
        int numLogosInitial;
        int numLogosMax;
        int limitLogoCache;
        bool logoPathSet;
        bool imagesPathSet;
        bool iconsPathSet;
        bool LoadTheme();
        void SetStyle(void);
        void setDynamicValues(void);
        void SetLogoPath(cString path);
        void SetImagesPath(cString path);
        void SetIconsPath(cString path);
        void SetDefaultPathes(void);
        bool SetupParse(const char *Name, const char *Value);
};

#ifdef DEFINE_CONFIG
    cTvguideConfig tvguideConfig;
    cOsdManager osdManager;
    cGeometryManager geoManager;
    cFontManager fontManager;
    cImageCache imgCache;
    cTheme theme;
    cPlugin* pRemoteTimers = NULL;
#else
    extern cTvguideConfig tvguideConfig;
    extern cOsdManager osdManager;
    extern cGeometryManager geoManager;
    extern cFontManager fontManager;
    extern cImageCache imgCache;
    extern cTheme theme;
    extern cPlugin* pRemoteTimers;
#endif


// --- Theme -------------------------------------------------------------
//Style SETUP
#define CLR_STYLE_BLENDING_MAGICK   0xFFFFFFFF
#define CLR_STYLE_BLENDING_DEFAULT  0xAAAAAAAA
#define CLR_STYLE_GRAPHICAL         0x66666666
#define CLR_STYLE_FLAT              0x00000000

THEME_CLR(theme, clrStyle, CLR_STYLE_BLENDING_DEFAULT);
THEME_CLR(theme, clrBackgroundOSD, 0xB012273f);
THEME_CLR(theme, clrBackground, 0xFF12273f);
THEME_CLR(theme, clrGrid1, 0x00000000);
THEME_CLR(theme, clrGrid1Blending, 0x00000000);
THEME_CLR(theme, clrGrid2, 0x00000000);
THEME_CLR(theme, clrGrid2Blending, 0x00000000);
THEME_CLR(theme, clrHighlight, 0xAA3A3A55);
THEME_CLR(theme, clrHighlightBlending, 0xDD000000);
THEME_CLR(theme, clrGridFontBack, clrTransparent);
THEME_CLR(theme, clrGridActiveFontBack, 0xFFAFD533);
THEME_CLR(theme, clrFont, clrWhite);
THEME_CLR(theme, clrFontActive, 0xFF363636);
THEME_CLR(theme, clrFontHeader, 0xFF363636);
THEME_CLR(theme, clrFontButtons, clrWhite);
THEME_CLR(theme, clrStatusHeader, 0x00000000);
THEME_CLR(theme, clrStatusHeaderBlending, 0x00000000);
THEME_CLR(theme, clrHeader, 0x00000000);
THEME_CLR(theme, clrHeaderBlending, 0x00000000);
THEME_CLR(theme, clrBorder, 0x00000000);
THEME_CLR(theme, clrTimeline1, clrWhite);
THEME_CLR(theme, clrTimeline1Blending, 0xFF828282);
THEME_CLR(theme, clrTimeline2, clrBlack);
THEME_CLR(theme, clrTimeline2Blending, 0xFF3F3F3F);
THEME_CLR(theme, clrTimeBase, 0xA0FF0000);
THEME_CLR(theme, clrTabInactive, 0xA01F3D7A);
THEME_CLR(theme, clrButtonRed, 0xFFFF0000);
THEME_CLR(theme, clrButtonRedBorder, 0x00000000);
THEME_CLR(theme, clrButtonGreen, 0x00000000);
THEME_CLR(theme, clrButtonGreenBorder, 0x00000000);
THEME_CLR(theme, clrButtonYellow, 0x00000000);
THEME_CLR(theme, clrButtonYellowBorder, 0x00000000);
THEME_CLR(theme, clrButtonBlue, 0x00000000);
THEME_CLR(theme, clrButtonBlueBorder, 0x00000000);
THEME_CLR(theme, clrButtonBlend, 0xDD000000);
THEME_CLR(theme, clrRecMenuBackground, 0xAA000000);
THEME_CLR(theme, clrRecMenuTimerConflictBackground, 0xFFCCCCCC);
THEME_CLR(theme, clrRecMenuTimerConflictBar, 0xFF222222);
THEME_CLR(theme, clrRecMenuTimerConflictOverlap, 0xAAFF0000);
THEME_CLR(theme, clrRecMenuDayActive, 0xFF00FF00);
THEME_CLR(theme, clrRecMenuDayInactive, 0xFFFF0000);
THEME_CLR(theme, clrRecMenuDayHighlight, 0x44FFFFFF);
THEME_CLR(theme, clrRecMenuTextBack, 0xFF000000);
THEME_CLR(theme, clrRecMenuTextActiveBack, 0xFF939376);
THEME_CLR(theme, clrRecMenuKeyboardBack, 0xFF000000);
THEME_CLR(theme, clrRecMenuKeyboardBorder, clrWhite);
THEME_CLR(theme, clrRecMenuKeyboardHigh, 0x40BB0000);
THEME_CLR(theme, clrButtonRedKeyboard, 0xFFBB0000);
THEME_CLR(theme, clrButtonGreenKeyboard, 0xFF00BB00);
THEME_CLR(theme, clrButtonYellowKeyboard, 0xFFBBBB00);
THEME_CLR(theme, clrRecMenuTimelineTimer, 0xB012273f);
THEME_CLR(theme, clrRecMenuTimelineBack, 0xFF828282);
THEME_CLR(theme, clrRecMenuTimelineActive, 0xFF3F3F3F);
THEME_CLR(theme, clrRecMenuTimelineConflict, 0x30FF0000);
THEME_CLR(theme, clrRecMenuTimelineConflictOverlap, 0x90FF0000);
#endif //__TVGUIDE_CONFIG_H