#include <string>
#include "config.h"

cTvguideConfig::cTvguideConfig() {
    debugImageLoading = 0;
    showMainMenuEntry = 1;
    replaceOriginalSchedule = 0;
    displayMode = eHorizontal;
    showTimeInGrid = 1;
    channelCols = 5;
    channelRows = 10;
    displayTime = 160;
    displayStatusHeader = 1;
    displayChannelGroups = 1;
    displayTimeBase = 1;
    headerHeightPercent = 20;
    channelGroupsPercent = 5;
    epgViewBorder = 50;
    scaleVideo = 1;
    decorateVideo = 1;
    timeLineWidthPercent = 8;
    timeLineHeightPercent = 4;
    displayChannelName = 1;
    channelHeaderWidthPercent = 20;
    channelHeaderHeightPercent = 15;
    footerHeightPercent = 7;
    stepMinutes = 30;
    bigStepHours = 3;
    hugeStepHours = 24;
    channelJumpMode = eNumJump;
    jumpChannels = 0;
    blueKeyMode = 2;
    closeOnSwitch = 1;
    numkeyMode = 0;
    useRemoteTimers = 0;
    hideLastGroup = 0;
    hideChannelLogos = 0;
    logoWidthRatio = 13;
    logoHeightRatio = 10;
    logoExtension = 0;
    hideEpgImages = 0;
    epgImageWidth = 315;
    epgImageHeight = 240;
    numAdditionalEPGPictures = 9;
    epgImageWidthLarge = 525;
    epgImageHeightLarge = 400;
    detailedViewScrollStep = 5;
    instRecFolderMode = eFolderRoot;
    instRecFixedFolder = "";
    favWhatsOnNow = 1;
    favWhatsOnNext = 1;
    favUseTime1 = 0;
    favUseTime2 = 0;
    favUseTime3 = 0;
    favUseTime4 = 0;
    favTime1 = 0;
    favTime2 = 0;
    favTime3 = 0;
    favTime4 = 0;
    descUser1 = "";
    descUser2 = "";
    descUser3 = "";
    descUser4 = "";
    favLimitChannels = 0;
    favStartChannel = 0;
    favStopChannel = 0;
    fontIndex = 0;
    fontNameDefault = "VDRSymbols Sans:Book";
    FontButtonDelta = 0;
    FontDetailViewDelta = 0;
    FontDetailViewSmallDelta = 0;
    FontDetailHeaderDelta = 0;
    FontMessageBoxDelta = 0;
    FontMessageBoxLargeDelta = 0;
    FontStatusHeaderDelta = 0;
    FontStatusHeaderLargeDelta = 0;
    FontChannelHeaderDelta = 0;
    FontChannelGroupsDelta = 0;
    FontGridDelta = 0;
    FontGridSmallDelta = 0;
    FontTimeLineWeekdayDelta = 0;
    FontTimeLineDateDelta = 0;
    FontTimeLineTimeDelta = 0;
    FontChannelHeaderHorizontalDelta = 0;
    FontChannelGroupsHorizontalDelta = 0;
    FontGridHorizontalDelta = 0;
    FontGridHorizontalSmallDelta = 0;
    FontTimeLineDateHorizontalDelta = 0;
    FontTimeLineTimeHorizontalDelta = 0;
    FontRecMenuItemDelta = 0;
    FontRecMenuItemSmallDelta = 0;
    FontRecMenuItemLargeDelta = 0;
    timeFormat = 1;
    useNopacityTheme = 1;
    themeIndex = -1;
    themeIndexCurrent = -1;
    themeName = "";
    nOpacityTheme = "";
    style = eStyleBlendingDefault;
    roundedCorners = 0;
    displayRerunsDetailEPGView = 1;
    numReruns = 5;
    useSubtitleRerun = 1;
    numLogosInitial = 30;
    numLogosMax = 50;
    limitLogoCache = 1;
    logoPathSet = false;
    imagesPathSet = false;
    iconsPathSet = false;
}

cTvguideConfig::~cTvguideConfig() {
}

void cTvguideConfig::setDynamicValues() {
    numGrids = (displayMode == eVertical)?channelCols:channelRows;
    jumpChannels = numGrids;
}

bool cTvguideConfig::LoadTheme() {
    //is correct theme already loaded?
    if (nOpacityTheme.size() == 0)
        nOpacityTheme = Setup.OSDTheme;
    if ((themeIndex > -1) && (themeIndex == themeIndexCurrent)) {
        if (!nOpacityTheme.compare(Setup.OSDTheme)) {
            return false;
        } else {
            nOpacityTheme = Setup.OSDTheme;
        }
    }
    //Load available Themes
    cThemes themes;
    themes.Load(*cString("tvguide"));
    int numThemesAvailable = themes.NumThemes();

    //Check for nOpacity Theme
    if (useNopacityTheme) {
        std::string nOpacity = "nOpacity";
        std::string currentSkin = Setup.OSDSkin;
        std::string currentSkinTheme = Setup.OSDTheme;
        if (!currentSkin.compare(nOpacity)) {
            for (int curThemeIndex = 0; curThemeIndex < numThemesAvailable; curThemeIndex++) {
                std::string curThemeName = themes.Name(curThemeIndex);
                if (!curThemeName.compare(currentSkinTheme)) {
                    themeIndex = curThemeIndex;
                    break;
                }
            }
        }
    }

    if (themeIndex == -1) {
        for (int curThemeIndex = 0; curThemeIndex < numThemesAvailable; curThemeIndex++) {
            std::string curThemeName = themes.Name(curThemeIndex);
            if (!curThemeName.compare("default")) {
                themeIndex = curThemeIndex;
                break;
            }
        }

    }

    if (themeIndex == -1)
        themeIndex = 0;

    themeIndexCurrent = themeIndex;

    const char *themePath = themes.FileName(themeIndex);
    if (access(themePath, F_OK) == 0) {
        ::theme.Load(themePath);
        themeName = themes.Name(themeIndex);        
    }
    esyslog("tvguide: set Theme to %s", *themeName);
    return true;
}


void cTvguideConfig::SetStyle(void) {
    if (theme.Color(clrStyle) == CLR_STYLE_FLAT) {
        style = eStyleFlat;
        esyslog("tvguide: set flat style");
    } else if (theme.Color(clrStyle) == CLR_STYLE_BLENDING_DEFAULT) {
        style = eStyleBlendingDefault;
        esyslog("tvguide: set blending style");
    } else if (theme.Color(clrStyle) == CLR_STYLE_GRAPHICAL) {
        style = eStyleGraphical;
        esyslog("tvguide: set graphical style");
    } else {
        style = eStyleBlendingMagick;
        esyslog("tvguide: set magick blending style");
    }
    
}

void cTvguideConfig::SetLogoPath(cString path) {
    logoPath = checkSlashAtEnd(*path);
    logoPathSet = true;
    esyslog("tvguide: Logo Path set to %s", *logoPath);
}

void cTvguideConfig::SetImagesPath(cString path) {
    epgImagePath = checkSlashAtEnd(*path);
    imagesPathSet = true;
    esyslog("tvguide: EPG Image Path set to %s", *epgImagePath);
}

void cTvguideConfig::SetIconsPath(cString path) {
    iconPath = checkSlashAtEnd(*path);
    iconsPathSet = true;
    esyslog("tvguide: Icon Path set to %s", *iconPath);
}

void cTvguideConfig::SetDefaultPathes(void) {
    logoPathDefault = cString::sprintf("%s/logos/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    iconPathDefault = cString::sprintf("%s/icons/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    epgImagePathDefault = cString::sprintf("%s/epgimages/", cPlugin::CacheDirectory(PLUGIN_NAME_I18N));

    if (!logoPathSet) {
        logoPath = logoPathDefault;
    }
    if (!imagesPathSet) {
        epgImagePath = epgImagePathDefault;
    }
    if (!iconsPathSet) {
        iconPath = iconPathDefault;
    }
}

cString cTvguideConfig::checkSlashAtEnd(std::string path) {
    try {
        if (!(path.at(path.size()-1) == '/'))
            return cString::sprintf("%s/", path.c_str());
    } catch (...) {return path.c_str();}
    return path.c_str();
}

bool cTvguideConfig::SetupParse(const char *Name, const char *Value) {
    if      (strcmp(Name, "timeFormat") == 0)               timeFormat = atoi(Value);
    else if (strcmp(Name, "debugImageLoading") == 0)        debugImageLoading = atoi(Value);
    else if (strcmp(Name, "showMainMenuEntry") == 0)        showMainMenuEntry = atoi(Value);
    else if (strcmp(Name, "replaceOriginalSchedule") == 0)  replaceOriginalSchedule = atoi(Value);
    else if (strcmp(Name, "useNopacityTheme") == 0)         useNopacityTheme = atoi(Value);   
    else if (strcmp(Name, "themeIndex") == 0)               themeIndex = atoi(Value);
    else if (strcmp(Name, "displayMode") == 0)              displayMode = atoi(Value);
    else if (strcmp(Name, "showTimeInGrid") == 0)           showTimeInGrid = atoi(Value);
    else if (strcmp(Name, "displayStatusHeader") == 0)      displayStatusHeader = atoi(Value);
    else if (strcmp(Name, "displayChannelGroups") == 0)     displayChannelGroups = atoi(Value);
    else if (strcmp(Name, "displayTimeBase") == 0)          displayTimeBase = atoi(Value);
    else if (strcmp(Name, "headerHeightPercent") == 0)      headerHeightPercent = atoi(Value);
    else if (strcmp(Name, "channelGroupsPercent") == 0)     channelGroupsPercent = atoi(Value);
    else if (strcmp(Name, "epgViewBorder") == 0)            epgViewBorder = atoi(Value);
    else if (strcmp(Name, "scaleVideo") == 0)               scaleVideo = atoi(Value);
    else if (strcmp(Name, "decorateVideo") == 0)            decorateVideo = atoi(Value);
    else if (strcmp(Name, "roundedCorners") == 0)           roundedCorners = atoi(Value);
    else if (strcmp(Name, "channelCols") == 0)              channelCols = atoi(Value);
    else if (strcmp(Name, "channelRows") == 0)              channelRows = atoi(Value);
    else if (strcmp(Name, "displayTime") == 0)              displayTime = atoi(Value);
    else if (strcmp(Name, "hideChannelLogos") == 0)         hideChannelLogos = atoi(Value);
    else if (strcmp(Name, "logoExtension") == 0)            logoExtension = atoi(Value);
    else if (strcmp(Name, "logoWidthRatio") == 0)           logoWidthRatio = atoi(Value);
    else if (strcmp(Name, "logoHeightRatio") == 0)          logoHeightRatio = atoi(Value);
    else if (strcmp(Name, "bigStepHours") == 0)             bigStepHours = atoi(Value);
    else if (strcmp(Name, "hugeStepHours") == 0)            hugeStepHours = atoi(Value);
    else if (strcmp(Name, "channelJumpMode") == 0)          channelJumpMode = atoi(Value);
    else if (strcmp(Name, "blueKeyMode") == 0)              blueKeyMode = atoi(Value);
    else if (strcmp(Name, "numkeyMode") == 0)               numkeyMode = atoi(Value);
    else if (strcmp(Name, "closeOnSwitch") == 0)            closeOnSwitch = atoi(Value);
    else if (strcmp(Name, "useRemoteTimers") == 0)          useRemoteTimers = atoi(Value);
    else if (strcmp(Name, "hideLastGroup") == 0)            hideLastGroup = atoi(Value);
    else if (strcmp(Name, "hideEpgImages") == 0)            hideEpgImages = atoi(Value);
    else if (strcmp(Name, "epgImageWidth") == 0)            epgImageWidth = atoi(Value);
    else if (strcmp(Name, "epgImageHeight") == 0)           epgImageHeight = atoi(Value);
    else if (strcmp(Name, "numAdditionalEPGPictures") == 0) numAdditionalEPGPictures = atoi(Value);
    else if (strcmp(Name, "epgImageWidthLarge") == 0)       epgImageWidthLarge = atoi(Value);
    else if (strcmp(Name, "epgImageHeightLarge") == 0)      epgImageHeightLarge = atoi(Value);
    else if (strcmp(Name, "detailedViewScrollStep") == 0)   detailedViewScrollStep = atoi(Value);
    else if (strcmp(Name, "timeLineWidthPercent") == 0)     timeLineWidthPercent = atoi(Value);
    else if (strcmp(Name, "timeLineHeightPercent") == 0)    timeLineHeightPercent = atoi(Value);
    else if (strcmp(Name, "displayChannelName") == 0)       displayChannelName = atoi(Value);
    else if (strcmp(Name, "channelHeaderWidthPercent") == 0)  channelHeaderWidthPercent = atoi(Value);
    else if (strcmp(Name, "channelHeaderHeightPercent") == 0) channelHeaderHeightPercent = atoi(Value);
    else if (strcmp(Name, "footerHeightPercent") == 0)      footerHeightPercent = atoi(Value);
    else if (strcmp(Name, "instRecFolderMode") == 0)        instRecFolderMode = atoi(Value);
    else if (strcmp(Name, "instRecFixedFolder") == 0)       instRecFixedFolder = Value;
    else if (strcmp(Name, "favWhatsOnNow") == 0)            favWhatsOnNow = atoi(Value);
    else if (strcmp(Name, "favWhatsOnNext") == 0)           favWhatsOnNext = atoi(Value);
    else if (strcmp(Name, "favUseTime1") == 0)              favUseTime1 = atoi(Value);
    else if (strcmp(Name, "favUseTime2") == 0)              favUseTime2 = atoi(Value);
    else if (strcmp(Name, "favUseTime3") == 0)              favUseTime3 = atoi(Value);
    else if (strcmp(Name, "favUseTime4") == 0)              favUseTime4 = atoi(Value);
    else if (strcmp(Name, "favTime1") == 0)                 favTime1 = atoi(Value);
    else if (strcmp(Name, "favTime2") == 0)                 favTime2 = atoi(Value);
    else if (strcmp(Name, "favTime3") == 0)                 favTime3 = atoi(Value);
    else if (strcmp(Name, "favTime4") == 0)                 favTime4 = atoi(Value);
    else if (strcmp(Name, "descUser1") == 0)                descUser1 = Value;
    else if (strcmp(Name, "descUser2") == 0)                descUser2 = Value;
    else if (strcmp(Name, "descUser3") == 0)                descUser3 = Value;
    else if (strcmp(Name, "descUser4") == 0)                descUser4 = Value;
    else if (strcmp(Name, "favLimitChannels") == 0)         favLimitChannels = atoi(Value);
    else if (strcmp(Name, "favStartChannel") == 0)          favStartChannel = atoi(Value);
    else if (strcmp(Name, "favStopChannel") == 0)           favStopChannel = atoi(Value);
    else if (strcmp(Name, "fontIndex") == 0)                fontIndex = atoi(Value);
    else if (strcmp(Name, "FontButtonDelta") == 0)          FontButtonDelta = atoi(Value);
    else if (strcmp(Name, "FontDetailViewDelta") == 0)      FontDetailViewDelta = atoi(Value);
    else if (strcmp(Name, "FontDetailHeaderDelta") == 0)    FontDetailHeaderDelta = atoi(Value);
    else if (strcmp(Name, "FontMessageBoxDelta") == 0)      FontMessageBoxDelta = atoi(Value);  
    else if (strcmp(Name, "FontMessageBoxLargeDelta") == 0) FontMessageBoxLargeDelta = atoi(Value); 
    else if (strcmp(Name, "FontStatusHeaderDelta") == 0)    FontStatusHeaderDelta = atoi(Value);    
    else if (strcmp(Name, "FontStatusHeaderLargeDelta") == 0) FontStatusHeaderLargeDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelHeaderDelta") == 0)   FontChannelHeaderDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelGroupsDelta") == 0)   FontChannelGroupsDelta = atoi(Value);
    else if (strcmp(Name, "FontGridDelta") == 0)            FontGridDelta = atoi(Value);    
    else if (strcmp(Name, "FontGridSmallDelta") == 0)       FontGridSmallDelta = atoi(Value);   
    else if (strcmp(Name, "FontTimeLineWeekdayDelta") == 0) FontTimeLineWeekdayDelta = atoi(Value); 
    else if (strcmp(Name, "FontTimeLineDateDelta") == 0)    FontTimeLineDateDelta = atoi(Value);    
    else if (strcmp(Name, "FontTimeLineTimeDelta") == 0)    FontTimeLineTimeDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelHeaderHorizontalDelta") == 0) FontChannelHeaderHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelGroupsHorizontalDelta") == 0) FontChannelGroupsHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontGridHorizontalDelta") == 0)  FontGridHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontGridHorizontalSmallDelta") == 0) FontGridHorizontalSmallDelta = atoi(Value);
    else if (strcmp(Name, "FontTimeLineDateHorizontalDelta") == 0) FontTimeLineDateHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontTimeLineTimeHorizontalDelta") == 0) FontTimeLineTimeHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontRecMenuItemDelta") == 0)     FontRecMenuItemDelta = atoi(Value);
    else if (strcmp(Name, "FontRecMenuItemSmallDelta") == 0) FontRecMenuItemSmallDelta = atoi(Value);
    else if (strcmp(Name, "FontRecMenuItemLargeDelta") == 0) FontRecMenuItemLargeDelta = atoi(Value);
    else if (strcmp(Name, "displayRerunsDetailEPGView") == 0) displayRerunsDetailEPGView = atoi(Value);
    else if (strcmp(Name, "numReruns") == 0)                numReruns = atoi(Value);
    else if (strcmp(Name, "useSubtitleRerun") == 0)         useSubtitleRerun = atoi(Value);
    else if (strcmp(Name, "numLogosInitial") == 0)         numLogosInitial = atoi(Value);
    else if (strcmp(Name, "numLogosMax") == 0)             numLogosMax = atoi(Value);
    else if (strcmp(Name, "limitLogoCache") == 0)          limitLogoCache = atoi(Value);
    else return false;
    return true;
}