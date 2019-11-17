#ifndef __NOPACITY_DISPLAYMESSAGE_H
#define __NOPACITY_DISPLAYMESSAGE_H

#include <vdr/thread.h>
#include <vdr/skins.h>
#include "imagecache.h"

class cNopacityDisplayMessage : public cSkinDisplayMessage , cThread {
private:
    cOsd *osd;
    cImageCache *imgCache;
    cPixmap *pixmap;
    cPixmap *pixmapBackground;
    int FrameTime;
    int FadeTime;
    virtual void Action(void);
public:
  cNopacityDisplayMessage(cImageCache *imgCache);
  virtual ~cNopacityDisplayMessage();
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

#endif //__NOPACITY_DISPLAYMESSAGE_H