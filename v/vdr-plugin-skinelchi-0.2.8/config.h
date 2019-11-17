/*
 * config.h: Setup and configuration file handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINELCHI_CONFIG_H
#define __SKINELCHI_CONFIG_H

#include <vdr/themes.h>
#include <vdr/config.h>


#define MAX_ELCHI_THEME_COLORS 68


class cSkinElchiConfig
{
private:
   char logoBaseDir[MaxFileName];
   char epgimageDir[MaxFileName];

public:
   cSkinElchiConfig(void);
   ~cSkinElchiConfig();
   bool SetupParse(const char *Name, const char *Value);
   void SetLogoBaseDir(const char *dir);
   char *GetLogoBaseDir(void) { return logoBaseDir; }
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   void SetEpgImageDir(const char *dir);
   char *GetEpgImageDir(void) { return epgimageDir; }
#endif

   // general
   int trySingle8BppArea;
   int useScrolling;
   int showTimer;
   int showRecDetails;
   int showEPGDetails;
   int showVideoInfo;
   int showMailIcon;
   int GraphicalProgressbar;
   int showIcons;
   int EpgImageSize; // is always defined, but only saved with SKINELCHI_HAVE_IMAGEMAGICK
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   int EpgImageDisplayTime;
   int ImgResizeAlgo;
#endif

   // channel display
   int showRecInfo;
   int showLogo;
   int tryChannelDisplay8Bpp;
   int useHQlogo;
   int LogoSearchNameFirst;
   int LogoMessages;

   // clr dialog
   bool clrdlgActive;
   tColor clrDlgFg;
   tColor clrDlgBg;

   int logox; //TODO
   int logoy; //TODO
   tColor ThemeColors[MAX_ELCHI_THEME_COLORS];
};


extern cSkinElchiConfig ElchiConfig;

extern cTheme Theme;

THEME_CLR(Theme, clrBackground,             0x77000066);//  0
THEME_CLR(Theme, clrButtonRedFg,            clrWhite);  //  1
THEME_CLR(Theme, clrButtonRedBg,            0xCCCC1111);//  2
THEME_CLR(Theme, clrButtonGreenFg,          clrBlack);  //  3
THEME_CLR(Theme, clrButtonGreenBg,          0xCC22BB22);//  4
THEME_CLR(Theme, clrButtonYellowFg,         clrBlack);  //  5
THEME_CLR(Theme, clrButtonYellowBg,         0xCCEEEE22);//  6
THEME_CLR(Theme, clrButtonBlueFg,           clrWhite);  //  7
THEME_CLR(Theme, clrButtonBlueBg,           0xCC2233CC);//  8
THEME_CLR(Theme, clrMessageStatusFg,        clrBlack);  //  9
THEME_CLR(Theme, clrMessageStatusBg,        0xCC2BA7F1);// 10
THEME_CLR(Theme, clrMessageInfoFg,          clrBlack);  // 11
THEME_CLR(Theme, clrMessageInfoBg,          0xCC22BB22);// 12
THEME_CLR(Theme, clrMessageWarningFg,       clrBlack);  // 13
THEME_CLR(Theme, clrMessageWarningBg,       clrYellow); // 14
THEME_CLR(Theme, clrMessageErrorFg,         clrWhite);  // 15
THEME_CLR(Theme, clrMessageErrorBg,         clrRed);    // 16
THEME_CLR(Theme, clrVolumePrompt,           clrWhite);  // 17
THEME_CLR(Theme, clrVolumeBarUpper,         0xFFCCBB22);// 18
THEME_CLR(Theme, clrVolumeBarLower,         0x77000066);// 19
THEME_CLR(Theme, clrVolumeSymbolMuteFg,     0xCCCC1111);// 20
THEME_CLR(Theme, clrVolumeSymbolMuteBg,     clrWhite);  // 21
THEME_CLR(Theme, clrVolumeSymbolVolumeFg,   0xFFCCBB22);// 22
THEME_CLR(Theme, clrVolumeSymbolVolumeBg,   0x77000066);// 23
THEME_CLR(Theme, clrChannelNameFg,          clrWhite);  // 24
THEME_CLR(Theme, clrChannelNameBg,          0xCC2BA7F1);// 25
THEME_CLR(Theme, clrChannelDateFg,          clrBlack);  // 26
THEME_CLR(Theme, clrChannelDateBg,          clrWhite);  // 27
THEME_CLR(Theme, clrChannelEpgTimeFg,       0xFFDDDDDD);// 28
THEME_CLR(Theme, clrChannelEpgTimeBg,       0xAABB0000);// 29
THEME_CLR(Theme, clrChannelEpgTitleFg,      0xFF00FCFC);// 30
THEME_CLR(Theme, clrChannelEpgTitleBg,      0x88000000);// 31
THEME_CLR(Theme, clrChannelEpgShortText,    0xFFCCBB22);// 32
THEME_CLR(Theme, clrChannelSymbolOn,        clrYellow); // 33
THEME_CLR(Theme, clrChannelSymbolOff,       0x77777777);// 34
THEME_CLR(Theme, clrSymbolRecFg,            clrWhite);  // 35
THEME_CLR(Theme, clrSymbolRecBg,            clrRed);    // 36
THEME_CLR(Theme, clrChannelTimebarSeen,     clrYellow); // 37
THEME_CLR(Theme, clrChannelTimebarRest,     clrGray50); // 38
THEME_CLR(Theme, clrMenuTitleFg,            clrBlack);  // 39
THEME_CLR(Theme, clrMenuTitleBg,            0xCC2BA7F1);// 40
THEME_CLR(Theme, clrMenuDate,               clrBlack);  // 41
THEME_CLR(Theme, clrMenuItemCurrentFg,      clrBlack);  // 42
THEME_CLR(Theme, clrMenuItemCurrentBg,      0xCC2BA7F1);// 43
THEME_CLR(Theme, clrMenuItemSelectable,     clrWhite);  // 44
THEME_CLR(Theme, clrMenuItemNonSelectable,  0xCC2BA7F1);// 45
THEME_CLR(Theme, clrMenuEventTime,          clrWhite);  // 46
THEME_CLR(Theme, clrMenuEventVpsFg,         clrBlack);  // 47
THEME_CLR(Theme, clrMenuEventVpsBg,         clrWhite);  // 48
THEME_CLR(Theme, clrMenuEventTitle,         clrYellow); // 49
THEME_CLR(Theme, clrMenuEventShortText,     clrWhite);  // 50
THEME_CLR(Theme, clrMenuEventDescription,   clrYellow); // 51
THEME_CLR(Theme, clrMenuScrollbarTotal,     clrWhite);  // 52
THEME_CLR(Theme, clrMenuScrollbarShown,     clrYellow); // 53
THEME_CLR(Theme, clrMenuText,               clrWhite);  // 54
THEME_CLR(Theme, clrReplayTitleFg,          clrBlack);  // 55
THEME_CLR(Theme, clrReplayTitleBg,          clrWhite);  // 56
THEME_CLR(Theme, clrReplayCurrent,          clrWhite);  // 57
THEME_CLR(Theme, clrReplayTotal,            clrWhite);  // 58
THEME_CLR(Theme, clrReplayModeJump,         clrWhite);  // 59
THEME_CLR(Theme, clrReplayProgressSeen,     0xCC22BB22);// 60
THEME_CLR(Theme, clrReplayProgressRest,     clrWhite);  // 61
THEME_CLR(Theme, clrReplayProgressSelected, 0xCCCC1111);// 62
THEME_CLR(Theme, clrReplayProgressMark,     clrBlack);  // 63
THEME_CLR(Theme, clrReplayProgressCurrent,  0xCCCC1111);// 64
THEME_CLR(Theme, clrReplaySymbolOn,         clrYellow); // 65
THEME_CLR(Theme, clrReplaySymbolOff,        0x77777777);// 66

#endif //__SKINELCHI_CONFIG_H
