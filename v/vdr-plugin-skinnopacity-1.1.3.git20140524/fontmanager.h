#ifndef __NOPACITY_FONTMANAGER_H
#define __NOPACITY_FONTMANAGER_H

#include <vdr/skins.h>

class cFontManager {
    private:
        void SetFontsMenu(void);
        void DeleteFontsMenu(void);
        void SetFontsChannel(void);
        void DeleteFontsChannel(void);
        void SetFontsReplay(void);
        void DeleteFontsReplay(void);
        void SetFontsMessage(void);
        void DeleteFontsMessage(void);
        void SetFontsTrack(void);
        void DeleteFontsTrack(void);
        void SetFontsVolume(void);
        void DeleteFontsVolume(void);
    public:
        cFontManager();
        ~cFontManager();
        cFont *CreateFont(int size);
        void SetFonts(void);
        void DeleteFonts(void);
        //Fonts DisplayMenu
        cFont *menuHeader;
        cFont *menuDate;
        cFont *menuItemLarge;
        cFont *menuItemSchedule;
        cFont *menuItemScheduleSmall;
        cFont *menuItemChannel;
        cFont *menuItemChannelSmall;
        cFont *menuItemRecordings;
        cFont *menuItemRecordingsSmall;
        cFont *menuItemTimers;
        cFont *menuItemTimersSmall;
        cFont *menuItemDefault;
        cFont *menuDiskUsage;
        cFont *menuDiskUsagePercent;
        cFont *menuTimers;
        cFont *menuTimersHead;
        cFont *menuButtons;
        cFont *menuMessage;
        cFont *menuEPGInfoWindow;
        cFont *menuEPGInfoWindowLarge;
        //Fonts DisplayChannel
        cFont *channelHeader;
        cFont *channelDate;
        cFont *channelEPG;
        cFont *channelEPGSmall;
        cFont *channelSourceInfo;
        cFont *channelChannelGroup;
        cFont *channelChannelGroupSmall;
        //Fonts DisplayReplay
        cFont *replayHeader;
        cFont *replayText;
        //Fonts DisplayMessage
        cFont *messageText;
        //Fonts DisplayTracks
        cFont *trackText;
        cFont *trackHeader;
        //Fonts DisplayVolume
        cFont *volumeText;
};

#endif //__NOPACITY_FONTMANAGER_H