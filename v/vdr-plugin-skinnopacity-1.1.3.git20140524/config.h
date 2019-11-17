#ifndef __NOPACITY_CONFIG_H
#define __NOPACITY_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <vdr/tools.h>
#include <vdr/skins.h>
#include <vdr/plugin.h>
#include "fontmanager.h"
#include "geometrymanager.h"

enum eDisplayType {
    dtFlat = 0,
    dtBlending,
    dtGraphical
};

class cNopacityConfig {
    private:
        std::map<std::string, int> conf;
        std::map<std::string, std::map<std::string, int> > themeConfigDefaults;
        std::map<std::string, std::map<std::string, int> > themeConfigSetup;
        void LoadThemeConfig(cString confFile, cString theme);
        std::pair<std::string, int> ReadThemeConfigLine(const char *line);
        cString checkSlashAtEnd(std::string path);
    public:
        cNopacityConfig();
        ~cNopacityConfig();
        int GetValue(std::string name);
        int *GetValueRef(std::string name);
        std::map<std::string, int>::const_iterator GetStart(void) { return conf.begin(); };
        std::map<std::string, int>::const_iterator GetEnd(void) { return conf.end(); };
        bool SetupParse(const char *Name, const char *Value);
        void SetThemeConfigSetupValue(std::string themeName, std::string key, int value);
        void SetLogoPath(cString path);
        void SetIconPath(cString path);
        void SetEpgImagePath(cString path);
        bool logoPathSet;
        bool epgImagePathSet;
        bool iconPathSet;
        cString logoPathDefault;
        cString iconPathDefault;
        cString epgImagePathDefault;
        void LoadDefaults(void);
        void LoadThemeSpecificConfigs(void);
        void SetThemeSpecificDefaults(void);
        void SetThemeSetup(void);
        void SetPathes(void);
        void DumpConfig(void);
        void DumpThemeConfig(void);
        void SetFontName();
        const char *fontDefaultName;
        char *fontName;
        cString logoPath;
        cString logoExtension;
        cString iconPath;
        cString epgImagePath;
        bool mainMenuEntry;
};
#ifdef DEFINE_CONFIG
   bool firstDisplay = true;
   cNopacityConfig config;
   cGeometryManager *geoManager;
   cFontManager *fontManager;
   cTheme Theme;
#else
   extern bool firstDisplay;
   extern cNopacityConfig config;
   extern cGeometryManager *geoManager;
   extern cFontManager *fontManager;
   extern cTheme Theme;
#endif

//COMMON
#define CLR_TRANSPARENT         0x00000000
#define CLR_MENUBACK            0xB012273F
#define CLR_FONT                0xFFFFFFFF
#define CLR_FONTINACTIVE        0xFF858585
#define CLR_FONTDARK            0xFF000000

//CHANNELS
#define CLR_CHANNELPROGRESSBAR  0xFFC6D590
#define CLR_CHANNELPROGRESSBARBACK 0xDD000000
#define CLR_CHANNELPROGRESSBARBLEND 0xFF8EAB21
#define CLR_FONTHEAD            0xFFEEEEEE
#define CLR_CHANNELSYMBOLOFF    0xDD858585
#define CLR_CHANNELRECACTIVE    0xDDFF0000
#define CLR_RECNEXT             0xDDFFFF00

//REPLAY
#define CLR_CURRENT             0x90FFFFFF
#define CLR_SEEN                0xFF8EAB21
#define CLR_REST                0xDD858585

//MENU
#define CLR_MENUBORDER          0xAA242A38
#define CLR_MENUSCROLLBAR       0x908EAB21
#define CLR_MENUSCROLLBARBACK   0x99242A38
#define CLR_MENUSCROLLBARBASE   0x00000000
#define CLR_DISKALERT           0xDDFF0000
#define CLR_TIMERS              0x7D000000
#define CLR_MENUHEADER          0xDD000000
#define CLR_PROGRESSBAR         0xFFC6D590
#define CLR_PROGRESSBARBACK     0xDD000000
#define CLR_PROGRESSBARBLEND    0xFF8EAB21
#define CLR_PROGRESSBARHIGH     0xDD4E78B1
#define CLR_PROGRESSBARBACKHIGH 0xDD000000
#define CLR_PROGRESSBARBLENDHIGH 0xDD80B3FF
#define CLR_CHANNELLOGOBACK     0x99C6C6C6
#define CLR_FONTMENUITEMHIGH    0xFF363636
#define CLR_TEXTWINDOW          0xB0000000
#define CLR_DETAILVIEWBACK      0x50000000
#define CLR_DETAILVIEWTABS      0x99242A38

//CHANNELS
THEME_CLR(Theme, clrChannelBackground,      CLR_MENUBACK);
THEME_CLR(Theme, clrChannelBackBlend,       CLR_TRANSPARENT);
THEME_CLR(Theme, clrChannelHead,            CLR_FONTHEAD);
THEME_CLR(Theme, clrChannelEPG,             CLR_FONT);
THEME_CLR(Theme, clrChannelEPGInfo,         CLR_FONT);
THEME_CLR(Theme, clrChannelEPGNext,         CLR_FONTINACTIVE);
THEME_CLR(Theme, clrChannelEPGInfoNext,     CLR_FONTINACTIVE);
THEME_CLR(Theme, clrChannelProgressBar,     CLR_CHANNELPROGRESSBAR);
THEME_CLR(Theme, clrChannelProgressBarBack, CLR_CHANNELPROGRESSBARBACK);
THEME_CLR(Theme, clrChannelProgressBarBlend,CLR_CHANNELPROGRESSBARBLEND);
THEME_CLR(Theme, clrStatusIconsBack,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrRecNow,                 CLR_CHANNELRECACTIVE);
THEME_CLR(Theme, clrRecNowFont,             CLR_FONT);
THEME_CLR(Theme, clrRecNext,                CLR_RECNEXT);
THEME_CLR(Theme, clrRecNextFont,            CLR_FONTDARK);

//REPLAY
THEME_CLR(Theme, clrReplayBackground,       CLR_MENUBACK);
THEME_CLR(Theme, clrReplayBackBlend,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrReplayHead,             CLR_FONT);
THEME_CLR(Theme, clrReplayDescription,      CLR_FONTINACTIVE);
THEME_CLR(Theme, clrReplayCurrentTotal,     CLR_FONT);
THEME_CLR(Theme, clrReplayProgressSeen,     CLR_SEEN);
THEME_CLR(Theme, clrReplayProgressRest,     CLR_REST);
THEME_CLR(Theme, clrReplayProgressSelected, CLR_FONTDARK);
THEME_CLR(Theme, clrReplayProgressMark,     CLR_FONTDARK);
THEME_CLR(Theme, clrReplayProgressCurrent,  CLR_CURRENT);
THEME_CLR(Theme, clrReplayHighlightIcon,    CLR_TRANSPARENT);

//TRACKS
THEME_CLR(Theme, clrTracksFontHead,         CLR_FONTINACTIVE);
THEME_CLR(Theme, clrTracksFontButtons,      CLR_FONT);

//Volume
THEME_CLR(Theme, clrVolumeFont,             CLR_FONT);

//MENU
THEME_CLR(Theme, clrMenuBack,               CLR_MENUBACK);
THEME_CLR(Theme, clrMenuBorder,             CLR_MENUBORDER);
THEME_CLR(Theme, clrMenuScrollBar,          CLR_MENUSCROLLBAR);
THEME_CLR(Theme, clrMenuScrollBarBack,      CLR_MENUSCROLLBARBACK);
THEME_CLR(Theme, clrMenuScrollBarBase,      CLR_MENUSCROLLBARBASE);
THEME_CLR(Theme, clrMenuItem,               CLR_TRANSPARENT);
THEME_CLR(Theme, clrMenuItemBlend,          CLR_TRANSPARENT);
THEME_CLR(Theme, clrMenuItemHigh,           CLR_TRANSPARENT);
THEME_CLR(Theme, clrMenuItemHighBlend,      CLR_TRANSPARENT);
THEME_CLR(Theme, clrSeparatorBorder,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrDiskAlert,              CLR_DISKALERT);
THEME_CLR(Theme, clrTimersBack,             CLR_TIMERS);
THEME_CLR(Theme, clrTimersBackBlend,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrMenuFontHeader,         CLR_FONT);
THEME_CLR(Theme, clrMenuFontDate,           CLR_FONT);
THEME_CLR(Theme, clrMenuFontDiscUsage,      CLR_FONTDARK);
THEME_CLR(Theme, clrMenuFontDiscUsagePerc,  CLR_FONT);
THEME_CLR(Theme, clrMenuFontTimers,         CLR_FONT);
THEME_CLR(Theme, clrMenuFontTimersHeader,   CLR_FONT);
THEME_CLR(Theme, clrMenuFontDetailViewText, CLR_FONT);
THEME_CLR(Theme, clrMenuFontDetailViewHeader, CLR_FONTINACTIVE);
THEME_CLR(Theme, clrMenuFontDetailViewHeaderTitle, CLR_FONT);
THEME_CLR(Theme, clrMenuFontMenuItem,       CLR_FONT);
THEME_CLR(Theme, clrMenuFontMenuItemHigh,   CLR_FONTMENUITEMHIGH);
THEME_CLR(Theme, clrMenuFontMenuItemTitle,  CLR_FONTMENUITEMHIGH);
THEME_CLR(Theme, clrMenuFontMenuItemSep,    CLR_FONTINACTIVE);
THEME_CLR(Theme, clrMenuHeader,             CLR_MENUHEADER);
THEME_CLR(Theme, clrMenuHeaderBlend,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrAudioMenuHeader,        CLR_MENUHEADER);
THEME_CLR(Theme, clrProgressBar,            CLR_PROGRESSBAR);
THEME_CLR(Theme, clrProgressBarBack,        CLR_PROGRESSBARBACK);
THEME_CLR(Theme, clrProgressBarBlend,       CLR_PROGRESSBARBLEND);
THEME_CLR(Theme, clrProgressBarHigh,        CLR_PROGRESSBARHIGH);
THEME_CLR(Theme, clrProgressBarBackHigh,    CLR_PROGRESSBARBACKHIGH);
THEME_CLR(Theme, clrProgressBarBlendHigh,   CLR_PROGRESSBARBLENDHIGH);
THEME_CLR(Theme, clrMenuTextWindow,         CLR_TEXTWINDOW);
THEME_CLR(Theme, clrMenuChannelLogoBack,    CLR_CHANNELLOGOBACK);
THEME_CLR(Theme, clrMenuDetailViewBack,     CLR_DETAILVIEWBACK);
THEME_CLR(Theme, clrMenuDetailViewTabs,     CLR_DETAILVIEWTABS);

//BUTTONS
THEME_CLR(Theme, clrButtonRed,              CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonRedBorder,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonRedFont,          CLR_FONT);
THEME_CLR(Theme, clrButtonGreen,            CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonGreenBorder,      CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonGreenFont,        CLR_FONT);
THEME_CLR(Theme, clrButtonYellow,           CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonYellowBorder,     CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonYellowFont,       CLR_FONT);
THEME_CLR(Theme, clrButtonBlue,             CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonBlueBorder,       CLR_TRANSPARENT);
THEME_CLR(Theme, clrButtonBlueFont,         CLR_FONT);

//MESSAGES
THEME_CLR(Theme, clrMessageFontStatus,      CLR_FONT);
THEME_CLR(Theme, clrMessageFontInfo,        CLR_FONT);
THEME_CLR(Theme, clrMessageFontWarning,     CLR_FONT);
THEME_CLR(Theme, clrMessageFontError,       CLR_FONT);
THEME_CLR(Theme, clrMessageStatus,          CLR_TRANSPARENT);
THEME_CLR(Theme, clrMessageInfo,            CLR_TRANSPARENT);
THEME_CLR(Theme, clrMessageWarning,         CLR_TRANSPARENT);
THEME_CLR(Theme, clrMessageError,           CLR_TRANSPARENT);
THEME_CLR(Theme, clrMessageBlend,           CLR_TRANSPARENT);

#endif //__NOPACITY_CONFIG_H
