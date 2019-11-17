#include "symbols/mute.xpm"
#include "displayvolume.h"

#include "config.h"
#include "helpers.h"

cNopacityDisplayVolume::cNopacityDisplayVolume(cImageCache *imgCache) {
    this->imgCache = imgCache;
    initial = true;
    muted = false;
    FadeTime = config.GetValue("volumeFadeTime");
    FrameTime = FadeTime / 10;

    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->volumeHeight - config.GetValue("volumeBorderBottom");
    int left = geoManager->osdLeft + (geoManager->osdWidth - geoManager->volumeWidth) / 2;
    osd = CreateOsd(left, top, geoManager->volumeWidth, geoManager->volumeHeight);

    pixmapBackground = osd->CreatePixmap(1, cRect(0, 0, geoManager->volumeWidth, geoManager->volumeHeight));

    if (config.GetValue("displayType") == dtGraphical) {
        cImage *imgBack = imgCache->GetSkinElement(seVolumeBackground);
        if (imgBack) {
            pixmapBackground->DrawImage(cPoint(0, 0), *imgBack);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrChannelBackground));
        if (config.GetValue("displayType") == dtBlending) {
            DrawBlendedBackground(pixmapBackground,
                                  0,
                                  geoManager->volumeWidth,
                                  Theme.Color(clrChannelBackground),
                                  Theme.Color(clrChannelBackBlend),
                                  true);
            DrawBlendedBackground(pixmapBackground,
                                  0,
                                  geoManager->volumeWidth,
                                  Theme.Color(clrChannelBackground),
                                  Theme.Color(clrChannelBackBlend),
                                  false);
        }
        int cornerRadius = geoManager->volumeHeight/4;
        if (cornerRadius > 2) {
            DrawRoundedCorners(pixmapBackground,
                               cornerRadius,
                               0,
                               0,
                               geoManager->volumeWidth,
                               geoManager->volumeHeight);
        }
    }
    pixmapLabel = osd->CreatePixmap(2, cRect(0, 5, geoManager->volumeWidth, geoManager->volumeLabelHeight));
    pixmapProgressBar = osd->CreatePixmap(2, cRect((geoManager->volumeWidth - geoManager->volumeProgressBarWidth) / 2, (geoManager->volumeHeight - geoManager->volumeProgressBarHeight)*2/3, geoManager->volumeProgressBarWidth, geoManager->volumeProgressBarHeight));

    if (FadeTime) {
        pixmapBackground->SetAlpha(0);
        pixmapProgressBar->SetAlpha(0);
        pixmapLabel->SetAlpha(0);
    }
}

cNopacityDisplayVolume::~cNopacityDisplayVolume() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmapLabel);
    osd->DestroyPixmap(pixmapProgressBar);
    delete osd;
}

void cNopacityDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    pixmapLabel->Fill(clrTransparent);
    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    pixmapLabel->DrawText(cPoint((geoManager->volumeWidth - fontManager->volumeText->Width(*label)) / 2, (geoManager->volumeLabelHeight - fontManager->volumeText->Height()) / 2), *label, Theme.Color(clrVolumeFont), clrTransparent, fontManager->volumeText);
    if (Mute) {
        cBitmap bmMute(mute_xpm);
        pixmapLabel->DrawBitmap(cPoint(geoManager->volumeWidth - 2*bmMute.Width(), (geoManager->volumeLabelHeight - bmMute.Height()) / 2), bmMute, Theme.Color(clrDiskAlert), clrTransparent);
    }
        DrawProgressBar(Current, Total);
}

void cNopacityDisplayVolume::DrawProgressBar(int Current, int Total) {
    pixmapProgressBar->Fill(clrTransparent);
    if (geoManager->volumeProgressBarHeight < 5)
        return;
    double percent = ((double)Current) / (double)Total;
    int barWidth = geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight;
    if ((Current > 0) || (Total > 0)) {
        pixmapProgressBar->DrawEllipse(cRect(0, 0, geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawEllipse(cRect(geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight, 0, geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawRectangle(cRect(geoManager->volumeProgressBarHeight/2, 0, geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));

        pixmapProgressBar->DrawEllipse(cRect(1, 1, geoManager->volumeProgressBarHeight-2, geoManager->volumeProgressBarHeight-2), Theme.Color(clrProgressBarBlend));
        if (Current > 0) {
            tColor colAct = DrawProgressbarBackground(geoManager->volumeProgressBarHeight / 2 - 1, 1, barWidth * percent - 2, geoManager->volumeProgressBarHeight - 2);
            pixmapProgressBar->DrawEllipse(cRect(barWidth * percent, 1, geoManager->volumeProgressBarHeight-2, geoManager->volumeProgressBarHeight-2), colAct);
        }
    }
}

tColor cNopacityDisplayVolume::DrawProgressbarBackground(int left, int top, int width, int height) {

    tColor clr1 = Theme.Color(clrProgressBar);
    tColor clr2 = Theme.Color(clrProgressBarBlend);
    tColor clr = 0x00000000;
    int step = width / 256;
    int alpha = 0x0;
    int alphaStep;
    int maximum = 0;
    if (step == 0) {    //width < 256
        step = 1;
        alphaStep = 256 / width;
        maximum = width;
    } else {            //width > 256
        alphaStep = 0x1;
        maximum = 256;
    }
    int x = 0;
    for (int i = 0; i < maximum; i++) {
        x = left + i*step;
        clr = AlphaBlend(clr1, clr2, alpha);
        pixmapProgressBar->DrawRectangle(cRect(x,top,step,height), clr);
        alpha += alphaStep;
    }
    if (step > 0) {
        int rest = width - step*256;
        pixmapProgressBar->DrawRectangle(cRect(left+step*256, top, rest, height), clr);
    }
    return clr;
}

void cNopacityDisplayVolume::Flush(void) {
    if (initial)
        if (FadeTime)
            Start();
    initial = false;
    osd->Flush();
}

void cNopacityDisplayVolume::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        pixmapBackground->SetAlpha(Alpha);
        pixmapProgressBar->SetAlpha(Alpha);
        pixmapLabel->SetAlpha(Alpha);
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
