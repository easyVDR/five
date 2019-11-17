#include <vdr/osd.h>
#include <vdr/menu.h>

#include "nopacity.h"
#include "config.h"
#include "displaychannel.h"
#include "displaymenuview.h"
#include "displaymenu.h"
#include "displayreplay.h"
#include "displayvolume.h"
#include "displaytracks.h"
#include "displaymessage.h"

cNopacity::cNopacity(cImageCache *imgCache) : cSkin("nOpacity", &::Theme) {
    displayMenu = NULL;
    config.LoadThemeSpecificConfigs();
    config.SetThemeSpecificDefaults();
    config.SetThemeSetup();
    config.SetPathes();
    config.SetFontName();
    geoManager = new cGeometryManager();
    geoManager->SetGeometry();
    fontManager = new cFontManager();
    fontManager->SetFonts();
    this->imgCache = imgCache;
    imgCache->CreateCache();
}

const char *cNopacity::Description(void) {
  return "nOpacity";
}

cSkinDisplayChannel *cNopacity::DisplayChannel(bool WithInfo) {
    ReloadCaches();
    return new cNopacityDisplayChannel(imgCache, WithInfo);
}

cSkinDisplayMenu *cNopacity::DisplayMenu(void) {
    ReloadCaches();
    cNopacityDisplayMenu *menu = new cNopacityDisplayMenu(imgCache);
    displayMenu = menu;
    return menu;
}

cSkinDisplayReplay *cNopacity::DisplayReplay(bool ModeOnly) {
  ReloadCaches();
  return new cNopacityDisplayReplay(imgCache, ModeOnly);
}

cSkinDisplayVolume *cNopacity::DisplayVolume(void) {
  ReloadCaches();
  return new cNopacityDisplayVolume(imgCache);
}

cSkinDisplayTracks *cNopacity::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {
  ReloadCaches();
  return new cNopacityDisplayTracks(imgCache, Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cNopacity::DisplayMessage(void) {
  ReloadCaches();
  return new cNopacityDisplayMessage(imgCache);
}

void cNopacity::ReloadCaches(void) {
    if (geoManager->GeometryChanged() || imgCache->ThemeChanged()) {
        int start = cTimeMs::Now();
        config.LoadDefaults();
        config.SetThemeSpecificDefaults();
        config.SetThemeSetup();
        config.SetFontName();
        geoManager->SetGeometry();
        fontManager->DeleteFonts();
        fontManager->SetFonts();
        imgCache->Reload();
        dsyslog("nopacity: Cache reloaded in %d ms", int(cTimeMs::Now()-start));
    }
}
