#ifndef _TVGUIDE_IMAGELOADER_H
#define _TVGUIDE_IMAGELOADER_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>
#include "imagemagickwrapper.h"

using namespace Magick;
 
class cImageLoader  : public cImageMagickWrapper {
public:
    cImageLoader();
    ~cImageLoader();
    cImage GetImage();
    bool LoadLogo(const cChannel *channel, int width, int height);
    bool LoadEPGImage(int eventID, int width, int height);
    bool LoadAdditionalEPGImage(cString name);
    bool LoadPoster(const char *poster, int width, int height);
    bool LoadIcon(const char *cIcon, int size);
    bool LoadOsdElement(cString name, int width, int height);
    bool DrawBackground(tColor back, tColor blend, int width, int height);
private:
    void CreateGradient(tColor back, tColor blend, int width, int height, double wfactor, double hfactor);
};

#endif //_TVGUIDE_IMAGELOADER_H
