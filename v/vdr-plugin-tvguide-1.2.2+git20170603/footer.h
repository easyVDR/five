#ifndef __TVGUIDE_FOOTER_H
#define __TVGUIDE_FOOTER_H

#include <vdr/osd.h>
#include "channelgroups.h"

// --- cFooter  -------------------------------------------------------------

class cFooter  {
private:
    cPixmap *footer;
    int buttonY;
    int positionButtons[4];
    cChannelGroups *channelGroups;
    int currentGroup;
    void SetButtonPositions(void);
    void DrawButton(const char *text, tColor color, tColor borderColor, eOsdElementType buttonType, int num);
    void ClearButton(int num);
public:
    cFooter(cChannelGroups *channelGroups);
    virtual ~cFooter(void);
    void drawRedButton();
    void drawGreenButton();
    void drawGreenButton(const char *text);
    void drawYellowButton();
    void drawYellowButton(const char *text);
    void drawBlueButton(bool detailedEPG = false);
    void UpdateGroupButtons(const cChannel *channel, bool force = false);
    void SetDetailedViewMode(bool fromRecMenu = false);
    void LeaveDetailedViewMode(const cChannel *channel);
};

#endif //__TVGUIDE_FOOTER_H