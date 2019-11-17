#ifndef __NOPACITY_TIMERS_H
#define __NOPACITY_TIMERS_H

#include <vdr/tools.h>
#include <vdr/osd.h>
#include <vdr/timers.h>
#include "imagecache.h"

class cNopacityTimer : public cListObject {
    private:
        cOsd *osd;
        cImageCache *imgCache;
        cPixmap *pixmapBackground;
        cPixmap *pixmap;
        cPixmap *pixmapLogo;
        cPixmap *pixmapText;
        const cTimer *timer;
        int numConflicts;
        bool isTimerConflict;
        const cFont *font;
        const cFont *fontLarge;
        int width;
        int height;
        int y;
        cString Date;
        cTextWrapper showName;
        int DrawLogo(void);
    public:
        cNopacityTimer(cOsd *osd, cImageCache *imgCache, const cTimer *timer, const cFont *font, const cFont *fontLarge);
        cNopacityTimer(cOsd *osd, cImageCache *imgCache, int numConflicts, const cFont *font, const cFont *fontLarge);
        virtual ~cNopacityTimer(void);
        void SetGeometry(int width, int y);
        void CreateDate(void);
        void CreateShowName(void);
        void CreateConflictText(void);
        void CalculateHeight(int space);
        void CreatePixmaps(int x);
        void SetAlpha(int alpha);
        void Show(void);
        void Hide(void);
        int GetHeight(void) {return pixmap->ViewPort().Height();}
        void Render(void);
};

#endif //__NOPACITY_TIMERS_H
