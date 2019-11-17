#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>
#include <vdr/channels.h>

#include "config.h"
#include "imageloader.h"
#include "tools.h"

using namespace Magick;

cImageLoader::cImageLoader() {
    InitializeMagick(NULL);
}

cImageLoader::~cImageLoader() {
}

bool cImageLoader::LoadLogo(const cChannel *channel, int width, int height) {
    if (!channel || (width == 0)||(height==0))
        return false;
    std::string channelID = StrToLowerCase(*(channel->GetChannelID().ToString()));
    std::string logoLower = StrToLowerCase(channel->Name());
    cString extension;
    if (tvguideConfig.logoExtension == 0) {
        extension = "png";
    } else if (tvguideConfig.logoExtension == 1) {
        extension = "jpg";
    }
    bool success = false;
    if (tvguideConfig.logoPathSet) {
        success = LoadImage(channelID.c_str(), *tvguideConfig.logoPath, *extension);
        if (!success) {
            success = LoadImage(logoLower.c_str(), *tvguideConfig.logoPath, *extension);
        }
    }
    if (!success)
        success = LoadImage(channelID.c_str(), *tvguideConfig.logoPathDefault, *extension);
    if (!success)
        success = LoadImage(logoLower.c_str(), *tvguideConfig.logoPathDefault, *extension);
    if (success)
        buffer.sample(Geometry(width, height));
    return success;
}

bool cImageLoader::LoadEPGImage(int eventID, int width, int height) {
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    success = LoadImage(*cString::sprintf("%d", eventID), *tvguideConfig.epgImagePath, "jpg");
    if (!success)
        success = LoadImage(*cString::sprintf("%d_0", eventID), *tvguideConfig.epgImagePath, "jpg");
    if (!success)
        return false;
    buffer.sample( Geometry(width, height));
    return true;
}

bool cImageLoader::LoadAdditionalEPGImage(cString name) {
    int width = tvguideConfig.epgImageWidthLarge;
    int height = tvguideConfig.epgImageHeightLarge;
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    success = LoadImage(*name, *tvguideConfig.epgImagePath, "jpg");
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadPoster(const char *poster, int width, int height) {
    if (LoadImage(poster)) {
        buffer.sample(Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::LoadIcon(const char *cIcon, int size) {
    if (size==0)
        return false;
    bool success = false;
    if (tvguideConfig.iconsPathSet) {
        cString iconPathTheme = cString::sprintf("%s%s/recmenuicons/", *tvguideConfig.iconPath, *tvguideConfig.themeName);
        success = LoadImage(cIcon, *iconPathTheme, "png");
        if (!success) {
            cString iconPath = cString::sprintf("%srecmenuicons/", *tvguideConfig.iconPath);
            success = LoadImage(cIcon, *iconPath, "png");
        }
    }
    if (!success) {
        cString iconPathTheme = cString::sprintf("%s%s/recmenuicons/", *tvguideConfig.iconPathDefault, *tvguideConfig.themeName);
        success = LoadImage(cIcon, *iconPathTheme, "png");
        if (!success) {
            cString iconPath = cString::sprintf("%srecmenuicons/", *tvguideConfig.iconPathDefault);
            success = LoadImage(cIcon, *iconPath, "png");
        }
    }
    if (!success)
        return false;
    buffer.sample(Geometry(size, size));
    return true;
}

bool cImageLoader::LoadOsdElement(cString name, int width, int height) {
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (tvguideConfig.iconsPathSet) {
        cString path = cString::sprintf("%s%s%s", *tvguideConfig.iconPath, *tvguideConfig.themeName, "/osdElements/");
        success = LoadImage(*name, *path, "png");
        if (!success) {
            path = cString::sprintf("%s%s", *tvguideConfig.iconPath, "/osdElements/");
            success = LoadImage(*name, *path, "png");
        }
    }
    if (!success) {
        cString path = cString::sprintf("%s%s%s", *tvguideConfig.iconPathDefault, *tvguideConfig.themeName, "/osdElements/");
        success = LoadImage(*name, *path, "png");
    }
    if (!success) {
        cString path = cString::sprintf("%s%s", *tvguideConfig.iconPathDefault, "/osdElements/");
        success = LoadImage(*name, *path, "png");
    }
    if (!success)
        return false;
    Geometry size(width, height);
    size.aspect(true);
    buffer.sample(size);
    return true;
}

bool cImageLoader::DrawBackground(tColor back, tColor blend, int width, int height) {
    if ((width < 1) || (height < 1) || (width > 1920) || (height > 1080))
        return false;
    CreateGradient(back, blend, width, height, 0.8, 0.8);
    return true;
}

cImage cImageLoader::GetImage() {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    cImage image (cSize(w, h));
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    for (int iy = 0; iy < h; ++iy) {
        for (int ix = 0; ix < w; ++ix) {
            tColor col = (~int(pixels->opacity * 255 / MaxRGB) << 24) 
            | (int(pixels->green * 255 / MaxRGB) << 8) 
            | (int(pixels->red * 255 / MaxRGB) << 16) 
            | (int(pixels->blue * 255 / MaxRGB) );
            image.SetPixel(cPoint(ix, iy), col);
            ++pixels;
        }
    }
    return image;
}

void cImageLoader::CreateGradient(tColor back, tColor blend, int width, int height, double wfactor, double hfactor) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    int maxw = MaxRGB * wfactor;
    int maxh = MaxRGB * hfactor;

    Image imgblend(Geometry(width, height), Blend);
    imgblend.modifyImage();
    imgblend.type(TrueColorMatteType);
    PixelPacket *pixels = imgblend.getPixels(0, 0, width, height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            PixelPacket *pixel = pixels + y * width + x;
            int opacity = (maxw / width * x + maxh - maxh / height * y) / 2;
            pixel->opacity = (opacity <= MaxRGB) ? opacity : MaxRGB;
        }
    }
    imgblend.syncPixels();

    Image imgback(Geometry(width, height), Back);
    imgback.composite(imgblend, 0, 0, OverCompositeOp);

    buffer = imgback;
}