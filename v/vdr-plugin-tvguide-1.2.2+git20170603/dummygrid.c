#include "dummygrid.h"

cDummyGrid::cDummyGrid(cChannelColumn *c, time_t start, time_t end) : cGrid(c) {
    this->start = start;
    this->end = end;
    strText = tr("No EPG Information available");
    dummy = true;
    hasTimer = false;
}

cDummyGrid::~cDummyGrid(void) {
}

time_t cDummyGrid::Duration(void) { 
    //max Duration 5h
    if (end - start > 18000)
        return 18000;
    return (end - start); 
};

void cDummyGrid::SetViewportHeight() {
    int viewportHeightOld = viewportHeight;
    viewportHeight = Duration() / 60 * geoManager.minutePixel;
    if (viewportHeight != viewportHeightOld)
        dirty = true;
}

void cDummyGrid::PositionPixmap() {
    int x0, y0;
    if (tvguideConfig.displayMode == eVertical) {
        x0 = column->getX();
        y0 = geoManager.statusHeaderHeight + geoManager.channelHeaderHeight + geoManager.channelGroupsHeight;
        if ( column->Start() < StartTime() ) {
            y0 += (StartTime() - column->Start())/60*geoManager.minutePixel;
        }
        if (!pixmap) {
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, geoManager.colWidth, viewportHeight));
        } else if (dirty) {
            osdManager.releasePixmap(pixmap);
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, geoManager.colWidth, viewportHeight));
        } else {
            pixmap->SetViewPort(cRect(x0, y0, geoManager.colWidth, viewportHeight));
        }
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x0 = geoManager.channelHeaderWidth + geoManager.channelGroupsWidth;
        y0 = column->getY();
        if ( column->Start() < StartTime() ) {
            x0 += (StartTime() - column->Start())/60*geoManager.minutePixel;
        }
        if (!pixmap) {
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, viewportHeight, geoManager.rowHeight));
        } else if (dirty) {
            osdManager.releasePixmap(pixmap);
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, viewportHeight, geoManager.rowHeight));
        } else {
            pixmap->SetViewPort(cRect(x0, y0, viewportHeight, geoManager.rowHeight));
        }
    }
}

void cDummyGrid::setText() {
    if (tvguideConfig.displayMode == eVertical) {
        text->Set(*strText, fontManager.FontGrid, geoManager.colWidth-2*borderWidth);
    }
}

void cDummyGrid::drawText() {
    tColor colorText = (active)?theme.Color(clrFontActive):theme.Color(clrFont);
    tColor colorTextBack;
    if (tvguideConfig.style == eStyleFlat)
        colorTextBack = color;
    else if (tvguideConfig.style == eStyleGraphical)
        colorTextBack = (active)?theme.Color(clrGridActiveFontBack):theme.Color(clrGridFontBack);
    else
        colorTextBack = clrTransparent;
    if (tvguideConfig.displayMode == eVertical) {
        if (Height()/geoManager.minutePixel < 6)
            return;
        int textHeight = fontManager.FontGrid->Height();
        int textLines = text->Lines();
        for (int i=0; i<textLines; i++) {
            pixmap->DrawText(cPoint(borderWidth, borderWidth + i*textHeight), text->GetLine(i), colorText, colorTextBack, fontManager.FontGrid);
        
        }
    } else if (tvguideConfig.displayMode == eHorizontal) {
        if (Width()/geoManager.minutePixel < 10) {
            int titleY = (geoManager.rowHeight - fontManager.FontGridHorizontal->Height())/2;
            pixmap->DrawText(cPoint(borderWidth - 2, titleY), "...", colorText, colorTextBack, fontManager.FontGridHorizontal);
            return;
        }
        int titleY = (geoManager.rowHeight - fontManager.FontGridHorizontal->Height())/2;
        pixmap->DrawText(cPoint(borderWidth, titleY), *strText, colorText, colorTextBack, fontManager.FontGridHorizontal);
    }
}
cString cDummyGrid::getText(void) {
    return strText;
}

cString cDummyGrid::getTimeString(void) {
    return cString::sprintf("%s - %s", *TimeString(start), *TimeString(end));
}

void cDummyGrid::debug() {
    esyslog("tvguide dummygrid: %s: %s, %s, viewportHeight: %d px, Duration: %ld min, active: %d", 
                column->Name(),
                *cMyTime::printTime(start), 
                *cMyTime::printTime(end), 
                viewportHeight, 
                Duration()/60,
                active);
}