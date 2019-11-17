#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "imagecache.h"
#include "cairoimage.h"
#include "../config.h"
#include "helpers.h"


cMutex cImageCache::mutex;

string cImageCache::items[16] = { "Schedule", "Channels", "Timers", "Recordings", "Setup", "Commands",
                   "OSD", "EPG", "DVB", "LNB", "CAM", "Recording", "Replay", "Miscellaneous", "Plugins", "Restart"};

cImageCache::cImageCache() {
    tempStaticLogo = NULL;
}

cImageCache::~cImageCache() {
    Clear();
    if (tempStaticLogo) {
        delete tempStaticLogo;
        tempStaticLogo = NULL;
    }
}

void cImageCache::SetPathes(void) {
    cString skinPath = config.GetSkinPath(Setup.OSDSkin);

    string logoPathSkin = *cString::sprintf("%s%s/themes/%s/logos/", *skinPath, Setup.OSDSkin, Setup.OSDTheme);
    if (FolderExists(logoPathSkin)) {
        logoPath = logoPathSkin;
    } else {
        logoPath = *config.logoPath;
    }

    iconPathSkin = *cString::sprintf("%s%s/", *skinPath, Setup.OSDSkin);
    skinPartsPathSkin = *cString::sprintf("%s%s/skinparts/", *skinPath, Setup.OSDSkin);

    iconPathTheme = *cString::sprintf("%s%s/themes/%s/", *skinPath, Setup.OSDSkin, Setup.OSDTheme);
    skinPartsPathTheme = *cString::sprintf("%s%s/themes/%s/skinparts/", *skinPath, Setup.OSDSkin, Setup.OSDTheme);

    svgTemplatePath = *cString::sprintf("%s%s/svgtemplates/", *skinPath, Setup.OSDSkin);
    
    dsyslog("skindesigner: using channel logo path %s", logoPath.c_str());
    dsyslog("skindesigner: using icon path %s", iconPathTheme.c_str());
    dsyslog("skindesigner: using skinparts path %s", skinPartsPathTheme.c_str());
    dsyslog("skindesigner: using svgtemplate path %s", svgTemplatePath.c_str());
}

void cImageCache::CacheLogo(int width, int height) {
    if (config.numLogosPerSizeInitial == 0)
        return;
    if (width == 0 || height == 0)
        return;

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
#else
    const cChannels* channels = &Channels;
#endif

    int logosCached = 0;     

    if (config.numLogosMax && config.numLogosMax < (int)channelLogoCache.size())
        return;

    for (const cChannel *channel = channels->First(); channel; channel = channels->Next(channel)) {
        if (logosCached >= config.numLogosPerSizeInitial)
            break;
        if (channel->GroupSep()) {
            continue;
        }
        stringstream logoName;
        logoName << *channel->GetChannelID().ToString() << "_" << width << "x" << height;
        map<string, cImage*>::iterator hit = channelLogoCache.find(logoName.str());
        if (hit != channelLogoCache.end()) {
            continue;
        }
        bool success = LoadLogo(channel);
        if (success) {
            logosCached++;
            cImage *image = CreateImage(width, height);
            channelLogoCache.insert(pair<string, cImage*>(logoName.str(), image));
        }
        if (config.numLogosMax && config.numLogosMax < (int)channelLogoCache.size())
            return;
    }
}

cImage *cImageCache::GetLogo(string channelID, int width, int height) {
    cMutexLock MutexLock(&mutex);

    stringstream logoName;
    logoName << channelID << "_" << width << "x" << height;

    std::map<std::string, cImage*>::iterator hit = channelLogoCache.find(logoName.str());

    if (hit != channelLogoCache.end()) {
        return (cImage*)hit->second;
    } else {
        tChannelID chanID = tChannelID::FromString(channelID.c_str());
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_CHANNELS_READ;
        const cChannels* channels = Channels;
#else
        cChannels* channels = &Channels;
#endif
        const cChannel *channel = channels->GetByChannelID(chanID);
        if (!channel)
            return NULL;
        bool success = LoadLogo(channel);
        if (success) {
            if (config.limitLogoCache && ((int)channelLogoCache.size() >= config.numLogosMax)) {
                //logo cache is full, don't cache anymore
                if (tempStaticLogo) {
                    delete tempStaticLogo;
                    tempStaticLogo = NULL;
                }
                tempStaticLogo = CreateImage(width, height);
                return tempStaticLogo;
            } else {
                //add requested logo to cache
                cImage *image = CreateImage(width, height);
                channelLogoCache.insert(pair<string, cImage*>(logoName.str(), image));
                hit = channelLogoCache.find(logoName.str());
                if (hit != channelLogoCache.end()) {
                    return (cImage*)hit->second;
                }
            }
        }
    }
    return NULL;
}

cImage *cImageCache::GetSeparatorLogo(string name, int width, int height) {
    cMutexLock MutexLock(&mutex);

    stringstream logoName;
    logoName << name << "_" << width << "x" << height;

    std::map<std::string, cImage*>::iterator hit = channelLogoCache.find(logoName.str());

    if (hit != channelLogoCache.end()) {
        return (cImage*)hit->second;
    } else {
        bool success = LoadSeparatorLogo(name);
        if (success) {
            //add requested logo to cache
            cImage *image = CreateImage(width, height);
            channelLogoCache.insert(pair<string, cImage*>(logoName.str(), image));
            hit = channelLogoCache.find(logoName.str());
            if (hit != channelLogoCache.end()) {
                return (cImage*)hit->second;
            }
        }
    }
    return NULL;
}

bool cImageCache::LogoExists(string channelID) {
    tChannelID chanID = tChannelID::FromString(channelID.c_str());
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
#else
    cChannels* channels = &Channels;
#endif
    const cChannel *channel = channels->GetByChannelID(chanID);
    if (!channel)
        return false;
    string logoLower = StrToLowerCase(channel->Name());
    string channelIDLower = StrToLowerCase(channelID.c_str());

    return (FileExists(logoPath.c_str(), logoLower, "svg") ||
            FileExists(logoPath.c_str(), logoLower, "png") ||
            FileExists(logoPath.c_str(), channelIDLower, "svg") ||
            FileExists(logoPath.c_str(), channelIDLower, "png"));
}

bool cImageCache::SeparatorLogoExists(string name) {
    string separatorPath = *cString::sprintf("%sseparatorlogos/", logoPath.c_str());
    string nameLower = StrToLowerCase(name.c_str());

    return (FileExists(separatorPath, nameLower, "svg") ||
            FileExists(separatorPath, nameLower, "png"));
}

void cImageCache::CacheIcon(eImageType type, string name, int width, int height) {
    if (width < 1 || width > 1920 || height < 1 || height > 1080)
        return;
    GetIcon(type, name, width, height);
}

cCachedImage *cImageCache::GetIcon(eImageType type, string name, int width, int height) {
    if (width < 1 || width > 1920 || height < 1 || height > 1080)
        return NULL;
    cMutexLock MutexLock(&mutex);
    stringstream iconName;
    iconName << name << "_" << width << "x" << height;
    map<string, cCachedImage*>::iterator hit = iconCache.find(iconName.str());
    if (hit != iconCache.end()) {
        return (cCachedImage*)hit->second;
    } else {
        bool success = LoadIcon(type, name);
        if (!success) 
            return NULL;
        cImage *image = CreateImage(width, height, true);
        cCachedImage *cachedImg = new cCachedImage();
        cachedImg->size = image->Width() * image->Height() * sizeof(tColor);
        int handle = cOsdProvider::StoreImage(*image);
        if (handle) {
            cachedImg->handle = handle;
            delete image;
        } else {
            cachedImg->image = image;
        }

        iconCache.insert(pair<string, cCachedImage*>(iconName.str(), cachedImg));
        hit = iconCache.find(iconName.str());
        if (hit != iconCache.end()) {
            return (cCachedImage*)hit->second;
        }
    }
    return NULL;    
}

string cImageCache::GetIconName(string label, eMenuCategory cat, string plugName) {
    //if cat is set, use standard menu entries
    switch (cat) {
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
        case mcEvent:
            return "standardicons/Schedule";
        case mcChannel:
        case mcChannelEdit:
            return "standardicons/Channels";
        case mcTimer:
        case mcTimerEdit:
            return "standardicons/Timers";
        case mcRecording:
        case mcRecordingInfo:
        case mcSetupRecord:
        case mcSetupReplay:
            return "standardicons/Recordings";
        case mcPlugin: {
            //check for Plugins
            for (int i = 0; ; i++) {
                cPlugin *p = cPluginManager::GetPlugin(i);
                if (p) {
                    const char *mainMenuEntry = p->MainMenuEntry();
                    if (mainMenuEntry) {
                        string plugMainEntry = mainMenuEntry;
                        try {
                            if (label.substr(0, plugMainEntry.size()) == plugMainEntry) {
                                return *cString::sprintf("pluginicons/%s", p->Name());
                            }
                        } catch (...) {}
                    }
                } else
                    break;
            }
            return "standardicons/Plugins";
        } 
        case mcPluginSetup:
        case mcSetupPlugins:
            return "standardicons/Plugins";
        case mcSetup:
            return "standardicons/Setup";
        case mcSetupOsd:
            return "standardicons/OSD";
        case mcSetupEpg:
            return "standardicons/EPG";
        case mcSetupDvb:
            return "standardicons/DVB";
        case mcSetupLnb:
            return "standardicons/LNB";
        case mcSetupCam:
            return "standardicons/CAM";
        case mcSetupMisc:
            return "standardicons/Miscellaneous";    
        case mcCommand:
            return "standardicons/Commands";
        default:
            break;
    }
    //check for standard menu entries
    for (int i=0; i<16; i++) {
        string s = trVDR(items[i].c_str());
        if (s == label) {
            return *cString::sprintf("standardicons/%s", items[i].c_str());
        }
    }
    //check for special main menu entries "stop recording", "stop replay"
    string stopRecording = skipspace(trVDR(" Stop recording "));
    string stopReplay = skipspace(trVDR(" Stop replaying"));
    try {
        if (label.substr(0, stopRecording.size()) == stopRecording) {
            return "standardicons/StopRecording";
        }
        if (label.substr(0, stopReplay.size()) == stopReplay) {
            return "standardicons/StopReplay";
        }
    } catch (...) {}
    //check for Plugins
    if (plugName.size() > 0) {
        return *cString::sprintf("pluginicons/%s", plugName.c_str());
    }
    for (int i = 0; ; i++) {
        cPlugin *p = cPluginManager::GetPlugin(i);
        if (p) {
            const char *mainMenuEntry = p->MainMenuEntry();
            if (mainMenuEntry) {
                string plugMainEntry = mainMenuEntry;
                try {
                    if (label.substr(0, plugMainEntry.size()) == plugMainEntry) {
                        return *cString::sprintf("pluginicons/%s", p->Name());
                    }
                } catch (...) {}
            }
        } else
            break;
    }
    return *cString::sprintf("customicons/%s", label.c_str());
}

bool cImageCache::MenuIconExists(string name) {
    //first check in theme specific icon folder
    cString iconThemePath = cString::sprintf("%smenuicons/", iconPathTheme.c_str());
    if (FileExists(*iconThemePath, name, "svg")) {
        return true;
    }
    if (FileExists(*iconThemePath, name, "png")) {
        return true;
    }
    //then check skin icon folder
    cString iconSkinPath = cString::sprintf("%smenuicons/", iconPathSkin.c_str());
    if (FileExists(*iconSkinPath, name, "svg")) {
        return true;
    }
    if (FileExists(*iconSkinPath, name, "png")) {
        return true;
    }
    //finally check svg templates
    cString iconTemplatePath = cString::sprintf("%smenuicons/", svgTemplatePath.c_str());
    if (FileExists(*iconTemplatePath, name, "svg")) {
        return true;
    }
    return false;
}

void cImageCache::CacheSkinpart(string name, int width, int height) {
    if (width < 1 || width > 1920 || height < 1 || height > 1080)
        return;
    GetSkinpart(name, width, height);
}

cCachedImage *cImageCache::GetSkinpart(string name, int width, int height) {
    if (width < 1 || width > 1920 || height < 1 || height > 1080)
        return NULL;
    cMutexLock MutexLock(&mutex);
    stringstream iconName;
    iconName << name << "_" << width << "x" << height;
    map<string, cCachedImage*>::iterator hit = skinPartsCache.find(iconName.str());
    if (hit != skinPartsCache.end()) {
        return (cCachedImage*)hit->second;
    } else {
        bool success = LoadSkinpart(name);
        if (!success) 
            return NULL;
        cImage *image = CreateImage(width, height, false);
        cCachedImage *cachedImg = new cCachedImage();
        cachedImg->size = image->Width() * image->Height() * sizeof(tColor);
        int handle = cOsdProvider::StoreImage(*image);
        if (handle) {
            cachedImg->handle = handle;
            delete image;
        } else {
            cachedImg->image = image;
        }
        skinPartsCache.insert(pair<string, cCachedImage*>(iconName.str(), cachedImg));
        hit = skinPartsCache.find(iconName.str());
        if (hit != skinPartsCache.end()) {
            return (cCachedImage*)hit->second;
        }
    }
    return NULL;    
}

cImage *cImageCache::GetVerticalText(string text, tColor color, string font, int size, int direction) {
    cMutexLock MutexLock(&mutex);
    stringstream buf;
    buf << text << "_" << size << "_" << direction;
    string imgName = buf.str();
    map<string, cImage*>::iterator hit = cairoImageCache.find(imgName);
    if (hit != cairoImageCache.end()) {
        return (cImage*)hit->second;
    } else {
        cCairoImage c;
        c.DrawTextVertical(text, color, font, size, direction);
        cImage *image = c.GetImage();
        cairoImageCache.insert(pair<string, cImage*>(imgName, image));
        hit = cairoImageCache.find(imgName);
        if (hit != cairoImageCache.end()) {
            return (cImage*)hit->second;
        }
    }
    return NULL;
}


bool cImageCache::LoadIcon(eImageType type, string name) {
    cString subdir("");
    if (type == eImageType::menuicon)
        subdir = "menuicons";
    else if (type == eImageType::icon)
        subdir = "icons";

    //first check in theme specific icon path
    cString subIconThemePath = cString::sprintf("%s%s/", iconPathTheme.c_str(), *subdir);

    if (FileExists(*subIconThemePath, name, "svg"))
        return LoadImage(*subIconThemePath, name, "svg");
    else if (FileExists(*subIconThemePath, name, "png"))
        return LoadImage(*subIconThemePath, name, "png");

    //then check in skin icon path
    cString subIconSkinPath = cString::sprintf("%s%s/", iconPathSkin.c_str(), *subdir);

    if (FileExists(*subIconSkinPath, name, "svg"))
        return LoadImage(*subIconSkinPath, name, "svg");
    else if (FileExists(*subIconSkinPath, name, "png"))
        return LoadImage(*subIconSkinPath, name, "png");

    //and finally check if a svg template exists
    cSVGTemplate svgTemplate(name, *subdir, svgTemplatePath);
    if (!svgTemplate.Exists())
        return false;
    svgTemplate.ReadTemplate();
    if (!svgTemplate.ParseTemplate())
        return false;
    string tmpImageName = svgTemplate.WriteImage();
    return LoadImage(tmpImageName.c_str());
}

bool cImageCache::LoadLogo(const cChannel *channel) {
    if (!channel)
        return false;
    string channelID = StrToLowerCase(*(channel->GetChannelID().ToString()));
    string logoLower = StrToLowerCase(channel->Name());

    if (FileExists(logoPath.c_str(), channelID.c_str(), "svg"))
        return LoadImage(logoPath.c_str(), channelID.c_str(), "svg");
    if (FileExists(logoPath.c_str(), channelID.c_str(), "png"))
        return LoadImage(logoPath.c_str(), channelID.c_str(), "png");
    if (FileExists(logoPath.c_str(), logoLower.c_str(), "svg"))
        return LoadImage(logoPath.c_str(), logoLower.c_str(), "svg");
    if (FileExists(logoPath.c_str(), logoLower.c_str(), "png"))
        return LoadImage(logoPath.c_str(), logoLower.c_str(), "png");

    return false;
}

bool cImageCache::LoadSeparatorLogo(string name) {
    string separatorPath = *cString::sprintf("%sseparatorlogos/", logoPath.c_str());
    string nameLower = StrToLowerCase(name.c_str());
    if (FileExists(separatorPath, nameLower.c_str(), "svg"))
        return LoadImage(separatorPath, nameLower.c_str(), "svg");
    else
        return LoadImage(separatorPath, nameLower.c_str(), "png");
}

bool cImageCache::LoadSkinpart(string name) {
    if (FileExists(skinPartsPathTheme.c_str(), name, "svg"))
        return LoadImage(skinPartsPathTheme.c_str(), name, "svg");
    
    else if (FileExists(skinPartsPathTheme.c_str(), name, "png"))
        return LoadImage(skinPartsPathTheme.c_str(), name, "png");
    
    else if (FileExists(skinPartsPathSkin.c_str(), name, "svg"))
        return LoadImage(skinPartsPathSkin.c_str(), name, "svg");
    
    else if (FileExists(skinPartsPathSkin.c_str(), name, "png"))
        return LoadImage(skinPartsPathSkin.c_str(), name, "png");

    //check if a svg template exists
    cSVGTemplate svgTemplate(name, "skinparts", svgTemplatePath);
    if (!svgTemplate.Exists())
        return false;
    svgTemplate.ReadTemplate();
    if (!svgTemplate.ParseTemplate())
        return false;
    string tmpImageName = svgTemplate.WriteImage();
    return LoadImage(tmpImageName.c_str());
}

void cImageCache::Clear(void) {
    for(map<string, cCachedImage*>::const_iterator it = iconCache.begin(); it != iconCache.end(); it++) {
        cCachedImage *img = (cCachedImage*)it->second;
        delete img;
    }
    iconCache.clear();

    for(map<string, cImage*>::const_iterator it = channelLogoCache.begin(); it != channelLogoCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    channelLogoCache.clear();

    for(map<string, cCachedImage*>::const_iterator it = skinPartsCache.begin(); it != skinPartsCache.end(); it++) {
        cCachedImage *img = (cCachedImage*)it->second;
        delete img;
    }
    skinPartsCache.clear();

    for(map<string, cImage*>::const_iterator it = cairoImageCache.begin(); it != cairoImageCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    cairoImageCache.clear();
}

void cImageCache::Debug(bool full) {
    float sizeIconCacheInternal = 0;
    float sizeIconCacheExternal = 0;
    int numIcons = 0;
    GetIconCacheSize(numIcons, sizeIconCacheInternal, sizeIconCacheExternal);
    dsyslog("skindesigner: cached %d icons - size internal mem %.2fMB, high level mem %.2fMB", numIcons, sizeIconCacheInternal, sizeIconCacheExternal);
    if (full) {
        for(std::map<std::string, cCachedImage*>::const_iterator it = iconCache.begin(); it != iconCache.end(); it++) {
            string name = it->first;
            cCachedImage *img = (cCachedImage*)it->second;
            dsyslog("skindesigner: cached icon %s, handle %d", name.c_str(), img->handle);
        }
    }

    float sizeLogoCache = 0;
    int numLogos = 0;
    GetLogoCacheSize(numLogos, sizeLogoCache);
    dsyslog("skindesigner: cached %d logos - size %.2fMB internal mem", numLogos, sizeLogoCache);
    if (full) {
        for(std::map<std::string, cImage*>::const_iterator it = channelLogoCache.begin(); it != channelLogoCache.end(); it++) {
            string name = it->first;
            dsyslog("skindesigner: cached logo %s", name.c_str());
        }
    }
    float sizeSkinpartCacheInternal = 0;
    float sizeSkinpartCacheExternal = 0;
    int numSkinparts = 0;
    GetSkinpartsCacheSize(numSkinparts, sizeSkinpartCacheInternal, sizeSkinpartCacheExternal);
    dsyslog("skindesigner: cached %d skinparts - size internal mem %.2fMB, high level mem %.2fMB", numSkinparts, sizeSkinpartCacheInternal, sizeSkinpartCacheExternal);
    if (full) {
        for(std::map<std::string, cCachedImage*>::const_iterator it = skinPartsCache.begin(); it != skinPartsCache.end(); it++) {
            string name = it->first;
            dsyslog("skindesigner: cached skinpart %s", name.c_str());
        }
    }
}

void cImageCache::GetIconCacheSize(int &num, float &sizeInternal, float &sizeExternal) {
    num = iconCache.size();
    int sizeByteInternal = 0;
    int sizeByteExternal = 0;
    for (map<string, cCachedImage*>::iterator icon = iconCache.begin(); icon != iconCache.end(); icon++) {
        cCachedImage* img = icon->second;
        if (img->image)
            sizeByteInternal += img->size;
        else
            sizeByteExternal += img->size;
    }
    sizeInternal = sizeByteInternal / 1024.0f / 1024.0f;
    sizeExternal = sizeByteExternal / 1024.0f / 1024.0f;
}

void cImageCache::GetLogoCacheSize(int &num, float &size) {
    num = channelLogoCache.size();
    int sizeByte = 0;
    for (map<string, cImage*>::iterator logo = channelLogoCache.begin(); logo != channelLogoCache.end(); logo++) {
        cImage* img = logo->second;
        sizeByte += img->Width() * img->Height() * sizeof(tColor);
    }
    size = sizeByte / 1024.0f;
}

void cImageCache::GetSkinpartsCacheSize(int &num, float &sizeInternal, float &sizeExternal) {
    num = skinPartsCache.size();
    int sizeByteInternal = 0;
    int sizeByteExternal = 0;
    for (map<string, cCachedImage*>::iterator skinpart = skinPartsCache.begin(); skinpart != skinPartsCache.end(); skinpart++) {
        cCachedImage* img = skinpart->second;
        if (img->image)
            sizeByteInternal += img->size;
        else
            sizeByteExternal += img->size;
    }
    sizeInternal = sizeByteInternal / 1024.0f / 1024.0f;
    sizeExternal = sizeByteExternal / 1024.0f / 1024.0f;
}
