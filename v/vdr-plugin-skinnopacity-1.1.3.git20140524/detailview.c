#include "detailview.h"

/********************************************************************************************
* cNopacityView
********************************************************************************************/

cNopacityView::cNopacityView(cOsd *osd, cImageCache *imgCache) {
    this->osd = osd;
    this->imgCache = imgCache;
    activeView = 0;
    scrollable = false;
    tabbed = false;
    font = NULL;
    fontSmall = NULL;
    fontHeader = NULL;
    fontHeaderLarge = NULL;
    pixmapHeader = NULL;
    pixmapHeaderLogo = NULL;
    pixmapContent = NULL;
    pixmapContentBack = NULL;
    pixmapTabs = NULL;
    title = "";
    subTitle = "";
    dateTime = "";
    infoText = "";
    channel = NULL;
    eventID = 0;
    recFileName = "";
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

cNopacityView::~cNopacityView(void) {
    if (font)
        delete font;
    if (fontSmall)
        delete fontSmall;
    if (fontHeader)
        delete fontHeader;
    if (fontHeaderLarge)
        delete fontHeaderLarge;
    if (pixmapHeader)
        osd->DestroyPixmap(pixmapHeader);
    if (pixmapHeaderLogo)
        osd->DestroyPixmap(pixmapHeaderLogo);   
    if (pixmapContent)
        osd->DestroyPixmap(pixmapContent);
    if (pixmapContentBack)
        osd->DestroyPixmap(pixmapContentBack);
    if (pixmapTabs)
        osd->DestroyPixmap(pixmapTabs);
}

void cNopacityView::SetGeometry(int x, int y, int width, int height, int border, int headerHeight) { 
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->border = border;
    this->headerHeight = headerHeight;
    if (tabbed)
        tabHeight = 2 * border;
    contentHeight = height - headerHeight - tabHeight;
}

void cNopacityView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + 3 + config.GetValue("fontDetailView"));
    fontSmall = cFont::CreateFont(config.fontName, contentHeight / 30 + config.GetValue("fontDetailViewSmall"));
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + 5 + config.GetValue("fontDetailViewHeaderLarge"));
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.GetValue("fontDetailViewHeader"));
}

void cNopacityView::DrawHeader(void) {
    if (!pixmapHeader) {
        pixmapHeader = osd->CreatePixmap(4, cRect(x, y, width, headerHeight));
    }
    if (!pixmapHeaderLogo) {
        pixmapHeaderLogo = osd->CreatePixmap(5, cRect(x, y, width, headerHeight));
    }
    pixmapHeader->Fill(Theme.Color(clrMenuDetailViewBack));
    pixmapHeaderLogo->Fill(clrTransparent);
    //Channel Logo
    int logoWidth = geoManager->channelLogoWidth;
    int xText = border;
    if (channel) {
        cImage *logo = imgCache->GetLogo(ctLogo, channel);
        if (logo) {
            pixmapHeaderLogo->DrawImage(cPoint(border, max((headerHeight - geoManager->channelLogoHeight - border)/2, 0)), *logo);
            xText += logoWidth + border;
        }
    }
    //Date and Time, Title, Subtitle
    int yDateTime = border;
    int yTitle = (headerHeight - fontHeaderLarge->Height()) / 2;
    int ySubtitle = headerHeight - fontHeader->Height() - border;
    pixmapHeader->DrawText(cPoint(xText, yDateTime), dateTime.c_str(), Theme.Color(clrMenuFontDetailViewHeader), Theme.Color(clrMenuDetailViewBack), fontHeader);
    pixmapHeader->DrawText(cPoint(xText, yTitle), title.c_str(), Theme.Color(clrMenuFontDetailViewHeaderTitle), Theme.Color(clrMenuDetailViewBack), fontHeaderLarge);
    pixmapHeader->DrawText(cPoint(xText, ySubtitle), subTitle.c_str(), Theme.Color(clrMenuFontDetailViewHeader), Theme.Color(clrMenuDetailViewBack), fontHeader);
}

void cNopacityView::DrawTabs(void) {
    if (!pixmapTabs) {
        pixmapTabs = osd->CreatePixmap(4, cRect(0, y + height - tabHeight, width, tabHeight));
    }
    tColor bgColor = Theme.Color(clrMenuDetailViewTabs);
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
        pixmapTabs->DrawText(cPoint(xCurrent + (tabWidth - textWidth) / 2, 2 + (tabHeight - font->Height())/2), tabText.c_str(), Theme.Color(clrMenuFontDetailViewText), (tab == activeView) ? clrTransparent : bgColor, font);
        xCurrent += tabWidth;
    }
}

void cNopacityView::ClearContent(void) {
    if (pixmapContent && Running()) {
        osd->DestroyPixmap(pixmapContent);
        pixmapContent = NULL;
    }
}

void cNopacityView::CreateContent(int fullHeight) {
    scrollable = false;
    int drawPortHeight = contentHeight; 
    if (fullHeight > contentHeight) {
        drawPortHeight = fullHeight;
        scrollable = true;
    }
    if (!pixmapContentBack) {
        pixmapContentBack = osd->CreatePixmap(3, cRect(x, y + headerHeight, width, contentHeight + tabHeight));
        pixmapContentBack->Fill(Theme.Color(clrMenuDetailViewBack));
    }
    pixmapContent = osd->CreatePixmap(4, cRect(x, y + headerHeight, width, contentHeight), cRect(0, 0, width, drawPortHeight));
    pixmapContent->Fill(clrTransparent);
}

void cNopacityView::DrawContent(std::string *text) {
    cTextWrapper wText;
    wText.Set(text->c_str(), font, width - 2 * border);
    int lineHeight = font->Height();
    int textLines = wText.Lines();
    int textHeight = lineHeight * textLines + 2*border;
    int yText = border;
    CreateContent(textHeight);
    for (int i=0; i < textLines; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wText.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        yText += lineHeight;
    }
}

void cNopacityView::DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2) {
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
        pixmapContent->DrawText(cPoint(border, yText), wTextTall.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        yText += lineHeight;
    }
    for (int i=0; i < textLinesFull; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wTextFull.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        yText += lineHeight;
    }
    osd->Flush();
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(img->path.c_str(), imgWidth, imgHeight)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(width - imgWidth - border, border), imgLoader.GetImage());
    }
    if (!img2)
        return;
    osd->Flush();
    if (imgLoader.LoadPoster(img2->path.c_str(), imgWidth2, imgHeight2)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(width - imgWidth2 - border, imgHeight + 2*border), imgLoader.GetImage());
    }
}

void cNopacityView::CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg) {
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

void cNopacityView::DrawActors(std::vector<cActor> *actors) {
    int numActors = actors->size();
    if (numActors < 1) {
        CreateContent(100);
        pixmapContent->DrawText(cPoint(border, border), tr("No Cast available"), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontHeaderLarge);
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
    pixmapContent->DrawText(cPoint(border, border), *header, Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontHeaderLarge);

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
                pixmapContent->DrawText(cPoint(xName, y + thumbHeight + border/2), name.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                pixmapContent->DrawText(cPoint(xRole, y + thumbHeight + border/2 + fontSmall->Height()), role.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                x += thumbWidth + 2*border;
            }
            actor++;
        }
        osd->Flush();
        x = 0;
        y += thumbHeight + 2 * fontSmall->Height() + border + border/2;
    }
}

void cNopacityView::ClearScrollbar(void) {
    pixmapScrollbar->Fill(clrTransparent);
    pixmapScrollbarBack->Fill(clrTransparent);
}

void cNopacityView::DrawScrollbar(void) {
    ClearScrollbar();
    if (!scrollable || !pixmapContent)
        return;
    int totalBarHeight = pixmapScrollbar->ViewPort().Height() - 6;
    
    int aktHeight = (-1)*pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();

    int barHeight = (double)(screenHeight * totalBarHeight) / (double)totalHeight ;
    int barTop = (double)(aktHeight * totalBarHeight) / (double)totalHeight ;

    if (config.GetValue("displayType") == dtGraphical) {
        cImage *image = imgCache->GetSkinElement(seScrollbar);
        if (image)
            pixmapScrollbarBack->DrawImage(cPoint(0, 0), *image);
    } else {
        pixmapScrollbarBack->Fill(Theme.Color(clrMenuScrollBar));
        pixmapScrollbarBack->DrawRectangle(cRect(2,2,geoManager->menuWidthScrollbar-4, pixmapScrollbarBack->ViewPort().Height() - 4), Theme.Color(clrMenuScrollBarBack));
    }

    pixmapScrollbar->DrawRectangle(cRect(3,3 + barTop,geoManager->menuWidthScrollbar-6,barHeight), Theme.Color(clrMenuScrollBar));
}

bool cNopacityView::KeyUp(void) { 
    if (!scrollable)
        return false;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int lineHeight = font->Height();
    if (aktHeight >= 0) {
        return false;
    }
    int step = config.GetValue("detailedViewScrollStep") * font->Height();
    int newY = aktHeight + step;
    if (newY > 0)
        newY = 0;
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

bool cNopacityView::KeyDown(void) { 
    if (!scrollable)
        return false;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();
    
    if (totalHeight - ((-1)*aktHeight) == screenHeight) {
        return false;
    } 
    int step = config.GetValue("detailedViewScrollStep") * font->Height();
    int newY = aktHeight - step;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

/********************************************************************************************
* cNopacityEPGView : cNopacityView
********************************************************************************************/

cNopacityEPGView::cNopacityEPGView(cOsd *osd, cImageCache *imgCache) : cNopacityView(osd, imgCache) {
    tabbed = true;
    pixmapHeaderEPGImage = NULL;
    numEPGPics = -1;
    numTabs = 0;
}

cNopacityEPGView::~cNopacityEPGView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (pixmapHeaderEPGImage)
        osd->DestroyPixmap(pixmapHeaderEPGImage);
}

void cNopacityEPGView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    if (numEPGPics > 0)
        tabs.push_back(tr("Image Galery"));
    numTabs = tabs.size();
}

void cNopacityEPGView::DrawHeaderEPGImage(void) {
    cImageLoader imgLoader;
    if (eventID > 0) {
        if (!imgLoader.LoadEPGImage(eventID))
            return;
    } else if (recFileName.size() > 0) {
        if (!imgLoader.LoadRecordingImage(recFileName.c_str()))
            return;    
    } else
        return;

    if (!pixmapHeaderEPGImage) {
        pixmapHeaderEPGImage = osd->CreatePixmap(3, cRect(x, y, width, headerHeight));
    }

    int imgWidthOrig = config.GetValue("epgImageWidth");
    int imgHeightOrig =  config.GetValue("epgImageHeight");

    int imgWidth =  imgWidthOrig;
    int imgHeight = imgHeightOrig;

    if (imgHeight > (headerHeight-10)) {
        imgHeight = headerHeight - 10;
        imgWidth = imgWidthOrig * ((double)imgHeight / (double)imgHeightOrig);     
    }

    pixmapHeaderEPGImage->Fill(clrTransparent);
    pixmapHeaderEPGImage->DrawImage(cPoint(width - imgWidth - border, (headerHeight - imgHeight)/2), imgLoader.GetImage());
}

void cNopacityEPGView::CheckEPGImages(void) {
    if (eventID > 0) {
        for (int i=1; i <= config.GetValue("numAdditionalEPGPictures"); i++) {
            cString epgimage;
            if (config.epgImagePathSet) {
                epgimage = cString::sprintf("%s%d_%d.jpg", *config.epgImagePath, eventID, i);
            } else {
                epgimage = cString::sprintf("%s%d_%d.jpg", *config.epgImagePathDefault, eventID, i);
            }
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
    } else if (recFileName.size() > 0) {
        DIR *dirHandle;
        struct dirent *dirEntry;
        dirHandle = opendir(recFileName.c_str());
        int picsFound = 0;
        if (dirHandle != NULL) {
            while ( 0 != (dirEntry = readdir(dirHandle))) {
                if (endswith(dirEntry->d_name, "jpg")) {
                    std::string fileName = dirEntry->d_name;
                    if (fileName.length() > 4) {
                        fileName = fileName.substr(0, fileName.length() - 4);
                        epgPics.push_back(fileName);
                        picsFound++;
                    }
                }
                if (picsFound >= config.GetValue("numAdditionalRecEPGPictures")) {
                    break;
                }
            }
            closedir(dirHandle);
        }
    } else {
        return;
    }
    numEPGPics = epgPics.size();
}

void cNopacityEPGView::DrawImages(void) {
    int imgWidth = config.GetValue("epgImageWidthLarge");
    int imgHeight = config.GetValue("epgImageHeightLarge");

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

        } else if (recFileName.size() > 0) {
            std::string path = recFileName + "/";
            if (imgLoader.LoadAdditionalRecordingImage(path.c_str(), epgPics[pic].c_str())) {
                drawPic = true;
            }
        }
        if (drawPic) {
            pixmapContent->DrawImage(cPoint((width - imgWidth) / 2, yPic), imgLoader.GetImage());
            yPic += imgHeight + border;
            osd->Flush();
        }
    } 
}

void cNopacityEPGView::KeyLeft(void) { 
    if (Running())
        return;
    activeView--;
    if (activeView < 0)
        activeView = numTabs - 1; 
}

void cNopacityEPGView::KeyRight(void) { 
    if (Running())
        return;
    activeView = (activeView + 1) % numTabs;
}

void cNopacityEPGView::Action(void) {
    ClearContent();
    if (!headerDrawn) {
        DrawHeader();
        osd->Flush();
        DrawHeaderEPGImage();
        osd->Flush();
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
    osd->Flush();
}

/********************************************************************************************
* cNopacitySeriesView : cNopacityView
********************************************************************************************/

cNopacitySeriesView::cNopacitySeriesView(cOsd *osd, cImageCache *imgCache, int seriesId, int episodeId) : cNopacityView(osd, imgCache) {
    this->seriesId = seriesId;
    this->episodeId = episodeId;
    tvdbInfo = "";
    pixmapHeaderBanner = NULL;
    tabbed = true;
}

cNopacitySeriesView::~cNopacitySeriesView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (pixmapHeaderBanner)
        osd->DestroyPixmap(pixmapHeaderBanner);
}

void cNopacitySeriesView::LoadMedia(void) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || seriesId < 1)
        return;
    series.seriesId = seriesId;
    series.episodeId = episodeId;
    pScraper->Service("GetSeries", &series);
}

void cNopacitySeriesView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    tabs.push_back(tr("Cast"));
    tabs.push_back(tr("TheTVDB Info"));
    tabs.push_back(tr("Image Galery"));
}

void cNopacitySeriesView::CreateTVDBInfo(void) {
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

void cNopacitySeriesView::DrawHeaderBanner(void) {
    if (series.banners.size() == 0)
        return;
    if (series.banners[0].height == 0)
        return;
 
    int bannerWidthOrig = series.banners[0].width;
    int bannerHeightOrig = series.banners[0].height;
    std::string bannerPath = series.banners[0].path;

    if (bannerWidthOrig == 0)
        return;

    int bannerWidth = bannerWidthOrig;
    int bannerHeight = bannerHeightOrig;

    if (headerHeight < bannerHeightOrig) {
        bannerHeight = headerHeight;
        bannerWidth = bannerWidthOrig * ((double)bannerHeight / (double)bannerHeightOrig);
    }

    int bannerX = width - bannerWidth - border;
    int bannerY = (headerHeight - bannerHeight) / 2;

    if (!pixmapHeaderBanner) {
        pixmapHeaderBanner = osd->CreatePixmap(3, cRect(x, y, width, headerHeight));
    }
    pixmapHeaderBanner->Fill(clrTransparent);

    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(bannerPath.c_str(), bannerWidth, bannerHeight) && Running()) {
        pixmapHeaderBanner->DrawImage(cPoint(bannerX, bannerY), imgLoader.GetImage());
    }   
}

void cNopacitySeriesView::DrawImages(void) {
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
                osd->Flush();
            }
        }
        if (imgLoader.LoadPoster(series.fanarts[i].path.c_str(), fanartWidth, fanartHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - fanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += fanartHeight + border;
            osd->Flush();
        }
    }
    if (numPosters >= 1) {
        if (imgLoader.LoadPoster(series.posters[0].path.c_str(), posterWidth, posterHeight) && Running()) {
            pixmapContent->DrawImage(cPoint(border, yPic), imgLoader.GetImage());
            osd->Flush();
            yPic += posterHeight + border;
        }
    }
    if (numPosters >= 2) {
        if (imgLoader.LoadPoster(series.posters[1].path.c_str(), posterWidth, posterHeight) && Running()) {
            pixmapContent->DrawImage(cPoint(2 * border + posterWidth, yPic - posterHeight - border), imgLoader.GetImage());
            osd->Flush();
        }
    }
    if (numPosters == 3) {
        if (imgLoader.LoadPoster(series.posters[2].path.c_str(), posterWidth, posterHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - posterWidth) / 2, yPic), imgLoader.GetImage());
            osd->Flush();
        }
    }
}

int cNopacitySeriesView::GetRandomPoster(void) {
    int numPosters = series.posters.size();
    if (numPosters == 0)
        return -1;
    srand((unsigned)time(NULL));
    int randPoster = rand()%numPosters;
    return randPoster;
}

void cNopacitySeriesView::KeyLeft(void) { 
    if (Running())
        return;
    activeView--;
    if (activeView < 0)
        activeView = mvtCount - 1; 
}

void cNopacitySeriesView::KeyRight(void) { 
    if (Running())
        return;
    activeView = (activeView + 1) % mvtCount;
}

void cNopacitySeriesView::Action(void) {
    ClearContent();
    if (!headerDrawn) {
        DrawHeader();
        osd->Flush();
        DrawHeaderBanner();
        osd->Flush();
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
    osd->Flush();
}

/********************************************************************************************
* cNopacityMovieView : cNopacityView
********************************************************************************************/

cNopacityMovieView::cNopacityMovieView(cOsd *osd, cImageCache *imgCache, int movieId) : cNopacityView(osd, imgCache) {
    this->movieId = movieId;
    pixmapHeaderPoster = NULL;
    tabbed = true;
}

cNopacityMovieView::~cNopacityMovieView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (pixmapHeaderPoster)
        osd->DestroyPixmap(pixmapHeaderPoster);
}

void cNopacityMovieView::LoadMedia(void) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || movieId < 1)
        return;
    movie.movieId = movieId;
    pScraper->Service("GetMovie", &movie);    
}

void cNopacityMovieView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    tabs.push_back(tr("Cast"));
    tabs.push_back(tr("TheTVDB Info"));
    tabs.push_back(tr("Image Galery"));
}

void cNopacityMovieView::CreateMovieDBInfo(void) {
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

void cNopacityMovieView::DrawHeaderPoster(void) {
    if (movie.poster.width == 0 || movie.poster.height == 0 || movie.poster.path.size() == 0)
        return;
 
    int posterHeight = headerHeight - 10;
    int posterWidth = movie.poster.width * ((double)posterHeight / (double)movie.poster.height);;
    int posterX = width - posterWidth - border;
    int posterY = (headerHeight - posterHeight) / 2;

    if (!pixmapHeaderPoster) {
        pixmapHeaderPoster = osd->CreatePixmap(3, cRect(x, y, width, headerHeight));
    }
    pixmapHeaderPoster->Fill(clrTransparent);

    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(movie.poster.path.c_str(), posterWidth, posterHeight) && Running()) {
        pixmapHeaderPoster->DrawImage(cPoint(posterX, posterY), imgLoader.GetImage());
    }   
}

void cNopacityMovieView::DrawImages(void) {
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
            osd->Flush();
        }
    }
    if (movie.collectionFanart.width > 0 && movie.collectionFanart.height > 0 && movie.collectionFanart.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.collectionFanart.path.c_str(), collectionFanartWidth, collectionFanartHeight) && Running()) {
            pixmapContent->DrawImage(cPoint((width - collectionFanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += collectionFanartHeight + border;
            osd->Flush();
        }
    }
    if (movie.poster.width > 0 && movie.poster.height > 0 && movie.poster.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.poster.path.c_str(), movie.poster.width, movie.poster.height) && Running()) {
            pixmapContent->DrawImage(cPoint((width - movie.poster.width) / 2, yPic), imgLoader.GetImage());
            yPic += movie.poster.height + border;
            osd->Flush();
        }
    }
    if (movie.collectionPoster.width > 0 && movie.collectionPoster.height > 0 && movie.collectionPoster.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.collectionPoster.path.c_str(), movie.collectionPoster.width, movie.collectionPoster.height) && Running()) {
            pixmapContent->DrawImage(cPoint((width - movie.collectionPoster.width) / 2, yPic), imgLoader.GetImage());
            yPic += movie.collectionPoster.height + border;
            osd->Flush();
        }
    }
}

void cNopacityMovieView::KeyLeft(void) { 
    if (Running())
        return;
    activeView--;
    if (activeView < 0)
        activeView = mvtCount - 1; 
}

void cNopacityMovieView::KeyRight(void) { 
    if (Running())
        return;
    activeView = (activeView + 1) % mvtCount;
}

void cNopacityMovieView::Action(void) {
    ClearContent();
    if (!headerDrawn) {
        DrawHeader();
        osd->Flush();
        DrawHeaderPoster();
        osd->Flush();
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
    osd->Flush();
}

/********************************************************************************************
* cNopacityTextView : cNopacityView
********************************************************************************************/

cNopacityTextView::cNopacityTextView(cOsd *osd, cImageCache *imgCache) : cNopacityView(osd, imgCache) {
}

cNopacityTextView::~cNopacityTextView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
}

void cNopacityTextView::KeyLeft(void) {
    if (!pixmapContent)
        return;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int screenHeight = pixmapContent->ViewPort().Height();
    int newY = aktHeight + screenHeight;
    if (newY > 0)
        newY = 0;
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    DrawScrollbar();
    osd->Flush();
}

void cNopacityTextView::KeyRight(void) { 
    if (!pixmapContent)
        return;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int screenHeight = pixmapContent->ViewPort().Height();
    int totalHeight = pixmapContent->DrawPort().Height();
    int newY = aktHeight - screenHeight;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    DrawScrollbar();
    osd->Flush();
}

void cNopacityTextView::Action(void) {
    if (pixmapContent)
        return;
    DrawContent(&infoText);
    DrawScrollbar();
    osd->Flush();
}
