#pragma once
#include <vdr/osd.h>
#include <vdr/skins.h>
#include <string>

#define MAX_IMAGE_CACHE     999
#define LOGO_PRE_CACHE      200
// note LOGO_PRE_CACHE is used twice
// one for displaychannel and one for menu
// you must double the value for the real amount of pre cached logos

class cImageCache {
private:
    cImage *CacheImage[MAX_IMAGE_CACHE];
    std::string CacheName[MAX_IMAGE_CACHE];
    int CacheWidth[MAX_IMAGE_CACHE];
    int CacheHeight[MAX_IMAGE_CACHE];

    int InsertIndex;
    bool Overflow;
public:
    cImageCache();
    ~cImageCache();

    void Create(void);
    void Clear(void);
    bool RemoveFromCache( std::string Name );

    int getCacheCount(void) { if(Overflow) return MAX_IMAGE_CACHE; return InsertIndex+1; }

    cImage *GetImage(std::string Name, int Width, int Height);
    void InsertImage(cImage *Image, std::string Name, int Width, int Height);

    void PreLoadImage(void);
};
