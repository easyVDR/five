#include <vdr/channels.h>
#include "config.h"
#include "geometrymanager.h"
#include "osdmanager.h"
#include "fontmanager.h"
#include "channelgroups.h"
#include "channeljump.h"

cChannelJump::cChannelJump(cChannelGroups *channelGroups) {
    this->channelGroups = channelGroups;
    pixmapText = NULL;
    channel = 0;
	if (!tvguideConfig.hideLastGroup) {
        maxChannels = Channels.MaxNumber();
	} else {
        maxChannels = channelGroups->GetLastValidChannel();
    }
    timeout = Setup.ChannelEntryTimeout;
    startTime = cTimeMs::Now();
    SetPixmaps();
    Draw();
}

cChannelJump::~cChannelJump(void) {
    osdManager.releasePixmap(pixmapBack);
    osdManager.releasePixmap(pixmapText);
}

void cChannelJump::SetPixmaps(void) {
	int x = (geoManager.osdWidth - geoManager.channelJumpWidth)/2;
	int y = (geoManager.osdHeight - geoManager.channelJumpHeight)/2;
	
	pixmapBack = osdManager.requestPixmap(4, cRect(x, y, geoManager.channelJumpWidth, geoManager.channelJumpHeight));
	pixmap = osdManager.requestPixmap(5, cRect(x, y, geoManager.channelJumpWidth, geoManager.channelJumpHeight));
	pixmapText = osdManager.requestPixmap(6, cRect(x, y, geoManager.channelJumpWidth, geoManager.channelJumpHeight));
}

void cChannelJump::Draw(void) {
	if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgChannelJump);
	} else {
        pixmap->Fill(theme.Color(clrBackground));
        drawBorder();
	}
	pixmapBack->Fill(clrTransparent);
	pixmapBack->DrawRectangle(cRect(5, Height()/2, Width()-10, Height()-3), theme.Color(clrBackground));
}

void cChannelJump::DrawText(void) {
    pixmapText->Fill(clrTransparent);

    cString header = cString::sprintf("%s:", tr("Channel"));

	const cFont *font = fontManager.FontMessageBox;
	const cFont *fontHeader = fontManager.FontMessageBoxLarge;

	int xHeader = (Width() - fontHeader->Width(*header)) / 2;
	int yHeader = (Height()/2 - fontHeader->Height()) / 2;
	pixmapText->DrawText(cPoint(xHeader, yHeader), *header, theme.Color(clrFont), clrTransparent, fontHeader);

	cString strChannel = BuildChannelString();
	int xChannel = (Width() - font->Width(*strChannel)) / 2;
	int yChannel = Height()/2 + (Height()/2 - font->Height()) / 2;
	pixmapText->DrawText(cPoint(xChannel, yChannel), *strChannel, theme.Color(clrFont), clrTransparent, font);

}

void cChannelJump::Set(int num) {
    startTime = cTimeMs::Now();
    if (channel == 0) {
	    channel = num;
	    return;
    }
    int newChannel = channel * 10 + num;
    if (newChannel <= maxChannels)
    	channel = newChannel;
}

cString cChannelJump::BuildChannelString(void) {
	if (channel*10 <= maxChannels)
		return cString::sprintf("%d-", channel);
	else
		return cString::sprintf("%d", channel); 
}

 bool cChannelJump::TimeOut(void) {
 	if ((cTimeMs::Now() - startTime) > timeout)
	 	return true;
	return false;
 }