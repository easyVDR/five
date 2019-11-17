#include <string>
#include <vector>
#include <map>
#include "config.h"
#include "helpers.h"
#include <vdr/skins.h>

cNopacityConfig::cNopacityConfig() {
    logoPathSet = false;
    epgImagePathSet = false;
    iconPathSet = false;
    //Common
    mainMenuEntry = false;
    fontName = NULL;
    fontDefaultName = "VDRSymbols Sans:Book";
    logoExtension = "png";
    LoadDefaults();
}

cNopacityConfig::~cNopacityConfig() {
}

int cNopacityConfig::GetValue(std::string name) {
    std::map<std::string, int>::iterator hit = conf.find(name);
    if (hit != conf.end()) {
        return (int)hit->second;
    } else {
        dsyslog("nopacity: ERROR: access to unknown config value %s", name.c_str());
    }
    return 0;
}

int *cNopacityConfig::GetValueRef(std::string name) {
    int *ref = NULL;
    std::map<std::string, int>::iterator hit = conf.find(name);
    if (hit != conf.end()) {
        ref = &hit->second;
    } else {
        dsyslog("nopacity: ERROR: access to unknown config value %s", name.c_str());
    }
    return ref;
}

void cNopacityConfig::LoadDefaults(void) {
    conf.clear();
    //Common Values
    conf.insert(std::pair<std::string, int>("displayType", dtBlending));
    conf.insert(std::pair<std::string, int>("fontIndex", 0));
    conf.insert(std::pair<std::string, int>("debugImageLoading", 0));
    //DisplayMenu
    conf.insert(std::pair<std::string, int>("scrollMode", 0));
    conf.insert(std::pair<std::string, int>("spaceMenu", 5));
    conf.insert(std::pair<std::string, int>("widthScrollbar", 20));
    conf.insert(std::pair<std::string, int>("menuAdjustLeft", 1));
    conf.insert(std::pair<std::string, int>("scalePicture", 1));
    conf.insert(std::pair<std::string, int>("roundedCorners", 1));
    conf.insert(std::pair<std::string, int>("cornerRadius", 12));
    conf.insert(std::pair<std::string, int>("useMenuIcons", 1));
    conf.insert(std::pair<std::string, int>("mainMenuTitleStyle", 0));
    conf.insert(std::pair<std::string, int>("narrowMainMenu", 1));
    conf.insert(std::pair<std::string, int>("narrowScheduleMenu", 1));
    conf.insert(std::pair<std::string, int>("narrowChannelMenu", 1));
    conf.insert(std::pair<std::string, int>("narrowTimerMenu", 1));
    conf.insert(std::pair<std::string, int>("narrowRecordingMenu", 1));
    conf.insert(std::pair<std::string, int>("narrowSetupMenu", 1));
    conf.insert(std::pair<std::string, int>("numReruns", 5));
    conf.insert(std::pair<std::string, int>("useSubtitleRerun", 1));
    conf.insert(std::pair<std::string, int>("menuChannelDisplayMode", 0));
    conf.insert(std::pair<std::string, int>("menuChannelDisplayTime", 1));
    conf.insert(std::pair<std::string, int>("numEPGEntriesChannelsMenu", 15));
    conf.insert(std::pair<std::string, int>("menuFadeTime", 0));
    conf.insert(std::pair<std::string, int>("menuEPGWindowFadeTime", 300));
    conf.insert(std::pair<std::string, int>("menuWidthMain", 30));
    conf.insert(std::pair<std::string, int>("menuWidthSchedules", 30));
    conf.insert(std::pair<std::string, int>("menuWidthChannels", 30));
    conf.insert(std::pair<std::string, int>("menuWidthTimers", 30));
    conf.insert(std::pair<std::string, int>("menuWidthRecordings", 30));
    conf.insert(std::pair<std::string, int>("menuWidthSetup", 30));
    conf.insert(std::pair<std::string, int>("menuHeightInfoWindow", 20));
    conf.insert(std::pair<std::string, int>("menuScrollDelay", 1));
    conf.insert(std::pair<std::string, int>("menuScrollSpeed", 2));
    conf.insert(std::pair<std::string, int>("menuInfoTextDelay", 2));
    conf.insert(std::pair<std::string, int>("menuInfoScrollDelay", 5));
    conf.insert(std::pair<std::string, int>("menuInfoScrollSpeed", 2));
    conf.insert(std::pair<std::string, int>("menuWidthRightItems", 12));
    conf.insert(std::pair<std::string, int>("menuSizeDiskUsage", 12));
    conf.insert(std::pair<std::string, int>("showDiscUsage", 1));
    conf.insert(std::pair<std::string, int>("discUsageStyle", 0));
    conf.insert(std::pair<std::string, int>("showTimers", 1));
    conf.insert(std::pair<std::string, int>("numberTimers", 10));
    conf.insert(std::pair<std::string, int>("timersLogoWidth", 60));    
    conf.insert(std::pair<std::string, int>("checkTimerConflict", 1));
    conf.insert(std::pair<std::string, int>("headerHeight", 7));
    conf.insert(std::pair<std::string, int>("footerHeight", 7));
    conf.insert(std::pair<std::string, int>("numDefaultMenuItems", 16));
    conf.insert(std::pair<std::string, int>("numMainMenuItems", 10));
    conf.insert(std::pair<std::string, int>("numSchedulesMenuItems", 10));
    conf.insert(std::pair<std::string, int>("numRecordingsMenuItems", 10));
    conf.insert(std::pair<std::string, int>("menuHeaderLogoWidth", 160));
    conf.insert(std::pair<std::string, int>("menuHeaderLogoHeight", 70));
    conf.insert(std::pair<std::string, int>("epgImageWidth", 210));
    conf.insert(std::pair<std::string, int>("epgImageHeight", 160));
    conf.insert(std::pair<std::string, int>("epgImageWidthLarge", 525));
    conf.insert(std::pair<std::string, int>("epgImageHeightLarge", 400));
    conf.insert(std::pair<std::string, int>("posterWidth", 500));
    conf.insert(std::pair<std::string, int>("posterHeight", 750));
    conf.insert(std::pair<std::string, int>("useFolderPoster", 1));
    conf.insert(std::pair<std::string, int>("menuChannelLogoBackground", 1));
    conf.insert(std::pair<std::string, int>("borderDetailedEPG", 30));
    conf.insert(std::pair<std::string, int>("borderDetailedRecordings", 30));
    conf.insert(std::pair<std::string, int>("headerDetailedEPG", 20));
    conf.insert(std::pair<std::string, int>("headerDetailedRecordings", 20));
    conf.insert(std::pair<std::string, int>("detailedViewScrollStep", 1));
    conf.insert(std::pair<std::string, int>("menuSchedulesWindowMode", 1));
    conf.insert(std::pair<std::string, int>("menuRecordingsWindowMode", 1));
    conf.insert(std::pair<std::string, int>("menuTimersWindowMode", 1));
    conf.insert(std::pair<std::string, int>("fontHeader", 0));
    conf.insert(std::pair<std::string, int>("fontDate", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemLarge", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemSchedule", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemScheduleSmall", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemChannel", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemChannelSmall", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemRecordings", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemRecordingsSmall", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemTimers", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemTimersSmall", 0));
    conf.insert(std::pair<std::string, int>("fontMenuitemDefault", 0));
    conf.insert(std::pair<std::string, int>("fontDiskUsage", 0));
    conf.insert(std::pair<std::string, int>("fontDiskUsagePercent", 0));
    conf.insert(std::pair<std::string, int>("fontTimers", 0));
    conf.insert(std::pair<std::string, int>("fontTimersHead", 0));
    conf.insert(std::pair<std::string, int>("fontButtons", 0));
    conf.insert(std::pair<std::string, int>("fontMessageMenu", 0));
    conf.insert(std::pair<std::string, int>("fontDetailView", 0));
    conf.insert(std::pair<std::string, int>("fontDetailViewSmall", 0));
    conf.insert(std::pair<std::string, int>("fontDetailViewHeader", 0));
    conf.insert(std::pair<std::string, int>("fontDetailViewHeaderLarge", 0));
    conf.insert(std::pair<std::string, int>("fontEPGInfoWindow", 0));
    conf.insert(std::pair<std::string, int>("fontEPGInfoWindowLarge", 0));
    //DisplayChannel
    conf.insert(std::pair<std::string, int>("channelHeight", 25));
    conf.insert(std::pair<std::string, int>("channelBorderVertical", 15));
    conf.insert(std::pair<std::string, int>("channelBorderBottom", 15));
    conf.insert(std::pair<std::string, int>("channelFadeTime", 300));
    conf.insert(std::pair<std::string, int>("channelBackgroundTransparency", 0));
    conf.insert(std::pair<std::string, int>("channelUseLogoBackground", 1));
    conf.insert(std::pair<std::string, int>("logoPosition", 1));
    conf.insert(std::pair<std::string, int>("logoVerticalAlignment", 1));
    conf.insert(std::pair<std::string, int>("logoWidth", 260));
    conf.insert(std::pair<std::string, int>("logoHeight", 200));
    conf.insert(std::pair<std::string, int>("logoWidthOriginal", 260));
    conf.insert(std::pair<std::string, int>("logoHeightOriginal", 200));
    conf.insert(std::pair<std::string, int>("backgroundStyle", 0));
    conf.insert(std::pair<std::string, int>("roundedCornersChannel", 1));
    conf.insert(std::pair<std::string, int>("displaySignalStrength", 1));
    conf.insert(std::pair<std::string, int>("displaySourceInfo", 1));
    conf.insert(std::pair<std::string, int>("displayPrevNextChannelGroup", 1));
    conf.insert(std::pair<std::string, int>("fontChannelHeaderSize", 0));
    conf.insert(std::pair<std::string, int>("fontChannelDateSize", 0));
    conf.insert(std::pair<std::string, int>("fontEPGSize", 0));
    conf.insert(std::pair<std::string, int>("fontEPGSmallSize", 0));
    conf.insert(std::pair<std::string, int>("fontChannelSourceInfoSize", 0));
    conf.insert(std::pair<std::string, int>("fontChannelGroupSize", 0));
    conf.insert(std::pair<std::string, int>("fontChannelGroupSmallSize", 0));
    conf.insert(std::pair<std::string, int>("progressCurrentSchedule", 0));
    conf.insert(std::pair<std::string, int>("displayPoster", 1));
    conf.insert(std::pair<std::string, int>("channelPosterBorder", 10));
    //Display Replay
    conf.insert(std::pair<std::string, int>("replayHeight", 25));
    conf.insert(std::pair<std::string, int>("replayBorderVertical", 15));
    conf.insert(std::pair<std::string, int>("replayBorderBottom", 15));
    conf.insert(std::pair<std::string, int>("replayFadeTime", 300));
    conf.insert(std::pair<std::string, int>("fontReplayHeader", 0));
    conf.insert(std::pair<std::string, int>("fontReplay", 0));
    //DisplayMessage
    conf.insert(std::pair<std::string, int>("messageFadeTime", 300));
    conf.insert(std::pair<std::string, int>("messageWidth", 90));
    conf.insert(std::pair<std::string, int>("messageHeight", 10));
    conf.insert(std::pair<std::string, int>("messageBorderBottom", 10));
    conf.insert(std::pair<std::string, int>("fontMessage", 0));
    //DisplayTracks
    conf.insert(std::pair<std::string, int>("tracksFadeTime", 300));
    conf.insert(std::pair<std::string, int>("tracksWidth", 25));
    conf.insert(std::pair<std::string, int>("tracksItemHeight", 80));
    conf.insert(std::pair<std::string, int>("tracksPosition", 0));
    conf.insert(std::pair<std::string, int>("tracksBorderHorizontal", 10));
    conf.insert(std::pair<std::string, int>("tracksBorderVertical", 10));
    conf.insert(std::pair<std::string, int>("fontTracksHeader", 0));
    conf.insert(std::pair<std::string, int>("fontTracks", 0));
    //DisplayVolume
    conf.insert(std::pair<std::string, int>("volumeFadeTime", 300));
    conf.insert(std::pair<std::string, int>("volumeWidth", 40));
    conf.insert(std::pair<std::string, int>("volumeHeight", 10));
    conf.insert(std::pair<std::string, int>("volumeBorderBottom", 10));
    conf.insert(std::pair<std::string, int>("fontVolume", 0));
    //Channel Logo Caching
    conf.insert(std::pair<std::string, int>("limitLogoCache", 1));
    conf.insert(std::pair<std::string, int>("numLogosInitial", 30));
    conf.insert(std::pair<std::string, int>("numLogosMax", 50));
}

void cNopacityConfig::SetFontName() {
    if (fontName)
        free(fontName);
    if (GetValue("fontIndex") == 0) {
        fontName = strdup(fontDefaultName);
    } else {
        cStringList availableFonts;
        cFont::GetAvailableFontNames(&availableFonts);
        if (availableFonts[GetValue("fontIndex")-1]) {
            fontName = strdup(availableFonts[GetValue("fontIndex")-1]);
        } else
            fontName = strdup(fontDefaultName);
    }
}

void cNopacityConfig::SetPathes(void) {
    logoPathDefault = cString::sprintf("%s/logos/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    iconPathDefault = cString::sprintf("%s/icons/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    epgImagePathDefault = cString::sprintf("%s/epgimages/", cPlugin::CacheDirectory(PLUGIN_NAME_I18N));

    dsyslog("nopacity: using Logo Directory %s", (logoPathSet)?(*logoPath):(*logoPathDefault));
    dsyslog("nopacity: using Icon Directory %s", (iconPathSet)?(*iconPath):(*iconPathDefault));
    dsyslog("nopacity: using EPG Images Directory %s", (epgImagePathSet)?(*epgImagePath):(*epgImagePathDefault));
}

void cNopacityConfig::SetLogoPath(cString path) {
    logoPath = checkSlashAtEnd(*path);
    logoPathSet = true;
}

void cNopacityConfig::SetIconPath(cString path) {
    iconPath = checkSlashAtEnd(*path);
    iconPathSet = true;
}

void cNopacityConfig::SetEpgImagePath(cString path) {
    epgImagePath = checkSlashAtEnd(*path);
    epgImagePathSet = true;
}

cString cNopacityConfig::checkSlashAtEnd(std::string path) {
    try {
        if (!(path.at(path.size()-1) == '/'))
            return cString::sprintf("%s/", path.c_str());
    } catch (...) {return path.c_str();}
    return path.c_str();
}

void cNopacityConfig::LoadThemeSpecificConfigs(void) {
    cThemes themes;
    themes.Load("nOpacity");
    int numThemes = themes.NumThemes();
    dsyslog("nopacity: %d themes available", numThemes);
    for (int theme = 0; theme < numThemes; theme++) {
        cString confFile = cString::sprintf("%s/themeconfigs/theme-%s.conf", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), themes.Name(theme));
        dsyslog("nopacity: trying to load theme config %s", *confFile);
        LoadThemeConfig(confFile, themes.Name(theme));
    }
}

void cNopacityConfig::LoadThemeConfig(cString confFile, cString theme) {
    FILE *f = fopen(*confFile, "r");
    if (f) {
        dsyslog("nopacity: %s successfully loaded", *confFile);
        std::map<std::string, int> themeConf;
        char *line;
        cReadLine ReadLine;
        bool insertConf = false;
        while ((line = ReadLine.Read(f)) != NULL) {
            std::pair<std::string, int> themeConfLine = ReadThemeConfigLine(line);
            std::string name = (std::string)themeConfLine.first;
            if (name.compare("undefined") != 0) {
                themeConf.insert(themeConfLine);
                insertConf = true;
            }
        }
        if (insertConf)
            themeConfigDefaults.insert(std::pair<std::string, std::map<std::string, int> >(*theme, themeConf));
    }
}

std::pair<std::string, int> cNopacityConfig::ReadThemeConfigLine(const char *line) {
    std::pair<std::string, int> themeConf;
    try {
        splitstring s(line);
        std::vector<std::string> flds = s.split('=', 0);
        if (flds.size() == 2) {
            std::string name = flds[0];
            name.erase(name.find_last_not_of(" ")+1);
            name.erase(0, name.find_first_not_of(" "));
            int value = atoi(flds[1].c_str());
            themeConf = std::make_pair (name,value);
        } else {
            themeConf = std::make_pair ("undefined",0);
        }
    } catch (...) {
        themeConf = std::make_pair ("undefined",0);
    }
    return themeConf;
}

void cNopacityConfig::SetThemeSpecificDefaults(void) {
    std::map<std::string, std::map<std::string, int> >::iterator themeConfHit = themeConfigDefaults.find(Setup.OSDTheme);
    if (themeConfHit != themeConfigDefaults.end()) {
        std::map<std::string, int> themeConf =  (std::map<std::string, int>)themeConfHit->second;
        for(std::map<std::string, int>::const_iterator it = themeConf.begin(); it != themeConf.end(); it++) {
            std::string name = (std::string)it->first;
            int value = (int)it->second;
            std::map<std::string, int>::iterator origVal = conf.find(name);
            if (origVal != conf.end()) {
                conf.erase(origVal);
                conf.insert(std::pair<std::string, int>(name, value));
            } else {
                dsyslog("nopacity: ERROR: unknown config parameter %s in default config for theme %s", name.c_str(), Setup.OSDTheme);
            }
        }
    }
}

void cNopacityConfig::SetThemeSetup(void) {
    std::map<std::string, std::map<std::string, int> >::iterator themeSetupHit = themeConfigSetup.find(Setup.OSDTheme);
    if (themeSetupHit != themeConfigSetup.end()) {
        std::map<std::string, int> themeSetup =  (std::map<std::string, int>)themeSetupHit->second;
        for(std::map<std::string, int>::const_iterator it = themeSetup.begin(); it != themeSetup.end(); it++) {
            std::string name = (std::string)it->first;
            int value = (int)it->second;
            std::map<std::string, int>::iterator origVal = conf.find(name);
            if (origVal != conf.end()) {
                conf.erase(origVal);
                conf.insert(std::pair<std::string, int>(name, value));
            }
        }
    }
}

bool cNopacityConfig::SetupParse(const char *Name, const char *Value) {
    splitstring s(Name);
    std::vector<std::string> flds = s.split('.', 0);
    if (flds.size() == 2) {
        std::string theme = flds[0];
        std::string name = flds[1];
        //check if valid value
        std::map<std::string, int>::iterator hit = conf.find(name);
        if (hit == conf.end())
            return false;
        SetThemeConfigSetupValue(theme, name, atoi(Value));
    } else {
        return false;
    }
    return true;
}

void cNopacityConfig::SetThemeConfigSetupValue(std::string themeName, std::string key, int value) {
    //check if theme already in map
    std::map<std::string, std::map<std::string, int> >::iterator hit = themeConfigSetup.find(themeName);
    if (hit != themeConfigSetup.end()) {
        std::map<std::string, int> existingValues = (std::map<std::string, int>)hit->second;
        existingValues.erase(key);
	existingValues.insert(std::pair<std::string, int>(key, value));
        themeConfigSetup.erase(themeName);
        themeConfigSetup.insert(std::pair<std::string, std::map<std::string, int> >(themeName, existingValues));
    } else {
        std::map<std::string, int> themeConf;
        themeConf.insert(std::pair<std::string, int>(key, value));
        themeConfigSetup.insert(std::pair<std::string, std::map<std::string, int> >(themeName, themeConf));
    }
}

void cNopacityConfig::DumpConfig(void) {
   esyslog("nopacity: current config -----------------");
   for(std::map<std::string, int>::const_iterator it = conf.begin(); it != conf.end(); it++) {
        esyslog("nopacity: %s: %d", (std::string(it->first)).c_str(), (int)it->second);
   }
   esyslog("nopacity: --------------------------------");
}

void cNopacityConfig::DumpThemeConfig(void) {
    for (std::map<std::string, std::map<std::string, int> >::const_iterator it = themeConfigDefaults.begin(); it != themeConfigDefaults.end(); it++) {
        esyslog("nopacity: Default Config for theme %s -----------------", (std::string(it->first)).c_str());
        std::map<std::string, int> themeValues = (std::map<std::string, int>)it->second;
        for (std::map<std::string, int>::const_iterator it2 = themeValues.begin(); it2 != themeValues.end(); it2++) {
            esyslog("nopacity: %s: %d", (std::string(it2->first)).c_str(), (int)it2->second);
        }
    }
    esyslog("nopacity: --------------------------------");
    for (std::map<std::string, std::map<std::string, int> >::const_iterator it3 = themeConfigSetup.begin(); it3 != themeConfigSetup.end(); it3++) {
        esyslog("nopacity: Setup for theme %s -----------------", (std::string(it3->first)).c_str());
        std::map<std::string, int> themeValues = (std::map<std::string, int>)it3->second;
        for (std::map<std::string, int>::const_iterator it4 = themeValues.begin(); it4 != themeValues.end(); it4++) {
            esyslog("nopacity: %s: %d", (std::string(it4->first)).c_str(), (int)it4->second);
        }
    }
    esyslog("nopacity: --------------------------------");
}
