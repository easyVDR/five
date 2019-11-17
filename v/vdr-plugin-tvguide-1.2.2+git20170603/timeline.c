#include "imageloader.h"
#include "timeline.h"

cTimeLine::cTimeLine(cMyTime *myTime) {
    this->myTime = myTime;
    if (tvguideConfig.displayMode == eVertical) {
        dateViewer = new cStyledPixmap(osdManager.requestPixmap(1, cRect(0, 
                                                                         geoManager.statusHeaderHeight, 
                                                                         geoManager.dateVieverWidth, 
                                                                         geoManager.dateVieverHeight)));
        timeline = osdManager.requestPixmap(2, cRect(0, 
                                                     geoManager.statusHeaderHeight + geoManager.channelHeaderHeight + geoManager.channelGroupsHeight, 
                                                     geoManager.timeLineWidth, 
                                                     geoManager.osdHeight - geoManager.statusHeaderHeight - geoManager.channelHeaderHeight - geoManager.channelGroupsHeight - geoManager.footerHeight)
                                             , cRect(0,
                                                     0, 
                                                     geoManager.timeLineWidth, 
                                                     1440*geoManager.minutePixel));
        timeBase = osdManager.requestPixmap(3, cRect(0, 
                                                     geoManager.statusHeaderHeight + geoManager.channelGroupsHeight + geoManager.channelHeaderHeight, 
                                                     geoManager.osdWidth, 
                                                     geoManager.timeLineGridHeight));
    } else if (tvguideConfig.displayMode == eHorizontal) {
        dateViewer = new cStyledPixmap(osdManager.requestPixmap(1, cRect(0, 
                                                                         geoManager.statusHeaderHeight, 
                                                                         geoManager.dateVieverWidth,
                                                                         geoManager.dateVieverHeight)));
        timeline = osdManager.requestPixmap(2, cRect(geoManager.channelHeaderWidth + geoManager.channelGroupsWidth, 
                                                     geoManager.statusHeaderHeight, 
                                                     geoManager.osdWidth - geoManager.channelHeaderWidth - geoManager.channelGroupsWidth,
                                                     geoManager.timeLineHeight)
                                             , cRect(0,
                                                     0, 
                                                     1440*geoManager.minutePixel, 
                                                     geoManager.timeLineHeight));
        timeBase = osdManager.requestPixmap(3, cRect(geoManager.channelGroupsWidth + geoManager.channelHeaderWidth, 
                                                     geoManager.statusHeaderHeight, 
                                                     geoManager.timeLineGridWidth, 
                                                     geoManager.timeLineHeight + tvguideConfig.channelRows * geoManager.rowHeight));
    }
    timeBase->Fill(clrTransparent);
    int clockY = 10;
    int clockX;
    if (tvguideConfig.scaleVideo) {
        clockX = geoManager.osdWidth - geoManager.tvFrameWidth - geoManager.clockWidth - 4;
    } else {
        clockX = geoManager.osdWidth - geoManager.clockWidth - 10;
    }
    clock = new cStyledPixmap(osdManager.requestPixmap(3, cRect(clockX, 
                                                                clockY, 
                                                                geoManager.clockWidth, 
                                                                geoManager.clockHeight)));
}

cTimeLine::~cTimeLine(void) {
    delete dateViewer;
    osdManager.releasePixmap(timeline);
    if (clock)
        delete clock;
}

void cTimeLine::drawDateViewer() {
    cString weekDay = myTime->GetWeekday();
    cString date = myTime->GetDate();   
    if (tvguideConfig.style != eStyleGraphical) {
        dateViewer->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
        dateViewer->drawBackground();
        dateViewer->drawBorder();
        
    } else {
        cImage *imgBack = imgCache.GetOsdElement(oeDateViewer);
        if (imgBack)
            dateViewer->DrawImage(cPoint(0,0), *imgBack);
        else
            dateViewer->Fill(clrTransparent);
    }
    tColor colorFont = theme.Color(clrFont);
    tColor colorFontBack = (tvguideConfig.style == eStyleFlat)?theme.Color(clrHeader):clrTransparent;

    if (tvguideConfig.displayMode == eVertical) {
        int textHeight = fontManager.FontTimeLineWeekday->Height();
        int weekdayWidth = fontManager.FontTimeLineWeekday->Width(*weekDay);
        int dateWidth = fontManager.FontTimeLineDate->Width(*date);
        dateViewer->DrawText(cPoint((geoManager.timeLineWidth-weekdayWidth)/2, (geoManager.channelHeaderHeight + geoManager.channelGroupsHeight -2*textHeight)/2), *weekDay, colorFont, colorFontBack, fontManager.FontTimeLineWeekday);
        dateViewer->DrawText(cPoint((geoManager.timeLineWidth-dateWidth)/2, (geoManager.channelHeaderHeight + geoManager.channelGroupsHeight -2*textHeight)/2 + textHeight + 5), *date, colorFont, colorFontBack, fontManager.FontTimeLineDate);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        cString strDate = cString::sprintf("%s %s", *weekDay, *date);
        int x = (dateViewer->Width() - fontManager.FontTimeLineDateHorizontal->Width(*strDate))/2;
        int y = (dateViewer->Height() - fontManager.FontTimeLineDateHorizontal->Height())/2;
        dateViewer->DrawText(cPoint(x, y), *strDate, colorFont, colorFontBack, fontManager.FontTimeLineDateHorizontal);
    }
}

void cTimeLine::drawTimeline() {
    timeline->SetTile(true);
    timeline->Fill(clrTransparent);
    tColor colorFont, colorBackground;
    
    int imgWidth = geoManager.timeLineGridWidth;
    int imgHeight = geoManager.timeLineGridHeight;
    const cImage *img1 = NULL;
    const cImage *img2 = NULL;
    if (tvguideConfig.style == eStyleGraphical) {
        img1 = imgCache.GetOsdElement(oeTimeline1);
        img2 = imgCache.GetOsdElement(oeTimeline2);
    } else {
        img1 = createBackgroundImage(imgWidth, imgHeight, theme.Color(clrTimeline1), theme.Color(clrTimeline1Blending));
        img2 = createBackgroundImage(imgWidth, imgHeight, theme.Color(clrTimeline2), theme.Color(clrTimeline2Blending));
    }
    const cImage *img = NULL;
    if (!img1 || !img2)
        return;
    int textWidth, posX, posY;
    char timetext[10];
    
    for (int i=0; i<48; i++) {
        if (i%2==0) {
            img = img1;
            colorFont = theme.Color(clrTimeline2);
            colorBackground = (tvguideConfig.style == eStyleFlat)?theme.Color(clrTimeline1):clrTransparent;
            if (tvguideConfig.timeFormat == e12Hours) {
                if (i == 0)
                    sprintf(timetext, "12:00 PM");
                else if (i/2 < 13)
                    sprintf(timetext, "%d:00 AM", i/2);
                else 
                    sprintf(timetext, "%d:00 PM", i/2-12);
            } else {
                sprintf(timetext, "%d:00", i/2);
            }
        } else {
            img = img2;
            colorFont = theme.Color(clrTimeline1);
            colorBackground = (tvguideConfig.style == eStyleFlat)?theme.Color(clrTimeline2):clrTransparent;
            if (tvguideConfig.timeFormat == e12Hours) {
                if (i == 1)
                    sprintf(timetext, "12:30 PM");
                else if (i/2 < 13)
                    sprintf(timetext, "%d:30 AM", i/2);
                else 
                    sprintf(timetext, "%d:30 PM", i/2-12);
            } else {
                sprintf(timetext, "%d:30", i/2);
            }
        }
        if (tvguideConfig.displayMode == eVertical) {
            posY = i*geoManager.minutePixel*30;
            timeline->DrawImage(cPoint(0, posY), *img);
            if (tvguideConfig.style != eStyleGraphical) {
                decorateTile(0, posY, imgWidth+2, imgHeight);
            }
            textWidth = fontManager.FontTimeLineTime->Width(timetext);
            timeline->DrawText(cPoint((geoManager.timeLineWidth-textWidth)/2, posY + 5), timetext, colorFont, colorBackground, fontManager.FontTimeLineTime);
        } else if (tvguideConfig.displayMode == eHorizontal) {
            posX = i*geoManager.minutePixel*30;
            timeline->DrawImage(cPoint(posX, 0), *img);
            if (tvguideConfig.style != eStyleGraphical) {
                decorateTile(posX, 0, imgWidth, imgHeight+2);
            }
            timeline->DrawText(cPoint(posX + 15, (dateViewer->Height() - fontManager.FontTimeLineTimeHorizontal->Height())/2), timetext, colorFont, colorBackground, fontManager.FontTimeLineTimeHorizontal);
       }
    }
    setTimeline();
    if (tvguideConfig.style != eStyleGraphical) {
        delete img1;
        delete img2;
    }
}

void cTimeLine::decorateTile(int posX, int posY, int tileWidth, int tileHeight) {
    timeline->DrawRectangle(cRect(posX,posY,tileWidth,2), clrTransparent);          //top
    timeline->DrawRectangle(cRect(posX,posY,2,tileHeight), clrTransparent);         //left
    timeline->DrawRectangle(cRect(posX,posY + tileHeight-2,tileWidth,2), clrTransparent);   //bottom
    timeline->DrawRectangle(cRect(posX + tileWidth-2,posY,2,tileHeight), clrTransparent);   //right

    timeline->DrawRectangle(cRect(2+posX,posY+2,tileWidth-4,1), theme.Color(clrBorder));            //top
    timeline->DrawRectangle(cRect(2+posX,posY+2,1,tileHeight-4), theme.Color(clrBorder));           //left
    timeline->DrawRectangle(cRect(2+posX,posY+tileHeight-3,tileWidth-4,1), theme.Color(clrBorder));     //bottom
    timeline->DrawRectangle(cRect(posX+tileWidth-3,posY+2,1,tileHeight-4), theme.Color(clrBorder));     //right
    
    if (tvguideConfig.roundedCorners) {
        int borderRadius = 12;
        drawRoundedCorners(posX, posY, tileWidth, tileHeight, borderRadius);
    }
}

void cTimeLine::drawRoundedCorners(int posX, int posY, int width, int height, int radius) {
    timeline->DrawEllipse(cRect(posX+2,posY+2,radius,radius), theme.Color(clrBorder), -2);
    timeline->DrawEllipse(cRect(posX+1,posY+1,radius,radius), clrTransparent, -2);

    timeline->DrawEllipse(cRect(posX+width-radius - 2,posY+2,radius,radius), theme.Color(clrBorder), -1);
    timeline->DrawEllipse(cRect(posX+width-radius - 1,posY+1,radius,radius), clrTransparent, -1);
    
    if( height > 2*radius) {
        timeline->DrawEllipse(cRect(posX+2,posY+height-radius - 2,radius,radius), theme.Color(clrBorder), -3);
        timeline->DrawEllipse(cRect(posX+1,posY+height-radius - 1,radius,radius), clrTransparent, -3);
        
        timeline->DrawEllipse(cRect(posX+width-radius - 2,posY+height-radius - 2,radius,radius), theme.Color(clrBorder), -4);
        timeline->DrawEllipse(cRect(posX+width-radius - 1,posY+height-radius - 1,radius,radius), clrTransparent, -4);
    }
}

void cTimeLine::drawCurrentTimeBase(void) {
    timeBase->Fill(clrTransparent);
    bool nowVisible = myTime->NowVisible();
    if (!nowVisible)
        return;
    int deltaTime = (myTime->GetNow() - myTime->GetStart()) / 60 * geoManager.minutePixel;
    if (tvguideConfig.displayMode == eVertical) {
        timeBase->DrawRectangle(cRect(0, deltaTime - 2, timeBase->ViewPort().Width(), 4), theme.Color(clrTimeBase));
    } else {
        timeBase->DrawRectangle(cRect(deltaTime-2, 0, 4, timeBase->ViewPort().Height()), theme.Color(clrTimeBase));
    }
}


cImage *cTimeLine::createBackgroundImage(int width, int height, tColor clrBgr, tColor clrBlend) {
    cImage *image = NULL;
    if (tvguideConfig.style == eStyleBlendingDefault) {
        image = new cImage(cSize(width, height));
        image->Fill(clrBgr);
        int stepY = 0.5*height / 64;
        int alpha = 0x00;
        tColor clr;
        for (int i = 0; i<64; i++) {
            clr = AlphaBlend(clrBgr, clrBlend, alpha);
            for (int y = i*stepY; y < (i+1)*stepY; y++) {
                for (int x=0; x<width; x++) {
                    image->SetPixel(cPoint(x,y), clr);
                }
            }
            alpha += 0x04;
        }
    } else  if (tvguideConfig.style == eStyleBlendingMagick) {
        cImageLoader imgLoader;
        if (imgLoader.DrawBackground(clrBgr, clrBlend, width, height)) {
            image = new cImage(imgLoader.GetImage());
        }
    } else {
        image = new cImage(cSize(width, height));
        image->Fill(clrBgr);
    }
    return image;
}

void cTimeLine::setTimeline() {
    int offset = myTime->GetTimelineOffset();
    int xNew, yNew;
    if (tvguideConfig.displayMode == eVertical) {
        xNew = 0;
        yNew = -offset*geoManager.minutePixel;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        xNew = -offset*geoManager.minutePixel;
        yNew = 0;
    }
    timeline->SetDrawPortPoint(cPoint(xNew, yNew));
    if (tvguideConfig.displayTimeBase)
        drawCurrentTimeBase();
}

void cTimeLine::drawClock() {
    clock->Fill(clrTransparent);
    cString currentTime = myTime->GetCurrentTime();
    const cFont *font = (tvguideConfig.displayMode == eVertical)?fontManager.FontTimeLineTime:fontManager.FontTimeLineTimeHorizontal;
    int textHeight = font->Height();
    int clockTextWidth = font->Width(*currentTime);
    tColor colorFontBack = (tvguideConfig.style == eStyleFlat)?theme.Color(clrHeader):clrTransparent;
    if (tvguideConfig.style == eStyleGraphical) {
        clock->drawBackgroundGraphical(bgClock);
    } else {
        clock->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
        clock->drawBackground();
        clock->drawBorder();
    }
    clock->DrawText(cPoint((geoManager.clockWidth-clockTextWidth)/2, (geoManager.clockHeight-textHeight)/2), *currentTime, theme.Color(clrFont), colorFontBack, font);
}