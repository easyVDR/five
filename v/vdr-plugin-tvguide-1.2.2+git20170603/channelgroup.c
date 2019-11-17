#include <algorithm>
#include "channelgroup.h"
#include "tools.h"

cChannelGroup::cChannelGroup(const char *name) {
    channelStart = 0;
    channelStop = 0;
    this->name = name;
}

cChannelGroup::~cChannelGroup(void) {
}

void cChannelGroup::Dump(void) {
    esyslog("tvguide: Group %s, startChannel %d, stopChannel %d", name, channelStart, channelStop);
}

// --- cChannelGroupGrid  -------------------------------------------------------------

cChannelGroupGrid::cChannelGroupGrid(const char *name) {
    this->name = name;
}

cChannelGroupGrid::~cChannelGroupGrid(void) {
}

void cChannelGroupGrid::SetBackground() {
    if (isColor1) {
        color = theme.Color(clrGrid1);
        colorBlending = theme.Color(clrGrid1Blending);
    } else {
        color = theme.Color(clrGrid2);
        colorBlending = theme.Color(clrGrid2Blending);
    }
}

void cChannelGroupGrid::SetGeometry(int start, int end) {
    int x, y, width, height;
    if (tvguideConfig.displayMode == eVertical) {
        x = geoManager.timeLineWidth + start*geoManager.colWidth;
        y = geoManager.statusHeaderHeight;
        width = (end - start + 1) * geoManager.colWidth;
        height = geoManager.channelGroupsHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x = 0;
        y = geoManager.statusHeaderHeight +  geoManager.timeLineHeight + start*geoManager.rowHeight;
        width = geoManager.channelGroupsWidth;
        height = (end - start + 1) * geoManager.rowHeight;
    }
    pixmap = osdManager.requestPixmap(1, cRect(x, y, width, height));
}

void cChannelGroupGrid::Draw(void) {
    if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgChannelGroup);
    } else {
        drawBackground();
        drawBorder();
    }
    tColor colorText = theme.Color(clrFont);
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
    if (tvguideConfig.displayMode == eVertical) {
        DrawVertical(colorText, colorTextBack);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        DrawHorizontal(colorText, colorTextBack);
    }
}

void cChannelGroupGrid::DrawVertical(tColor colorText, tColor colorTextBack) {
    int textY = (Height() - fontManager.FontChannelGroups->Height()) / 2;
    cString text = CutText(name, Width() - 4, fontManager.FontChannelGroups).c_str();
    int textWidth = fontManager.FontChannelGroups->Width(*text);
    int x = (Width() - textWidth) / 2;
    pixmap->DrawText(cPoint(x, textY), *text, colorText, colorTextBack, fontManager.FontChannelGroups);
}

void cChannelGroupGrid::DrawHorizontal(tColor colorText, tColor colorTextBack) {
    std::string nameUpper = name;
    std::transform(nameUpper.begin(), nameUpper.end(),nameUpper.begin(), ::toupper);
    int numChars = nameUpper.length();
    int charHeight = fontManager.FontChannelGroupsHorizontal->Height();
    int textHeight = numChars * charHeight;
    int y = 5;
    if ((textHeight +5) < Height()) {
        y = (Height() - textHeight) / 2;
    }
    for (int i=0; i < numChars; i++) {
        if (((y + 2*charHeight) > Height()) && ((i+1)<numChars)) {
            int x = (Width() - fontManager.FontChannelGroupsHorizontal->Width("...")) / 2;
            pixmap->DrawText(cPoint(x, y), "...", colorText, colorTextBack, fontManager.FontChannelGroupsHorizontal);
            break;
        }
        cString currentChar = cString::sprintf("%c", nameUpper.at(i));
        int x = (Width() - fontManager.FontChannelGroupsHorizontal->Width(*currentChar)) / 2;
        pixmap->DrawText(cPoint(x, y), *currentChar, colorText, colorTextBack, fontManager.FontChannelGroupsHorizontal);
        y += fontManager.FontChannelGroupsHorizontal->Height();
    }
}
