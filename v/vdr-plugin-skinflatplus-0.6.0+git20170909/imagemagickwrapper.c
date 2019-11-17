#include <string>
#include <sstream>
#include "imagemagickwrapper.h"
#include "imagescaler.h"

cImageMagickWrapper::cImageMagickWrapper() {
    InitializeMagick(NULL);
}

cImageMagickWrapper::~cImageMagickWrapper() {
}

cImage *cImageMagickWrapper::CreateImage(int width, int height, bool preserveAspect) {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    if (width == 0)
        width = w;
    if (height == 0)
        height = h;
    if (preserveAspect) {
        unsigned scale_w = 1000 * width / w;
        unsigned scale_h = 1000 * height / h;
        if (scale_w > scale_h)
          width = w * height / h;
        else
          height = h * width / w;
    }
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    cImage *image = new cImage(cSize(width, height));
    tColor *imgData = (tColor *)image->Data();
    if (w != width || h != height) {
        ImageScaler scaler;
        scaler.SetImageParameters(imgData, width, width, height, w, h);
        for (const void *pixels_end = &pixels[w*h]; pixels < pixels_end; ++pixels)
            scaler.PutSourcePixel(pixels->blue / ((MaxRGB + 1) / 256),
                                  pixels->green / ((MaxRGB + 1) / 256),
                                  pixels->red / ((MaxRGB + 1) / 256),
                                  ~((unsigned char)(pixels->opacity / ((MaxRGB + 1) / 256))));
        return image;
    }
    for (const void *pixels_end = &pixels[width*height]; pixels < pixels_end; ++pixels)
        *imgData++ = ((~int(pixels->opacity / ((MaxRGB + 1) / 256)) << 24) |
                      (int(pixels->green / ((MaxRGB + 1) / 256)) << 8) |
                      (int(pixels->red / ((MaxRGB + 1) / 256)) << 16) |
                      (int(pixels->blue / ((MaxRGB + 1) / 256)) ));
    return image;
}

cImage cImageMagickWrapper::CreateImageCopy() {
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

bool cImageMagickWrapper::LoadImage(const char *fullpath) {
    if ((fullpath == NULL) || (strlen(fullpath) < 5))
        return false;
    try {
        buffer.read(fullpath);
    } catch(...) {
        return false;
    }
    return true;
}

Color cImageMagickWrapper::Argb2Color(tColor col) {
    tIndex alpha = (col & 0xFF000000) >> 24;
    tIndex red = (col & 0x00FF0000) >> 16;
    tIndex green = (col & 0x0000FF00) >> 8;
    tIndex blue = (col & 0x000000FF);
    Color color(MaxRGB*red/255, MaxRGB*green/255, MaxRGB*blue/255, MaxRGB*(0xFF-alpha)/255);
    return color;
}
