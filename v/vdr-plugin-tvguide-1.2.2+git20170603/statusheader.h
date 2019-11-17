#ifndef __TVGUIDE_STATUSHEADER_H
#define __TVGUIDE_STATUSHEADER_H

#include "styledpixmap.h"
#include "grid.h"

// --- cStatusHeader  -------------------------------------------------------------

class cStatusHeader : public cStyledPixmap {
private:
    int width, height;
    int tvFrameWidth;
    cPixmap *pixmapText;
    cPixmap *pixmapTVFrame;
    int DrawPoster(const cEvent *event, int x, int y, int height, int border);
    void DecorateVideoFrame(void);
public:
    cStatusHeader(void);
    virtual ~cStatusHeader(void);
    void Draw(void);
    void ScaleVideo(void);
    void DrawInfoText(cGrid *grid);
};

#endif //__TVGUIDE_STATUSHEADER_H