#include "detailview.h"

/********************************************************************************************
* cView
********************************************************************************************/

cView::cView(void) {
    activeView = 0;
    scrollable = false;
    tabbed = false;
    font = NULL;
    fontSmall = NULL;
    fontHeader = NULL;
    fontHeaderLarge = NULL;
    pixmapBackground = NULL;
    pixmapHeader = NULL;
    pixmapHeaderLogo = NULL;
    pixmapContent = NULL;
    pixmapTabs = NULL;
    pixmapScrollbar = NULL;
    pixmapScrollbarBack = NULL;
    imgScrollBar = NULL;
    title = "";
    subTitle = "";
    dateTime = "";
    infoText = "";
    channel = NULL;
    eventID = 0;
    event = NULL;
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    border = 0;
    headerHeight = 0;
    contentHeight = 0;
    tabHeight = 0;
    headerDrawn = false;
}

cView::~cView(void) {
    if (pixmapBackground)
        osdManager.releasePixmap(pixmapBackground);
    if (pixmapHeader)
        delete pixmapHeader;
    if (pixmapHeaderLogo)
        osdManager.releasePixmap(pixmapHeaderLogo);   
    if (pixmapContent)
        osdManager.releasePixmap(pixmapContent);
    if (pixmapTabs)
        osdManager.releasePixmap(pixmapTabs);
    if (pixmapScrollbar)
        osdManager.releasePixmap(pixmapScrollbar);
    if (pixmapScrollbarBack)
        osdManager.releasePixmap(pixmapScrollbarBack);
    if (imgScrollBar)
        delete imgScrollBar;
}

void cView::SetFonts(void) {
    font = fontManager.FontDetailView;
    fontSmall = fontManager.FontDetailViewSmall;
    fontHeaderLarge = fontManager.FontDetailHeaderLarge;
    fontHeader = fontManager.FontDetailHeader;
}

void cView::SetGeometry(void) { 
    x = 0;
    y = 0;
    scrollbarWidth = 40;
    width = geoManager.osdWidth - scrollbarWidth;
    height = geoManager.osdHeight;
    border = tvguideConfig.epgViewBorder;
    headerWidth = geoManager.headerContentWidth;
    headerHeight = geoManager.epgViewHeaderHeight;
    if (tabbed)
        tabHeight = font->Height() * 3 / 2;
    contentHeight = height - headerHeight - tabHeight - geoManager.footerHeight;
}

void cView::DrawHeader(void) {
    if (!pixmapHeader) {
        pixmapHeader = new cStyledPixmap(osdManager.requestPixmap(5, cRect(0, 0, headerWidth, headerHeight)));
        pixmapHeader->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    }
    if (!pixmapHeaderLogo) {
        pixmapHeaderLogo = osdManager.requestPixmap(6, cRect(0, 0, width, headerHeight));
    }
    pixmapHeader->Fill(clrTransparent);
    pixmapHeaderLogo->Fill(clrTransparent);
    if (tvguideConfig.style == eStyleGraphical) {
        if (tvguideConfig.scaleVideo) {
            pixmapHeader->drawBackgroundGraphical(bgStatusHeaderWindowed);
        } else {
            pixmapHeader->drawBackgroundGraphical(bgStatusHeaderFull);
        }
    } else {
        pixmapHeader->drawBackground();
        pixmapHeader->drawBoldBorder();
    }
    //Channel Logo
    int logoHeight = 2 * headerHeight / 3;
    int logoWidth = logoHeight * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    int xText = border / 2;
    if (channel && !tvguideConfig.hideChannelLogos) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(channel, logoWidth, logoHeight)) {
            cImage logo = imgLoader.GetImage();
            pixmapHeaderLogo->DrawImage(cPoint(border/2, (headerHeight - logoHeight)/2), logo);
            xText += logoWidth + border / 2;
        }
    }
    //Date and Time, Title, Subtitle
    int yDateTime = border / 2;
    int yTitle = (headerHeight - fontHeaderLarge->Height()) / 2;
    int ySubtitle = headerHeight - fontHeader->Height() - border / 3;
    int textWidthMax = headerWidth - xText;
    pixmapHeader->DrawText(cPoint(xText, yDateTime), CutText(dateTime, textWidthMax, fontHeader).c_str(), theme.Color(clrFont), theme.Color(clrStatusHeader), fontHeader);
    pixmapHeader->DrawText(cPoint(xText, yTitle), CutText(title, textWidthMax, fontHeaderLarge).c_str(), theme.Color(clrFont), theme.Color(clrStatusHeader), fontHeaderLarge);
    pixmapHeader->DrawText(cPoint(xText, ySubtitle), CutText(subTitle, textWidthMax, fontHeader).c_str(), theme.Color(clrFont), theme.Color(clrStatusHeader), fontHeader);
    //REC Icon
    eTimerMatch timerMatch=tmNone; 
#if VDRVERSNUM >= 20301
    LOCK_TIMERS_READ;
    const cTimer *ti;
#else
    cTimer *ti;
#endif
    if (!event)
        return;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        timerMatch = (eTimerMatch)rtMatch.timerMatch;
        ti = rtMatch.timer;
    } else {
#if VDRVERSNUM >= 20301
        LOCK_TIMERS_READ;
        ti=Timers->GetMatch(event, &timerMatch);
#else
        ti=Timers.GetMatch(event, &timerMatch);
#endif
    }
    if (timerMatch == tmFull) {
        cString recIconText(" REC ");
        int widthIcon = fontManager.FontDetailHeader->Width(*recIconText);
        int height = fontManager.FontDetailHeader->Height()+10;
        int posX = headerWidth - widthIcon - 20;
        int posY = 20;
        pixmapHeader->DrawRectangle( cRect(posX, posY, widthIcon, height), theme.Color(clrButtonRed));
        pixmapHeader->DrawText(cPoint(posX, posY+5), *recIconText, theme.Color(clrFont), theme.Color(clrButtonRed), fontManager.FontDetailHeader);
    }
}

void cView::DrawTabs(void) {
    if (!pixmapTabs) {
        pixmapTabs = osdManager.requestPixmap(4, cRect(0, y + headerHeight + contentHeight, width + scrollbarWidth, tabHeight));
    }
    tColor bgColor = theme.Color(clrTabInactive);
    pixmapTabs->Fill(clrTransparent);
    pixmapTabs->DrawRectangle(cRect(0, 0, width, 2), bgColor);
    int numTabs = tabs.size();
    int xCurrent = 0;
    for (int tab = 0; tab < numTabs; tab++) {
        std::string tabText = tabs[tab];
        int textWidth = font->Width(tabText.c_str());
        int tabWidth = textWidth + border;
        pixmapTabs->DrawRectangle(cRect(xCurrent, (tab == activeView) ? 0 : 2, tabWidth - 2, tabHeight), bgColor);
        pixmapTabs->DrawEllipse(cRect(xCurrent, tabHeight - border/2, border/2, border/2), clrTransparent, -3);
        pixmapTabs->DrawEllipse(cRect(xCurrent + tabWidth - border / 2 - 2, tabHeight - border/2, border/2, border/2), clrTransparent, -4);
        if (tab == activeView) {
            pixmapTabs->DrawRectangle(cRect(xCurrent + 2, 0, tabWidth - 6, tabHeight - border/2 - 1), clrTransparent);
            pixmapTabs->DrawRectangle(cRect(xCurrent + border / 2, tabHeight - border/2 - 1, tabWidth - border, border/2 - 1), clrTransparent);
            pixmapTabs->DrawEllipse(cRect(xCurrent + 2, tabHeight - border/2 - 2, border/2, border/2), clrTransparent, 3);
            pixmapTabs->DrawEllipse(cRect(xCurrent + tabWidth - border / 2 - 4, tabHeight - border/2 - 2, border/2, border/2), clrTransparent, 4);
        }
        pixmapTabs->DrawText(cPoint(xCurrent + (tabWidth - textWidth) / 2, 2 + (tabHeight - font->Height())/2), tabText.c_str(), theme.Color(clrFont), (tab == activeView) ? clrTransparent : bgColor, font);
        xCurrent += tabWidth;
    }
}

void cView::ClearContent(void) {
    if (pixmapContent && Running()) {
        osdManager.releasePixmap(pixmapContent);
        pixmapContent = NULL;
    }
    if (pixmapBackground && Running()) {
        osdManager.releasePixmap(pixmapBackground);
        pixmapBackground = NULL;
    }
}

void cView::CreateContent(int fullHeight) {
    scrollable = false;
    pixmapBackground = osdManager.requestPixmap(3, cRect(x, y + headerHeight, width + scrollbarWidth, contentHeight + tabHeight));
    pixmapBackground->Fill(theme.Color(clrBackground));

    int drawPortHeight = contentHeight; 
    if (fullHeight > contentHeight) {
        drawPortHeight = fullHeight;
        scrollable = true;
    }
    pixmapContent = osdManager.requestPixmap(4, cRect(x, y + headerHeight, width, contentHeight), cRect(0, 0, width, drawPortHeight));
    pixmapContent->Fill(clrTransparent);
}

void cView::DrawContent(std::string *text) {
    cTextWrapper wText;
    wText.Set(text->c_str(), font, width - 2 * border);
    int lineHeight = font->Height();
    int textLines = wText.Lines();
    int textHeight = lineHeight * textLines + 2*border;
    int yText = border;
    CreateContent(textHeight);
    for (int i=0; i < textLines; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wText.GetLine(i), theme.Color(clrFont), clrTransparent, font);
        yText += lineHeight;
    }
}

void cView::DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2) {
    cTextWrapper wTextTall;
    cTextWrapper wTextFull;
    int imgWidth = img->width;
    int imgHeight = img->height;
    int imgWidth2 = 0;
    int imgHeight2 = 0;
    if (imgHeight > (contentHeight - 2 * border)) {
        imgHeight = contentHeight - 2 * border;
        imgWidth = imgWidth * ((double)imgHeight / (double)img->height);
    }
    int imgHeightTotal = imgHeight;
    if (img2) {
        imgWidth2 = imgWidth;
        imgHeight2 = img2->height * ((double)img2->width / (double)imgWidth2);
        imgHeightTotal += img2->height + border;
    }
    CreateFloatingTextWrapper(&wTextTall, &wTextFull, infoText, imgWidth, imgHeightTotal);
    int lineHeight = font->Height();
    int textLinesTall = wTextTall.Lines();
    int textLinesFull = wTextFull.Lines();
    int textHeight = lineHeight * (textLinesTall + textLinesFull) + 2*border;
    int yText = border;
    CreateContent(max(textHeight, imgHeight + 2*border));
    for (int i=0; i < textLinesTall; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wTextTall.GetLine(i), theme.Color(clrFont), clrTransparent, font);
        yText += lineHeight;
    }
    for (int i=0; i < textLinesFull; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wTextFull.GetLine(i), theme.Color(clrFont), clrTransparent, font);
        yText += lineHeight;
    }
    osdManager.flush();
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(img->path.c_str(), imgWidth, imgHeight)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(width - imgWidth - border, border), imgLoader.GetImage());
    }
    if (!img2)
        return;
    osdManager.flush();
    if (imgLoader.LoadPoster(img2->path.c_str(), imgWidth2, imgHeight2)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(width - imgWidth2 - border, imgHeight + 2*border), imgLoader.GetImage());
    }
}

void cView::CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg) {
    int lineHeight = font->Height();
    int linesNarrow = (heightImg + 2*border)/ lineHeight;
    int linesDrawn = 0;
    int y = 0;
    int widthNarrow = width - 3 * border - widthImg;
    bool drawNarrow = true;
    
    splitstring s(text->c_str());
    std::vector<std::string> flds = s.split('\n', 1);

    if (flds.size() < 1)
        return;

    std::stringstream sstrTextTall;
    std::stringstream sstrTextFull;
      
    for (int i=0; i<flds.size(); i++) {
        if (!flds[i].size()) {
            //empty line
            linesDrawn++;
            y += lineHeight;
            if (drawNarrow)
                sstrTextTall << "\n";
            else
                sstrTextFull << "\n";
        } else {
            cTextWrapper wrapper;
            if (drawNarrow) {
                wrapper.Set((flds[i].c_str()), font, widthNarrow);
                int newLines = wrapper.Lines();
                //check if wrapper fits completely into narrow area
                if (linesDrawn + newLines < linesNarrow) {
                    for (int line = 0; line < wrapper.Lines(); line++) {
                        sstrTextTall << wrapper.GetLine(line) << " ";
                    }
                    sstrTextTall << "\n";
                    linesDrawn += newLines;
                } else {
                    //this wrapper has to be splitted
                    for (int line = 0; line < wrapper.Lines(); line++) {
                        if (line + linesDrawn < linesNarrow) {
                            sstrTextTall << wrapper.GetLine(line) << " ";
                        } else {
                            sstrTextFull << wrapper.GetLine(line) << " ";
                        }
                    }
                    sstrTextFull << "\n";
                    drawNarrow = false;
                }
            } else {
                wrapper.Set((flds[i].c_str()), font, width - 2*border);
                for (int line = 0; line < wrapper.Lines(); line++) {
                    sstrTextFull << wrapper.GetLine(line) << " ";        
                }
                sstrTextFull << "\n";
            }
        }
    }
    twNarrow->Set(sstrTextTall.str().c_str(), font, widthNarrow);
    twFull->Set(sstrTextFull.str().c_str(), font, width - 2 * border);
}

void cView::DrawActors(std::vector<cActor> *actors) {
    int numActors = actors->size();
    if (numActors < 1) {
        CreateContent(100);
        pixmapContent->DrawText(cPoint(border, border), tr("No Cast available"), theme.Color(clrFont), clrTransparent, fontHeaderLarge);
        return;
    }
    int thumbWidth = actors->at(0).actorThumb.width;
    int thumbHeight = actors->at(0).actorThumb.height;
    
    int picsPerLine = width / (thumbWidth + 2 * border);
    if (picsPerLine < 1)
        return;

    int picLines = numActors / picsPerLine;
    if (numActors%picsPerLine != 0)
        picLines++;
    
    int totalHeight = picLines * (thumbHeight + 2*fontSmall->Height() + border + border/2) + 2*border + fontHeaderLarge->Height();

    CreateContent(totalHeight);
    cString header = cString::sprintf("%s:", tr("Cast"));
    pixmapContent->DrawText(cPoint(border, border), *header, theme.Color(clrFont), clrTransparent, fontHeaderLarge);

    int x = 0;
    int y = 2 * border + fontHeaderLarge->Height();
    if (!Running())
        return;
        cImageLoader imgLoader;
    int actor = 0;
    for (int row = 0; row < picLines; row++) {
        for (int col = 0; col < picsPerLine; col++) {
            if (!Running())
                return;
            if (actor == numActors)
                break;
            std::string path = actors->at(actor).actorThumb.path;
            std::string name = actors->at(actor).name;
            std::stringstream sstrRole;
            sstrRole << "\"" << actors->at(actor).role << "\"";
            std::string role = sstrRole.str();
            if (imgLoader.LoadPoster(path.c_str(), thumbWidth, thumbHeight)) {
                if (Running() && pixmapContent)
                    pixmapContent->DrawImage(cPoint(x + border, y), imgLoader.GetImage());
            }

            if (fontSmall->Width(name.c_str()) > thumbWidth + 2*border)
                name = CutText(name, thumbWidth + 2*border, fontSmall);
            if (fontSmall->Width(role.c_str()) > thumbWidth + 2*border)
                role = CutText(role, thumbWidth + 2*border, fontSmall);
            int xName = x + ((thumbWidth+2*border) - fontSmall->Width(name.c_str()))/2;
            int xRole = x + ((thumbWidth+2*border) - fontSmall->Width(role.c_str()))/2;
            if (Running() && pixmapContent) {
                pixmapContent->DrawText(cPoint(xName, y + thumbHeight + border/2), name.c_str(), theme.Color(clrFont), clrTransparent, fontSmall);
                pixmapContent->DrawText(cPoint(xRole, y + thumbHeight + border/2 + fontSmall->Height()), role.c_str(), theme.Color(clrFont), clrTransparent, fontSmall);
                x += thumbWidth + 2*border;
            }
            actor++;
        }
        osdManager.flush();
        x = 0;
        y += thumbHeight + 2 * fontSmall->Height() + border + border/2;
    }
}

void cView::ClearScrollbar(void) {
    if (pixmapScrollbar)
        pixmapScrollbar->Fill(clrTransparent);
    if (pixmapScrollbarBack)
        pixmapScrollbarBack->Fill(clrTransparent);
}

void cView::ClearScrollbarImage(void) {
    if (imgScrollBar) {
        delete imgScrollBar;
        imgScrollBar = NULL;
    }
}

void cView::DrawScrollbar(void) {
    ClearScrollbar();
    if (!scrollable || !pixmapContent)
        return;

    if (!pixmapScrollbar) {
        pixmapScrollbar = osdManager.requestPixmap(6, cRect(width, y + headerHeight, scrollbarWidth, contentHeight));
        pixmapScrollbar->Fill(clrTransparent);
    }
    if (!pixmapScrollbarBack) {
        pixmapScrollbarBack = osdManager.requestPixmap(5, cRect(width, y + headerHeight, scrollbarWidth, contentHeight));
        pixmapScrollbarBack->Fill(clrTransparent);
    }

    int totalBarHeight = pixmapScrollbar->ViewPort().Height() - 6;
    
    int aktHeight = (-1)*pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();

    int barHeight = (double)(screenHeight * totalBarHeight) / (double)totalHeight ;
    int barTop = (double)(aktHeight * totalBarHeight) / (double)totalHeight ;

    if (!imgScrollBar) {
        imgScrollBar = CreateScrollbarImage(pixmapScrollbar->ViewPort().Width()-10, barHeight, theme.Color(clrHighlight), theme.Color(clrHighlightBlending));
    }
    
    pixmapScrollbarBack->Fill(theme.Color(clrHighlightBlending));
    pixmapScrollbarBack->DrawRectangle(cRect(2, 2, pixmapScrollbar->ViewPort().Width() - 4, pixmapScrollbar->ViewPort().Height() - 4), theme.Color(clrHighlightBlending));

    pixmapScrollbar->DrawImage(cPoint(3, 3 + barTop), *imgScrollBar);
}

cImage *cView::CreateScrollbarImage(int width, int height, tColor clrBgr, tColor clrBlend) {
    cImage *image = new cImage(cSize(width, height));
    image->Fill(clrBgr);
    if (tvguideConfig.style != eStyleFlat) {
        int numSteps = 64;
        int alphaStep = 0x03;
        if (height < 30)
            return image;
        else if (height < 100) {
            numSteps = 32;
            alphaStep = 0x06;
        }
        int stepY = 0.5*height / numSteps;
        if (stepY == 0)
            stepY = 1;
        int alpha = 0x40;
        tColor clr;
        for (int i = 0; i<numSteps; i++) {
            clr = AlphaBlend(clrBgr, clrBlend, alpha);
            for (int y = i*stepY; y < (i+1)*stepY; y++) {
                for (int x=0; x<width; x++) {
                    image->SetPixel(cPoint(x,y), clr);
                }
            }
            alpha += alphaStep;
        }
    }
    return image;
}

bool cView::KeyUp(void) { 
    if (!scrollable)
        return false;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int lineHeight = font->Height();
    if (aktHeight >= 0) {
        return false;
    }
    int step = tvguideConfig.detailedViewScrollStep * font->Height();
    int newY = aktHeight + step;
    if (newY > 0)
        newY = 0;
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

bool cView::KeyDown(void) { 
    if (!scrollable)
        return false;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();
    
    if (totalHeight - ((-1)*aktHeight) == screenHeight) {
        return false;
    } 
    int step = tvguideConfig.detailedViewScrollStep * font->Height();
    int newY = aktHeight - step;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

/********************************************************************************************
* cEPGView : cView
********************************************************************************************/

cEPGView::cEPGView(void) : cView() {
    tabbed = true;
    numEPGPics = -1;
    numTabs = 0;
}

cEPGView::~cEPGView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
}

void cEPGView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    if (numEPGPics > 0)
        tabs.push_back(tr("Image Galery"));
    numTabs = tabs.size();
}

void cEPGView::CheckEPGImages(void) {
    if (eventID > 0) {
        for (int i=1; i <= tvguideConfig.numAdditionalEPGPictures; i++) {
            cString epgimage;
            epgimage = cString::sprintf("%s%d_%d.jpg", *tvguideConfig.epgImagePath, eventID, i);
            FILE *fp = fopen(*epgimage, "r");
            if (fp) {
                std::stringstream ss;
                ss << i;
                epgPics.push_back(ss.str());
                fclose(fp);
            } else {
                break;
            }
        }
    } else {
        return;
    }
    numEPGPics = epgPics.size();
}

void cEPGView::DrawImages(void) {
    int imgWidth = tvguideConfig.epgImageWidthLarge;
    int imgHeight = tvguideConfig.epgImageHeightLarge;

    int totalHeight = numEPGPics * (imgHeight + border);

    CreateContent(totalHeight);
    
    cImageLoader imgLoader;
    int yPic = border;
    for (int pic = 0; pic < numEPGPics; pic++) {
        bool drawPic = false;
        if (eventID > 0) {
            cString epgimage = cString::sprintf("%d_%d", eventID, atoi(epgPics[pic].c_str()));
            if (imgLoader.LoadAdditionalEPGImage(epgimage)) {
                drawPic = true;
            }

        }
        if (drawPic) {
            pixmapContent->DrawImage(cPoint((width - imgWidth) / 2, yPic), imgLoader.GetImage());
            yPic += imgHeight + border;
            osdManager.flush();
        }
    } 
}

void cEPGView::KeyLeft(void) { 
    if (Running())
        return;
    ClearScrollbarImage();
    activeView--;
    if (activeView < 0)
        activeView = numTabs - 1; 
}

void cEPGView::KeyRight(void) { 
    if (Running())
        return;
    ClearScrollbarImage();
    activeView = (activeView + 1) % numTabs;
}

void cEPGView::Action(void) {
    ClearContent();
    if (!headerDrawn) {
        DrawHeader();
        osdManager.flush();
        headerDrawn = true;
    }
    if (tabs.size() == 0) {
        CheckEPGImages();
        SetTabs();
    }
    DrawTabs();
    switch (activeView) {
        case evtInfo:
            DrawContent(&infoText);
            break;
        case evtAddInfo:
            DrawContent(&addInfoText);
            break;
        case evtImages:
            DrawImages();
            break;
    }
    DrawScrollbar();
    osdManager.flush();
}

/********************************************************************************************
* cSeriesView : cView
********************************************************************************************/

cSeriesView::cSeriesView(int seriesId, int episodeId) : cView() {
    this->seriesId = seriesId;
    this->episodeId = episodeId;
    tvdbInfo = "";
    tabbed = true;
}

cSeriesView::~cSeriesView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
}

void cSeriesView::LoadMedia(void) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || seriesId < 1)
        return;
    series.seriesId = seriesId;
    series.episodeId = episodeId;
    pScraper->Service("GetSeries", &series);
}

void cSeriesView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    tabs.push_back(tr("Cast"));
    tabs.push_back(tr("TheTVDB Info"));
    tabs.push_back(tr("Image Galery"));
}

void cSeriesView::CreateTVDBInfo(void) {
    if (tvdbInfo.size() > 0)
        return;
    std::stringstream info;
    info << tr("TheTVDB Information") << ":\n\n";

    if (series.episode.name.size() > 0) {
        info << tr("Episode") << ": " << series.episode.name << " (" << tr("Season") << " " << series.episode.season << ", " << tr("Episode") << " " << series.episode.number << ")\n\n";
    }
    if (series.episode.overview.size() > 0) {
        info << tr("Episode Overview") << ": " << series.episode.overview << "\n\n";
    }
    if (series.episode.firstAired.size() > 0) {
        info << tr("First aired") << ": " << series.episode.firstAired << "\n\n";
    }
    if (series.episode.guestStars.size() > 0) {
        info << tr("Guest Stars") << ": " << series.episode.guestStars << "\n\n";
    }
    if (series.episode.rating > 0) {
        info << tr("TheMovieDB Rating") << ": " << series.episode.rating << "\n\n";
    }
    if (series.overview.size() > 0) {
        info << tr("Series Overview") << ": " << series.overview << "\n\n";
    }
    if (series.firstAired.size() > 0) {
        info << tr("First aired") << ": " << series.firstAired << "\n\n";
    }
    if (series.genre.size() > 0) {
        info << tr("Genre") << ": " << series.genre << "\n\n";
    }
    if (series.network.size() > 0) {
        info << tr("Network") << ": " << series.network << "\n\n";
    }
    if (series.rating > 0) {
        info << tr("TheMovieDB Rating") << ": " << series.rating << "\n\n";
    }
    if (series.status.size() > 0) {
        info << tr("Status") << ": " << series.status << "\n\n";
    }
    tvdbInfo = info.str();
}

void cSeriesView::DrawImages(void) {
    int numPosters = series.posters.size();
    int numFanarts = series.fanarts.size();
    int numBanners = series.banners.size();
    
    int totalHeight = border;
    //Fanart Height
    int fanartWidth = width - 2 * border;
    int fanartHeight = 0;
    if (numFanarts > 0 && series.fanarts[0].width > 0) {
        fanartHeight = series.fanarts[0].height * ((double)fanartWidth / (double)series.fanarts[0].width);
        if (fanartHeight > contentHeight - 2 * border) {
            int fanartHeightOrig = fanartHeight;
            fanartHeight = contentHeight - 2 * border;
            fanartWidth = fanartWidth * ((double)fanartHeight / (double)fanartHeightOrig);
        }
        totalHeight += series.fanarts.size() * (fanartHeight + border);
    }
    //Poster Height
    int posterWidth = (width - 4 * border) / 2;
    int posterHeight = 0;
    if (numPosters > 0 && series.posters[0].width > 0) {
        posterHeight = series.posters[0].height * ((double)posterWidth / (double)series.posters[0].width);
    }
    if (numPosters > 0)
        totalHeight += posterHeight + border;
    if (numPosters == 3)
        totalHeight += posterHeight + border;
    //Banners Height
    if (numBanners > 0)
        totalHeight += (series.banners[0].height + border) * numBanners;
       
    CreateContent(totalHeight);
    
    cImageLoader imgLoader;
    int yPic = border;
    for (int i=0; i < numFanarts; i++) {
        if (numBanners > i) {
            if (imgLoader.LoadPoster(series.banners[i].path.c_str(), series.banners[i].width, series.banners[i].height) && Running()) {
                pixmapContent->DrawImage(cPoint((width - series.banners[i].width) / 2, yPic), imgLoader.GetImage());
                yPic += series.banners[i].height + border;
                osdManager.flush();
            }
        }
        if (imgLoader.LoadPoster(series.fanarts[i].path.c_str(), fanartWidth, fanartHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - fanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += fanartHeight + border;
            osdManager.flush();
        }
    }
    if (numPosters >= 1) {
        if (imgLoader.LoadPoster(series.posters[0].path.c_str(), posterWidth, posterHeight) && Running()) {
            pixmapContent->DrawImage(cPoint(border, yPic), imgLoader.GetImage());
            osdManager.flush();
            yPic += posterHeight + border;
        }
    }
    if (numPosters >= 2) {
        if (imgLoader.LoadPoster(series.posters[1].path.c_str(), posterWidth, posterHeight) && Running()) {
            pixmapContent->DrawImage(cPoint(2 * border + posterWidth, yPic - posterHeight - border), imgLoader.GetImage());
            osdManager.flush();
        }
    }
    if (numPosters == 3) {
        if (imgLoader.LoadPoster(series.posters[2].path.c_str(), posterWidth, posterHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - posterWidth) / 2, yPic), imgLoader.GetImage());
            osdManager.flush();
        }
    }
}

int cSeriesView::GetRandomPoster(void) {
    int numPosters = series.posters.size();
    if (numPosters == 0)
        return -1;
    srand((unsigned)time(NULL));
    int randPoster = rand()%numPosters;
    return randPoster;
}

void cSeriesView::KeyLeft(void) { 
    if (Running())
        return;
    ClearScrollbarImage();
    activeView--;
    if (activeView < 0)
        activeView = mvtCount - 1; 
}

void cSeriesView::KeyRight(void) { 
    if (Running())
        return;
    ClearScrollbarImage();
    activeView = (activeView + 1) % mvtCount;
}

void cSeriesView::Action(void) {
    ClearContent();
    if (!headerDrawn) {
        DrawHeader();
        osdManager.flush();
        headerDrawn = true;
    }
    if (tabs.size() == 0) {
        SetTabs();
    }
    DrawTabs();
    int randomPoster = GetRandomPoster();
    switch (activeView) {
        case mvtInfo:
            if (randomPoster >= 0) {
                DrawFloatingContent(&infoText, &series.posters[randomPoster]);
            } else
                DrawContent(&infoText);
            break;
        case mvtAddInfo:
            if (randomPoster >= 0)
                DrawFloatingContent(&addInfoText, &series.posters[randomPoster]);
            else
                DrawContent(&addInfoText);
            break;
        case mvtCast:
            DrawActors(&series.actors);
            break;
        case mvtOnlineInfo:
            CreateTVDBInfo();
            if ((series.seasonPoster.path.size() > 0) && series.episode.episodeImage.path.size() > 0)
                DrawFloatingContent(&tvdbInfo, &series.episode.episodeImage, &series.seasonPoster);
            else if (series.seasonPoster.path.size() > 0)
                DrawFloatingContent(&tvdbInfo, &series.seasonPoster);
            else if (series.episode.episodeImage.path.size() > 0)
                DrawFloatingContent(&tvdbInfo, &series.episode.episodeImage);
            else if (randomPoster >= 0)
                DrawFloatingContent(&tvdbInfo, &series.posters[randomPoster]);
            else
                DrawContent(&tvdbInfo);
            break;
        case mvtImages:
            DrawImages();
            break;
    }
    DrawScrollbar();
    osdManager.flush();
}

/********************************************************************************************
* cMovieView : cView
********************************************************************************************/

cMovieView::cMovieView(int movieId) : cView() {
    this->movieId = movieId;
    tabbed = true;
}

cMovieView::~cMovieView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
}

void cMovieView::LoadMedia(void) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || movieId < 1)
        return;
    movie.movieId = movieId;
    pScraper->Service("GetMovie", &movie);
}

void cMovieView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    tabs.push_back(tr("Cast"));
    tabs.push_back(tr("TheTVDB Info"));
    tabs.push_back(tr("Image Galery"));
}

void cMovieView::CreateMovieDBInfo(void) {
    if (movieDBInfo.size() > 0)
        return;
    std::stringstream info;
    info << tr("TheMovieDB Information") << ":\n\n";
    if (movie.originalTitle.size() > 0) {
        info << tr("Original Title") << ": " << movie.originalTitle << "\n\n";
    }
    if (movie.tagline.size() > 0) {
        info << tr("Tagline") << ": " << movie.tagline << "\n\n";
    }
    if (movie.overview.size() > 0) {
        info << tr("Overview") << ": " << movie.overview << "\n\n";
    }
    std::string strAdult = (movie.adult)?(tr("yes")):(tr("no"));
    info << tr("Adult") << ": " << strAdult << "\n\n";
    if (movie.collectionName.size() > 0) {
        info << tr("Collection") << ": " << movie.collectionName << "\n\n";
    }
    if (movie.budget > 0) {
        info << tr("Budget") << ": " << movie.budget << "$\n\n";
    }
    if (movie.revenue > 0) {
        info << tr("Revenue") << ": " << movie.revenue << "$\n\n";
    }
    if (movie.genres.size() > 0) {
        info << tr("Genre") << ": " << movie.genres << "\n\n";
    }
    if (movie.homepage.size() > 0) {
        info << tr("Homepage") << ": " << movie.homepage << "\n\n";
    }
    if (movie.releaseDate.size() > 0) {
        info << tr("Release Date") << ": " << movie.releaseDate << "\n\n";
    }
    if (movie.runtime > 0) {
        info << tr("Runtime") << ": " << movie.runtime << " " << tr("minutes") << "\n\n";
    }
    if (movie.popularity > 0) {
        info << tr("TheMovieDB Popularity") << ": " << movie.popularity << "\n\n";
    }
    if (movie.voteAverage > 0) {
        info << tr("TheMovieDB Vote Average") << ": " << movie.voteAverage << "\n\n";
    }
    movieDBInfo = info.str();
}

void cMovieView::DrawImages(void) {
    int totalHeight = border;
    //Fanart Height
    int fanartWidth = width - 2 * border;
    int fanartHeight = 0;
    if (movie.fanart.width > 0 && movie.fanart.height > 0 && movie.fanart.path.size() > 0) {
        fanartHeight = movie.fanart.height * ((double)fanartWidth / (double)movie.fanart.width);
        if (fanartHeight > contentHeight - 2 * border) {
            int fanartHeightOrig = fanartHeight;
            fanartHeight = contentHeight - 2 * border;
            fanartWidth = fanartWidth * ((double)fanartHeight / (double)fanartHeightOrig);
        }
        totalHeight += fanartHeight + border;
    }
    //Collection Fanart Height
    int collectionFanartWidth = width - 2 * border;
    int collectionFanartHeight = 0;
    if (movie.collectionFanart.width > 0 && movie.collectionFanart.height > 0 && movie.collectionFanart.path.size() > 0) {
        collectionFanartHeight = movie.collectionFanart.height * ((double)collectionFanartWidth / (double)movie.collectionFanart.width);
        if (collectionFanartHeight > contentHeight - 2 * border) {
            int fanartHeightOrig = collectionFanartHeight;
            collectionFanartHeight = contentHeight - 2 * border;
            collectionFanartWidth = collectionFanartWidth * ((double)collectionFanartHeight / (double)fanartHeightOrig);
        }
        totalHeight += collectionFanartHeight + border;
    }
    //Poster Height
    if (movie.poster.width > 0 && movie.poster.height > 0 && movie.poster.path.size() > 0) {
        totalHeight += movie.poster.height + border;
    }
    //Collection Popster Height
    if (movie.collectionPoster.width > 0 && movie.collectionPoster.height > 0 && movie.collectionPoster.path.size() > 0) {
        totalHeight += movie.collectionPoster.height + border;
    }

    CreateContent(totalHeight);
    
    cImageLoader imgLoader;
    int yPic = border;
    if (movie.fanart.width > 0 && movie.fanart.height > 0 && movie.fanart.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.fanart.path.c_str(), fanartWidth, fanartHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - fanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += fanartHeight + border;
            osdManager.flush();
        }
    }
    if (movie.collectionFanart.width > 0 && movie.collectionFanart.height > 0 && movie.collectionFanart.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.collectionFanart.path.c_str(), collectionFanartWidth, collectionFanartHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - collectionFanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += collectionFanartHeight + border;
            osdManager.flush();
        }
    }
    if (movie.poster.width > 0 && movie.poster.height > 0 && movie.poster.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.poster.path.c_str(), movie.poster.width, movie.poster.height) && Running()) {
            pixmapContent->DrawImage(cPoint((width - movie.poster.width) / 2, yPic), imgLoader.GetImage());
            yPic += movie.poster.height + border;
            osdManager.flush();
        }
    }
    if (movie.collectionPoster.width > 0 && movie.collectionPoster.height > 0 && movie.collectionPoster.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.collectionPoster.path.c_str(), movie.collectionPoster.width, movie.collectionPoster.height) && Running()) {
            pixmapContent->DrawImage(cPoint((width - movie.collectionPoster.width) / 2, yPic), imgLoader.GetImage());
            yPic += movie.collectionPoster.height + border;
            osdManager.flush();
        }
    }
}

void cMovieView::KeyLeft(void) { 
    if (Running())
        return;
    ClearScrollbarImage();
    activeView--;
    if (activeView < 0)
        activeView = mvtCount - 1; 
}

void cMovieView::KeyRight(void) { 
    if (Running())
        return;
    ClearScrollbarImage();
    activeView = (activeView + 1) % mvtCount;
}

void cMovieView::Action(void) {
    ClearContent();
    if (!headerDrawn) {
        DrawHeader();
        osdManager.flush();
        headerDrawn = true;
    }
    if (tabs.size() == 0) {
        SetTabs();
    }
    DrawTabs();
    bool posterAvailable = (movie.poster.path.size() > 0 && movie.poster.width > 0 && movie.poster.height > 0) ? true : false;
    switch (activeView) {
        case mvtInfo:
            if (posterAvailable)
                DrawFloatingContent(&infoText, &movie.poster);
            else
                DrawContent(&infoText);
            break;
        case mvtAddInfo:
            if (posterAvailable)
                DrawFloatingContent(&addInfoText, &movie.poster);
            else
                DrawContent(&addInfoText);
            break;
        case mvtCast:
            DrawActors(&movie.actors);
            break;
        case mvtOnlineInfo:
            CreateMovieDBInfo();
            if (posterAvailable)
                DrawFloatingContent(&movieDBInfo, &movie.poster);
            else
                DrawContent(&movieDBInfo);
            break;
        case mvtImages:
            DrawImages();
            break;
    }
    DrawScrollbar();
    osdManager.flush();
}