#ifndef __NOPACITY_DISPLAYTRACKS_H
#define __NOPACITY_DISPLAYTRACKS_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "menuitem.h"

class cNopacityDisplayTracks : public cSkinDisplayTracks, cThread {
private:
    cOsd *osd;
    cImageCache *imgCache;
    int FrameTime;
    int FadeTime;
    int width, height;
    int menuItemWidth;
    int menuItemHeight;
    bool initial;
    int currentIndex;
    int numTracks;
    int audioChannelLast;
    cPixmap *pixmapContainer;
    cPixmap *pixmapHeader;
    cPixmap *pixmapHeaderAudio;
    cPixmap *pixmapIcon;
    cList<cNopacityMenuItem> menuItems;
    virtual void Action(void);
    void SetItem(const char *Text, int Index, bool Current);
    void SetGeometry(void);
    void CreatePixmaps(void);
    void DrawHeader(const char *Title);
public:
    cNopacityDisplayTracks(cImageCache *imgCache, const char *Title, int NumTracks, const char * const *Tracks);
    virtual ~cNopacityDisplayTracks();
    virtual void SetTrack(int Index, const char * const *Tracks);
    virtual void SetAudioChannel(int AudioChannel);
    virtual void Flush(void);
};


#endif //__NOPACITY_DISPLAYTRACKS_H