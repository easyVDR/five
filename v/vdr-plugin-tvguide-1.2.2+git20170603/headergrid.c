#include "imageloader.h"
#include "tools.h"
#include "headergrid.h"

cHeaderGrid::cHeaderGrid(void) : cGrid(NULL) {
    pixmap = NULL;
    pixmapLogo = NULL;
}

cHeaderGrid::~cHeaderGrid(void) {
    osdManager.releasePixmap(pixmapLogo);
}

void cHeaderGrid::createBackground(int num) {
    color = theme.Color(clrHeader);
    colorBlending = theme.Color(clrHeaderBlending);
    int x, y;
    if (tvguideConfig.displayMode == eVertical) {
        x = geoManager.timeLineWidth + num*geoManager.colWidth;
        y = geoManager.statusHeaderHeight + geoManager.channelGroupsHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x = geoManager.channelGroupsWidth;
        y = geoManager.statusHeaderHeight + geoManager.timeLineHeight + num*geoManager.rowHeight;
    }
    pixmap = osdManager.requestPixmap(1, cRect(x, y, geoManager.channelLogoWidth, geoManager.channelLogoHeight));
    pixmapLogo = osdManager.requestPixmap(2, cRect(x, y, geoManager.channelLogoWidth, geoManager.channelLogoHeight));
    if ((!pixmap) || (!pixmapLogo)){
        return;
    }
    pixmapLogo->Fill(clrTransparent);
    if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgChannelHeader);
    } else {
        drawBackground();
        drawBorder();
    }
}

void cHeaderGrid::drawChannel(const cChannel *channel) {
    if (tvguideConfig.displayMode == eVertical) {
        drawChannelVertical(channel);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        drawChannelHorizontal(channel);
    }
}

void cHeaderGrid::drawChannelHorizontal(const cChannel *channel) {
    int logoWidth = geoManager.logoWidth;
    int logoX = tvguideConfig.displayChannelName?2:(Width()-logoWidth)/2;
    int textX = 5;
    int textY = (Height() - fontManager.FontChannelHeaderHorizontal->Height())/2;
    bool logoFound = false;
    if (!tvguideConfig.hideChannelLogos) {
        cImage *logo = imgCache.GetLogo(channel);
        if (logo) {
            pixmapLogo->DrawImage(cPoint(logoX, 0), *logo);
            logoFound = true;
        }
    }
    bool drawText = false;
    int textWidthMax = Width() - 10;
    if (!logoFound) {
        drawText = true;
    } else  if (tvguideConfig.displayChannelName) {
        drawText = true;
        textX += logoWidth;
        textWidthMax -= logoWidth;
    }
    if (drawText) {
        tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
        cString strChannel = cString::sprintf("%d %s", channel->Number(), channel->Name());
        strChannel = CutText(*strChannel, textWidthMax, fontManager.FontChannelHeaderHorizontal).c_str();
        pixmap->DrawText(cPoint(textX, textY), *strChannel, theme.Color(clrFontHeader), colorTextBack, fontManager.FontChannelHeaderHorizontal);
    }
}

void cHeaderGrid::drawChannelVertical(const cChannel *channel) {
    int logoWidth = geoManager.logoWidth;
    int logoHeight = geoManager.logoHeight;
    cTextWrapper tw;
    cString headerText = cString::sprintf("%d - %s", channel->Number(), channel->Name());
    tw.Set(*headerText, fontManager.FontChannelHeader, geoManager.colWidth - 8);
    int lines = tw.Lines();
    int lineHeight = fontManager.FontChannelHeader->Height();
    int yStart = (geoManager.channelHeaderHeight - lines*lineHeight)/2 + 8;
    bool logoFound = false;
    if (!tvguideConfig.hideChannelLogos) {
        cImage *logo = imgCache.GetLogo(channel);
        if (logo) {
                pixmapLogo->DrawImage(cPoint((Width() - logoWidth)/2, 4), *logo);
                logoFound = true;
        }
    }
    bool drawText = false;
    if (!logoFound) {
        drawText = true;
    } else if (tvguideConfig.displayChannelName) {
        drawText = true;
        yStart = logoHeight;
    }
    if (!drawText)
        return;
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
    for (int i=0; i<lines; i++) {
        int textWidth = fontManager.FontChannelHeader->Width(tw.GetLine(i));
        int xText = (geoManager.colWidth - textWidth) / 2;
        if (xText < 0) 
            xText = 0;
        pixmap->DrawText(cPoint(xText, yStart + i*lineHeight), tw.GetLine(i), theme.Color(clrFontHeader), colorTextBack, fontManager.FontChannelHeader);
    }
}

void cHeaderGrid::setPosition(int num) {
    int x, y, width, height;
    if (tvguideConfig.displayMode == eVertical) {
        x = geoManager.timeLineWidth + num*geoManager.colWidth;
        y = geoManager.statusHeaderHeight + geoManager.channelGroupsHeight;
        width = geoManager.colWidth;
        height = geoManager.channelHeaderHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x = geoManager.channelGroupsWidth;
        y = geoManager.statusHeaderHeight + geoManager.timeLineHeight + num*geoManager.rowHeight;
        width = geoManager.channelHeaderWidth;
        height = geoManager.rowHeight;
    }
    pixmap->SetViewPort(cRect(x, y, width, height));
    pixmapLogo->SetViewPort(cRect(x, y, width, height));
}
