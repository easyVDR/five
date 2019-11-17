#include "displaytracks.h"
#include "config.h"
#include "helpers.h"
#include "imageloader.h"


cNopacityDisplayTracks::cNopacityDisplayTracks(cImageCache *imgCache, const char *Title, int NumTracks, const char * const *Tracks) {
    this->imgCache = imgCache;
    initial = true;
    currentIndex = -1;
    numTracks = NumTracks;
    audioChannelLast = -5;
    FadeTime = config.GetValue("tracksFadeTime");
    FrameTime = FadeTime / 10;
    SetGeometry();
    CreatePixmaps();
    DrawHeader(Title);
    for (int i = 0; i < NumTracks; i++)
        SetItem(Tracks[i], i, false);
}

cNopacityDisplayTracks::~cNopacityDisplayTracks() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapContainer);
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapHeaderAudio);
    menuItems.Clear();
    delete osd;
}

void cNopacityDisplayTracks::SetGeometry(void) {
    width = geoManager->trackWidth;
    height = (config.GetValue("tracksItemHeight") +4) * (numTracks+1);

    menuItemWidth = geoManager->menuItemWidthTracks;
    menuItemHeight = geoManager->menuItemHeightTracks;

    int top, left;
    switch(config.GetValue("tracksPosition")) {
        case 0:     //middle bottom
            top = geoManager->osdTop  + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + (geoManager->osdWidth - width) / 2;
            break;
        case 1:     //left bottom
            top = geoManager->osdTop + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + config.GetValue("tracksBorderHorizontal");
            break;
        case 2:     //left middle
            top = geoManager->osdTop + (geoManager->osdHeight - height) / 2;
            left = geoManager->osdLeft + config.GetValue("tracksBorderHorizontal");
            break;
        case 3:     //left top
            top = geoManager->osdTop + config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + config.GetValue("tracksBorderHorizontal");
            break;
        case 4:     //top middle
            top = geoManager->osdTop + config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + (geoManager->osdWidth - width) / 2;
            break;
        case 5:     //top right
            top = geoManager->osdTop + config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + geoManager->osdWidth - width - config.GetValue("tracksBorderHorizontal");
            break;
        case 6:     //right middle
            top = geoManager->osdTop + (geoManager->osdHeight - height) / 2;
            left = geoManager->osdLeft + geoManager->osdWidth - width - config.GetValue("tracksBorderHorizontal");
            break;
        case 7:     //right bottom
            top = geoManager->osdTop + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + geoManager->osdWidth - width - config.GetValue("tracksBorderHorizontal");
            break;
        default:    //middle bottom
            top = geoManager->osdTop  + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + (geoManager->osdWidth - width) / 2;
            break;
    }
    osd = CreateOsd(left, top, width, height);
}

void cNopacityDisplayTracks::CreatePixmaps(void) {
    pixmapContainer = osd->CreatePixmap(1, cRect(0, 0, width, height));
    pixmapHeader = osd->CreatePixmap(2, cRect(2, 2, menuItemWidth, menuItemHeight));
    pixmapHeaderAudio = osd->CreatePixmap(3, cRect(menuItemWidth - menuItemHeight, 2, menuItemHeight, menuItemHeight));
    if (FadeTime) {
        pixmapContainer->SetAlpha(0);
        pixmapHeader->SetAlpha(0);
        pixmapHeaderAudio->SetAlpha(0);
    }
}

void cNopacityDisplayTracks::DrawHeader(const char *Title) {
    pixmapContainer->Fill(Theme.Color(clrMenuBorder));
    pixmapContainer->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrMenuBack));
    if (config.GetValue("displayType") == dtBlending) {
        pixmapHeader->Fill(Theme.Color(clrMenuItem));
        cImage *back = imgCache->GetSkinElement(seTracks);
        if (back)
            pixmapHeader->DrawImage(cPoint(1, 1), *back);
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(seTracks);
        if (back)
            pixmapHeader->DrawImage(cPoint(0, 0), *back);
    } else {
        pixmapHeader->Fill(Theme.Color(clrMenuItem));
        pixmapHeader->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrAudioMenuHeader));
    }
    pixmapIcon = osd->CreatePixmap(3, cRect(2, 2, menuItemHeight-2, menuItemHeight-2));
    pixmapIcon->Fill(clrTransparent);

    cImage *imgTracks = imgCache->GetSkinIcon("skinIcons/tracks", menuItemHeight-6, menuItemHeight-6);
    if (imgTracks)
        pixmapIcon->DrawImage(cPoint(3,3), *imgTracks);
    pixmapIcon->DrawText(cPoint((width - fontManager->trackHeader->Width(Title)) / 2, (menuItemHeight - fontManager->trackHeader->Height()) / 2), Title, Theme.Color(clrTracksFontHead), clrTransparent, fontManager->trackHeader);
}

void cNopacityDisplayTracks::SetItem(const char *Text, int Index, bool Current) {
    cNopacityMenuItem *item;
    item = new cNopacityTrackMenuItem(osd, imgCache, Text);
    item->SetCurrent(Current);
    item->SetFont(fontManager->trackText);
    item->SetGeometry(Index, menuItemHeight+4, 2, menuItemWidth, menuItemHeight, 4);
    item->CreatePixmapBackground();
    item->CreatePixmapStatic();
    if (config.GetValue("displayType") == dtGraphical)
        item->CreatePixmapForeground();
    menuItems.Add(item);
    item->Render();
}

void cNopacityDisplayTracks::SetTrack(int Index, const char * const *Tracks) {
    cNopacityMenuItem *item;
    if (currentIndex >= 0) {
        item = menuItems.Get(currentIndex);
        item->SetCurrent(false);
        item->Render();
    }
    item = menuItems.Get(Index);
    item->SetCurrent(true);
    item->Render();
    currentIndex = Index;
}

void cNopacityDisplayTracks::SetAudioChannel(int AudioChannel) {
    if (AudioChannel != audioChannelLast) {
        audioChannelLast = AudioChannel;
        pixmapHeaderAudio->Fill(clrTransparent);
        cString icon("");
        switch (AudioChannel) {
            case -1:
                icon = "skinIcons/ac3";
                break;
            case 0:
                icon = "skinIcons/stereo";
                break;
            default:
                icon = "skinIcons/stereo";
                break;
        }
        cImage *imgIcon = imgCache->GetSkinIcon(*icon, menuItemHeight-2, menuItemHeight-2);
        if (imgIcon)
            pixmapHeaderAudio->DrawImage(cPoint(1,1), *imgIcon);

    }
}

void cNopacityDisplayTracks::Flush(void) {
    if (initial)
        if (FadeTime)
            Start();
    initial = false;
    osd->Flush();
}

void cNopacityDisplayTracks::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        pixmapContainer->SetAlpha(Alpha);
        pixmapHeader->SetAlpha(Alpha);
        pixmapHeaderAudio->SetAlpha(Alpha);
        for (cNopacityMenuItem *item = menuItems.First(); Running() && item; item = menuItems.Next(item)) {
            item->SetAlpha(Alpha);
        }
        cPixmap::Unlock();
        if (Running())
            osd->Flush();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
