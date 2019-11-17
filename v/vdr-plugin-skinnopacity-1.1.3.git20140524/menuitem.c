#include "menuitem.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include "config.h"
#include "helpers.h"
#include "imageloader.h"

// cNopacityMenuItem  -------------

cNopacityMenuItem::cNopacityMenuItem(cOsd *osd, cImageCache *imgCache, const char *text, bool sel) {
    this->osd = osd;
    this->imgCache = imgCache;
    drawn = false;
    Text = text;
    selectable = sel;
    current = false;
    wasCurrent = false;
    scrollable = false;
    itemTabs = NULL;
    tabWidth = NULL;
    pixmapStatic = NULL;
    pixmapTextScroller = NULL;
    pixmapForeground = NULL;
    infoTextWindow = NULL;
}

cNopacityMenuItem::~cNopacityMenuItem(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    delete [] itemTabs;
    delete [] tabWidth;
    osd->DestroyPixmap(pixmapBackground);
    if (pixmapStatic) {
        osd->DestroyPixmap(pixmapStatic);
    }
    if (pixmapTextScroller) {
        osd->DestroyPixmap(pixmapTextScroller);
    }
    if (pixmapForeground) {
        osd->DestroyPixmap(pixmapForeground);
    }
    if (infoTextWindow) {
        delete infoTextWindow;
    }
}

void cNopacityMenuItem::SetCurrent(bool cur) {
    wasCurrent = current;
    current = cur;
}

void cNopacityMenuItem::SetGeometry(int index, int top, int left, int width, int height, int spaceMenu) {
    this->index = index;
    this->top = top;
    this->left = left;
    this->width = width;
    this->height = height;
    this->spaceMenu = spaceMenu;
}

void cNopacityMenuItem::CreatePixmapBackground() {
    pixmapBackground = osd->CreatePixmap(3, cRect(left, top + index * (height + spaceMenu), width, height));
}

void cNopacityMenuItem::CreatePixmapForeground() {
    pixmapForeground = osd->CreatePixmap(6, cRect(left, top + index * (height + spaceMenu), width, height));
}

void cNopacityMenuItem::CreatePixmapStatic(void) {
    pixmapStatic = osd->CreatePixmap(5, cRect(left, top + index * (height + spaceMenu), width, height));
    pixmapStatic->Fill(clrTransparent);
}

void cNopacityMenuItem::CreatePixmapTextScroller(int totalWidth) {
    pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + spaceMenu), width, height), cRect(0, 0, totalWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityMenuItem::SetTabs(cString *tabs, int *tabWidths, int numtabs) {
    if (itemTabs)
        delete [] itemTabs;
    if (tabWidth)
        delete [] tabWidth;
    itemTabs = tabs;
    tabWidth = tabWidths;
    numTabs = numtabs;
}

void cNopacityMenuItem::DrawDelimiter(const char *del, const char *icon, eSkinElementType seType) {
    if ((config.GetValue("displayType") == dtBlending) && (seType != seNone)) {
        pixmapBackground->Fill(Theme.Color(clrSeparatorBorder));
        cImage *back = imgCache->GetSkinElement(seType);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrSeparatorBorder));
    } else if ((config.GetValue("displayType") == dtGraphical) && (seType != seNone)) {
        cImage *back = imgCache->GetSkinElement(seType);
        if (back)
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        else
            pixmapBackground->Fill(clrTransparent);
        pixmapForeground->Fill(clrTransparent);
    } else {
        pixmapBackground->Fill(Theme.Color(clrSeparatorBorder));
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrMenuItem));
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrSeparatorBorder));
    }
    if (!drawn) {
        cImage *imgIcon = imgCache->GetSkinIcon(icon, height-2*geoManager->menuSpace, height-2*geoManager->menuSpace);
        if (imgIcon) {
            if (pixmapStatic == NULL) {
                pixmapStatic = osd->CreatePixmap(5, cRect(left, top + index * (height + spaceMenu), width, height));
                pixmapStatic->Fill(clrTransparent);
            }
            pixmapStatic->DrawImage(cPoint(geoManager->menuSpace, geoManager->menuSpace), *imgIcon);
        }
        drawn = true;
    }
    std::string delimiter = del;
    try {
        if (delimiter.find_first_not_of("-") > 0)
            delimiter.erase(0, delimiter.find_first_not_of("-")+1);
        if (delimiter.find_last_not_of("-") != std::string::npos)
            delimiter.erase(delimiter.find_last_not_of("-")+1);
    } catch (...) {}
    int x = height + 3;
    int y = (height - font->Height()) / 2;
    pixmapStatic->DrawText(cPoint(x, y), delimiter.c_str(), Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
}

void cNopacityMenuItem::Action(void) {
    bool carriageReturn = (config.GetValue("scrollMode") == 0) ? true : false;
    int scrollDelta = 1;
    int scrollDelay = config.GetValue("menuScrollDelay") * 1000;
    DoSleep(scrollDelay);
    cPixmap::Lock();
    if (Running())
        SetTextFull();
    cPixmap::Unlock();
    int drawPortX;

    int FrameTime = 0;
    if (config.GetValue("menuScrollSpeed") == 1)
        FrameTime = 50;
    else if (config.GetValue("menuScrollSpeed") == 2)
        FrameTime = 30;
    else if (config.GetValue("menuScrollSpeed") == 3)
        FrameTime = 15;
    if (!Running())
        return;
    int maxX = pixmapTextScroller->DrawPort().Width() - pixmapTextScroller->ViewPort().Width();
    bool doSleep = false;
    while (Running()) {
        if (doSleep) {
            DoSleep(scrollDelay);
            doSleep = false;
        }
        if (!Running())
            return;
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        drawPortX = pixmapTextScroller->DrawPort().X();
        drawPortX -= scrollDelta;
        cPixmap::Unlock();

        if (abs(drawPortX) > maxX) {
            DoSleep(scrollDelay);
            if (carriageReturn)
                drawPortX = 0;
            else {
                scrollDelta *= -1;
                drawPortX -= scrollDelta;
            }
            doSleep = true;
        }
        if (!carriageReturn && (drawPortX == 0)) {
            scrollDelta *= -1;
            doSleep = true;
        }
        cPixmap::Lock();
        if (Running())
            pixmapTextScroller->SetDrawPortPoint(cPoint(drawPortX, 0));
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running())
            osd->Flush();
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
    }
}

void cNopacityMenuItem::DoSleep(int duration) {
    int sleepSlice = 10;
    for (int i = 0; Running() && (i*sleepSlice < duration); i++)
        cCondWait::SleepMs(sleepSlice);
}

void cNopacityMenuItem::DrawRoundedCorners(tColor borderColor) {
    int radius = config.GetValue("cornerRadius");
    if (radius < 3)
        return;
    pixmapBackground->DrawEllipse(cRect(0,0,radius,radius), borderColor, -2);
    pixmapBackground->DrawEllipse(cRect(-1,-1,radius,radius), clrTransparent, -2);

    pixmapBackground->DrawEllipse(cRect(width-radius,0,radius,radius), borderColor, -1);
    pixmapBackground->DrawEllipse(cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);

    pixmapBackground->DrawEllipse(cRect(0,height-radius,radius,radius), borderColor, -3);
    pixmapBackground->DrawEllipse(cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);

    pixmapBackground->DrawEllipse(cRect(width-radius,height-radius,radius,radius), borderColor, -4);
    pixmapBackground->DrawEllipse(cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
}

void cNopacityMenuItem::DrawChannelLogoBackground(void) {
    if (!config.GetValue("menuChannelLogoBackground"))
        return;
    int logoWidth = geoManager->menuLogoWidth;
    pixmapBackground->DrawRectangle(cRect(4,6,logoWidth-4, height-12), Theme.Color(clrMenuChannelLogoBack));
}

// cNopacityMainMenuItem  -------------
cNopacityMainMenuItem::cNopacityMainMenuItem(cOsd *osd, cImageCache *imgCache, const char *text, bool sel, bool setup) : cNopacityMenuItem (osd, imgCache, text, sel) {
    this->isSetup = setup;
}

cNopacityMainMenuItem::~cNopacityMainMenuItem(void) {
}

void cNopacityMainMenuItem::DrawBackground(void) {
    cImage *back = NULL;
    if ((config.GetValue("displayType") == dtBlending) || (config.GetValue("displayType") == dtGraphical)){
        eSkinElementType type;
        if (!isSetup)
            type = (current)?seMainHigh:seMain;
        else
            type = (current)?seSetupHigh:seSetup;
        back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        }
    }
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        if (back) {
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        }
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
        eSkinElementType menuButtonTop;
        if (!isSetup)
            menuButtonTop = seMainTop;
        else
            menuButtonTop = seSetupTop;
        cImage *fore = imgCache->GetSkinElement(menuButtonTop);
        if (fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            pixmapForeground->Fill(clrTransparent);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
}

std::string cNopacityMainMenuItem::items[16] = { "Schedule", "Channels", "Timers", "Recordings", "Setup", "Commands",
                                                "OSD", "EPG", "DVB", "LNB", "CAM", "Recording", "Replay", "Miscellaneous", "Plugins", "Restart"};

cString cNopacityMainMenuItem::GetIconName() {
    std::string element = *menuEntry;
    //check for standard menu entries
    for (int i=0; i<16; i++) {
        std::string s = trVDR(items[i].c_str());
        if (s == element) {
            cString menuIcon = cString::sprintf("menuIcons/%s", items[i].c_str());
            return menuIcon;
        }
    }
    //check for special main menu entries "stop recording", "stop replay"
    std::string stopRecording = skipspace(trVDR(" Stop recording "));
    std::string stopReplay = skipspace(trVDR(" Stop replaying"));
    try {
        if (element.substr(0, stopRecording.size()) == stopRecording)
            return "menuIcons/StopRecording";
        if (element.substr(0, stopReplay.size()) == stopReplay)
            return "menuIcons/StopReplay";
    } catch (...) {}
    //check for Plugins
    for (int i = 0; ; i++) {
        cPlugin *p = cPluginManager::GetPlugin(i);
        if (p) {
            const char *mainMenuEntry = p->MainMenuEntry();
            if (mainMenuEntry) {
                std::string plugMainEntry = mainMenuEntry;
                try {
                    if (element.substr(0, plugMainEntry.size()) == plugMainEntry) {
                        return cString::sprintf("pluginIcons/%s", p->Name());
                    }
                } catch (...) {}
            }
        } else
            break;
    }
    return cString::sprintf("extraIcons/%s", *menuEntry);
}

void cNopacityMainMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + 10;
    if (config.GetValue("useMenuIcons"))
        pixmapLeft += geoManager->menuMainMenuIconSize;
    int pixmapWidth = width - 10;
    if (config.GetValue("useMenuIcons"))
        pixmapWidth -= geoManager->menuMainMenuIconSize;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityMainMenuItem::CreateText() {
    std::string text = skipspace(Text);
    bool found = false;
    bool doBreak = false;
    size_t i = 0;
    for (; i < text.length(); i++) {
        char s = text.at(i);
        if (i==0) {
            //if text directly starts with nonnumeric, break
            if (!(s >= '0' && s <= '9')) {
                break;
            }
        }
        if (found) {
            if (!(s >= '0' && s <= '9')) {
                doBreak = true;
            }
        }
        if (s >= '0' && s <= '9') {
            found = true;
        }
        if (doBreak)
            break;
        if (i>4)
            break;
    }
    if (found) {
        menuNumber = skipspace(text.substr(0,i).c_str());
        menuEntry = skipspace(text.substr(i).c_str());
    } else {
        menuNumber = "";
        menuEntry = text.c_str();
    }
    strEntry = *menuEntry;
}

int cNopacityMainMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += geoManager->menuMainMenuIconSize;
    int totalTextWidth = width - spaceLeft;
    int numberWidth = font->Width("xxx");
    int textWidth = font->Width(*menuEntry);
    if ((numberWidth +  textWidth) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(numberWidth + textWidth, totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, width - spaceLeft - numberWidth, font);
    }
    return totalTextWidth;
}

void cNopacityMainMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = 0;
    int numberTotalWidth = font->Width("xxx");
    int numberWidth = font->Width(*menuNumber);
    pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
    x += numberTotalWidth;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = 0;
    int numberTotalWidth = font->Width("xxx");
    int numberWidth = font->Width(*menuNumber);
    pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
    x += numberTotalWidth;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::Render() {
    DrawBackground();
    if (selectable) {
        if (config.GetValue("useMenuIcons")) {
            cString cIcon = GetIconName();
            if (!drawn) {
                cImage *imgIcon = imgCache->GetMenuIcon(*cIcon);
                if (imgIcon)
                    pixmapStatic->DrawImage(cPoint(geoManager->menuSpace, geoManager->menuSpace), *imgIcon);
                drawn = true;
            }
        }
        SetTextShort();
        if (current && scrollable && !Running() && config.GetValue("menuScrollSpeed")) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
    } else {
        DrawDelimiter(strEntry.c_str(), "skinIcons/channeldelimiter", (config.GetValue("displayType") != dtFlat)?seNone:(isSetup?seSetup:seMain));
    }
}

// cNopacityScheduleMenuItem  -------------

cNopacityScheduleMenuItem::cNopacityScheduleMenuItem(cOsd *osd, cImageCache *imgCache, const cEvent *Event, const cChannel *Channel, eTimerMatch TimerMatch, bool sel, eMenuCategory category, cRect *vidWin)
                            : cNopacityMenuItem (osd, imgCache, "", sel) {
    this->category = category;
    this->Event = Event;
    this->Channel = Channel;
    this->TimerMatch = TimerMatch;
    this->vidWin = vidWin;
    strDateTime = "";
    strTitle = "";
    strSubTitle = "";
    strTitleFull = "";
    strSubTitleFull = "";
    scrollTitle = false;
    scrollSubTitle = false;
}

cNopacityScheduleMenuItem::~cNopacityScheduleMenuItem(void) {
}

void cNopacityScheduleMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int drawPortWidth = totalWidth + 10;
    int pixmapLeft = left;
    int pixmapWidth = width;
    if (Channel) {
        pixmapLeft += geoManager->menuLogoWidth + geoManager->menuSpace;
        pixmapWidth = pixmapWidth - geoManager->menuLogoWidth - geoManager->menuSpace;
    }
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityScheduleMenuItem::CreateText() {
    if (!Event)
        return;
    std::stringstream sstrDateTime;
    time_t now = time(NULL);
    std::string dayNow = *WeekDayName(now);
    std::string dayEvent = *WeekDayName(Event->StartTime());
    if ((Event->StartTime() - now > 86400) || (dayNow != dayEvent))
        sstrDateTime << dayEvent << " " << *ShortDateString(Event->StartTime()) << " ";
    sstrDateTime << *Event->GetTimeString();
    sstrDateTime << " - " << *Event->GetEndTimeString();
    strDateTime = sstrDateTime.str();
    if (Event->Title())
        strTitle = Event->Title();
    if (Event->ShortText())
        strSubTitle = Event->ShortText();
}

int cNopacityScheduleMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += geoManager->menuLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strTitle.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        scrollTitle = true;
        totalTextWidth = max(font->Width(strTitle.c_str()), totalTextWidth);
        strTitleFull = strTitle.c_str();
        strSubTitleFull = strSubTitle.c_str();
        strTitle = CutText(strTitle, width - spaceLeft, font);
    }
    if (fontSmall->Width(strSubTitle.c_str()) > (width - spaceLeft)) {
        if (!scrollable) {
            scrollable = true;
            strTitleFull = strTitle.c_str();
            strSubTitleFull = strSubTitle.c_str();
        }
        scrollSubTitle = true;
        totalTextWidth = max(fontSmall->Width(strSubTitle.c_str()), totalTextWidth);
        strSubTitle = CutText(strSubTitle, width - spaceLeft, fontSmall);
    }
    return totalTextWidth;

}

void cNopacityScheduleMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(5, titleY), strTitleFull.c_str(), clrFont, clrTransparent, font);
    pixmapTextScroller->DrawText(cPoint(5, titleY + font->Height() - 2), strSubTitleFull.c_str(), clrFont, clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(5, titleY), strTitle.c_str(), clrFont, clrTransparent, font);
    pixmapTextScroller->DrawText(cPoint(5, titleY + font->Height() - 2), strSubTitle.c_str(), clrFont, clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::Render() {
    int logoWidth = geoManager->menuLogoWidth;
    int logoHeight = geoManager->menuLogoHeight;
    textLeft = 5;
    if (Channel && Channel->Name())
        textLeft = logoWidth + 10;

    if (selectable) {
        titleY = (height - font->Height())/2 - 2;
        DrawBackground(textLeft);
        if (Channel && Channel->Name())
            DrawChannelLogoBackground();
        int progressBarDelta = 0;
        if (config.GetValue("displayType") == dtGraphical && textLeft < 20)
            progressBarDelta = 10;
        DrawRemaining(textLeft + progressBarDelta, height*7/8, width - textLeft - 20 - progressBarDelta);
        if (!drawn) {
            DrawLogo(logoWidth, logoHeight);
            drawn = true;
        }
        SetTextShort();
        if (current && scrollable && !Running() && config.GetValue("menuScrollSpeed")) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        if (current && Event) {
            if (config.GetValue("menuSchedulesWindowMode") == 0) {
                //window mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetText(Event->Description());
                infoTextWindow->SetPoster(Event, NULL);
                infoTextWindow->Start();
            } else {
                //fullscreen mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetPoster(Event, NULL, true);
                infoTextWindow->SetEvent(Event);
            }
        }
    } else {
        if (Event) {
            DrawDelimiter(Event->Title(), "skinIcons/daydelimiter", (config.GetValue("displayType")!=dtFlat)?seSchedules:seNone);
        } else if (Channel) {
            DrawDelimiter(Channel->Name(), "skinIcons/channeldelimiter", (config.GetValue("displayType")!=dtFlat)?seSchedules:seNone);
        }
    }
}

void cNopacityScheduleMenuItem::DrawBackground(int textLeft) {
    eSkinElementType type = (current)?seSchedulesHigh:seSchedules;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
        cImage *fore = imgCache->GetSkinElement(seSchedulesTop);
        if (fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            pixmapForeground->Fill(clrTransparent);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }

    if (TimerMatch == tmFull) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/activetimer", 64, 64);
        if (imgIcon)
            pixmapStatic->DrawImage(cPoint(width - 66, 2), *imgIcon);
    } else if (TimerMatch == tmPartial) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/activetimersmall", 32, 32);
        if (imgIcon)
            pixmapStatic->DrawImage(cPoint(width - 34, 2), *imgIcon);

    }
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(textLeft, 3), strDateTime.c_str(), clrFont, clrTransparent, font);
}

void cNopacityScheduleMenuItem::DrawLogo(int logoWidth, int logoHeight) {
    if (Channel && Channel->Name()) {
        cImage *logo = imgCache->GetLogo(ctLogoMenuItem, Channel);
        if (logo) {
            pixmapStatic->DrawImage(cPoint(1,1), *logo);
        } else {
            cTextWrapper channel;
            channel.Set(Channel->Name(), font, logoWidth);
            int lines = channel.Lines();
            int lineHeight = height / 3;
            int heightChannel = lines * lineHeight;
            int y = (heightChannel>height)?0:(height-heightChannel)/2;
            for (int line = 0; line < lines; line++) {
                pixmapStatic->DrawText(cPoint((logoWidth - font->Width(channel.GetLine(line)))/2, y+lineHeight*line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, font);
            }
        }
    }
}

void cNopacityScheduleMenuItem::DrawRemaining(int x, int y, int width) {
    if (!Event)
        return;
    time_t now = time(NULL);
    if ((now < Event->StartTime()) || (now > Event->EndTime()))
        return;
    int seen = now - Event->StartTime();
    int total = Event->EndTime() - Event->StartTime();
    if (total == 0)
        return;
    double percentSeen = (double)seen/total;

    tColor clrBack = (current)?Theme.Color(clrProgressBarBackHigh):Theme.Color(clrProgressBarBack);
    tColor clrBar = (current)?Theme.Color(clrProgressBarHigh):Theme.Color(clrProgressBar);
    pixmapBackground->DrawEllipse(cRect(x, y, 7, 7), clrBack);
    pixmapBackground->DrawEllipse(cRect(x+width, y, 7, 7), clrBack);
    pixmapBackground->DrawRectangle(cRect(x+4, y, width-1, 7), clrBack);
    pixmapBackground->DrawEllipse(cRect(x+1, y+1, 5, 5), clrBar);

    if (percentSeen > 0.0)
        pixmapBackground->DrawEllipse(cRect(x+(width*percentSeen), y+1, 5, 5), clrBar);
    pixmapBackground->DrawRectangle(cRect(x+4, y+1, (width-1)*percentSeen, 5), clrBar);
}

// cNopacityChannelMenuItem  -------------

cNopacityChannelMenuItem::cNopacityChannelMenuItem(cOsd *osd, cImageCache *imgCache, const cChannel *Channel, bool sel, cRect *vidWin) : cNopacityMenuItem (osd, imgCache, "", sel) {
    this->Channel = Channel;
    this->vidWin = vidWin;
    strEntry = "";
    strEntryFull = "";
    strChannelSource = "";
    strChannelInfo = "";
    strEpgInfo = "";
    strEpgInfoFull = "";
    strTimeInfo = "";
}

cNopacityChannelMenuItem::~cNopacityChannelMenuItem(void) {
}

void cNopacityChannelMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + geoManager->menuLogoWidth + geoManager->menuSpace;
    int pixmapWidth = width - geoManager->menuLogoWidth - geoManager->menuSpace;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityChannelMenuItem::CreateText() {
    strEntry = cString::sprintf("%d %s", Channel->Number(), Channel->Name());
    if (config.GetValue("menuChannelDisplayMode") == 0) {
        const cSource *source = Sources.Get(Channel->Source());
        if (source)
            strChannelSource = cString::sprintf("%s - %s", *cSource::ToString(source->Code()),  source->Description());
        else
            strChannelSource = "";
        strChannelInfo = cString::sprintf("%s %d, %d MHz", tr("Transp."), Channel->Transponder(), Channel->Frequency()/1000);
    } else {
        readCurrentEPG();
    }
}

int cNopacityChannelMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += geoManager->menuLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strEntry.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, width - spaceLeft, font);
    } else
        strEntryFull = strEntry;

    if (fontSmall->Width(strEpgInfo.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(fontSmall->Width(strEpgInfo.c_str()), totalTextWidth);
        strEpgInfoFull = strEpgInfo.c_str();
        strEpgInfo = CutText(strEpgInfo, width - spaceLeft, fontSmall);
    } else
        strEpgInfoFull = strEpgInfo;

    return totalTextWidth;
}

void cNopacityChannelMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int heightChannelName = 0;
    if (config.GetValue("menuChannelDisplayMode") == 2) {
        heightChannelName = (height - font->Height())/2;
    } else {
        heightChannelName = (height/2 - font->Height())/2;
    }
    pixmapTextScroller->DrawText(cPoint(5, heightChannelName), strEntryFull.c_str(), clrFont, clrTransparent, font);
    if (config.GetValue("menuChannelDisplayMode") == 1) {
        int heightTimeInfo = 0;
        int heightEPGInfo = 0;
        if (config.GetValue("menuChannelDisplayTime")) {
            heightTimeInfo = height/2 + (height/4 - fontSmall->Height())/2;
            heightEPGInfo = 3*height/4 + (height/4 - fontSmall->Height())/2;
        } else {
            heightEPGInfo = height/2 + (height/4 - fontSmall->Height())/2;
        }
        if (config.GetValue("menuChannelDisplayTime")) {
            pixmapTextScroller->DrawText(cPoint(5, heightTimeInfo), strTimeInfo.c_str(), clrFont, clrTransparent, fontSmall);
        }
        pixmapTextScroller->DrawText(cPoint(5, heightEPGInfo), strEpgInfoFull.c_str(), clrFont, clrTransparent, fontSmall);
    }
}

void cNopacityChannelMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int heightChannelName = 0;
    if (config.GetValue("menuChannelDisplayMode") == 2) {
        heightChannelName = (height - font->Height())/2;
    } else {
        heightChannelName = (height/2 - font->Height())/2;
    }
    pixmapTextScroller->DrawText(cPoint(5, heightChannelName), strEntry.c_str(), clrFont, clrTransparent, font);
    if (config.GetValue("menuChannelDisplayMode") == 1) {
        int heightTimeInfo = 0;
        int heightEPGInfo = 0;
        if (config.GetValue("menuChannelDisplayTime")) {
            heightTimeInfo = height/2 + (height/4 - fontSmall->Height())/2;
            heightEPGInfo = 3*height/4 + (height/4 - fontSmall->Height())/2;
        } else {
            heightEPGInfo = height/2 + (height/4 - fontSmall->Height())/2;
        }
        if (config.GetValue("menuChannelDisplayTime")) {
            pixmapTextScroller->DrawText(cPoint(5, heightTimeInfo), strTimeInfo.c_str(), clrFont, clrTransparent, fontSmall);
        }
        pixmapTextScroller->DrawText(cPoint(5, heightEPGInfo), strEpgInfo.c_str(), clrFont, clrTransparent, fontSmall);
    }
}

void cNopacityChannelMenuItem::DrawBackground(void) {
    eSkinElementType type = (current)?seChannelsHigh:seChannels;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
        cImage *fore = imgCache->GetSkinElement(seChannelsTop);
        if (fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            pixmapForeground->Fill(clrTransparent);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }

    if (config.GetValue("menuChannelDisplayMode") == 0) {
        int encryptedSize = height/4-2;
        int sourceX = geoManager->menuLogoWidth + 15;
        tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
        pixmapStatic->DrawText(cPoint(sourceX, 3*height/4 + (height/4 - fontSmall->Height())/2), *strChannelInfo, clrFont, clrTransparent, fontSmall);
        if (Channel->Ca()) {
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/encrypted", encryptedSize, encryptedSize);
            if (imgIcon) {
                pixmapStatic->DrawImage(cPoint(sourceX, height/2+1), *imgIcon);
                sourceX += encryptedSize + 10;
            }
        }
        pixmapStatic->DrawText(cPoint(sourceX, height/2 + (height/4 - fontSmall->Height())/2), *strChannelSource, clrFont, clrTransparent, fontSmall);
    }
}

void cNopacityChannelMenuItem::readCurrentEPG(void) {
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
    const cSchedule *Schedule = NULL;
	Schedule = schedules->GetSchedule(Channel);
	if (!Schedule) {
		strEpgInfo = tr("No EPG Information found");
        strTimeInfo = "";
	} else {
        const cEvent *PresentEvent = Schedule->GetPresentEvent();
        if (!PresentEvent) {
            strEpgInfo = tr("No EPG Information found");
            strTimeInfo = "";
        } else {
            strEpgInfo = PresentEvent->Title();
            strTimeInfo = *cString::sprintf("%s - %s:", *PresentEvent->GetTimeString(), *PresentEvent->GetEndTimeString());
        }
    }
}

std::string cNopacityChannelMenuItem::readEPG(void) {
    std::stringstream sstrText;
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
    const cSchedule *Schedule = NULL;
	Schedule = schedules->GetSchedule(Channel);
	if (!Schedule) {
		sstrText << tr("No EPG Information found");
	} else {
        const cEvent *PresentEvent = Schedule->GetPresentEvent();
        int i=0;
        if (!PresentEvent) {
            sstrText << tr("No EPG Information found");
        } else {
            for (const cEvent *event = PresentEvent; event; event = Schedule->Events()->Next(event)) {
                if (event) {
                    sstrText << *event->GetTimeString();
                    sstrText << " ";
                    sstrText << event->Title();
                    if (event->ShortText() && (strlen(event->ShortText()) > 1))
                        sstrText << " ~ " << event->ShortText();
                    i++;
                    if (i < config.GetValue("numEPGEntriesChannelsMenu"))
                        sstrText << "\n";
                    else
                        break;
                }
            }
        }
    }
    return sstrText.str();
}

void cNopacityChannelMenuItem::Render() {
    if (selectable) {                           //Channels
        DrawBackground();
        DrawChannelLogoBackground();
        if (!drawn) {
            cImage *logo = imgCache->GetLogo(ctLogoMenuItem, Channel);
            if (logo) {
                pixmapStatic->DrawImage(cPoint(1,1), *logo);
            }
            drawn = true;
        }
        SetTextShort();
        if (current && scrollable && !Running() && config.GetValue("menuScrollSpeed")) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        if (current && Channel && (config.GetValue("menuChannelDisplayMode") == 0)) {
            infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
            infoTextWindow->SetGeometry(textWindow);
            infoTextWindow->SetText(readEPG().c_str());
            infoTextWindow->Start();
        }
    } else {                                    //Channelseparators
        DrawDelimiter(Channel->Name(), "skinIcons/channeldelimiter", (config.GetValue("displayType")!=dtFlat )?seChannels:seNone);
    }
}

// cNopacityTimerMenuItem  -------------

cNopacityTimerMenuItem::cNopacityTimerMenuItem(cOsd *osd, cImageCache *imgCache, const cTimer *Timer, bool sel, cRect *vidWin) : cNopacityMenuItem (osd, imgCache, "", sel) {
    this->Timer = Timer;
    this->vidWin = vidWin;
}

cNopacityTimerMenuItem::~cNopacityTimerMenuItem(void) {
}

void cNopacityTimerMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + geoManager->menuLogoWidth + geoManager->menuSpace;
    int pixmapWidth = width - geoManager->menuLogoWidth - geoManager->menuSpace;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityTimerMenuItem::CreateText() {
    const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(Timer->File(), FOLDERDELIMCHAR);
    if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
        File++;
    else
        File = Timer->File();
    strEntry = File;
    strDateTime = CreateDate();
}

std::string cNopacityTimerMenuItem::CreateDate(void) {
    cString day, dayName("");
    if (Timer->WeekDays())
        day = Timer->PrintDay(0, Timer->WeekDays(), false);
    else if (Timer->Day() - time(NULL) < 28 * SECSINDAY) {
        day = itoa(Timer->GetMDay(Timer->Day()));
        dayName = WeekDayName(Timer->Day());
    } else {
        struct tm tm_r;
        time_t Day = Timer->Day();
        localtime_r(&Day, &tm_r);
        char buffer[16];
        strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
        day = buffer;
    }
    cString dateString;
    if (Timer->Channel())
        dateString = *cString::sprintf("%d - %s%s%s (%02d:%02d-%02d:%02d)", Timer->Channel()->Number(), *dayName, *dayName && **dayName ? " " : "", *day, Timer->Start() / 100, Timer->Start() % 100, Timer->Stop() / 100, Timer->Stop() % 100);
    else
        dateString = *cString::sprintf("%s%s%s (%02d:%02d-%02d:%02d)", *dayName, *dayName && **dayName ? " " : "", *day, Timer->Start() / 100, Timer->Start() % 100, Timer->Stop() / 100, Timer->Stop() % 100);
    return *dateString;
}

int cNopacityTimerMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += geoManager->menuLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strEntry.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, width - spaceLeft, font);
    }
    return totalTextWidth;
}

void cNopacityTimerMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, height/2 + (height/2 - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityTimerMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, height/2 + (height/2 - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityTimerMenuItem::DrawBackground(int textLeft) {
    eSkinElementType type = (current)?seTimersHigh:seTimers;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
        cImage *fore = imgCache->GetSkinElement(seTimersTop);
        if (fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            pixmapForeground->Fill(clrTransparent);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
    int iconSize = height/2;
    cString iconName("");
    bool firstDay = false;
    if (!(Timer->HasFlags(tfActive)))
        iconName = "skinIcons/timerInactive";
    else if (Timer->FirstDay()) {
        iconName = "skinIcons/timerActive";
        firstDay = true;
    } else if (Timer->Recording())
        iconName = "skinIcons/timerRecording";
    else
        iconName = "skinIcons/timerActive";

    cImage *imgIcon = imgCache->GetSkinIcon(*iconName, iconSize, iconSize);
    if (imgIcon)
        pixmapStatic->DrawImage(cPoint(textLeft, 0), *imgIcon);

    cString dateTime("");
    if (firstDay)
        dateTime = cString::sprintf("! %s", strDateTime.c_str());
    else
        dateTime = strDateTime.c_str();
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(textLeft + iconSize, (height/2 - fontSmall->Height())/2), *dateTime, clrFont, clrTransparent, fontSmall);
}

void cNopacityTimerMenuItem::Render() {
    textLeft = geoManager->menuLogoWidth + geoManager->menuSpace;
    if (selectable) {
        DrawBackground(textLeft);
        DrawChannelLogoBackground();
        int logoWidth = geoManager->menuLogoWidth;
        int logoHeight = geoManager->menuLogoHeight;
        if (!drawn) {
            DrawLogo(logoWidth, logoHeight);
            drawn = true;
        }
        if (!Running())
            SetTextShort();
        if (current && scrollable && !Running() && config.GetValue("menuScrollSpeed")) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        const cEvent *Event = Timer->Event();
        if (current && Event) {
            if (config.GetValue("menuTimersWindowMode") == 0) {
                //window mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetText(Event->Description());
                infoTextWindow->SetPoster(Event, NULL, false);
                infoTextWindow->Start();
            } else {
                //fullscreen mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetPoster(Event, NULL, true);
                infoTextWindow->SetEvent(Event);
            }
        }
    }
}

void cNopacityTimerMenuItem::DrawLogo(int logoWidth, int logoHeight) {
    if (Timer && Timer->Channel() && Timer->Channel()->Name()) {
        cImage *logo = imgCache->GetLogo(ctLogoMenuItem, Timer->Channel());
        if (logo) {
            pixmapStatic->DrawImage(cPoint(1,1), *logo);
        } else {
            cTextWrapper channel;
            channel.Set(Timer->Channel()->Name(), font, logoWidth);
            int lines = channel.Lines();
            int lineHeight = height / 3;
            int heightChannel = lines * lineHeight;
            int y = (heightChannel>height)?0:(height-heightChannel)/2;
            for (int line = 0; line < lines; line++) {
                pixmapStatic->DrawText(cPoint((logoWidth - font->Width(channel.GetLine(line)))/2, y+lineHeight*line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, font);
            }
        }
    }
}
// cNopacityRecordingMenuItem  -------------

cNopacityRecordingMenuItem::cNopacityRecordingMenuItem(cOsd *osd, cImageCache *imgCache, const cRecording *Recording, bool sel, bool isFolder, int Level, int Total, int New, cRect *vidWin) : cNopacityMenuItem (osd, imgCache, "", sel) {
    this->Recording = Recording;
    this->isFolder = isFolder;
    this->Level = Level;
    this->Total = Total;
    this->New = New;
    this->vidWin = vidWin;
    posterWidth = 0;
    posterHeight = 0;
    hasManualPoster = false;
    manualPosterPath = "";
    hasPoster = false;
    hasThumb = false;
}

cNopacityRecordingMenuItem::~cNopacityRecordingMenuItem(void) {
}

void cNopacityRecordingMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = 0;
    int pixmapWidth = 0;
    int drawPortWidth = totalWidth + 10;
    pixmapLeft = posterWidth + left + 20;
    pixmapWidth = width - posterWidth - 20;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityRecordingMenuItem::CreateText() {
    std::string recName = Recording->Name();
    try {
        std::vector<std::string> tokens;
        std::istringstream f(recName.c_str());
        std::string s;
        while (std::getline(f, s, FOLDERDELIMCHAR)) {
            tokens.push_back(s);
        }
        strRecName = tokens.at(Level);
        if (!isFolder && Recording->IsEdited()) {
            strRecName = strRecName.substr(1);
        }
    } catch (...) {
        strRecName = recName.c_str();
    }
}

void cNopacityRecordingMenuItem::SetPoster(void) {
    posterHeight = height - 10;
    posterWidth = config.GetValue("posterWidth") * ((double)posterHeight / (double)config.GetValue("posterHeight"));
    //check first if manually set poster exists
    cString posterFound;
    cImageLoader imgLoader;
    hasManualPoster = imgLoader.SearchRecordingPoster(Recording->FileName(), posterFound);
    if (hasManualPoster)
        manualPosterPath = posterFound;
    //no manually set poster found, check scraper
    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper) {
        thumb.event = NULL;
        thumb.recording = Recording;
        if (pScraper->Service("GetPosterThumb", &thumb)) {
            hasThumb = true;
        } else {
            hasThumb = false;
        }
    }
}

int cNopacityRecordingMenuItem::CheckScrollable(bool hasIcon) {
    int totalWidth = 0;
    if (isFolder)
        totalWidth = CheckScrollableFolder();
    else
        totalWidth = CheckScrollableRecording();
    return totalWidth;
}

int cNopacityRecordingMenuItem::CheckScrollableRecording(void) {
    int spaceLeft = spaceMenu + posterWidth + 15;
    int iconWidth = 0;
    int totalTextWidth = width - spaceLeft;
    strRecNameFull = strRecName.c_str();
    if (font->Width(strRecName.c_str()) + iconWidth > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strRecName.c_str()) + iconWidth, totalTextWidth);
        strRecName = CutText(strRecName, width - spaceLeft - iconWidth, font);
    }
    return totalTextWidth;
}

int cNopacityRecordingMenuItem::CheckScrollableFolder(void) {
    int spaceLeft = spaceMenu + posterWidth + 15;
    int totalTextWidth = width - spaceLeft;
    strRecNameFull = strRecName.c_str();
    if (font->Width(strRecName.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strRecName.c_str()), totalTextWidth);
        strRecName = CutText(strRecName, width - spaceLeft, font);
    }
    return totalTextWidth;
}


void cNopacityRecordingMenuItem::SetTextFull(void) {
    if (isFolder)
        SetTextFullFolder();
    else
        SetTextFullRecording();
}

void cNopacityRecordingMenuItem::DrawBackground(void) {
    eSkinElementType type = (current)?seRecordingsHigh:seRecordings;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
        cImage *fore = imgCache->GetSkinElement(seRecordingsTop);
        if (fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            pixmapForeground->Fill(clrTransparent);
        }

    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
}

void cNopacityRecordingMenuItem::SetTextFullFolder(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    if (config.GetValue("useFolderPoster") && (hasPoster || hasThumb || hasManualPoster))
        DrawPoster();
    else
        DrawFolderIcon();
    int heightRecName = (2*height/3 - font->Height())/2 + 10;
    pixmapTextScroller->DrawText(cPoint(5, heightRecName), strRecNameFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityRecordingMenuItem::SetTextFullRecording(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int textLeft = 5;
    int heightRecName = (height / 2 - font->Height())/2;
    pixmapTextScroller->DrawText(cPoint(textLeft, heightRecName), strRecNameFull.c_str(), clrFont, clrTransparent, font);
}


void cNopacityRecordingMenuItem::SetTextShort(void) {
    if (isFolder)
        SetTextShortFolder();
    else
        SetTextShortRecording();
}

void cNopacityRecordingMenuItem::SetTextShortFolder(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    if (config.GetValue("useFolderPoster") && (hasPoster || hasThumb || hasManualPoster))
        DrawPoster();
    else
        DrawFolderIcon();
    int heightRecName = (2*height/3 - font->Height())/2 + 10;
    pixmapTextScroller->DrawText(cPoint(5, heightRecName), strRecName.c_str(), clrFont, clrTransparent, font);
}

void cNopacityRecordingMenuItem::SetTextShortRecording(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int textLeft = 5;
    int heightRecName = (height / 2 - font->Height())/2;
    pixmapTextScroller->DrawText(cPoint(textLeft, heightRecName), strRecName.c_str(), clrFont, clrTransparent, font);
}


void cNopacityRecordingMenuItem::DrawRecordingNewIcon(void) {
    int iconNewSize = height/3;

    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/newrecording", iconNewSize, iconNewSize);
    if (imgIcon) {
        int iconX = pixmapStatic->ViewPort().Width() - iconNewSize;
        int iconY = height/2;
        pixmapStatic->DrawImage(cPoint(iconX, iconY), *imgIcon);
    }
}

void cNopacityRecordingMenuItem::DrawRecordingEditedIcon(void) {
    int iconCutSize = height/3;
    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/recordingcutted", iconCutSize, iconCutSize);
    if (imgIcon) {
        int iconX = pixmapStatic->ViewPort().Width() - iconCutSize;
        if (Recording->IsNew())
            iconX -= iconCutSize;
        int iconY = height/2;
        pixmapStatic->DrawImage(cPoint(iconX, iconY), *imgIcon);
    }
}

void cNopacityRecordingMenuItem::DrawFolderIcon(void) {
    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/recfolder", posterWidth, posterHeight);
    if (imgIcon) {
        pixmapStatic->DrawImage(cPoint(10, 1), *imgIcon);
    }
}

void cNopacityRecordingMenuItem::DrawRecDateTime(void) {
    int left = posterWidth + 20;
    pixmapStatic->DrawRectangle(cRect(left, 0, width, height), clrTransparent);
    const cEvent *Event = NULL;
    Event = Recording->Info()->GetEvent();
    cString strDateTime("");
    cString strDuration("");
    if (Event) {
        std::string strDate = *(Event->GetDateString());
        cString strTime = Event->GetTimeString();
        if (strDate.find("1970") != std::string::npos) {
            time_t start = Recording->Start();
            strDateTime = cString::sprintf("%s %s", *DateString(start),*TimeString(start));
        } else {
            strDateTime = cString::sprintf("%s - %s", strDate.c_str(), *strTime);
        }
        int duration = Event->Duration() / 60;
        int recDuration = Recording->LengthInSeconds();
        recDuration = (recDuration>0)?(recDuration / 60):0;
        strDuration = cString::sprintf("%s: %d %s, %s: %d %s", tr("Duration"), duration, tr("min"), tr("recording"), recDuration, tr("min"));
    }

    int textHeight = height/2 + (height/4 - fontSmall->Height())/2;
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(10 + left, textHeight), *strDateTime, clrFont, clrTransparent, fontSmall);
    textHeight += height/4;
    pixmapStatic->DrawText(cPoint(10 + left, textHeight), *strDuration, clrFont, clrTransparent, fontSmall);
}

void cNopacityRecordingMenuItem::DrawFolderNewSeen(void) {
    int textHeight = 2*height/3 + (height/3 - fontSmall->Height())/2 - 10;
    cString strTotalNew = cString::sprintf("%d %s (%d %s)", Total, (Total > 1)?tr("recordings"):tr("recording"), New, tr("new"));
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(posterWidth + 30, textHeight), *strTotalNew, clrFont, clrTransparent, fontSmall);
}

void cNopacityRecordingMenuItem::DrawPoster(void) {
    cImageLoader imgLoader;
    bool posterDrawn = false;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            posterDrawn = true;
            pixmapStatic->DrawImage(cPoint(10, 5), imgLoader.GetImage());
        }
    } else if (hasThumb) {
        if (imgLoader.LoadPoster(thumb.poster.path.c_str(), posterWidth, posterHeight)) {
            posterDrawn = true;
            pixmapStatic->DrawImage(cPoint(10, 5), imgLoader.GetImage());
        }
    }
    if (!posterDrawn) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/defaultPoster", posterWidth, posterHeight);
        if (imgIcon)
            pixmapStatic->DrawImage(cPoint(10,5), *imgIcon);
    }
}

void cNopacityRecordingMenuItem::Render() {
    if (selectable) {
        DrawBackground();
        if (isFolder) {
            DrawFolderNewSeen();
            SetTextShort();
        } else {
            DrawPoster();
            DrawRecDateTime();
            if (Recording->IsNew()) {
                DrawRecordingNewIcon();
            }
            if (Recording->IsEdited()) {
                DrawRecordingEditedIcon();
            }
            SetTextShort();
        }
        if (current && scrollable && !Running() && config.GetValue("menuScrollSpeed")) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (!isFolder) {
            if (wasCurrent)
                if (infoTextWindow) {
                    delete infoTextWindow;
                    infoTextWindow = NULL;
                }
            if (current) {
                if (config.GetValue("menuRecordingsWindowMode") == 0) {
                    //window mode
                    infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                    infoTextWindow->SetGeometry(textWindow);
                    infoTextWindow->SetText(Recording->Info()->Description());
                    if (!infoTextWindow->SetManualPoster(Recording))
                        infoTextWindow->SetPoster(NULL, Recording);
                    infoTextWindow->Start();
                } else {
                    //fullscreen mode
                    infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                    infoTextWindow->SetGeometry(textWindow);
                    if (!infoTextWindow->SetManualPoster(Recording, true))
                        infoTextWindow->SetPoster(NULL, Recording, true);
                    infoTextWindow->SetRecording(Recording);
                }
            }
        }
    }
}


// cNopacityDefaultMenuItem  -------------

cNopacityDefaultMenuItem::cNopacityDefaultMenuItem(cOsd *osd, cImageCache *imgCache, const char *text, bool sel) : cNopacityMenuItem (osd, imgCache, text, sel) {
    scrollCol = -1;
}

cNopacityDefaultMenuItem::~cNopacityDefaultMenuItem(void) {
}

bool cNopacityDefaultMenuItem::CheckProgressBar(const char *text) {
    if (strlen(text) > 5
        && text[0] == '['
        && ((text[1] == '|')||(text[1] == ' '))
        && ((text[2] == '|')||(text[2] == ' '))
        && text[strlen(text) - 1] == ']')
        return true;
    return false;
}

void cNopacityDefaultMenuItem::DrawBackground(void) {
    eSkinElementType type = (current)?seDefaultHigh:seDefault;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
}

void cNopacityDefaultMenuItem::DrawProgressBar(int x, int width, const char *bar, tColor color) {
    const char *p = bar + 1;
    bool isProgressbar = true;
    int total = 0;
    int now = 0;
    for (; *p != ']'; ++p) {
        if (*p == ' ' || *p == '|') {
            ++total;
            if (*p == '|')
                ++now;
        } else {
            isProgressbar = false;
            break;
        }
    }
    if (isProgressbar) {
        pixmapStatic->DrawRectangle(cRect(x+5, height/4, width-10, height/2), color);
        pixmapStatic->DrawRectangle(cRect(x+7, height/4+2, width-14, height/2-4), clrTransparent);
        double progress = (double)now/(double)total;
        pixmapStatic->DrawRectangle(cRect(x+8, height/4+3, (width-16)*progress, height/2-6), color);

    }
}

void cNopacityDefaultMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = (scrollCol == 0)?5:0;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityDefaultMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = (scrollCol == 0)?5:0;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), strEntry.c_str(), clrFont, clrTransparent, font);
}

int cNopacityDefaultMenuItem::CheckScrollable(bool hasIcon) {
    if (!selectable)
        return 0;
    scrollable = false;
    int colWidth = 0;
    int colTextWidth = 0;
    for (int i=0; i<numTabs; i++) {
        if (tabWidth[i] > 0) {
            if (CheckProgressBar(*itemTabs[i]))
                continue;
            colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
            colTextWidth = font->Width(*itemTabs[i]);
            if (colTextWidth > colWidth) {
                cTextWrapper itemTextWrapped;
                scrollable = true;
                scrollCol = i;
                strEntryFull = *itemTabs[i];
                itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
                strEntry = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
                break;
            }
        } else
            break;
    }
    if (scrollable) {
        if (!pixmapTextScroller) {
            pixmapTextScroller = osd->CreatePixmap(4, cRect(left + tabWidth[scrollCol], top + index * (height + spaceMenu), tabWidth[scrollCol+numTabs], height), cRect(0, 0, colTextWidth+10, height));
            pixmapTextScroller->Fill(clrTransparent);
        }
    } else {
        if (pixmapTextScroller) {
            while (Running()) {
                Cancel(-1);
                DoSleep(10);
            }
            osd->DestroyPixmap(pixmapTextScroller);
            pixmapTextScroller = NULL;
            scrollCol = -1;
        }
    }
    return 0;
}

bool cNopacityDefaultMenuItem::DrawHeaderElement(void) {
    const char *c = Text + 3;
    while (*c == '-')
        c++;
    if (*c == ' ' || *c == '\t') {
        // it's a headline
        while (*c == ' ' || *c == '\t') // find start of headline text
            c++;

        // find end of headline text
        char *c2 = (char*)(c + strlen(c) - 1);
        while (*c2 == '-')
            c2--;
        while (*c2 == ' ' || *c2 == '\t')
            c2--;
        *(c2 + 1) = 0;

        int left = 5 + tabWidth[0];
        pixmapStatic->DrawText(cPoint(left, (height - font->Height()) / 2), c, Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
        return true;
    }
    return false;
}

void cNopacityDefaultMenuItem::Render() {
    DrawBackground();
    pixmapStatic->Fill(clrTransparent);

    tColor clrFont;
    if (current)
        clrFont = Theme.Color(clrMenuFontMenuItemHigh);
    else if (!selectable)
        clrFont = Theme.Color(clrMenuFontMenuItemSep);
    else
        clrFont = Theme.Color(clrMenuFontMenuItem);

    if (!selectable && (strncmp(Text, "---", 3) == 0)) {
        if (DrawHeaderElement())
            return;
    }
    int colWidth = 0;
    int colTextWidth = 0;
    cString itemText("");
    for (int i=0; i<numTabs; i++) {
        if (tabWidth[i] > 0) {
            colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
            int posX = tabWidth[i];
            if (CheckProgressBar(*itemTabs[i])) {
                    DrawProgressBar(posX, colWidth, *itemTabs[i], clrFont);
            } else if (i != scrollCol) {
                colTextWidth = font->Width(*itemTabs[i]);
                if (colTextWidth > colWidth) {
                    cTextWrapper itemTextWrapped;
                    if (selectable) {
                        itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
                        itemText = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
                    } else {
                        itemTextWrapped.Set(*itemTabs[i], font, colWidth);
                        itemText = cString::sprintf("%s", itemTextWrapped.GetLine(0));
                    }
                } else {
                    itemText = itemTabs[i];
                }
                if (i==0) posX += 5;
                pixmapStatic->DrawText(cPoint(posX, (height - font->Height()) / 2), *itemText, clrFont, clrTransparent, font);
            } else {
                if (!Running())
                    SetTextShort();
            }
        } else
            break;
    }

    if (current && scrollable && !Running() && config.GetValue("menuScrollSpeed")) {
        Start();
    }
    if (wasCurrent && !current && scrollable && Running()) {
        pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
        SetTextShort();
        Cancel(-1);
    }
}

// cNopacityTrackMenuItem  -------------

cNopacityTrackMenuItem::cNopacityTrackMenuItem(cOsd *osd, cImageCache *imgCache, const char *text) : cNopacityMenuItem (osd, imgCache, text, true) {
}

cNopacityTrackMenuItem::~cNopacityTrackMenuItem(void) {
}

void cNopacityTrackMenuItem::Render() {
    eSkinElementType type = (current)?seTracksHigh:seTracks;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            pixmapBackground->Fill(clrTransparent);
        }
        cImage *fore = imgCache->GetSkinElement(seTracksTop);
        if (fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            pixmapForeground->Fill(clrTransparent);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrMenuBorder));
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrTracksFontButtons);
    pixmapStatic->DrawText(cPoint(5, (height - font->Height())/2), Text, clrFont, clrTransparent, font);
}
