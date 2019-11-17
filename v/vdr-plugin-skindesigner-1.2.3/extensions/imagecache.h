#ifndef __NOPACITY_IMAGECACHE_H
#define __NOPACITY_IMAGECACHE_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <vector>
#include "imageloader.h"
#include "../coreengine/definitions.h"

class cCachedImage {
public:
    int handle;
    cImage *image;
    int size;
    cCachedImage(void) {
        handle = 0;
        image = NULL;
        size = 0;
    };
    ~cCachedImage(void) {
        if (handle)
            cOsdProvider::DropImage(handle);
        if (image)
            delete image;
    };
};

class cImageCache : public cImageLoader {
public:
    cImageCache();
    ~cImageCache();
    void Lock(void) { mutex.Lock(); }
    void Unlock(void) { mutex.Unlock(); }
    void SetPathes(void);
    //channel logos
    void CacheLogo(int width, int height);
    cImage *GetLogo(string channelID, int width, int height);
    bool LogoExists(string channelID);
    cImage *GetSeparatorLogo(string name, int width, int height);
    bool SeparatorLogoExists(string name);
    //icons
    void CacheIcon(eImageType type, string path, int width, int height);
    cCachedImage *GetIcon(eImageType type, string name, int width, int height);
    string GetIconName(string label, eMenuCategory cat = mcUndefined, string plugName = "");
    bool MenuIconExists(string name);
    //skinparts
    void CacheSkinpart(string path, int width, int height);
    cCachedImage *GetSkinpart(string name, int width, int height);
    //cairo special images
    cImage *GetVerticalText(string text, tColor color, string font, int size, int direction);
    //helpers
    void Clear(void);
    void Debug(bool full);
    void GetIconCacheSize(int &num, float &sizeInternal, float &sizeExternal);
    void GetLogoCacheSize(int &num, float &size);
    void GetSkinpartsCacheSize(int &num, float &sizeInternal, float &sizeExternal);
private:
    static cMutex mutex;
    static string items[16];
    cImage *tempStaticLogo;
    string logoPath;
    string iconPathSkin;
    string skinPartsPathSkin;
    string iconPathTheme;
    string skinPartsPathTheme;
    string svgTemplatePath;
    map<string, cCachedImage*> iconCache;
    map<string, cImage*> channelLogoCache;
    map<string, cCachedImage*> skinPartsCache;
    map<string, cImage*> cairoImageCache;
    bool LoadIcon(eImageType type, string name);
    bool LoadLogo(const cChannel *channel);
    bool LoadSeparatorLogo(string name);
    bool LoadSkinpart(string name);
};

#endif //__NOPACITY_IMAGECACHE_H
