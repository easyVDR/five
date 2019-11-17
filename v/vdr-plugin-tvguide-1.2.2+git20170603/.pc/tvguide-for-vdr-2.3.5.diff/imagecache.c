#include <string>
#include <sstream>
#include <map>
#include <vdr/channels.h>
#include "config.h"
#include "imagescaler.h"
#include "tools.h"
#include "imagecache.h"

cImageCache::cImageCache() : cImageMagickWrapper() {
    tempStaticLogo = NULL;
    groupsHead = NULL;
    groupsBottom = NULL;
    groupsLeft = NULL;
    groupsRight = NULL;
    imgLeft = NULL;
    imgLeftActive = NULL;
    imgRight = NULL;
    imgRightActive = NULL;
    imgHead = NULL;
    imgHeadActive = NULL;
    imgBottom = NULL;
    imgBottomActive = NULL;
}

cImageCache::~cImageCache() {
    Clear();
}

void cImageCache::CreateCache(void) {
    if (tvguideConfig.style != eStyleGraphical)
        return;
    esyslog("tvguide: Creating Image Cache");
    int start = cTimeMs::Now();
    int startNext = cTimeMs::Now();
    CreateOsdIconCache();
    esyslog("tvguide: Osd Icon Cash created in %d ms", int(cTimeMs::Now()-startNext));
    startNext = cTimeMs::Now();
    PrepareGridIconCache();
    CreateGridIconCache();
    esyslog("tvguide: Grid Icon Cash created in %d ms", int(cTimeMs::Now()-startNext));
    startNext = cTimeMs::Now();
    CreateChannelGroupCache();
    esyslog("tvguide: Channelgroup Cash created in %d ms", int(cTimeMs::Now()-startNext));
    startNext = cTimeMs::Now();
    CreateLogoCache();
    esyslog("tvguide: Logo Cash created in %d ms", int(cTimeMs::Now()-startNext));
    startNext = cTimeMs::Now();
    esyslog("tvguide: Complete Image Cash created in %d ms", int(cTimeMs::Now()-start));
}

void cImageCache::CreateOsdIconCache(void) {
    bool success = false;
    //Status Header
    std::string imgStatusHeaderContentFull = "osdElements/statusheader_content_full";
    std::string imgStatusHeaderContentWindowed = "osdElements/statusheader_content_windowed";
    std::string imgStatusHeaderTVFrame = "osdElements/statusheader_tvframe";
    success = LoadIcon(imgStatusHeaderContentFull);
    if (success)
        InsertIntoOsdElementCache(oeStatusHeaderContentFull, geoManager.headerContentWidth, geoManager.statusHeaderHeight);
    success = LoadIcon(imgStatusHeaderContentWindowed);
    if (success)
        InsertIntoOsdElementCache(oeStatusHeaderContentWindowed, geoManager.headerContentWidth, geoManager.statusHeaderHeight);
    success = LoadIcon(imgStatusHeaderTVFrame);
    if (success)
        InsertIntoOsdElementCache(oeStatusHeaderTVFrame, geoManager.tvFrameWidth, geoManager.statusHeaderHeight);
   

    //Color Buttons
    std::string imgButtonRed = "osdElements/buttonred";
    std::string imgButtonGreen = "osdElements/buttongreen";
    std::string imgButtonYellow = "osdElements/buttonyellow";
    std::string imgButtonBlue = "osdElements/buttonblue";
    success = LoadIcon(imgButtonRed);
    if (success)
        InsertIntoOsdElementCache(oeButtonRed, geoManager.buttonWidth, geoManager.buttonHeight);
    success = LoadIcon(imgButtonGreen);
    if (success)
        InsertIntoOsdElementCache(oeButtonGreen, geoManager.buttonWidth, geoManager.buttonHeight);
    success = LoadIcon(imgButtonYellow);
    if (success)
        InsertIntoOsdElementCache(oeButtonYellow, geoManager.buttonWidth, geoManager.buttonHeight);
    success = LoadIcon(imgButtonBlue);
    if (success)
        InsertIntoOsdElementCache(oeButtonBlue, geoManager.buttonWidth, geoManager.buttonHeight);

    //Channel Logo Background
    if (tvguideConfig.displayMode == eHorizontal) {
        success = LoadIcon("osdElements/channellogoback_horizontal");
    } else {
        success = LoadIcon("osdElements/channellogoback_vertical");        
    }
    if (success)
        InsertIntoOsdElementCache(oeLogoBack, geoManager.channelLogoWidth, geoManager.channelLogoHeight);

    //Timeline Elements
    std::string imgTimeline1, imgTimeline2, imgDateViewer;
    if (tvguideConfig.displayMode == eHorizontal) {
        imgTimeline1 = "osdElements/timeline1_horizontal";
        imgTimeline2 = "osdElements/timeline2_horizontal";
        imgDateViewer = "osdElements/timeline2_horizontal";
    } else {
        imgTimeline1 = "osdElements/timeline1_vertical";
        imgTimeline2 = "osdElements/timeline2_vertical";
        imgDateViewer = "osdElements/date_vertical";
    }
    std::string imgClock = "osdElements/clock";
    success = LoadIcon(imgTimeline1);
    if (success)
        InsertIntoOsdElementCache(oeTimeline1, geoManager.timeLineGridWidth, geoManager.timeLineGridHeight);
    success = LoadIcon(imgTimeline2);
    if (success)
        InsertIntoOsdElementCache(oeTimeline2, geoManager.timeLineGridWidth, geoManager.timeLineGridHeight);
    success = LoadIcon(imgDateViewer);
    if (success)
        InsertIntoOsdElementCache(oeDateViewer, geoManager.dateVieverWidth, geoManager.dateVieverHeight);
    success = LoadIcon(imgClock);
    if (success)
        InsertIntoOsdElementCache(oeClock, geoManager.clockWidth, geoManager.clockHeight);

    //Channel Jump
    success = LoadIcon("osdElements/channel_jump");
    if (success)
        InsertIntoOsdElementCache(oeChannelJump, geoManager.channelJumpWidth, geoManager.channelJumpHeight);
}

void cImageCache::PrepareGridIconCache(void) {
    bool success = false;
    //Create Buffers for Background
    gridsAvailable = true;
    std::string grid, grid_active;
    if (tvguideConfig.displayMode == eHorizontal) {
        grid = "osdElements/grid_horizontal";
        grid_active = "osdElements/grid_active_horizontal";
    } else {
        grid = "osdElements/grid_vertical";
        grid_active = "osdElements/grid_active_vertical";        
    }
    success = LoadIcon(grid);
    if (success) {
        bufferGrid = buffer;
    } else {
        gridsAvailable = false;
    }
    success = LoadIcon(grid_active);
    if (success) {
        bufferGridActive = buffer;
    } else {
        gridsAvailable = false;
    }
    //Create Grid Background Templates
    imgLeft = NULL;
    imgLeftActive = NULL;
    imgRight = NULL;
    imgRightActive = NULL;
    imgHead = NULL;
    imgHeadActive = NULL;
    imgBottom = NULL;
    imgBottomActive = NULL;

    if (tvguideConfig.displayMode == eHorizontal) {
        std::string left = "osdElements/grid_left";
        std::string right = "osdElements/grid_right";
        std::string left_active = "osdElements/grid_active_left";
        std::string right_active = "osdElements/grid_active_right";
        cornerWidth = 0;
        cornerHeight = geoManager.rowHeight;
        //first image determinates width
        success = LoadIcon(left);
        if (!success)
            return;
        int widthOriginal = 0;
        int heightOriginal = 0;
        widthOriginal = buffer.columns();
        heightOriginal = buffer.rows();
        cornerWidth = widthOriginal * cornerHeight / heightOriginal;
        imgLeft = CreateImage(cornerWidth, cornerHeight, false);
        success = LoadIcon(right);
        if (success)
            imgRight = CreateImage(cornerWidth, cornerHeight, false);
        success = LoadIcon(left_active);
        if (success)
            imgLeftActive = CreateImage(cornerWidth, cornerHeight, false);
        success = LoadIcon(right_active);
        if (success)
            imgRightActive = CreateImage(cornerWidth, cornerHeight, false);
    } else {
        std::string head = "osdElements/grid_head";
        std::string bottom = "osdElements/grid_bottom";
        std::string head_active = "osdElements/grid_active_head";
        std::string bottom_active = "osdElements/grid_active_bottom";
        cornerWidth = geoManager.colWidth;
        cornerHeight = 0;
        //first image determinates height
        success = LoadIcon(head);
        if (!success)
            return;
        int widthOriginal = 0;
        int heightOriginal = 0;
        widthOriginal = buffer.columns();
        heightOriginal = buffer.rows();
        cornerHeight = heightOriginal * cornerWidth / widthOriginal;
        imgHead = CreateImage(cornerWidth, cornerHeight, false);
        success = LoadIcon(bottom);
        if (success)
            imgBottom = CreateImage(cornerWidth, cornerHeight, false);
        success = LoadIcon(head_active);
        if (success)
            imgHeadActive = CreateImage(cornerWidth, cornerHeight, false);
        success = LoadIcon(bottom_active);
        if (success)
            imgBottomActive = CreateImage(cornerWidth, cornerHeight, false);
    }
}

void cImageCache::CreateGridIconCache(void) {
    if (tvguideConfig.displayMode == eHorizontal) {
        int gridHeight = geoManager.rowHeight;
        for (int minutes = 5; minutes <= 120; minutes += 5) {
            GetGrid(minutes * geoManager.minutePixel, gridHeight, false);
        }
    } else {
        int gridWidth = geoManager.colWidth;
        for (int minutes = 5; minutes <= 120; minutes += 5) {
            GetGrid(gridWidth, minutes * geoManager.minutePixel, false);
        }
    }
}

void cImageCache::CreateChannelGroupCache(void) {
    bool success = false;
    groupsHead = NULL;
    groupsBottom = NULL;
    groupsLeft = NULL;
    groupsRight = NULL;
    if (tvguideConfig.displayMode == eHorizontal) {
        std::string channelGroupHead = "osdElements/channelgroup_head";
        std::string channelGroupBottom = "osdElements/channelgroup_bottom";
        int width = geoManager.channelGroupsWidth;
        int heightHeadBottom = 0;
        success = LoadIcon(channelGroupHead);
        if (success) {
            int widthOriginal = buffer.columns();
            int heightOriginal = buffer.rows();
            heightHeadBottom = heightOriginal * width / widthOriginal;
            groupsHead = CreateImage(width, heightHeadBottom, false);
        }
        success = LoadIcon(channelGroupBottom);
        if (success && heightHeadBottom) {
            groupsBottom = CreateImage(width, heightHeadBottom, false);
        }
        for (int size = 1; size <= tvguideConfig.numGrids; ++size) {
            InsertIntoGroupsCacheHorizontal(size);
        }
    } else {
        std::string channelGroupLeft = "osdElements/channelgroup_left";
        std::string channelGroupRight = "osdElements/channelgroup_right";
        int widthHeadBottom = 0;
        int height = geoManager.channelGroupsHeight;
        success = LoadIcon(channelGroupLeft);
        if (success) {
            int widthOriginal = buffer.columns();
            int heightOriginal = buffer.rows();
            widthHeadBottom = widthOriginal * height / heightOriginal;
            groupsLeft = CreateImage(widthHeadBottom, height, false);
        }
        success = LoadIcon(channelGroupRight);
        if (success && widthHeadBottom) {
            groupsRight = CreateImage(widthHeadBottom, height, false);
        }
        for (int size = 1; size <= tvguideConfig.numGrids; ++size) {
            InsertIntoGroupsCacheVertical(size);
        }
    }
    
}

void cImageCache::CreateLogoCache(void) {
    if (tvguideConfig.hideChannelLogos)
        return;
    if (tvguideConfig.numLogosInitial > 0) {
        int channelsCached = 0;
        for (const cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
            if (channelsCached >= tvguideConfig.numLogosInitial)
                break;
            if (!channel->GroupSep()) {
                bool success = LoadLogo(channel);
                if (success) {
                    channelsCached++;
                    InsertIntoLogoCache(*channel->GetChannelID().ToString());
                }
            }

        }
    }
}


cImage *cImageCache::GetOsdElement(eOsdElementType type) {
	std::map<eOsdElementType, cImage*>::iterator hit = osdElementCache.find(type);
    if (hit != osdElementCache.end()) {
        return (cImage*)hit->second;
    }
    return NULL;
}

cImage *cImageCache::GetGrid(int width, int height, bool active) {
    if (!gridsAvailable)
        return NULL;
    std::stringstream gridImageName;
    gridImageName << width << "x" << height;
    if (active)
        gridImageName << "active";
    std::string gridImg = gridImageName.str();
    std::map<std::string, cImage*>::iterator hit = gridCache.find(gridImg);
    if (hit != gridCache.end()) {
        return (cImage*)hit->second;
    } else {
        InsertIntoGridCache(gridImg, width, height, active);
        hit = gridCache.find(gridImg);
        if (hit != gridCache.end()) {
            return (cImage*)hit->second;
        }
    }
    return NULL;
}

cImage *cImageCache::GetChannelGroup(int width, int height) {
    std::stringstream groupName;
    groupName << width << "x" << height;
    std::map<std::string, cImage*>::iterator hit = groupsCache.find(groupName.str());
    if (hit != groupsCache.end()) {
        return (cImage*)hit->second;
    }
    return NULL;
}

cImage *cImageCache::GetLogo(const cChannel *channel) {
    if (!channel)
        return NULL;

    std::map<std::string, cImage*>::iterator hit = logoCache.find(*channel->GetChannelID().ToString());

    if (hit != logoCache.end()) {
        return (cImage*)hit->second;
    } else {
        bool success = LoadLogo(channel);
        if (success) {
            if ((tvguideConfig.limitLogoCache) && (logoCache.size() >= tvguideConfig.numLogosMax)) {
            //logo cache is full, don't cache anymore
                if (tempStaticLogo) {
                    delete tempStaticLogo;
                    tempStaticLogo = NULL;
                }
                tempStaticLogo = CreateImage(geoManager.logoWidth, geoManager.logoHeight);
                return tempStaticLogo;
            } else {
            //add requested logo to cache
                InsertIntoLogoCache(*channel->GetChannelID().ToString());
                hit = logoCache.find(*channel->GetChannelID().ToString());
                if (hit != logoCache.end()) {
                    return (cImage*)hit->second;
                }
            }
        }
    }
    return NULL;
}

cImage *cImageCache::GetIcon(std::string name, int width, int height) {
    std::stringstream iconName;
    iconName << name << width << "x" << height;
    std::map<std::string, cImage*>::iterator hit = iconCache.find(iconName.str());
    if (hit != iconCache.end()) {
        return (cImage*)hit->second;
    } else {
        std::stringstream iconPath;
        iconPath << "recmenuicons/" << name;
        bool success = LoadIcon(iconPath.str());
        if (success) {
            cImage *image = CreateImage(width, height);
            iconCache.insert(std::pair<std::string, cImage*>(iconName.str(), image));
            hit = iconCache.find(iconName.str());
            if (hit != iconCache.end()) {
                return (cImage*)hit->second;
            }
        }
    }
    return NULL;
}

void cImageCache::InsertIntoOsdElementCache(eOsdElementType type, int width, int height) {
    cImage *image = CreateImage(width, height, false);
    if (image)
        osdElementCache.insert(std::pair<eOsdElementType, cImage*>(type, image));
}

void cImageCache::InsertIntoGridCache(std::string name, int width, int height, bool active) {
    cImage *image = CreateGrid(width, height, active);
    if (image) {
        if (tvguideConfig.displayMode == eHorizontal) {
            AddCornersHorizontal(image, active);
        } else {
            AddCornersVertical(image, active);
        }
        gridCache.insert(std::pair<std::string, cImage*>(name, image));
    }
}

cImage *cImageCache::CreateGrid(int width, int height, bool active) {
    if (width > geoManager.osdWidth || width < 6 || height > geoManager.osdHeight || height < 6)
        return NULL;
    Image *currentGridBuffer = NULL;
    if (active)
        currentGridBuffer = &bufferGridActive;
    else
        currentGridBuffer = &bufferGrid;
    int w, h;
    w = currentGridBuffer->columns();
    h = currentGridBuffer->rows();
    const PixelPacket *pixels = currentGridBuffer->getConstPixels(0, 0, w, h);
    cImage *image = NULL;
    image = new cImage(cSize(width, height));
    if (!image)
        return NULL;
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

void cImageCache::AddCornersHorizontal(cImage *img, bool active) {
    int imgWidth = img->Width();
    cImage *cornerLeft = NULL;
    cImage *cornerRight = NULL;
    if (active) {
        cornerLeft = imgLeftActive;
        cornerRight = imgRightActive;
    } else {
        cornerLeft = imgLeft;
        cornerRight = imgRight;
    }
    if (!cornerLeft || !cornerRight)
        return;
    int maxX = min(cornerWidth, imgWidth);
    for (int row = 0; row < cornerHeight; row++) {
        for (int col = 0; col < maxX; ++col) {
            img->SetPixel(cPoint(col, row), cornerLeft->GetPixel(cPoint(col, row)));
            img->SetPixel(cPoint(imgWidth - maxX + col, row), cornerRight->GetPixel(cPoint(col, row)));
        }
    }
}

void cImageCache::AddCornersVertical(cImage *img, bool active) {
    int imgHeight = img->Height();
    cImage *cornerHead = NULL;
    cImage *cornerBottom = NULL;
    if (active) {
        cornerHead = imgHeadActive;
        cornerBottom = imgBottomActive;
    } else {
        cornerHead = imgHead;
        cornerBottom = imgBottom;
    }
    if (!cornerHead || !cornerBottom)
        return;
    int maxY = min(cornerHeight, imgHeight);
    for (int row = 0; row < maxY; row++) {
        for (int col = 0; col < cornerWidth; ++col) {
            img->SetPixel(cPoint(col, row), cornerHead->GetPixel(cPoint(col, row)));
            img->SetPixel(cPoint(col, imgHeight - maxY + row), cornerBottom->GetPixel(cPoint(col, row)));
        }
    } 
}

void cImageCache::InsertIntoGroupsCacheHorizontal(int size) {
    int width = geoManager.channelGroupsWidth;
    bool success = LoadIcon("osdElements/channelgroup_horizontal");
    if (success) {
        int height = size * geoManager.rowHeight;
        std::stringstream name;
        name << width << "x" << height;
        cImage *image = CreateImage(width, height, false);
        if (image) {
            AddCornersGroupHorizontal(image);
            groupsCache.insert(std::pair<std::string, cImage*>(name.str(), image));
        }
    }
}

void cImageCache::InsertIntoGroupsCacheVertical(int size) {
    int height = geoManager.channelGroupsHeight;
    bool success = LoadIcon("osdElements/channelgroup_vertical");
    if (success) {
        int width = size * geoManager.colWidth;
        std::stringstream name;
        name << width << "x" << height;
        cImage *image = CreateImage(width, height, false);
        if (image) {
            AddCornersGroupVertical(image);
            groupsCache.insert(std::pair<std::string, cImage*>(name.str(), image));
        }
    }
}

void cImageCache::AddCornersGroupHorizontal(cImage *img) {
    if (!groupsHead || !groupsBottom  || !img)
        return;
    int imgWidth = img->Width();
    int imgHeight = img->Height();
    int heightHeadBottom = groupsHead->Height();
    int maxY = min(heightHeadBottom, imgHeight);
    for (int row = 0; row < maxY; row++) {
        for (int col = 0; col < imgWidth; ++col) {
            img->SetPixel(cPoint(col, row), groupsHead->GetPixel(cPoint(col, row)));
            img->SetPixel(cPoint(col, imgHeight - maxY + row), groupsBottom->GetPixel(cPoint(col, row)));
        }
    }
}

void cImageCache::AddCornersGroupVertical(cImage *img) {
    if (!groupsLeft || !groupsRight  || !img)
        return;
    int imgWidth = img->Width();
    int imgHeight = img->Height();
    int widthLeftRight = groupsLeft->Width();
    int maxX = min(widthLeftRight, imgWidth);
    for (int row = 0; row < imgHeight; row++) {
        for (int col = 0; col < maxX; ++col) {
            img->SetPixel(cPoint(col, row), groupsLeft->GetPixel(cPoint(col, row)));
            img->SetPixel(cPoint(imgWidth - maxX + col, row), groupsRight->GetPixel(cPoint(col, row)));
        }
    }
}

bool cImageCache::LoadIcon(std::string name) {
    bool success = false;
    if (tvguideConfig.iconsPathSet) {
        cString iconPathTheme = cString::sprintf("%s%s/", *tvguideConfig.iconPath, *tvguideConfig.themeName);
        success = LoadImage(name, *iconPathTheme, "png");
        if (success) {
            return true;
        } else {
            success = LoadImage(name, *tvguideConfig.iconPath, "png");
            if (success) {
                return true;
            }
        }
    }
    if (!success) {
        cString iconPathTheme = cString::sprintf("%s%s/", *tvguideConfig.iconPathDefault, *tvguideConfig.themeName);
        success = LoadImage(name, *iconPathTheme, "png");
        if (success) {
            return true;
        } else {
            success = LoadImage(name, *tvguideConfig.iconPathDefault, "png");
            if (success) {
                return true;
            }
        }
    }
    return false;
}

bool cImageCache::LoadLogo(const cChannel *channel) {
    if (!channel)
        return false;
    std::string channelID = StrToLowerCase(*(channel->GetChannelID().ToString()));
    std::string logoLower = StrToLowerCase(channel->Name());
    bool success = false;
    cString extension;
    if (tvguideConfig.logoExtension == 0) {
        extension = "png";
    } else if (tvguideConfig.logoExtension == 1) {
        extension = "jpg";
    }

    if (tvguideConfig.logoPathSet) {
        success = LoadImage(channelID.c_str(), *tvguideConfig.logoPath, *extension);
        if (success)
            return true;
        success = LoadImage(logoLower.c_str(), *tvguideConfig.logoPath, *extension);
        if (success)
            return true;
    }
    success = LoadImage(channelID.c_str(), *tvguideConfig.logoPathDefault, *extension);
    if (success)
        return true;
    success = LoadImage(logoLower.c_str(), *tvguideConfig.logoPathDefault, *extension);
    return success;
}

void cImageCache::InsertIntoLogoCache(std::string channelID) {
    cImage *image = CreateImage(geoManager.logoWidth, geoManager.logoHeight);
    logoCache.insert(std::pair<std::string, cImage*>(channelID, image));
}

std::string cImageCache::GetCacheSize(eCacheType type) {
    std::stringstream result;
    int sizeByte = 0;
    int numImages = 0;
    if (type == ctOsdElement) {
        for(std::map<eOsdElementType, cImage*>::const_iterator it = osdElementCache.begin(); it != osdElementCache.end(); it++) {
            cImage *img = (cImage*)it->second;
            if (img)
                sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = osdElementCache.size();
    } else if ((type == ctGrid) || (type == ctLogo) || (type == ctChannelGroup) || (type == ctIcon)) {
        std::map<std::string, cImage*> *cache;
        if (type == ctGrid)
            cache = &gridCache;
        else if (type == ctLogo)
            cache = &logoCache;
        else if (type == ctChannelGroup)
            cache = &groupsCache;
        else if (type == ctIcon)
            cache = &iconCache;

        for(std::map<std::string, cImage*>::const_iterator it = cache->begin(); it != cache->end(); it++) {
            cImage *img = (cImage*)it->second;
            if (img)
                sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = cache->size();
    }
    result << numImages << " " << tr("images") << " / " << sizeByte/1024 << " KByte";
    return result.str();
}

void cImageCache::Clear(void) {
	for(std::map<eOsdElementType, cImage*>::const_iterator it = osdElementCache.begin(); it != osdElementCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    osdElementCache.clear();
    
    for(std::map<std::string, cImage*>::const_iterator it = gridCache.begin(); it != gridCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    gridCache.clear();

    for(std::map<std::string, cImage*>::const_iterator it = groupsCache.begin(); it != groupsCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    groupsCache.clear();

    for(std::map<std::string, cImage*>::const_iterator it = logoCache.begin(); it != logoCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoCache.clear();

    for(std::map<std::string, cImage*>::const_iterator it = iconCache.begin(); it != iconCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    iconCache.clear();
    
    if (tempStaticLogo)
        delete tempStaticLogo;
    if (groupsHead)
        delete groupsHead;
    if (groupsBottom)
        delete groupsBottom;
    if (groupsLeft)
        delete groupsLeft;
    if (groupsRight)
        delete groupsRight;
    if (imgLeft)
        delete imgLeft;
    if (imgLeftActive)
        delete imgLeftActive;
    if (imgRight)
        delete imgRight;
    if (imgRightActive)
        delete imgRightActive;
    if (imgHead)
        delete imgHead;
    if (imgHeadActive)
        delete imgHeadActive;
    if (imgBottom)
        delete imgBottom;
    if (imgBottomActive)
        delete imgBottomActive;
    
    tempStaticLogo = NULL;
    groupsHead = NULL;
    groupsBottom = NULL;
    groupsLeft = NULL;
    groupsRight = NULL;
    imgLeft = NULL;
    imgLeftActive = NULL;
    imgRight = NULL;
    imgRightActive = NULL;
    imgHead = NULL;
    imgHeadActive = NULL;
    imgBottom = NULL;
    imgBottomActive = NULL;
}
