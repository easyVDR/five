#pragma once

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>

#include <Magick++.h>

#include "flat.h"

using namespace Magick;
 
class cImageLoader {
public:
    cImageLoader();
    ~cImageLoader();
    cImage GetImage();
    bool LoadLogo(const char *logo, int width, int height);
    bool LoadIcon(const char *cIcon, int size);
    bool LoadIcon(const char *cIcon, int width, int height, bool preserveAspect = true);
   
    int Height(void);
    int Width(void);
private:
    int epgImageWidthLarge, epgImageHeightLarge;
    int epgImageWidth, epgImageHeight;
    int logoWidth, logoHeight;
    cString logoExtension;

    Image buffer;
    Color Argb2Color(tColor col);
    void toLowerCase(std::string &str);
    bool LoadImage(cString FileName, cString Path, cString Extension);
};
