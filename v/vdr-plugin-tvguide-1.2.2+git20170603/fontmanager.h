#ifndef __TVGUIDE_FONTMANAGER_H
#define __TVGUIDE_FONTMANAGER_H

#include <vdr/skins.h>

class cFontManager {
		cString fontName;
        void InitialiseFontType(void);
        cFont *CreateFont(int size);
    public:
        cFontManager();
        ~cFontManager();
        void SetFonts(void);
        void DeleteFonts(void);
        cFont *FontChannelHeader;
        cFont *FontChannelHeaderHorizontal;
        cFont *FontChannelGroups;
        cFont *FontChannelGroupsHorizontal;
        cFont *FontStatusHeader;
        cFont *FontStatusHeaderLarge;
        cFont *FontGrid;
        cFont *FontGridSmall;
        cFont *FontGridHorizontal;
        cFont *FontGridHorizontalSmall;
        cFont *FontTimeLineWeekday;
        cFont *FontTimeLineDate;
        cFont *FontTimeLineDateHorizontal;
        cFont *FontTimeLineTime;
        cFont *FontTimeLineTimeHorizontal;
        cFont *FontButton;
        cFont *FontDetailView;
        cFont *FontDetailViewSmall;
        cFont *FontDetailHeader;
        cFont *FontDetailHeaderLarge;
        cFont *FontMessageBox;
        cFont *FontMessageBoxLarge;
        cFont *FontRecMenuItem;
        cFont *FontRecMenuItemSmall;
        cFont *FontRecMenuItemLarge;
};
#endif //__TVGUIDE_FONTMANAGER_H