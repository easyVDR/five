#include "designer.h"
#include "extensions/helpers.h"

cSkinDesigner::cSkinDesigner(string skin, cTheme *theme) : cSkin(skin.c_str(), theme) {
    init = true;
    this->skin = skin;

    backupSkin = NULL;
    useBackupSkin = false;
    
    globals = NULL;
    channelView = NULL;
    menuView = NULL;
    messageView = NULL;
    replayView = NULL;
    volumeView = NULL;
    tracksView = NULL;
    currentMenu = NULL;

    dsyslog("skindesigner: skin %s started", skin.c_str());
}

cSkinDesigner::~cSkinDesigner(void) {
    if (globals)
        delete globals;
    DeleteViews();
    if (backupSkin)
        delete backupSkin;
}

const char *cSkinDesigner::Description(void) {
  return skin.c_str();
}

cSkinDisplayChannel *cSkinDesigner::DisplayChannel(bool WithInfo) {
    currentMenu = NULL;
    cSkinDisplayChannel *displayChannel = NULL;
    Init();
    if (!useBackupSkin) {
        displayChannel = new cSDDisplayChannel(channelView, WithInfo);
    } else {
        displayChannel = backupSkin->DisplayChannel(WithInfo);
    }
    return displayChannel;
}

cSkinDisplayMenu *cSkinDesigner::DisplayMenu(void) {
    if (!useBackupSkin) {
        cSDDisplayMenu *displayMenu = NULL;
        Init();
        displayMenu = new cSDDisplayMenu(menuView);
        currentMenu = displayMenu;
        return displayMenu;
    } else {
        cSkinDisplayMenu *displayMenu = backupSkin->DisplayMenu();
        currentMenu = NULL;
        return displayMenu;
    }
}

cSkinDisplayReplay *cSkinDesigner::DisplayReplay(bool ModeOnly) {
    currentMenu = NULL;
    cSkinDisplayReplay *displayReplay = NULL;
    if (!useBackupSkin) {
        Init();
        displayReplay = new cSDDisplayReplay(replayView, ModeOnly);
    } else {
        displayReplay = backupSkin->DisplayReplay(ModeOnly);
    }
    return displayReplay;
}

cSkinDisplayVolume *cSkinDesigner::DisplayVolume(void) {
    currentMenu = NULL;
    cSkinDisplayVolume *displayVolume = NULL;
    if (!useBackupSkin) {
        Init();
        displayVolume = new cSDDisplayVolume(volumeView);
    } else {
        displayVolume = backupSkin->DisplayVolume();
    }
    return displayVolume;
}

cSkinDisplayTracks *cSkinDesigner::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {
    currentMenu = NULL;
    cSkinDisplayTracks *displayTracks = NULL;
    if (!useBackupSkin) {
        Init();
        displayTracks = new cSDDisplayTracks(tracksView, Title, NumTracks, Tracks);
    } else {
        displayTracks = backupSkin->DisplayTracks(Title, NumTracks, Tracks);
    }
    return displayTracks;
}

cSkinDisplayMessage *cSkinDesigner::DisplayMessage(void) {
    currentMenu = NULL;
    cSkinDisplayMessage *displayMessage = NULL;
    if (!useBackupSkin) {
        Init();
        displayMessage = new cSDDisplayMessage(messageView);
    } else {
        displayMessage = backupSkin->DisplayMessage();
    }
    return displayMessage;
}

void cSkinDesigner::Reload(void) {
    dsyslog("skindesigner: forcing full reload of templates");
    if (cOsd::IsOpen()) {
        esyslog("skindesigner: OSD is open, close first!");
        return;
    }

    cStopWatch watch;
    bool ok = LoadViews();
    if (!ok) {
        esyslog("skindesigner: error during loading of templates - using LCARS as backup");
        if (!backupSkin)
            backupSkin = new cSkinLCARS();
        useBackupSkin = true;
    } else {
        CacheViews();
        useBackupSkin = false;        
        watch.Stop("templates reloaded and cache created");
    }
}

void cSkinDesigner::ListAvailableFonts(void) {
    fontManager->ListAvailableFonts();
}

bool cSkinDesigner::SetCustomIntToken(string option) {
    splitstring s(option.c_str());
    vector<string> flds = s.split('=', 0);
    if (flds.size() != 2)
        return false;
    int key = atoi(trim(flds[0]).c_str());
    string val = trim(flds[1]);
    if (!globals)
        return true;
    if (key > 0 && isNumber(val)) {
        globals->AddCustomInt(key, atoi(val.c_str()));
    } else {
        return false;
    }
    return true;
}

bool cSkinDesigner::SetCustomStringToken(string option) {
    splitstring s(option.c_str());
    vector<string> flds = s.split('=', 0);
    if (flds.size() != 2)
        return false;
    int key = atoi(trim(flds[0]).c_str());
    string val = trim(flds[1]);
    if (!globals)
        return true;
    if (key > 0)
        globals->AddCustomString(key, val);
    else
        return false;
    return true;
}

void cSkinDesigner::ListCustomTokens(void) {
    if (!globals)
        return;
    globals->ListCustomTokens();
}

skindesignerapi::ISkinDisplayPlugin *cSkinDesigner::GetDisplayPlugin(int plugId) {
    map<int, cViewPlugin*>::iterator hit = pluginViews.find(plugId);
    if (hit == pluginViews.end())
        return NULL;
    return hit->second;
}

/*********************************************************************************
* PRIVATE FUNCTIONS
*********************************************************************************/    
void cSkinDesigner::Init(void) {
    if (   init 
        || config.OsdSizeChanged() 
        || config.SkinChanged() 
        || config.OsdLanguageChanged() || 
        config.setupCloseDoReload ) 
    {
        config.setupCloseDoReload = false;
        if (init) {
            config.SetSkin();
            config.SetOSDSize();
            config.SetOSDFonts();
        }
        dsyslog("skindesigner: initializing skin %s", skin.c_str());
        
        config.CheckDecimalPoint();
        plgManager->Reset();

        if (fontManager)
            delete fontManager;
        fontManager = new cFontManager();
        if (imgCache)
            delete imgCache;
        imgCache = new cImageCache();
        imgCache->SetPathes();

        cStopWatch watch;
        bool ok = LoadViews();
        if (!ok) {
            esyslog("skindesigner: error during loading of templates - using LCARS as backup");
            backupSkin = new cSkinLCARS();
            useBackupSkin = true;
        } else {
            CacheViews();
            watch.Stop("templates loaded and caches created");
        }
        init = false;
    } 
    else if (config.OsdFontsChanged()) 
    {
        dsyslog("skindesigner: reloading fonts");
        if (fontManager)
            delete fontManager;
        fontManager = new cFontManager();
        cStopWatch watch;
        bool ok = LoadViews();
        if (!ok) {
            esyslog("skindesigner: error during loading of templates - using LCARS as backup");
            backupSkin = new cSkinLCARS();
            useBackupSkin = true;
        } else {
            CacheViews();
            watch.Stop("templates loaded and caches created");
        }
    }
}

void cSkinDesigner::DeleteViews(void) {
    delete channelView;
    channelView = NULL;

    delete menuView;
    menuView = NULL;

    delete messageView;
    messageView = NULL;

    delete replayView;
    replayView = NULL;

    delete volumeView;
    volumeView = NULL;

    delete tracksView;
    tracksView = NULL;

    for (map<int,cViewPlugin*>::iterator it = pluginViews.begin(); it != pluginViews.end(); it++) {
        cViewPlugin *plugView = it->second;
        delete plugView;
    }
    pluginViews.clear();
}

bool cSkinDesigner::LoadViews(void) {
    if (globals)
        delete globals;
    globals = new cGlobals();
    bool ok = globals->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error parsing globals, aborting");
        return false;
    }
    config.SetGlobals(globals);

    cSkinSetup *skinSetup = config.GetSkinSetup(skin);
    if (skinSetup) {
        skinSetup->AddToGlobals(globals);
    }

    DeleteViews();

    channelView = new cViewChannel();
    ok = channelView->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displaychannel template, aborting");
        DeleteViews();
        return false;
    }
    channelView->SetGlobals(globals);

    menuView = new cViewMenu();
    ok = menuView->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displaymenu template, aborting");
        DeleteViews();
        return false;
    }
    menuView->SetGlobals(globals);

    messageView = new cViewMessage();
    ok = messageView->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displaymessage template, aborting");
        DeleteViews();
        return false;
    }
    messageView->SetGlobals(globals);

    replayView = new cViewReplay();
    ok = replayView->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displayreplay template, aborting");
        DeleteViews();
        return false;
    }
    replayView->SetGlobals(globals);

    volumeView = new cViewVolume();
    ok = volumeView->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displayvolume template, aborting");
        DeleteViews();
        return false;
    }
    volumeView->SetGlobals(globals);

    tracksView = new cViewTracks();
    ok = tracksView->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displayaudiotracks template, aborting");
        DeleteViews();
        return false;
    }
    tracksView->SetGlobals(globals);

    LoadPluginViews();

    dsyslog("skindesigner: templates successfully validated and parsed");        
    return true;
}

void cSkinDesigner::LoadPluginViews(void) {
    plgManager->InitPluginViewIterator();
    string plugName = "";
    string viewTpl = "";
    int plugId = -1;
    while ( plgManager->GetNextPluginView(plugName, plugId, viewTpl) ) {
        cViewPlugin *plugView = new cViewPlugin(0, plugId);
        bool ok = plugView->ReadFromXML(plugName.c_str(), viewTpl.c_str());
        if (!ok) {
            esyslog("skindesigner: error during loading templates for plugin %s", plugName.c_str());
            continue;
        }
        ok = plugView->ReadSubViews(plugName.c_str());
        if (!ok) {
            esyslog("skindesigner: error during loading templates for plugin %s", plugName.c_str());
            continue;
        }
        plugView->SetGlobals(globals);
        pluginViews.insert(pair<int, cViewPlugin*>(plugId, plugView));
    }
}

void cSkinDesigner::CacheViews(void) {
    channelView->PreCache();
    menuView->PreCache();
    messageView->PreCache();
    replayView->PreCache();
    volumeView->PreCache();
    tracksView->PreCache();
    for (map<int,cViewPlugin*>::iterator it = pluginViews.begin(); it != pluginViews.end(); it++) {
        cViewPlugin *plugView = it->second;
        plugView->PreCache();
    }
    dsyslog("skindesigner: templates and images cached");
    imgCache->Debug(false);
}

