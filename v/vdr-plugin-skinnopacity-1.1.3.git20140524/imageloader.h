#ifndef __NOPACITY_IMAGELOADER_H
#define __NOPACITY_IMAGELOADER_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>
#include "imagemagickwrapper.h"

using namespace Magick;

class cImageLoader : public cImageMagickWrapper {
public:
    cImageLoader();
    ~cImageLoader();
    cImage GetImage();
    bool LoadLogo(const char *logo, int width = config.GetValue("logoWidthOriginal"), int height = config.GetValue("logoHeightOriginal"));
    bool LoadEPGImage(int eventID);
    bool LoadAdditionalEPGImage(cString name);
    bool LoadRecordingImage(cString Path);
    bool LoadAdditionalRecordingImage(cString path, cString name);
    bool LoadPoster(const char *poster, int width, int height);
    bool SearchRecordingPoster(cString recPath, cString &found);
private:
    bool FirstImageInFolder(cString Path, cString Extension, cString *recImage);
};

#endif //__NOPACITY_IMAGELOADER_H
