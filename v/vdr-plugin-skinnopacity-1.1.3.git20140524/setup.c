#include "setup.h"

cNopacitySetup::cNopacitySetup(cImageCache *imgCache) {
    this->imgCache = imgCache;
    tmpConf = config;
    cFont::GetAvailableFontNames(&fontNames);
    fontNames.Insert(strdup(config.fontDefaultName));
    Setup();
}

cNopacitySetup::~cNopacitySetup() {
    config.SetFontName();
    int start = cTimeMs::Now();
    geoManager->SetGeometry();
    fontManager->DeleteFonts();
    fontManager->SetFonts();
    imgCache->Reload();
    dsyslog("nopacity: Cache reloaded in %d ms", int(cTimeMs::Now()-start));
}


void cNopacitySetup::Setup(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditStraItem(tr("Font"), tmpConf.GetValueRef("fontIndex"), fontNames.Size(), &fontNames[0]));
    Add(new cOsdItem(tr("VDR Menu: Common Settings")));
    Add(new cOsdItem(tr("VDR Menu: Main and Setup Menu")));
    Add(new cOsdItem(tr("VDR Menu: Schedules Menu")));
    Add(new cOsdItem(tr("VDR Menu: Channels Menu")));
    Add(new cOsdItem(tr("VDR Menu: Timers Menu")));
    Add(new cOsdItem(tr("VDR Menu: Recordings Menu")));
    Add(new cOsdItem(tr("VDR Menu: Detailed EPG & Recordings View")));
    Add(new cOsdItem(tr("Channel Switching")));
    Add(new cOsdItem(tr("Replay")));
    Add(new cOsdItem(tr("Audio Tracks")));
    Add(new cOsdItem(tr("Messages")));
    Add(new cOsdItem(tr("Volume")));
    Add(new cOsdItem(tr("Image Caching")));
    SetCurrent(Get(currentItem));
    Display();
}

eOSState cNopacitySetup::ProcessKey(eKeys Key) {
    bool hadSubMenu = HasSubMenu();
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    if (hadSubMenu && Key == kOk)
        Store();
    if (!hadSubMenu && (state == osUnknown || Key == kOk)) {
        if ((Key == kOk && !hadSubMenu)) {
            const char* ItemText = Get(Current())->Text();
            if (strcmp(ItemText, tr("VDR Menu: Common Settings")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplay(&tmpConf));
            if (strcmp(ItemText, tr("VDR Menu: Main and Setup Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayMain(&tmpConf));
            if (strcmp(ItemText, tr("VDR Menu: Schedules Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplaySchedules(&tmpConf));
            if (strcmp(ItemText, tr("VDR Menu: Channels Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayChannels(&tmpConf));
            if (strcmp(ItemText, tr("VDR Menu: Timers Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayTimers(&tmpConf));
            if (strcmp(ItemText, tr("VDR Menu: Recordings Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayRecordings(&tmpConf));
            if (strcmp(ItemText, tr("VDR Menu: Detailed EPG & Recordings View")) == 0)
                state = AddSubMenu(new cNopacitySetupDetailedView(&tmpConf));
            if (strcmp(ItemText, tr("Channel Switching")) == 0)
                state = AddSubMenu(new cNopacitySetupChannelDisplay(&tmpConf));
            if (strcmp(ItemText, tr("Replay")) == 0)
                state = AddSubMenu(new cNopacitySetupReplayDisplay(&tmpConf));
            if (strcmp(ItemText, tr("Audio Tracks")) == 0)
                state = AddSubMenu(new cNopacitySetupTrackDisplay(&tmpConf));
            if (strcmp(ItemText, tr("Messages")) == 0)
                state = AddSubMenu(new cNopacitySetupMessageDisplay(&tmpConf));
            if (strcmp(ItemText, tr("Volume")) == 0)
                state = AddSubMenu(new cNopacitySetupVolumeDisplay(&tmpConf));
            if (strcmp(ItemText, tr("Image Caching")) == 0)
                state = AddSubMenu(new cNopacitySetupCaching(&tmpConf, imgCache));
        }
    }
    return state;
}

void cNopacitySetup::Store(void) {
    const char *themeName = Skins.Current()->Theme()->Name();
    for(std::map<std::string, int>::const_iterator it = tmpConf.GetStart(); it != tmpConf.GetEnd(); it++) {
        std::string name = (std::string)it->first;
        int value = (int)it->second;
        int origValue = config.GetValue(name);
        if (value != origValue) {
            //Save changed value in setup.conf
            SetupStore(*cString::sprintf("%s.%s", themeName, name.c_str()), value);
            //Save changed value also in cConfig::themeConfigSetup
            tmpConf.SetThemeConfigSetupValue(themeName, name, value);
        }
    }
    config = tmpConf;
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cNopacityConfig* data) : cOsdMenu(Title, 30) {
    tmpConf = data;
    spacer = "   ";
}

cOsdItem *cMenuSetupSubMenu::InfoItem(const char *label, const char *value) {
    cOsdItem *item;
    item = new cOsdItem(cString::sprintf("%s: %s", label, value));
    item->SetSelectable(false);
    return item;
}

eOSState cMenuSetupSubMenu::ProcessKey(eKeys Key) {
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        return osBack;
      default:
        break;
    }
  }
  if ((Key == kLeft)||(Key == kRight))
    Set();
  return state;
}

//-----MenuDisplay Common Settings -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplay::cNopacitySetupMenuDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Common Settings"), data) {
    adjustLeft[0] = tr("right");
    adjustLeft[1] = tr("left");
    scrollMode[0] = tr("Carriage Return");
    scrollMode[1] = tr("Forward and Back again");
    scrollSpeed[0] = tr("off");
    scrollSpeed[1] = tr("slow");
    scrollSpeed[2] = tr("medium");
    scrollSpeed[3] = tr("fast");
    scalePic[0] = tr("no");
    scalePic[1] = tr("yes");
    scalePic[2] = tr("auto");
    Set();
}

void cNopacitySetupMenuDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditBoolItem(tr("Create Log Messages for image loading"), tmpConf->GetValueRef("debugImageLoading")));
    Add(new cMenuEditIntItem(tr("Number of Default Menu Entries per Page"), tmpConf->GetValueRef("numDefaultMenuItems"), 10, 40));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Default Menu Item"), tmpConf->GetValueRef("fontMenuitemDefault"), -20, 20));
    Add(new cMenuEditStraItem(tr("Adjustment of narrow menus"), tmpConf->GetValueRef("menuAdjustLeft"), 2, adjustLeft));
    Add(new cMenuEditStraItem(tr("Scale Video size to fit into menu window"), tmpConf->GetValueRef("scalePicture"), 3, scalePic));
    Add(new cMenuEditIntItem(tr("Header Height (Percent of OSD Height)"), tmpConf->GetValueRef("headerHeight"), 5, 30));
    Add(new cMenuEditIntItem(tr("Footer Height (Percent of OSD Height)"), tmpConf->GetValueRef("footerHeight"), 5, 30));
    Add(new cMenuEditBoolItem(tr("Rounded Corners for menu items and buttons"), tmpConf->GetValueRef("roundedCorners")));
    if (tmpConf->GetValue("roundedCorners"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Radius of rounded corners")), tmpConf->GetValueRef("cornerRadius"), 5, 30));
    Add(new cMenuEditBoolItem(tr("Use Channel Logo background"), tmpConf->GetValueRef("menuChannelLogoBackground")));
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("menuFadeTime"), 0, 1000));
    Add(new cMenuEditStraItem(tr("Menu Items Scroll Style"), tmpConf->GetValueRef("scrollMode"), 2, scrollMode));
    Add(new cMenuEditStraItem(tr("Menu Items Scrolling Speed"), tmpConf->GetValueRef("menuScrollSpeed"), 4, scrollSpeed));
    Add(new cMenuEditIntItem(tr("Menu Items Scrolling Delay in s"), tmpConf->GetValueRef("menuScrollDelay"), 0, 3));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), tmpConf->GetValueRef("fontHeader"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Date"), tmpConf->GetValueRef("fontDate"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Color Buttons"), tmpConf->GetValueRef("fontButtons"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Messages"), tmpConf->GetValueRef("fontMessageMenu"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Text"), tmpConf->GetValueRef("fontDetailView"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Text Small"), tmpConf->GetValueRef("fontDetailViewSmall"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Header"), tmpConf->GetValueRef("fontDetailViewHeader"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Header Large"), tmpConf->GetValueRef("fontDetailViewHeaderLarge"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Main and Setup Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayMain::cNopacitySetupMenuDisplayMain(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Main and Setup Menu"), data) {
    titleStyle[0] = tr("\"VDR\" plus VDR version");
    titleStyle[1] = tr("only VDR version");
    titleStyle[2] = tr("no title");
    discUsageStyle[0] = tr("free time in hours");
    discUsageStyle[1] = tr("free space in GB");
    showTimers[0] = trVDR("no");
    showTimers[1] = trVDR("yes");
    showTimers[2] = tr("small without logo");
    Set();
}

void cNopacitySetupMenuDisplayMain::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditBoolItem(tr("Use narrow main menu"), tmpConf->GetValueRef("narrowMainMenu")));
    if (tmpConf->GetValue("narrowMainMenu"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthMain"), 10, 97));
    Add(new cMenuEditBoolItem(tr("Use narrow setup menu"), tmpConf->GetValueRef("narrowSetupMenu")));
    if (tmpConf->GetValue("narrowSetupMenu"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthSetup"), 10, 97));
    Add(new cMenuEditIntItem(tr("Number of entires per page"), tmpConf->GetValueRef("numMainMenuItems"), 3, 20));
    Add(new cMenuEditBoolItem(tr("Use menu icons"), tmpConf->GetValueRef("useMenuIcons")));
    Add(new cMenuEditStraItem(tr("Main menu title style"), tmpConf->GetValueRef("mainMenuTitleStyle"), 3, titleStyle));
    Add(new cMenuEditBoolItem(tr("Display Disk Usage"), tmpConf->GetValueRef("showDiscUsage")));
    if (tmpConf->GetValue("showDiscUsage")) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Size (square, Percent of OSD Width)")), tmpConf->GetValueRef("menuSizeDiskUsage"), 5, 30));
        Add(new cMenuEditStraItem(cString::sprintf("%s%s", *spacer, tr("Free Disc Display")), tmpConf->GetValueRef("discUsageStyle"), 2, discUsageStyle));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - free")), tmpConf->GetValueRef("fontDiskUsage"), -20, 20));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - percent")), tmpConf->GetValueRef("fontDiskUsagePercent"), -20, 20));
    }
    Add(new cMenuEditStraItem(tr("Display Timers"), tmpConf->GetValueRef("showTimers"), 3, showTimers));
    if (tmpConf->GetValue("showTimers")) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Maximum number of Timers")), tmpConf->GetValueRef("numberTimers"), 1, 10));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width of Timers (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthRightItems"), 5, 30));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width of Channel Logos (Percent of Timer Width)")), tmpConf->GetValueRef("timersLogoWidth"), 20, 98));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - Header")), tmpConf->GetValueRef("fontTimersHead"), -20, 20));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - Title")), tmpConf->GetValueRef("fontTimers"), -20, 20));
    }
    Add(new cMenuEditBoolItem(tr("Show Timer Conflicts"), tmpConf->GetValueRef("checkTimerConflict")));
    Add(new cMenuEditIntItem(tr("Header Logo Width"), tmpConf->GetValueRef("menuHeaderLogoWidth"), 30, 500));
    Add(new cMenuEditIntItem(tr("Header Logo Height"), tmpConf->GetValueRef("menuHeaderLogoHeight"), 30, 500));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Items"), tmpConf->GetValueRef("fontMenuitemLarge"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Schedules Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplaySchedules::cNopacitySetupMenuDisplaySchedules(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Schedules Menu"), data) {
    scrollSpeed[0] = tr("off");
    scrollSpeed[1] = tr("slow");
    scrollSpeed[2] = tr("medium");
    scrollSpeed[3] = tr("fast");
    windowMode[0] = tr("window");
    windowMode[1] = tr("full screen");
    Set();
}

void cNopacitySetupMenuDisplaySchedules::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Use narrow menu"), tmpConf->GetValueRef("narrowScheduleMenu")));
    if (tmpConf->GetValue("narrowScheduleMenu"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthSchedules"), 10, 97));
    Add(new cMenuEditIntItem(tr("Number of entires per page"), tmpConf->GetValueRef("numSchedulesMenuItems"), 3, 20));
    Add(new cMenuEditStraItem(tr("Mode of EPG Window"), tmpConf->GetValueRef("menuSchedulesWindowMode"), 2, windowMode));
    Add(new cMenuEditIntItem(tr("EPG Window Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("menuEPGWindowFadeTime"), 0, 1000));
    Add(new cMenuEditIntItem(tr("EPG Window Display Delay in s"), tmpConf->GetValueRef("menuInfoTextDelay"), 0, 10));
    Add(new cMenuEditIntItem(tr("EPG Window Scroll Delay in s"), tmpConf->GetValueRef("menuInfoScrollDelay"), 0, 10));
    Add(new cMenuEditStraItem(tr("EPG Window Text Scrolling Speed"), tmpConf->GetValueRef("menuInfoScrollSpeed"), 4, scrollSpeed));
    Add(new cMenuEditIntItem(tr("Height of EPG Info Window (Percent of OSD Height)"), tmpConf->GetValueRef("menuHeightInfoWindow"), 10, 100));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), tmpConf->GetValueRef("fontMenuitemSchedule"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), tmpConf->GetValueRef("fontMenuitemScheduleSmall"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Info Window"), tmpConf->GetValueRef("fontEPGInfoWindow"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Info Window Header"), tmpConf->GetValueRef("fontEPGInfoWindowLarge"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Channels Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayChannels::cNopacitySetupMenuDisplayChannels(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Channels Menu"), data) {
    displayModes[0] = tr("Transponder Information");
    displayModes[1] = tr("Current Schedule");
    displayModes[2] = tr("Plain Channels");
    Set();
}

void cNopacitySetupMenuDisplayChannels::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Use narrow menu"), tmpConf->GetValueRef("narrowChannelMenu")));
    if (tmpConf->GetValue("narrowChannelMenu"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthChannels"), 10, 97));
    Add(new cMenuEditStraItem(tr("Menu Items display mode"), tmpConf->GetValueRef("menuChannelDisplayMode"), 3, displayModes));
    if (tmpConf->GetValue("menuChannelDisplayMode") == 1) {
        Add(new cMenuEditBoolItem(cString::sprintf("%s%s", *spacer, tr("Display schedules with time info")), tmpConf->GetValueRef("menuChannelDisplayTime")));
    }
    Add(new cMenuEditIntItem(tr("Number of EPG Entries in Schedules Info Window"), tmpConf->GetValueRef("numEPGEntriesChannelsMenu"), 1, 100));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), tmpConf->GetValueRef("fontMenuitemChannel"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), tmpConf->GetValueRef("fontMenuitemChannelSmall"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Timers Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayTimers::cNopacitySetupMenuDisplayTimers(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Timers Menu"), data) {
    windowMode[0] = tr("window");
    windowMode[1] = tr("full screen");
    Set();
}

void cNopacitySetupMenuDisplayTimers::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Use narrow menu"), tmpConf->GetValueRef("narrowTimerMenu")));
    if (tmpConf->GetValue("narrowTimerMenu"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthTimers"), 10, 97));
    Add(new cMenuEditStraItem(tr("Mode of EPG Window"), tmpConf->GetValueRef("menuTimersWindowMode"), 2, windowMode));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), tmpConf->GetValueRef("fontMenuitemTimers"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), tmpConf->GetValueRef("fontMenuitemTimersSmall"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Recordings Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayRecordings::cNopacitySetupMenuDisplayRecordings(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Recordings Menu"), data) {
    displayEPGPictures[0] = tr("never");
    displayEPGPictures[1] = tr("always");
    displayEPGPictures[2] = tr("only if no tvscraper media available");
    windowMode[0] = tr("window");
    windowMode[1] = tr("full screen");
    Set();
}

void cNopacitySetupMenuDisplayRecordings::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Use narrow menu"), tmpConf->GetValueRef("narrowRecordingMenu")));
    if (tmpConf->GetValue("narrowRecordingMenu"))
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), tmpConf->GetValueRef("menuWidthRecordings"), 10, 97));
    Add(new cMenuEditIntItem(tr("Number of entires per page"), tmpConf->GetValueRef("numRecordingsMenuItems"), 3, 20));
    Add(new cMenuEditStraItem(tr("Mode of recording Window"), tmpConf->GetValueRef("menuRecordingsWindowMode"), 2, windowMode));
    Add(new cMenuEditIntItem(tr("Border around detailed recording view"), tmpConf->GetValueRef("borderDetailedRecordings"), 1, 300));
    Add(new cMenuEditBoolItem(tr("Use folder poster if available"), tmpConf->GetValueRef("useFolderPoster")));
    Add(new cMenuEditIntItem(tr("Width of manually set recording poster"), tmpConf->GetValueRef("posterWidth"), 100, 1000));
    Add(new cMenuEditIntItem(tr("Height of manually set recording poster"), tmpConf->GetValueRef("posterHeight"), 100, 1000));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), tmpConf->GetValueRef("fontMenuitemRecordings"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), tmpConf->GetValueRef("fontMenuitemRecordingsSmall"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Detailed EPG & Recordings View -------------------------------------------------------------------------------------------------------------

cNopacitySetupDetailedView::cNopacitySetupDetailedView(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Detailed EPG & Recordings View"), data) {
    useSubtitleRerunTexts[0] = tr("never");
    useSubtitleRerunTexts[1] = tr("if exists");
    useSubtitleRerunTexts[2] = tr("always");
    Set();
}

void cNopacitySetupDetailedView::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Border around view"), tmpConf->GetValueRef("borderDetailedEPG"), 1, 300));
    Add(new cMenuEditIntItem(tr("Scroll Speed with up / down (number of lines)"), tmpConf->GetValueRef("detailedViewScrollStep"), 1, 30));
    Add(new cMenuEditIntItem(tr("Header Height detailed EPG view (Perc. of OSD Height)"), tmpConf->GetValueRef("headerDetailedEPG"), 10, 50));
    Add(new cMenuEditIntItem(tr("Header Height detailed recording view (Perc. of OSD Height)"), tmpConf->GetValueRef("headerDetailedRecordings"), 10, 50));
    Add(new cMenuEditIntItem(tr("Number of reruns to display"), tmpConf->GetValueRef("numReruns"), 1, 10));
    Add(new cMenuEditStraItem(tr("Use Subtitle for reruns"), tmpConf->GetValueRef("useSubtitleRerun"), 3, useSubtitleRerunTexts));
    Add(new cMenuEditIntItem(tr("EPG Image Width"), tmpConf->GetValueRef("epgImageWidth"), 30, 500));
    Add(new cMenuEditIntItem(tr("EPG Image Height"), tmpConf->GetValueRef("epgImageHeight"), 30, 500));
    Add(new cMenuEditIntItem(tr("Large EPG Image Width"), tmpConf->GetValueRef("epgImageWidthLarge"), 100, 800));
    Add(new cMenuEditIntItem(tr("Large EPG Image Height"), tmpConf->GetValueRef("epgImageHeightLarge"), 100, 800));

    SetCurrent(Get(currentItem));
    Display();
}

//----ChannelDisplay--------------------------------------------------------------------------------------------------------------

cNopacitySetupChannelDisplay::cNopacitySetupChannelDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Channel Switching"), data) {
    bgStyle[0] = tr("transparent channel logo");
    bgStyle[1] = tr("full osd width");
    logoPos[0] = tr("do not display");
    logoPos[1] = tr("left");
    logoPos[2] = tr("right");
    logoVerticalPos[0] = tr("top");
    logoVerticalPos[1] = tr("middle");
    logoVerticalPos[2] = tr("bottom");
    progressStyleCurrentSchedule[0] = tr("show elapsed time");
    progressStyleCurrentSchedule[1] = tr("show remaining time");
    Set();
}

void cNopacitySetupChannelDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("channelFadeTime"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Height of Channel Display (Percent of OSD Height)"), tmpConf->GetValueRef("channelHeight"), 15, 80));
    Add(new cMenuEditIntItem(tr("Left & Right Border Width"), tmpConf->GetValueRef("channelBorderVertical"), 0, 300));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), tmpConf->GetValueRef("channelBorderBottom"), 0, 300));
    Add(new cMenuEditIntItem(tr("Background Transparency in Percent"), tmpConf->GetValueRef("channelBackgroundTransparency"), 0, 80));
    if (config.GetValue("displayType") != dtGraphical) {
        Add(new cMenuEditStraItem(tr("Background Style"), tmpConf->GetValueRef("backgroundStyle"), 2, bgStyle));
        Add(new cMenuEditBoolItem(tr("Rounded Corners"), tmpConf->GetValueRef("roundedCornersChannel")));
    }
    Add(new cMenuEditStraItem(tr("Vertical Channel Logo Alignment"), tmpConf->GetValueRef("logoVerticalAlignment"), 3, logoVerticalPos));
    Add(new cMenuEditStraItem(tr("Channel Logo Position"), tmpConf->GetValueRef("logoPosition"), 3, logoPos));
    if (tmpConf->GetValue("logoPosition")) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo original Width")), tmpConf->GetValueRef("logoWidthOriginal"), 30, 500));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo original Height")), tmpConf->GetValueRef("logoHeightOriginal"), 30, 500));
        Add(new cMenuEditBoolItem(cString::sprintf("%s%s", *spacer, tr("Use Channel Logo background")), tmpConf->GetValueRef("channelUseLogoBackground")));
    }
    Add(new cMenuEditStraItem(tr("Kind of time display for current schedule"), tmpConf->GetValueRef("progressCurrentSchedule"), 2, progressStyleCurrentSchedule));
    Add(new cMenuEditBoolItem(tr("Display Signal Strength & Quality"), tmpConf->GetValueRef("displaySignalStrength")));
    Add(new cMenuEditBoolItem(tr("Display Channel Source & Rec. Info"), tmpConf->GetValueRef("displaySourceInfo")));
    Add(new cMenuEditBoolItem(tr("Display Poster or Fanart from TVScraper"), tmpConf->GetValueRef("displayPoster")));
    if (tmpConf->GetValue("displayPoster")) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Border in Pixel")), tmpConf->GetValueRef("channelPosterBorder"), 0, 200));
    }
    Add(new cMenuEditBoolItem(tr("Display previous and next Channel Group"), tmpConf->GetValueRef("displayPrevNextChannelGroup")));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), tmpConf->GetValueRef("fontChannelHeaderSize"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Date"), tmpConf->GetValueRef("fontChannelDateSize"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Text"), tmpConf->GetValueRef("fontEPGSize"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Infotext"), tmpConf->GetValueRef("fontEPGSmallSize"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Channel Source Info"), tmpConf->GetValueRef("fontChannelSourceInfoSize"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Channel Group"), tmpConf->GetValueRef("fontChannelGroupSize"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Next/Prev Channel Group"), tmpConf->GetValueRef("fontChannelGroupSmallSize"), -20, 20));
    SetCurrent(Get(currentItem));
    Display();
}

//-----ReplayDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupReplayDisplay::cNopacitySetupReplayDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Replay"), data) {
    Set();
}

void cNopacitySetupReplayDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("replayFadeTime"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Height of Replay Display (Percent of OSD Height)"), tmpConf->GetValueRef("replayHeight"), 15, 80));
    Add(new cMenuEditIntItem(tr("Left & Right Border Width"), tmpConf->GetValueRef("replayBorderVertical"), 0, 300));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), tmpConf->GetValueRef("replayBorderBottom"), 0, 300));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), tmpConf->GetValueRef("fontReplayHeader"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Text"), tmpConf->GetValueRef("fontReplay"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----TrackDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupTrackDisplay::cNopacitySetupTrackDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Audio Tracks"), data) {
    Set();
}

void cNopacitySetupTrackDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("tracksFadeTime"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Width of Tracks Display (Percent of OSD Width)"), tmpConf->GetValueRef("tracksWidth"), 10, 100));
    Add(new cMenuEditIntItem(tr("Height of Track Items (in pixels)"), tmpConf->GetValueRef("tracksItemHeight"), 30, 200));
    Add(new cMenuEditIntItem(tr("Position (0: bot. center, 1: bot. left, ... , 7: bot. right)"), tmpConf->GetValueRef("tracksPosition"), 0, 7));
    Add(new cMenuEditIntItem(tr("Border Left / Right"), tmpConf->GetValueRef("tracksBorderHorizontal"), 0, 100));
    Add(new cMenuEditIntItem(tr("Border Top / Bottom"), tmpConf->GetValueRef("tracksBorderVertical"), 0, 100));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), tmpConf->GetValueRef("fontTracksHeader"), -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Buttons"), tmpConf->GetValueRef("fontTracks"), -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MessageDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupMessageDisplay::cNopacitySetupMessageDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Messages"), data) {
    Set();
}

void cNopacitySetupMessageDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("messageFadeTime"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Width of Message Display (Percent of OSD Height)"), tmpConf->GetValueRef("messageWidth"), 30, 100));
    Add(new cMenuEditIntItem(tr("Height of Message Display (Percent of OSD Height)"), tmpConf->GetValueRef("messageHeight"), 5, 100));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), tmpConf->GetValueRef("messageBorderBottom"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Adjust Font Size"), tmpConf->GetValueRef("fontMessage"), -30, 30));

    SetCurrent(Get(currentItem));
    Display();
}

//-----VolumeDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupVolumeDisplay::cNopacitySetupVolumeDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Volume Display"), data) {
    Set();
}

void cNopacitySetupVolumeDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), tmpConf->GetValueRef("volumeFadeTime"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Width of Volume Display (Percent of OSD Height)"), tmpConf->GetValueRef("volumeWidth"), 10, 100));
    Add(new cMenuEditIntItem(tr("Height of Volume Display (Percent of OSD Height)"), tmpConf->GetValueRef("volumeHeight"), 5, 100));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), tmpConf->GetValueRef("volumeBorderBottom"), 0, 1000));
    Add(new cMenuEditIntItem(tr("Adjust Font Size"), tmpConf->GetValueRef("fontVolume"), -30, 30));

    SetCurrent(Get(currentItem));
    Display();
}

//-----Image Caching-------------------------------------------------------------------------------------------------------------

cNopacitySetupCaching::cNopacitySetupCaching(cNopacityConfig* data, cImageCache *imgCache)  : cMenuSetupSubMenu(tr("Image Caching"), data) {
    this->imgCache = imgCache;
    Set();
}

void cNopacitySetupCaching::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Limit Logo Cache"), tmpConf->GetValueRef("limitLogoCache")));
    if (tmpConf->GetValue("limitLogoCache")) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Maximal number of logos to cache")), tmpConf->GetValueRef("numLogosMax"), 1, 9999));
    }
    Add(new cMenuEditIntItem(tr("Number of  logos to cache at start"), tmpConf->GetValueRef("numLogosInitial"), 0, 9999));

    Add(InfoItem(tr("Cache Sizes"), ""));
    Add(InfoItem(tr("Menu Icon cache"), (imgCache->GetCacheSize(ctMenuIcon)).c_str()));
    Add(InfoItem(tr("Skin Icon image cache"), (imgCache->GetCacheSize(ctSkinIcon)).c_str()));
    Add(InfoItem(tr("Logo cache"), (imgCache->GetCacheSize(ctLogo)).c_str()));
    Add(InfoItem(tr("Menu Item Logo cache"), (imgCache->GetCacheSize(ctLogoMenuItem)).c_str()));
    Add(InfoItem(tr("Timer Logo cache"), (imgCache->GetCacheSize(ctLogoTimer)).c_str()));
    Add(InfoItem(tr("Background Images cache"), (imgCache->GetCacheSize(ctSkinElement)).c_str()));

    SetCurrent(Get(currentItem));
    Display();
}