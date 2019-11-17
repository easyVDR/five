#include "tools.h"
#include "services/scraper2vdr.h"
#include "imageloader.h"
#include "statusheader.h"

cStatusHeader::cStatusHeader(void) {
    color = theme.Color(clrStatusHeader);
    colorBlending = theme.Color(clrStatusHeaderBlending);
    height = geoManager.statusHeaderHeight;
    width = geoManager.headerContentWidth;
    tvFrameWidth = geoManager.tvFrameWidth;
    pixmap = osdManager.requestPixmap(1, cRect(0, 0, width, height));
    pixmapText = osdManager.requestPixmap(2, cRect(0, 0, width, height));
    pixmapTVFrame = osdManager.requestPixmap(1, cRect(width, 0, tvFrameWidth, height));
}

cStatusHeader::~cStatusHeader(void) {
    osdManager.releasePixmap(pixmapText);
    osdManager.releasePixmap(pixmapTVFrame);
    if (tvguideConfig.scaleVideo) {
        cRect vidWin = cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
        cDevice::PrimaryDevice()->ScaleVideo(vidWin);
    }
}

void cStatusHeader::Draw(void) {
    pixmapText->Fill(clrTransparent);
    pixmapTVFrame->Fill(clrTransparent);
    if (tvguideConfig.style == eStyleGraphical) {
        if (tvguideConfig.scaleVideo) {
            drawBackgroundGraphical(bgStatusHeaderWindowed);
            cImage *tvFrameBack = imgCache.GetOsdElement(oeStatusHeaderTVFrame);
            if (tvFrameBack)
                pixmapTVFrame->DrawImage(cPoint(0,0), *tvFrameBack);
        } else {
            drawBackgroundGraphical(bgStatusHeaderFull);
        }
    } else {
        if (tvguideConfig.decorateVideo) {
            DecorateVideoFrame();
        }
        drawBackground();
        drawBorder();
    }
}

void cStatusHeader::ScaleVideo(void) {
    if (tvguideConfig.scaleVideo) {
        int width = height * 16 / 9;
        int x = osdManager.Left() + geoManager.osdWidth - width;
        int y = osdManager.Top();
        cRect availableRect(x, y, width, height);
        cRect vidWin = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
        cDevice::PrimaryDevice()->ScaleVideo(vidWin);
    }
}

void cStatusHeader::DrawInfoText(cGrid *grid) {
    int border = 10;
    int textWidth = width - 2 * border - geoManager.clockWidth - 2;
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
    pixmapText->Fill(clrTransparent);
    int x = border;
    int y = border;
    if (!grid->isDummy()) {
        const cEvent *event = grid->GetEvent();
        int newX = DrawPoster(event, x, y, height-2*border, border);
        if (newX > 0) {
            textWidth -= (newX - x);
            x += newX;
        }
        cString time = grid->getTimeString();
        cString title("");
        title = cString::sprintf(": %s", event->Title());
        cString header = cString::sprintf("%s%s", *time, *title);
        header = CutText(*header, textWidth, fontManager.FontStatusHeaderLarge).c_str();
        pixmapText->DrawText(cPoint(x,y), *header, theme.Color(clrFont), colorTextBack, fontManager.FontStatusHeaderLarge);
        y += fontManager.FontStatusHeaderLarge->Height() + border;
        int heightText = pixmapText->ViewPort().Height() - y;
        cTextWrapper description;
        description.Set(event->Description(), fontManager.FontStatusHeader, textWidth);
        int lineHeight = fontManager.FontStatusHeader->Height();
        int textLines = description.Lines();
        int maxLines = heightText / lineHeight;
        int lines = min(textLines, maxLines);
        for (int i = 0; i < lines-1; i++) {
            pixmapText->DrawText(cPoint(x,y), description.GetLine(i), theme.Color(clrFont), colorTextBack, fontManager.FontStatusHeader);
            y += lineHeight;
        }
        cString lastLine = description.GetLine(lines-1);
        if (textLines > maxLines) {
            lastLine = cString::sprintf("%s...", *lastLine);
        }
        pixmapText->DrawText(cPoint(x,y), *lastLine, theme.Color(clrFont), colorTextBack, fontManager.FontStatusHeader);
    } else {
        int heightText = pixmapText->ViewPort().Height() - y;
        y += (heightText - fontManager.FontStatusHeaderLarge->Height() - 2*border)/2;
        pixmapText->DrawText(cPoint(x,y), *grid->getText(), theme.Color(clrFont), colorTextBack, fontManager.FontStatusHeaderLarge);
    }
}

int cStatusHeader::DrawPoster(const cEvent *event, int x, int y, int height, int border) {
    bool hasPoster = false;
    ScraperGetPoster posterScraper2Vdr;
    int posterWidth = 0;
    int posterHeight = 0;
    static cPlugin *pScraper2Vdr = cPluginManager::GetPlugin("scraper2vdr");
    if (pScraper2Vdr) {
        posterScraper2Vdr.event = event;
        posterScraper2Vdr.recording = NULL;
        if (pScraper2Vdr->Service("GetPoster", &posterScraper2Vdr)) {
            hasPoster = true;
            int posterWidthOrig = posterScraper2Vdr.poster.width;
            int posterHeightOrig = posterScraper2Vdr.poster.height;
            posterHeight = height;
            posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
        } else {
            hasPoster = false;
        }
    }
    if (hasPoster) {
        cImageLoader imgLoader;
        if (imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
            pixmapText->DrawImage(cPoint(x, y), imgLoader.GetImage());
            return posterWidth + border;
        }
    }
    return 0;
}

void cStatusHeader::DecorateVideoFrame(void) {
    int radius = 16;
    int frame = 2;
    pixmapTVFrame->DrawRectangle(cRect(0, 0, tvFrameWidth, frame), theme.Color(clrBackgroundOSD));
    pixmapTVFrame->DrawEllipse(cRect(frame,frame,radius,radius), theme.Color(clrBackgroundOSD), -2);
    pixmapTVFrame->DrawRectangle(cRect(tvFrameWidth - frame, frame, frame, height - 2*frame), theme.Color(clrBackgroundOSD));      
    pixmapTVFrame->DrawEllipse(cRect(tvFrameWidth - radius - frame, frame, radius, radius), theme.Color(clrBackgroundOSD), -1);
    pixmapTVFrame->DrawRectangle(cRect(0, frame, frame, height - 2*frame), theme.Color(clrBackgroundOSD)); 
    pixmapTVFrame->DrawEllipse(cRect(frame, height - radius - frame, radius, radius), theme.Color(clrBackgroundOSD), -3);
    pixmapTVFrame->DrawRectangle(cRect(0, height - frame, tvFrameWidth, frame), theme.Color(clrBackgroundOSD));
    pixmapTVFrame->DrawEllipse(cRect(tvFrameWidth - radius - frame, height - radius - frame, radius, radius), theme.Color(clrBackgroundOSD), -4);
}