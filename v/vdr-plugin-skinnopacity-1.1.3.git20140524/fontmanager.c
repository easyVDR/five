#include "fontmanager.h"
#include "geometrymanager.h"
#include "config.h"


cFontManager::cFontManager() {
}

cFontManager::~cFontManager() {
    DeleteFonts();
}

void cFontManager::SetFonts() {
    SetFontsMenu();
    SetFontsChannel();
    SetFontsReplay();
    SetFontsMessage();
    SetFontsTrack();
    SetFontsVolume();
}

void cFontManager::DeleteFonts() {
    DeleteFontsMenu();
    DeleteFontsChannel();
    DeleteFontsReplay();
    DeleteFontsMessage();
    DeleteFontsTrack();
    DeleteFontsVolume();
}

cFont *cFontManager::CreateFont(int size) {
    return cFont::CreateFont(config.fontName, size);
}

void cFontManager::SetFontsMenu(void) {
    menuHeader = CreateFont(geoManager->menuHeaderHeight / 2 + config.GetValue("fontHeader"));
    menuDate = CreateFont(geoManager->menuHeaderHeight / 2 + config.GetValue("fontDate"));
    menuItemLarge = CreateFont(geoManager->menuItemHeightMain/3 + 4 + config.GetValue("fontMenuitemLarge"));
    menuItemSchedule = CreateFont(geoManager->menuItemHeightSchedule / 4 + 5 + config.GetValue("fontMenuitemSchedule"));
    menuItemScheduleSmall = CreateFont(geoManager->menuItemHeightSchedule / 4 - 5 + config.GetValue("fontMenuitemScheduleSmall"));
    menuItemChannel = CreateFont(geoManager->menuItemHeightSchedule / 3 + config.GetValue("fontMenuitemChannel"));
    menuItemChannelSmall = CreateFont(geoManager->menuItemHeightSchedule / 5 - 2 + config.GetValue("fontMenuitemChannelSmall"));
    menuItemRecordings = CreateFont(geoManager->menuItemHeightRecordings / 2 - 14 + config.GetValue("fontMenuitemRecordings"));
    menuItemRecordingsSmall = CreateFont(geoManager->menuItemHeightRecordings / 4 - 3 + config.GetValue("fontMenuitemRecordingsSmall"));
    menuItemTimers = CreateFont(geoManager->menuItemHeightSchedule / 3 + config.GetValue("fontMenuitemTimers"));
    menuItemTimersSmall = CreateFont(geoManager->menuItemHeightSchedule / 4 - 3 + config.GetValue("fontMenuitemTimersSmall"));
    menuItemDefault = CreateFont(geoManager->menuItemHeightDefault * 2 / 3 + config.GetValue("fontMenuitemDefault"));
    menuDiskUsage = CreateFont(geoManager->menuDiskUsageHeight/6 - 2 + config.GetValue("fontDiskUsage"));
    menuDiskUsagePercent = CreateFont(geoManager->menuDiskUsageHeight/5 - 4 + config.GetValue("fontDiskUsagePercent"));
    menuTimersHead = CreateFont((geoManager->menuContentHeight - 3*geoManager->menuSpace - geoManager->menuDiskUsageHeight) / 25 + config.GetValue("fontTimersHead"));
    menuTimers = CreateFont((geoManager->menuContentHeight - 3*geoManager->menuSpace - geoManager->menuDiskUsageHeight) / 25 - 6 + config.GetValue("fontTimers"));
    menuButtons = CreateFont(geoManager->menuButtonHeight*0.8 + config.GetValue("fontButtons"));
    menuMessage = CreateFont(geoManager->menuMessageHeight / 3 + config.GetValue("fontMessageMenu"));
    menuEPGInfoWindow = CreateFont(geoManager->menuContentHeight / 30 + config.GetValue("fontEPGInfoWindow"));
    menuEPGInfoWindowLarge = CreateFont(geoManager->menuContentHeight / 20 + config.GetValue("fontEPGInfoWindowLarge"));
}

void cFontManager::DeleteFontsMenu(void) {
    delete menuHeader;
    delete menuDate;
    delete menuItemLarge;
    delete menuItemSchedule;
    delete menuItemScheduleSmall;
    delete menuItemChannel;
    delete menuItemChannelSmall;
    delete menuItemRecordings;
    delete menuItemRecordingsSmall;
    delete menuItemTimers;
    delete menuItemTimersSmall;
    delete menuItemDefault;
    delete menuDiskUsage;
    delete menuDiskUsagePercent;
    delete menuTimers;
    delete menuTimersHead;
    delete menuButtons;
    delete menuMessage;
    delete menuEPGInfoWindow;
    delete menuEPGInfoWindowLarge;
}

void cFontManager::SetFontsChannel(void) {
    channelHeader = CreateFont(geoManager->channelHeaderHeight - 8 + config.GetValue("fontChannelHeaderSize"));
    channelDate = CreateFont(geoManager->channelHeaderHeight/2 + config.GetValue("fontChannelDateSize"));
    channelEPG = CreateFont(geoManager->channelEpgInfoLineHeight + config.GetValue("fontEPGSize"));
    channelEPGSmall = CreateFont(geoManager->channelEpgInfoLineHeight - 6 + config.GetValue("fontEPGSmallSize"));
    channelSourceInfo = CreateFont(geoManager->channelFooterHeight/2 + config.GetValue("fontChannelSourceInfoSize"));;
    channelChannelGroup = CreateFont(geoManager->channelEpgInfoHeight/3 + config.GetValue("fontChannelGroupSize"));
    channelChannelGroupSmall = CreateFont(geoManager->channelEpgInfoHeight/3 - 5 + config.GetValue("fontChannelGroupSmallSize"));
}

void cFontManager::DeleteFontsChannel(void) {
    delete channelHeader;
    delete channelDate;
    delete channelEPG;
    delete channelEPGSmall;
    delete channelSourceInfo;
    delete channelChannelGroup;
    delete channelChannelGroupSmall;
}

void cFontManager::SetFontsReplay(void) {
    replayHeader = CreateFont(geoManager->replayHeaderHeight - 8 + config.GetValue("fontReplayHeader"));
    replayText = CreateFont(geoManager->replayCurrentHeight);
}

void cFontManager::DeleteFontsReplay(void) {
    delete replayHeader;
    delete replayText;
}

void cFontManager::SetFontsMessage(void) {
    messageText = CreateFont(geoManager->messageHeight / 4 + 15 + config.GetValue("fontMessage"));
}

void cFontManager::DeleteFontsMessage(void) {
    delete messageText;
}

void cFontManager::SetFontsTrack(void) {
    trackText = CreateFont(geoManager->menuItemHeightTracks/3 + config.GetValue("fontTracks"));
    trackHeader = CreateFont(geoManager->menuItemHeightTracks/2 + config.GetValue("fontTracksHeader"));
}

void cFontManager::DeleteFontsTrack(void) {
    delete trackText;
    delete trackHeader;
}

void cFontManager::SetFontsVolume(void) {
    volumeText = CreateFont(geoManager->volumeLabelHeight - 6 + config.GetValue("fontVolume"));
}

void cFontManager::DeleteFontsVolume(void) {
    delete volumeText;
}