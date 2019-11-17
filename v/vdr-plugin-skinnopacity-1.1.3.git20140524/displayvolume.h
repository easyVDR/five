#ifndef __NOPACITY_DISPLAYVOLUME_H
#define __NOPACITY_DISPLAYVOLUME_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "imagecache.h"

class cNopacityDisplayVolume : public cSkinDisplayVolume, cThread {
private:
    int FrameTime;
    int FadeTime;
    bool initial;
    bool muted;
    cOsd *osd;
    cImageCache *imgCache;
    cPixmap *pixmapBackground;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapLabel;
    virtual void Action(void);
    void DrawProgressBar(int Current, int Total);
    tColor DrawProgressbarBackground(int left, int top, int width, int height);
public:
  cNopacityDisplayVolume(cImageCache *imgCache);
  virtual ~cNopacityDisplayVolume();
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
  };

#endif //__NOPACITY_DISPLAYVOLUME_H