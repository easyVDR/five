#ifndef __NOPACITY_DISPLAYCHANNELVIEW_H
#define __NOPACITY_DISPLAYCHANNELVIEW_H

#include "imagecache.h"
#include <vdr/osd.h>

class cNopacityDisplayChannelView {
private:
    cImageCache *imgCache;
    cOsd *osd;
    cString lastDate;
    int statusIconBorder;
    int statusIconSize;
    bool isRadioChannel;
    int signalWidth, signalHeight, signalX;
    int lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
    cPixmap *pixmapBackground;
    cPixmap *pixmapTop;
    cPixmap *pixmapLogo;
    cPixmap *pixmapLogoBackground;
    cPixmap *pixmapChannelName;
    cPixmap *pixmapDate;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapEPGInfo;
    cPixmap *pixmapStatusIcons;
    cPixmap *pixmapStatusIconsBackground;
    cPixmap *pixmapSignalStrength;
    cPixmap *pixmapSignalQuality;
    cPixmap *pixmapSignalMeter;
    cPixmap *pixmapSignalLabel;
    cPixmap *pixmapSourceInfo;
    cPixmap *pixmapPoster;
    tColor DrawProgressbarProgress(int left, int top, int width, int height);
    cString GetScreenResolutionIcon(void);
    std::string GetChannelSep(const cChannel *channel, bool prev);
public:
    cNopacityDisplayChannelView(cImageCache *imgCache);
    virtual ~cNopacityDisplayChannelView();
    bool createOsd(void);
    void CreatePixmaps(void);
    void SetAlpha(int alpha);
    void DrawBackground(void);
    void DrawChannelLogo(const cChannel *Channel);
    void ClearChannelLogo(void);
    void DrawChannelName(cString number, cString name);
    void ClearChannelName(void);
    void DrawDate(void);
    void DrawProgressbarBackground(void);
    void DrawProgressBar(int Current, int Total);
    void ClearProgressBar(void);
    void DrawEPGInfo(const cEvent *e, bool present, bool recording);
    void ClearEPGInfo(void);
    void DrawStatusIcons(const cChannel *Channel);
    void DrawScreenResolution(void);
    void ClearStatusIcons(void);
    void DrawPoster(const cEvent *event, bool initial);
    void DrawSignalMeter(void);
    void DrawSignal(void);
    void ShowSignalMeter(void);
    void HideSignalMeter(void);
    void DrawChannelGroups(const cChannel *Channel, cString ChannelName);
    void DrawSourceInfo(void);
    void ClearSourceInfo(void);
    void DisplayMessage(const char *Text);
    void Flush(void) { osd->Flush(); };
};
#endif //__NOPACITY_DISPLAYCHANNELVIEW_H
