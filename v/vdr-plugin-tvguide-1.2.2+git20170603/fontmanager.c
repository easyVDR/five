#include "geometrymanager.h"
#include "config.h"
#include "fontmanager.h"

cFontManager::cFontManager() {
}

cFontManager::~cFontManager() {
    DeleteFonts();
}

void cFontManager::SetFonts() {
    InitialiseFontType();
    //Common Fonts
    FontButton = CreateFont(geoManager.footerHeight/3 + 4 + tvguideConfig.FontButtonDelta);
    FontDetailView = CreateFont(geoManager.osdHeight/30 + tvguideConfig.FontDetailViewDelta);
    FontDetailViewSmall = CreateFont(geoManager.osdHeight/40 + tvguideConfig.FontDetailViewSmallDelta);
    FontDetailHeader = CreateFont(geoManager.osdHeight/27 + tvguideConfig.FontDetailHeaderDelta);
    FontDetailHeaderLarge = CreateFont(geoManager.osdHeight/20 + tvguideConfig.FontDetailHeaderDelta);
    FontMessageBox = CreateFont(geoManager.osdHeight/33 + tvguideConfig.FontMessageBoxDelta);
    FontMessageBoxLarge = CreateFont(geoManager.osdHeight/30 + tvguideConfig.FontMessageBoxLargeDelta);    
    FontStatusHeader = CreateFont(geoManager.statusHeaderHeight/6 - 4 + tvguideConfig.FontStatusHeaderDelta);
    FontStatusHeaderLarge = CreateFont(geoManager.statusHeaderHeight/5 + tvguideConfig.FontStatusHeaderLargeDelta);
    //Fonts for vertical Display 
    FontChannelHeader = CreateFont(geoManager.colWidth/10 + tvguideConfig.FontChannelHeaderDelta);
    FontChannelGroups = CreateFont(geoManager.colWidth/8 + tvguideConfig.FontChannelGroupsDelta);
    FontGrid = CreateFont(geoManager.colWidth/12 + tvguideConfig.FontGridDelta);
    FontGridSmall = CreateFont(geoManager.colWidth/12 + tvguideConfig.FontGridSmallDelta);
    FontTimeLineWeekday = CreateFont(geoManager.timeLineWidth/3 + tvguideConfig.FontTimeLineWeekdayDelta);
    FontTimeLineDate = CreateFont(geoManager.timeLineWidth/4 + tvguideConfig.FontTimeLineDateDelta);
    FontTimeLineTime = CreateFont(geoManager.timeLineWidth/4 + tvguideConfig.FontTimeLineTimeDelta);
    //Fonts for horizontal Display 
    FontChannelHeaderHorizontal = CreateFont(geoManager.rowHeight/3 + tvguideConfig.FontChannelHeaderHorizontalDelta);
    FontChannelGroupsHorizontal = CreateFont(geoManager.rowHeight/3 + 5 + tvguideConfig.FontChannelGroupsHorizontalDelta);
    FontGridHorizontal = CreateFont(geoManager.rowHeight/3 + 5 + tvguideConfig.FontGridHorizontalDelta);
    FontGridHorizontalSmall = CreateFont(geoManager.rowHeight/4 + tvguideConfig.FontGridHorizontalSmallDelta);
    FontTimeLineDateHorizontal = CreateFont(geoManager.timeLineHeight/2 + 5 + tvguideConfig.FontTimeLineDateHorizontalDelta);
    FontTimeLineTimeHorizontal = CreateFont(geoManager.timeLineHeight/2 + tvguideConfig.FontTimeLineTimeHorizontalDelta);
    //Fonts for RecMenu
    FontRecMenuItem = CreateFont(geoManager.osdHeight/30 + tvguideConfig.FontRecMenuItemDelta);
    FontRecMenuItemSmall = CreateFont(geoManager.osdHeight/40 + tvguideConfig.FontRecMenuItemSmallDelta);
    FontRecMenuItemLarge = CreateFont(geoManager.osdHeight/25 + tvguideConfig.FontRecMenuItemLargeDelta);
}

void cFontManager::DeleteFonts() {
    delete FontButton;
    delete FontDetailView;
    delete FontDetailViewSmall;
    delete FontDetailHeader;
    delete FontDetailHeaderLarge;
    delete FontMessageBox;
    delete FontMessageBoxLarge;
    delete FontStatusHeader;
    delete FontStatusHeaderLarge;
    delete FontChannelHeader;
    delete FontChannelGroups;
    delete FontGrid;
    delete FontGridSmall;
    delete FontTimeLineWeekday;
    delete FontTimeLineDate;
    delete FontTimeLineTime;
    delete FontChannelHeaderHorizontal;
    delete FontChannelGroupsHorizontal;
    delete FontGridHorizontal;
    delete FontGridHorizontalSmall;
    delete FontTimeLineDateHorizontal;
    delete FontTimeLineTimeHorizontal;
    delete FontRecMenuItem;
    delete FontRecMenuItemSmall;
    delete FontRecMenuItemLarge;
}

void cFontManager::InitialiseFontType(void) {
    if (tvguideConfig.fontIndex == 0) {
        fontName = tvguideConfig.fontNameDefault;
    } else {
        cStringList availableFonts;
        cFont::GetAvailableFontNames(&availableFonts);
        if (availableFonts[tvguideConfig.fontIndex-1]) {
            fontName = availableFonts[tvguideConfig.fontIndex-1];
        } else 
            fontName = tvguideConfig.fontNameDefault;
    }
    cFont *test = NULL;
    test = cFont::CreateFont(*fontName, 30);
    if (!test) {
        fontName = DefaultFontSml;
    }
    delete test;
    esyslog("tvguide: Set Font to %s", *fontName);
}

cFont *cFontManager::CreateFont(int size) {
    return cFont::CreateFont(*fontName, size);
}

