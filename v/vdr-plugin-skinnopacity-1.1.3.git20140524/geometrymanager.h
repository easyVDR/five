#ifndef __NOPACITY_GEOMETRYMANAGER_H
#define __NOPACITY_GEOMETRYMANAGER_H

enum eLogoPosition {lpNone = 0, lpLeft, lpRight};
enum eLogoVerticalPosition {lvTop = 0, lvMiddle, lvBottom};
enum eBackgroundStyle {bsTrans = 0, bsFull};

class cGeometryManager {
    private:
        void SetOSDSize(void);
        void SetDisplayMenuSizes(void);
        void SetDisplayChannelSizes(void);
        void SetDisplayReplaySizes(void);
        void SetDisplayMessageSizes(void);
        void SetDisplayTrackSizes(void);
        void SetDisplayVolumeSizes(void);
    public:
        cGeometryManager();
        ~cGeometryManager();
        void SetGeometry(void);
        bool GeometryChanged(void);
        int osdWidth;
        int osdHeight;
        int osdLeft;
        int osdTop;
        //DisplayMenu Sizes
        int menuSpace;
        int menuWidthScrollbar;
        int menuDateWidth;
        int menuHeaderHeight;
        int menuFooterHeight;
        int menuContentHeight;
        int menuContentWidthMain;
        int menuContentWidthSchedules;
        int menuContentWidthChannels;
        int menuContentWidthTimers;
        int menuContentWidthRecordings;
        int menuContentWidthSetup;
        int menuContentWidthFull;
        int menuContentWidthMinimum;
        int menuItemWidthDefault;
        int menuItemWidthMain;
        int menuItemWidthSchedule;
        int menuItemWidthChannel;
        int menuItemWidthTimer;
        int menuItemWidthRecording;
        int menuItemWidthSetup;
        int menuItemWidthTracks;
        int menuItemHeightMain;
        int menuItemHeightSchedule;
        int menuItemHeightDefault;
        int menuItemHeightRecordings;
        int menuItemHeightTracks;
        int menuMainMenuIconSize;
        int menuLogoWidth;
        int menuLogoHeight;
        int menuTimersLogoWidth;
        int menuTimersLogoHeight;
        int menuHeaderVDRLogoWidth;
        int menuButtonsBorder;
        int menuButtonWidth;
        int menuButtonHeight;
        int menuDiskUsageWidth;
        int menuDiskUsageHeight;
        int menuTimersWidth;
        int menuMessageWidth;
        int menuMessageHeight;
        //DisplayChannel Sizes
        int channelX, channelTop;
        int channelWidth, channelHeight;
        int channelHeaderHeight;
        int channelFooterHeight;
        int channelContentHeight;
        int channelContentX, channelContentWidth;
        int channelLogoWidthTotal;
        int channelLogoX, channelLogoY;
        int channelLogoWidth, channelLogoHeight;
        int channelChannelNameWidth;
        int channelDateWidth;
        int channelFooterY;
        int channelProgressBarHeight;
        int channelEpgInfoHeight;
        int channelEpgInfoLineHeight;
        //DisplayReplay Sizes
        int replayWidth;
        int replayHeight;
        int replayHeaderHeight;
        int replayInfo2Height;
        int replayProgressBarHeight;
        int replayCurrentHeight;
        int replayControlsHeight;
        int replayFooterHeight;
        int replayInfoWidth;
        int replayDateWidth;
        int replayIconSize, replayIconBorder;
        int replayResolutionSize;
        int replayResolutionX, replayResolutionY;
        int replayJumpX, replayJumpY;
        int replayJumpWidth, replayJumpHeight;
        int replayMessageY;
        int replayMessageWidth, replayMessageHeight;
        //DisplayMessage Sizes
        int messageWidth;
        int messageHeight;
        //DisplayTracks Sizes
        int trackWidth;
        //DisplayVolume Sizes
        int volumeWidth;
        int volumeHeight;
        int volumeLabelHeight;
        int volumeProgressBarWidth;
        int volumeProgressBarHeight;
};

#endif //__NOPACITY_GEOMETRYMANAGER_H