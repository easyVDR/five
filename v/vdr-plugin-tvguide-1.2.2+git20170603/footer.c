#include <string>
#include "imageloader.h"
#include "tools.h"
#include "footer.h"

cFooter::cFooter(cChannelGroups *channelGroups) {
    this->channelGroups = channelGroups;
    currentGroup = -1;
    buttonY = (geoManager.footerHeight - geoManager.buttonHeight)/2;
    SetButtonPositions();
    footer = osdManager.requestPixmap(2, cRect( 0, 
                                                geoManager.footerY, 
                                                geoManager.osdWidth, 
                                                geoManager.footerHeight),
                                         cRect::Null);
    footer->Fill(clrTransparent);
}

cFooter::~cFooter(void) {
    osdManager.releasePixmap(footer);
}

void cFooter::drawRedButton() {
    cString text(tr("Search & Rec"));
    DrawButton(*text, theme.Color(clrButtonRed), theme.Color(clrButtonRedBorder), oeButtonRed, positionButtons[0]);
}

void cFooter::drawGreenButton() {
    cString text = cString::sprintf("%d %s", tvguideConfig.jumpChannels, tr("Channels back"));
    DrawButton(*text, theme.Color(clrButtonGreen), theme.Color(clrButtonGreenBorder), oeButtonGreen, positionButtons[1]);
}

void cFooter::drawGreenButton(const char *text) {
    std::string cuttedText = CutText(text, geoManager.buttonWidth-6, fontManager.FontButton);
    DrawButton(cuttedText.c_str(), theme.Color(clrButtonGreen), theme.Color(clrButtonGreenBorder), oeButtonGreen, positionButtons[1]);
}

void cFooter::drawYellowButton() {
    cString text = cString::sprintf("%d %s", tvguideConfig.jumpChannels, tr("Channels forward"));
    DrawButton(*text, theme.Color(clrButtonYellow), theme.Color(clrButtonYellowBorder), oeButtonYellow, positionButtons[2]);
}

void cFooter::drawYellowButton(const char *text) {
    std::string cuttedText = CutText(text, geoManager.buttonWidth-6, fontManager.FontButton);
    DrawButton(cuttedText.c_str(), theme.Color(clrButtonYellow), theme.Color(clrButtonYellowBorder), oeButtonYellow, positionButtons[2]);
}

void cFooter::drawBlueButton(bool detailedEPG) {
    cString text;
    if (tvguideConfig.blueKeyMode == eBlueKeySwitch)
        text = tr("Switch to Channel");
    else if (tvguideConfig.blueKeyMode == eBlueKeyEPG) {
        if (!detailedEPG)
            text = tr("Detailed EPG");
        else
            text = tr("Close detailed EPG");
    } else if (tvguideConfig.blueKeyMode == eBlueKeyFavorites) {
        if (!detailedEPG)
            text = tr("Favorites");
        else
            text = tr("Switch to Channel");
    }
    DrawButton(*text, theme.Color(clrButtonBlue), theme.Color(clrButtonBlueBorder), oeButtonBlue, positionButtons[3]);
}

void cFooter::UpdateGroupButtons(const cChannel *channel, bool force) {
    if (!channel)
        return;
    int group = channelGroups->GetGroup(channel);
    if ((group != currentGroup) || force) {
        currentGroup = group;
        drawGreenButton(channelGroups->GetPrev(group));
        drawYellowButton(channelGroups->GetNext(group));
    }
}

void cFooter::SetDetailedViewMode(bool fromRecMenu) {
    ClearButton(positionButtons[1]);
    ClearButton(positionButtons[2]);
    if (fromRecMenu) {
        ClearButton(positionButtons[0]);
        ClearButton(positionButtons[3]);
    } else if (tvguideConfig.blueKeyMode != eBlueKeySwitch) {
        ClearButton(positionButtons[3]);
        drawBlueButton(true);
    }
}

void cFooter::LeaveDetailedViewMode(const cChannel *channel) {
    drawRedButton();
    drawBlueButton();
    if (tvguideConfig.channelJumpMode == eNumJump) {
        drawGreenButton();
        drawYellowButton();
    } else {
        UpdateGroupButtons(channel, true);
    }
}

void cFooter::SetButtonPositions(void) {
    for (int i=0; i < 4; i++) {
        positionButtons[i] = -1;
    }
    /*
    red button = 0
    green button = 1
    yellow button = 2
    blue button = 3
    */
    for (int button=0; button<4; button++) {
        if (Setup.ColorKey0 == button) {
            positionButtons[button] = 0;
            continue;
        }
        if (Setup.ColorKey1 == button) {
            positionButtons[button] = 1;
            continue;
        }
        if (Setup.ColorKey2 == button) {
            positionButtons[button] = 2;
            continue;
        }
        if (Setup.ColorKey3 == button) {
            positionButtons[button] = 3;
            continue;
        }
    }
}

void cFooter::DrawButton(const char *text, tColor color, tColor borderColor, eOsdElementType buttonType, int num) {
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
    int left = num * geoManager.buttonWidth + (2 * num + 1) * geoManager.buttonBorder;

    if ((tvguideConfig.style == eStyleBlendingMagick) || (tvguideConfig.style == eStyleBlendingDefault)) {
        cImageLoader imgLoader;
        imgLoader.DrawBackground(theme.Color(clrButtonBlend), color, geoManager.buttonWidth-4, geoManager.buttonHeight-4);
        footer->DrawRectangle(cRect(left, buttonY, geoManager.buttonWidth, geoManager.buttonHeight), borderColor);
        footer->DrawImage(cPoint(left+2, buttonY+2), imgLoader.GetImage());    
        if (tvguideConfig.roundedCorners) {
            int borderRadius = 12;
            int borderWidth = 2;
            DrawRoundedCorners(footer, left, buttonY, geoManager.buttonWidth, geoManager.buttonHeight, borderRadius, borderWidth, borderColor);
        }
    } else if (tvguideConfig.style == eStyleGraphical) {
        cImage *button = imgCache.GetOsdElement(buttonType);
        if (button) {
            footer->DrawImage(cPoint(left, buttonY), *button);    
        }
    } else {
        footer->DrawRectangle(cRect(left, buttonY, geoManager.buttonWidth, geoManager.buttonHeight), borderColor); 
        footer->DrawRectangle(cRect(left+1, buttonY+1, geoManager.buttonWidth-2, geoManager.buttonHeight-2), color);
        if (tvguideConfig.roundedCorners) {
            int borderRadius = 12;
            int borderWidth = 1;
            DrawRoundedCorners(footer, left, buttonY, geoManager.buttonWidth, geoManager.buttonHeight, borderRadius, borderWidth, borderColor);
        }
    }
    
    int textWidth = fontManager.FontButton->Width(text);
    int textHeight = fontManager.FontButton->Height();
    footer->DrawText(cPoint(left + (geoManager.buttonWidth-textWidth)/2, buttonY + (geoManager.buttonHeight-textHeight)/2), text, theme.Color(clrFontButtons), colorTextBack, fontManager.FontButton);
}

void cFooter::ClearButton(int num) {
    int left = num * geoManager.buttonWidth + (2 * num + 1) * geoManager.buttonBorder;
    footer->DrawRectangle(cRect(left, buttonY, geoManager.buttonWidth, geoManager.buttonHeight), clrTransparent); 
}