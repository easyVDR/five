#include "textwindow.h"
#include "config.h"
#include "imagecache.h"
#include "imageloader.h"
#include "helpers.h"
#include <sstream>

cNopacityTextWindow::cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin) {
    this->osd = osd;
    this->font = font;
    this->fontHeader = NULL;
    this->vidWin = vidWin;
    pixmapBackground = NULL;
    pixmap = NULL;
    scaledWindow = false;
    posterWidth = 0;
    posterHeight = 0;
    hasManualPoster = false;
    manualPosterPath = "";
    hasPoster = false;
}

cNopacityTextWindow::cNopacityTextWindow(cOsd *osd, cFont *font, cFont *fontHeader) {
    this->osd = osd;
    this->font = font;
    this->fontHeader = fontHeader;
    this->vidWin = NULL;
    pixmapBackground = NULL;
    pixmap = NULL;
    scaledWindow = false;
    posterWidth = 0;
    posterHeight = 0;
    hasManualPoster = false;
    manualPosterPath = "";
    hasPoster = false;
}

cNopacityTextWindow::~cNopacityTextWindow(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (pixmapBackground) {
        osd->DestroyPixmap(pixmapBackground);
        pixmapBackground = NULL;
    }
    if (pixmap) {
        osd->DestroyPixmap(pixmap);
        pixmap = NULL;
    }
    if ((config.GetValue("scalePicture") == 2) && scaledWindow) {
        cRect vidWinNew = cDevice::PrimaryDevice()->CanScaleVideo(oldVidWin);
        if (vidWinNew != cRect::Null) {
            vidWin->SetX(vidWinNew.X());
            vidWin->SetY(vidWinNew.Y());
            vidWin->SetWidth(vidWinNew.Width());
            vidWin->SetHeight(vidWinNew.Height());
        }
    }
}

bool cNopacityTextWindow::SetManualPoster(const cRecording *recording, bool fullscreen) {
    cString posterFound;
    cImageLoader imgLoader;
    hasManualPoster = imgLoader.SearchRecordingPoster(recording->FileName(), posterFound);
    if (hasManualPoster) {
        manualPosterPath = posterFound;
        int posterWidthOrig = config.GetValue("posterWidth");
        int posterHeightOrig = config.GetValue("posterHeight");
        if (!fullscreen) {
            posterHeight = geometry->Height() - 5;
            posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
        } else {
            posterWidth = geometry->Width() / 4;
            posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
        }
        return true;
    }
    return false;
}

void cNopacityTextWindow::SetPoster(const cEvent *event, const cRecording *recording, bool fullscreen) {
    if (!event && !recording)
        return;
    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper) {
        posterScraper2Vdr.event = event;
        posterScraper2Vdr.recording = recording;
        if (pScraper->Service("GetPoster", &posterScraper2Vdr)) {
            hasPoster = true;
            int posterWidthOrig = posterScraper2Vdr.poster.width;
            int posterHeightOrig = posterScraper2Vdr.poster.height;
            if (!fullscreen) {
                posterHeight = geometry->Height() - 5;
                posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
            } else {
                posterWidth = geometry->Width() / 4;
                posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
            }
        } else {
            hasPoster = false;
            posterHeight = 0;
            posterWidth = 0;
        }
    }
}

bool cNopacityTextWindow::SetTextScroller(int border, int left) {
    int lineHeight = font->Height();
    bool scrolling = false;
    drawportHeight = geometry->Height();
    if (!(hasPoster || hasManualPoster)) {
        drawTextTall = false;
        drawTextFull = true;
        twTextTall.Set("", font, 5);
        twTextFull.Set(*text, font, geometry->Width() - 2*border);
        int pixmapTotalHeight = lineHeight * (twTextFull.Lines()+1);
        if ((pixmapTotalHeight - (lineHeight/2)) > drawportHeight) {
            drawportHeight = pixmapTotalHeight;
            scrolling = true;
        }
    } else {
        cTextWrapper test;
        int widthTall = geometry->Width() - 2*border - left;
        test.Set(*text, font, widthTall);
        int linesTotal = test.Lines();
        int textHeight =  linesTotal * lineHeight;
        if (textHeight > posterHeight) {
            drawTextTall = true;
            drawTextFull = true;
            int lineSplit = posterHeight / lineHeight + 1;
            std::stringstream textTall;
            std::stringstream textFull;
            for (int line = 0; line < linesTotal; line++) {
                bool lineWrap = false;
                if (font->Width(test.GetLine(line)) < (widthTall - 100))
                    lineWrap = true;
                if (line < lineSplit) {
                    textTall << test.GetLine(line);
                    if (lineWrap)
                        textTall << "\n";
                    else
                        textTall << " ";
                } else {
                    textFull << test.GetLine(line);
                    if (lineWrap)
                        textFull << "\n";
                    else
                        textFull << " ";
                }
            }
            twTextTall.Set(textTall.str().c_str(), font, widthTall);
            twTextFull.Set(textFull.str().c_str(), font, geometry->Width() - 2*border);
            scrolling = true;
            drawportHeight = lineHeight * (twTextTall.Lines() + twTextFull.Lines() + 1);
        } else {
            scrolling = false;
            drawTextTall = true;
            drawTextFull = false;
            twTextTall.Set(*text, font, geometry->Width() - 2*border - left);
            twTextFull.Set("", font, 5);
        }
    }
    return scrolling;
}

void cNopacityTextWindow::CreatePixmap(void) {
    cPixmap::Lock();
    pixmapBackground = osd->CreatePixmap(4, cRect(geometry->X()-1, geometry->Y()-1, geometry->Width()+2, geometry->Height()+2));
    pixmap = osd->CreatePixmap(5, cRect(geometry->X(), geometry->Y(), geometry->Width(), geometry->Height()),
                                  cRect(0, 0, geometry->Width(), drawportHeight));
    pixmapBackground->Fill(Theme.Color(clrMenuBorder));
    pixmapBackground->DrawRectangle(cRect(1, 1, geometry->Width(), geometry->Height()), clrBlack);
    pixmap->Fill(Theme.Color(clrMenuBack));
    if (config.GetValue("menuEPGWindowFadeTime")) {
        pixmap->SetAlpha(0);
        pixmapBackground->SetAlpha(0);
    }
    cPixmap::Unlock();
}

void cNopacityTextWindow::CreatePixmapFullScreen(void) {
    pixmapBackground = osd->CreatePixmap(4, cRect(geometry->X()-1, geometry->Y()-1, geometry->Width()+2, geometry->Height()+2));
    pixmap = osd->CreatePixmap(5, cRect(geometry->X(), geometry->Y(), geometry->Width(), geometry->Height()));
    pixmapBackground->Fill(Theme.Color(clrMenuBorder));
    pixmapBackground->DrawRectangle(cRect(1, 1, geometry->Width(), geometry->Height()), Theme.Color(clrMenuTextWindow));
    pixmap->Fill(clrTransparent);
}

void cNopacityTextWindow::DrawText(int border, int left) {
    int lineHeight = font->Height();
    int currentLineHeight = lineHeight/2;
    tColor clrFontBack = (config.GetValue("displayType") != dtFlat)?(clrTransparent):(Theme.Color(clrMenuBack));
    cPixmap::Lock();
    if (drawTextTall) {
        for (int i=0; (i < twTextTall.Lines()) && Running(); i++) {
            pixmap->DrawText(cPoint(border + left, currentLineHeight), twTextTall.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrFontBack, font);
            currentLineHeight += lineHeight;
        }
    }
    if (drawTextFull) {
        for (int i=0; (i < twTextFull.Lines()) && Running(); i++) {
            pixmap->DrawText(cPoint(border, currentLineHeight), twTextFull.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrFontBack, font);
            currentLineHeight += lineHeight;
        }
    }
    cPixmap::Unlock();
}

void cNopacityTextWindow::SetEvent(const cEvent *event) {
    if (!event)
        return;
    CreatePixmapFullScreen();
    int border = config.GetValue("borderDetailedEPG");
    int width = geometry->Width();
    int height = geometry->Height();
    int widthTextHeader = width - 2 * border;
    int widthText = widthTextHeader;
    int y = border;
    cImageLoader imgLoader;
    bool epgImageFound = false;
    if (hasPoster) {
        int posterX = width - posterWidth - border;
        if (imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
            pixmap->DrawImage(cPoint(posterX, border), imgLoader.GetImage());
            widthTextHeader -= posterWidth + border;
        }
    } else if (imgLoader.LoadEPGImage(event->EventID())) {
        epgImageFound = true;
        pixmap->DrawImage(cPoint(width - config.GetValue("epgImageWidth") - border, y), imgLoader.GetImage());
        widthTextHeader -= config.GetValue("epgImageWidth") + border;
    }
    //Title
    y = DrawTextWrapper(event->Title(), widthTextHeader, y, border, fontHeader, Theme.Color(clrMenuFontDetailViewHeaderTitle), height);
    //Short Text
    y = DrawTextWrapper(event->ShortText(), widthTextHeader, y, border, font, Theme.Color(clrMenuFontDetailViewHeader), height);

    y += fontHeader->Height();
    //Description
    if (hasPoster && (y < (border + posterHeight))) {
        int heightNarrow = border + posterHeight - y;
        DrawTextWrapperFloat(event->Description(),
                             widthTextHeader, widthText, y, heightNarrow,
                             border, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else if (epgImageFound && (y < (border + config.GetValue("epgImageHeight")))) {
        y = border + config.GetValue("epgImageHeight");
        DrawTextWrapper(event->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else {
        DrawTextWrapper(event->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), height);
    }
}

void cNopacityTextWindow::SetRecording(const cRecording *recording) {
    if (!recording)
        return;
    CreatePixmapFullScreen();
    int border = config.GetValue("borderDetailedRecordings");
    int width = geometry->Width();
    int height = geometry->Height();
    int widthTextHeader = width - 2 * border;
    int widthText = widthTextHeader;
    int y = border;
    //Image
    cImageLoader imgLoader;
    bool recImageFound = false;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            int posterX = width - posterWidth - border;
            pixmap->DrawImage(cPoint(posterX, border), imgLoader.GetImage());
            widthTextHeader -= posterWidth + border;
        }
    } else if (hasPoster) {
        int posterX = width - posterWidth - border;
        if (imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
            pixmap->DrawImage(cPoint(posterX, border), imgLoader.GetImage());
            widthTextHeader -= posterWidth + border;
        }
    } else if (imgLoader.LoadRecordingImage(recording->FileName())) {
        pixmap->DrawImage(cPoint(width - config.GetValue("epgImageWidth") - border, y), imgLoader.GetImage());
        widthTextHeader -= config.GetValue("epgImageWidth") + border;
        recImageFound = true;
    }
    const cRecordingInfo *info = recording->Info();
    if (!info)
        return;
    cString recTitle;
    if (info->Title())
        recTitle = info->Title();
    else
        recTitle = recording->Name();
    //Title
    y = DrawTextWrapper(*recTitle, widthTextHeader, y, border, fontHeader, Theme.Color(clrMenuFontDetailViewHeaderTitle), height);
    //Short Text
    if (!isempty(info->ShortText())) {
        y = DrawTextWrapper(info->ShortText(), widthTextHeader, y, border, font, Theme.Color(clrMenuFontDetailViewHeader), height);
    }
    y += fontHeader->Height();
    //Description
    if ((hasPoster || hasManualPoster) && (y < (border + posterHeight))) {
        int heightNarrow = border + posterHeight - y;
        DrawTextWrapperFloat(recording->Info()->Description(),
                             widthTextHeader, widthText, y, heightNarrow,
                             border, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else if (recImageFound && (y < (border + config.GetValue("epgImageHeight")))) {
        y = border + config.GetValue("epgImageHeight");
        DrawTextWrapper(recording->Info()->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else {
        DrawTextWrapper(recording->Info()->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), height);
    }
}

int cNopacityTextWindow::DrawTextWrapper(const char *text, int width, int top, int x,
                                         const cFont *font, tColor color, int maxHeight) {
    cTextWrapper wrapper;
    int lineHeight = font->Height();
    wrapper.Set(text, font, width);
    int y = top;
    for (int i=0; i < wrapper.Lines(); i++) {
        if (y + 2*lineHeight > maxHeight) {
            pixmap->DrawText(cPoint(x, y), "...", color, clrTransparent, font);
            y += 2*lineHeight;
            break;
        }
        pixmap->DrawText(cPoint(x, y), wrapper.GetLine(i), color, clrTransparent, font);
        y += lineHeight;
    }
    return y;
}

void cNopacityTextWindow::DrawTextWrapperFloat(const char *text, int widthSmall, int widthFull,
                                              int top, int heightNarrow, int x, const cFont *font,
                                              tColor color, int maxHeight) {

    if (!text)
        return;
    int lineHeight = font->Height();
    int numLinesNarrow = heightNarrow / lineHeight + 1;
    splitstring s(text);
    std::vector<std::string> flds = s.split('\n', 1);
    if (flds.size() < 1)
        return;
    int y = top;
    int linesDrawn = 0;
    bool drawNarrow = true;
    for (int i=0; i<flds.size(); i++) {
        if (!flds[i].size()) {
            //empty line
            linesDrawn++;
            y += lineHeight;
        } else {
            cTextWrapper wrapper;
            if (drawNarrow) {
                wrapper.Set((flds[i].c_str()), font, widthSmall);
                int newLines = wrapper.Lines();
                //check if wrapper fits completely into narrow area
                if (linesDrawn + newLines < numLinesNarrow) {
                    y = DrawTextWrapper(flds[i].c_str(), widthSmall, y, x, font, color, maxHeight);
                    linesDrawn += newLines;
                } else {
                    //this wrapper has to be splitted
                    std::stringstream sstrTextTall;
                    std::stringstream sstrTextFull;
                    for (int line = 0; line < wrapper.Lines(); line++) {
                        if (line + linesDrawn < numLinesNarrow) {
                            sstrTextTall << wrapper.GetLine(line) << " ";
                        } else {
                            sstrTextFull << wrapper.GetLine(line) << " ";
                        }
                    }
                    y = DrawTextWrapper(sstrTextTall.str().c_str(), widthSmall, y, x, font, color, maxHeight);
                    y = DrawTextWrapper(sstrTextFull.str().c_str(), widthFull, y, x, font, color, maxHeight);
                    drawNarrow = false;
                }
            } else {
                if (y > maxHeight)
                    break;
                y = DrawTextWrapper(flds[i].c_str(), widthFull, y, x, font, color, maxHeight);
            }
        }
    }
}

void cNopacityTextWindow::DrawPoster(int border) {
    int posterY = font->Height() / 2;
    cImageLoader imgLoader;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            pixmap->DrawImage(cPoint(border, posterY), imgLoader.GetImage());
        }
    } else if (imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
        pixmap->DrawImage(cPoint(border, posterY), imgLoader.GetImage());
    }
}

void cNopacityTextWindow::DoSleep(int duration) {
    int sleepSlice = 10;
    for (int i = 0; Running() && (i*sleepSlice < duration); i++)
        cCondWait::SleepMs(sleepSlice);
}

void cNopacityTextWindow::ScaleVideoWindow(void) {
    oldVidWin.SetX(vidWin->X());
    oldVidWin.SetY(vidWin->Y());
    oldVidWin.SetWidth(vidWin->Width());
    oldVidWin.SetHeight(vidWin->Height());
    cRect availableRect(vidWin->X(), vidWin->Y(), vidWin->Width(), vidWin->Height() - geometry->Height());
    cRect vidWinNew = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
    if (vidWinNew != cRect::Null) {
        vidWin->SetX(vidWinNew.X());
        vidWin->SetY(vidWinNew.Y());
        vidWin->SetWidth(vidWinNew.Width());
        vidWin->SetHeight(vidWinNew.Height());
        scaledWindow = true;
    }
}

void cNopacityTextWindow::Action(void) {
    if (! *text)
        return;

    DoSleep(config.GetValue("menuInfoTextDelay")*1000);

    if (config.GetValue("scalePicture") == 2) {
          ScaleVideoWindow();
    }

    int border = 5;
    int left = 0;
    if (hasPoster || hasManualPoster)
        left = 10 + posterWidth;
    bool scrolling = false;
    if (Running()) {
        scrolling = SetTextScroller(border, left);
        CreatePixmap();
    }
    if (Running()) {
        DrawText(border, left);
    }
    if (Running() && (hasPoster || hasManualPoster)) {
        DrawPoster(border);
    }
    //FadeIn
    if (config.GetValue("menuEPGWindowFadeTime")) {
        uint64_t Start = cTimeMs::Now();
        int FadeTime = config.GetValue("menuEPGWindowFadeTime");
        int FadeFrameTime = FadeTime / 10;
        while (Running()) {
            uint64_t Now = cTimeMs::Now();
            cPixmap::Lock();
            double t = min(double(Now - Start) / FadeTime, 1.0);
            int Alpha = t * ALPHA_OPAQUE;
            pixmapBackground->SetAlpha(Alpha);
            pixmap->SetAlpha(Alpha);
            cPixmap::Unlock();
            if (Running())
                osd->Flush();
            int Delta = cTimeMs::Now() - Now;
            if (Running() && (Delta < FadeFrameTime))
                cCondWait::SleepMs(FadeFrameTime - Delta);
            if ((int)(Now - Start) > FadeTime)
                break;
        }
    }

    if (scrolling && Running()) {
        int scrollDelay = config.GetValue("menuInfoScrollDelay") * 1000;
        DoSleep(scrollDelay);
        int drawPortY;
        int FrameTime = 0;
        if (config.GetValue("menuInfoScrollSpeed") == 1)
            FrameTime = 50;
        else if (config.GetValue("menuInfoScrollSpeed") == 2)
            FrameTime = 30;
        else if (config.GetValue("menuInfoScrollSpeed") == 3)
            FrameTime = 15;

        int maxY = pixmap->DrawPort().Height() - pixmap->ViewPort().Height();
        bool doSleep = false;
        while (Running()) {
            uint64_t Now = cTimeMs::Now();
            cPixmap::Lock();
            drawPortY = pixmap->DrawPort().Y();
            drawPortY -= 1;
            cPixmap::Unlock();
            if (abs(drawPortY) > maxY) {
                doSleep = true;
                DoSleep(scrollDelay);
                drawPortY = 0;
            }
            cPixmap::Lock();
            if (Running())
                pixmap->SetDrawPortPoint(cPoint(0, drawPortY));
            cPixmap::Unlock();
            if (doSleep) {
                doSleep = false;
                DoSleep(scrollDelay);
            }
            int Delta = cTimeMs::Now() - Now;
            if (Running())
                osd->Flush();
            if (Running() && (Delta < FrameTime))
                cCondWait::SleepMs(FrameTime - Delta);
        }
    }
}
