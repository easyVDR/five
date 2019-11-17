#include <vdr/osd.h>

#include "config.h"
#include "geometrymanager.h"

cGeometryManager::cGeometryManager() {
    osdWidth = 0;
    osdHeight = 0;
}

cGeometryManager::~cGeometryManager() {
}

bool cGeometryManager::SetGeometry(int osdWidth, int osdHeight, bool force) {
	if (!force && (this->osdWidth == osdWidth) && (this->osdHeight == osdHeight)) {
		esyslog("tvgudie: GeoManager SetGeometry nothing to change");
		return false;
	}
	this->osdWidth = osdWidth;
	this->osdHeight = osdHeight;
	esyslog("tvguide: Set OSD to %d x %d px", osdWidth, osdHeight);

	statusHeaderHeight = (tvguideConfig.displayStatusHeader)?(tvguideConfig.headerHeightPercent * osdHeight / 100):0;
    tvFrameWidth = statusHeaderHeight * 16 / 9;
    headerContentWidth = (tvguideConfig.scaleVideo)?(osdWidth -  tvFrameWidth):osdWidth;
    channelGroupsWidth = (tvguideConfig.displayChannelGroups)?(tvguideConfig.channelGroupsPercent * osdWidth / 100):0;
    channelGroupsHeight = (tvguideConfig.displayChannelGroups)?(tvguideConfig.channelGroupsPercent * osdHeight / 100):0;
    channelHeaderWidth = tvguideConfig.channelHeaderWidthPercent * osdWidth / 100;
    channelHeaderHeight = tvguideConfig.channelHeaderHeightPercent * osdHeight / 100;
    timeLineWidth = tvguideConfig.timeLineWidthPercent * osdWidth / 100;
    timeLineHeight = tvguideConfig.timeLineHeightPercent * osdHeight / 100;
    clockWidth = tvFrameWidth / 3;
    clockHeight = timeLineHeight;
    footerHeight = tvguideConfig.footerHeightPercent * osdHeight / 100;
    footerY = osdHeight - footerHeight;

    if (tvguideConfig.displayMode == eVertical) {
        colWidth = (osdWidth - timeLineWidth) / tvguideConfig.channelCols;
        rowHeight = 0;
        minutePixel = (osdHeight - statusHeaderHeight - channelGroupsHeight - channelHeaderHeight - footerHeight) / tvguideConfig.displayTime;
        channelLogoWidth = colWidth;
        channelLogoHeight = channelHeaderHeight;
        logoWidth = channelLogoWidth/2 - 15;
        logoHeight = logoWidth * tvguideConfig.logoHeightRatio / tvguideConfig.logoWidthRatio;
        timeLineGridWidth = timeLineWidth;
        timeLineGridHeight = minutePixel*30;
        dateVieverWidth = timeLineWidth;
        dateVieverHeight = channelHeaderHeight + channelGroupsHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        colWidth = 0;
        rowHeight = (osdHeight - statusHeaderHeight - timeLineHeight - footerHeight) / tvguideConfig.channelRows;
        minutePixel = (osdWidth - channelHeaderWidth - channelGroupsWidth) / tvguideConfig.displayTime;
        channelLogoWidth = channelHeaderWidth;
        channelLogoHeight = rowHeight;
        logoWidth = channelLogoHeight * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
        logoHeight = channelLogoHeight;
        timeLineGridWidth = geoManager.minutePixel*30;
        timeLineGridHeight = geoManager.timeLineHeight;
        dateVieverWidth = channelHeaderWidth + channelGroupsWidth;
        dateVieverHeight = timeLineHeight;
    }
    buttonBorder = footerHeight / 6;
    buttonWidth = osdWidth / 4 - 2 * buttonBorder;
    buttonHeight = footerHeight - 3 * buttonBorder;

    epgViewHeaderHeight = tvguideConfig.headerHeightPercent * osdHeight / 100;

    borderRecMenus = 10;

    channelJumpWidth = osdWidth * 30 / 100;
    channelJumpHeight = osdHeight * 20 / 100;

    return true;
}