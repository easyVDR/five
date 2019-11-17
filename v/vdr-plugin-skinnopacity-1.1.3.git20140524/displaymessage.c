#include "displaymessage.h"
#include "config.h"
#include "imageloader.h"
#include "helpers.h"

cNopacityDisplayMessage::cNopacityDisplayMessage(cImageCache *imgCache) {
    this->imgCache = imgCache;
    FadeTime = config.GetValue("messageFadeTime");
    FrameTime = FadeTime / 10;
    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->messageHeight - config.GetValue("messageBorderBottom");
    int left = geoManager->osdLeft + (geoManager->osdWidth - geoManager->messageWidth) / 2;
    osd = CreateOsd(left, top, geoManager->messageWidth, geoManager->messageHeight);
    pixmap = osd->CreatePixmap(2, cRect(0, 0, geoManager->messageWidth, geoManager->messageHeight));
    pixmapBackground = osd->CreatePixmap(1, cRect(0, 0, geoManager->messageWidth, geoManager->messageHeight));
    if (FadeTime) {
        pixmap->SetAlpha(0);
        pixmapBackground->SetAlpha(0);
    }
}

cNopacityDisplayMessage::~cNopacityDisplayMessage() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmap);
    osd->DestroyPixmap(pixmapBackground);
    delete osd;
}

void cNopacityDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
    tColor col = Theme.Color(clrMessageStatus);
    tColor colFont = Theme.Color(clrMessageFontStatus);
    eSkinElementType seType = seMessageStatus;
    switch (Type) {
        case mtStatus:
            col = Theme.Color(clrMessageStatus);
            colFont = Theme.Color(clrMessageFontStatus);
            seType = seMessageStatus;
            break;
        case mtInfo:
            col = Theme.Color(clrMessageInfo);
            colFont = Theme.Color(clrMessageFontInfo);
            seType = seMessageInfo;
            break;
        case mtWarning:
            col = Theme.Color(clrMessageWarning);
            colFont = Theme.Color(clrMessageFontWarning);
            seType = seMessageWarning;
            break;
        case mtError:
            col = Theme.Color(clrMessageError);
            colFont = Theme.Color(clrMessageFontError);
            seType = seMessageError;
            break;
    }

    pixmap->Fill(clrTransparent);
    if (config.GetValue("displayType") == dtGraphical) {
        pixmapBackground->Fill(clrTransparent);
        cImage *imgBack = imgCache->GetSkinElement(seType);
        if (imgBack) {
            pixmapBackground->DrawImage(cPoint(0, 0), *imgBack);
        }
    } else {
        pixmapBackground->Fill(col);
        if (config.GetValue("displayType") == dtBlending) {
            cImage imgBack = imgCache->GetBackground(Theme.Color(clrMessageBlend), col, geoManager->messageWidth-2, geoManager->messageHeight-2, true);
            pixmapBackground->DrawImage(cPoint(1, 1), imgBack);
        }
        if (config.GetValue("roundedCorners")) {
            DrawRoundedCornersWithBorder(pixmapBackground, col, config.GetValue("cornerRadius"), geoManager->messageWidth, geoManager->messageHeight);
        }
    }
    int textWidth = fontManager->messageText->Width(Text);
    pixmap->DrawText(cPoint((geoManager->messageWidth - textWidth) / 2,
                            (geoManager->messageHeight - fontManager->messageText->Height()) / 2),
                            Text,
                            colFont,
                            clrTransparent,
                            fontManager->messageText);
    if (FadeTime)
        Start();
}

void cNopacityDisplayMessage::Flush(void) {
    osd->Flush();
}

void cNopacityDisplayMessage::Action(void) {
    uint64_t Start = cTimeMs::Now();
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
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
