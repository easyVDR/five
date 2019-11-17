#include <vdr/interface.h>
#include "extensions/curlfuncs.h"
#include <vdr/menu.h>
#include "setup.h"

// --- cInstallManager -----------------------------------------------------------
cInstallManager::cInstallManager(void) {
    installing = false;
    updating = false;
    runningInst = NULL;
    installationStart = 0;
    lastInstallDuration = -1;
    timeout = 120;              //2 Minutes timeout
    currentSkin = "";
}

cInstallManager::~cInstallManager(void) {
}

bool cInstallManager::StartInstallation(string skin) {
    runningInst = config.GetSkinRepo(skin);
    if (!runningInst) {
        return false;
    }
    installing = true;
    installationStart = cTimeMs::Now();
    runningInst->Install(*config.installerSkinPath, config.vdrThemesPath);
    return true;
}

bool cInstallManager::StartUpdate(string skin) {
    runningInst = config.GetSkinRepo(skin);
    if (!runningInst || runningInst->Type() != rtGit) {
        return false;
    }
    updating = true;
    installationStart = cTimeMs::Now();
    runningInst->Update(*config.installerSkinPath);
    return true;
}

bool cInstallManager::Finished(void) {
    if (!runningInst)
        return true;
    if (runningInst->InstallationFinished()) {
        installing = false;
        updating = false;
        return true;
    }
    return false;
}

bool cInstallManager::SuccessfullyInstalled(void) {
    if (!runningInst)
        return false;
    bool ok = runningInst->SuccessfullyInstalled();
    runningInst = NULL;
    return ok;
    
}

bool cInstallManager::SuccessfullyUpdated(void) {
    if (!runningInst)
        return false;
    bool ok = runningInst->SuccessfullyUpdated();
    runningInst = NULL;
    return ok;
}

int cInstallManager::Duration(void) {
    return (cTimeMs::Now() - installationStart) / 1000;
}

eOSState cInstallManager::ProcessInstallationStatus(void) {
    if (Installing()) {
        if (Finished()) {
            if (SuccessfullyInstalled()) {
                config.AddNewSkinRef(currentSkin);
                Skins.Message(mtStatus, tr("Skin successfully installed"));
            } else {
                Skins.Message(mtError, tr("Skin NOT successfully installed"));
            }
            cCondWait::SleepMs(1000);
            return osEnd;
        } else {
            int duration = Duration();
            if (duration > timeout) {
                Skins.Message(mtError, tr("Timeout"));
                cCondWait::SleepMs(1000);
                return osEnd;
            } else if (duration != lastInstallDuration) {
                Skins.Message(mtStatus, *cString::sprintf("%s (%d %s)...", tr("Installing Skin"), duration, tr("sec")));
                lastInstallDuration = duration;
            }
        }
    } else if (Updating()) {
        if (Finished()) {
            if (SuccessfullyUpdated()) {
                Skins.Message(mtStatus, tr("Skin successfully updated"));
                cCondWait::SleepMs(1000);
                return osEnd;
            } else {
                Skins.Message(mtStatus, tr("Skin already up to date"));
                return osContinue;
            }            
        } else {
            int duration = Duration();
            if (duration > timeout) {
                Skins.Message(mtError, tr("Timeout"));
                cCondWait::SleepMs(1000);
                return osEnd;
            } else if (duration != lastInstallDuration) {
                Skins.Message(mtStatus, *cString::sprintf("%s (%d %s)...", tr("Updating Skin from Git"), duration, tr("sec")));
                lastInstallDuration = duration;
            }
        }        
    }
    return osContinue;    
}

// --- cSkinDesignerSetup -----------------------------------------------------------
bool cSkinDesignerSetup::skinrepoUpdated = false;

cSkinDesignerSetup::cSkinDesignerSetup(skindesignerapi::cPluginStructure *skinPreviewStruct) {
    this->skinPreviewStruct = skinPreviewStruct;
    numLogosPerSizeInitial = config.numLogosPerSizeInitial;
    cacheImagesInitial = config.cacheImagesInitial;    
    limitLogoCache = config.limitLogoCache;
    numLogosMax = config.numLogosMax;
    debugImageLoading = config.debugImageLoading;
    rerunAmount = config.rerunAmount;
    rerunDistance = config.rerunDistance;
    rerunMaxChannel = config.rerunMaxChannel;
    numCustomTokens = config.numCustomTokens;
    FPS = config.FPS;
    menuDisplayStyle[0] = tr("after one another");
    menuDisplayStyle[1] = tr("at one go");
#ifndef DO_NOT_USE_SKININSTALLER
    if (!skinrepoUpdated) {
        Skins.Message(mtStatus, *cString::sprintf("%s...", tr("Updating Skinrepositories")));
        skinrepoUpdated = true;
        config.ReadSkinRepos();
    }
#endif
    Setup();
}

cSkinDesignerSetup::~cSkinDesignerSetup(void) {
    config.setupCloseDoReload = true;
}


void cSkinDesignerSetup::Setup(void) {
    int current = Current();
    Clear();

    SkinSetup();
    InstallSkins();
    PluginSetup();
    ImageCacheStatistics();

    SetCurrent(Get(current));
    Display();
}

eOSState cSkinDesignerSetup::ProcessKey(eKeys Key) {
    eOSState state = ProcessInstallationStatus();
    if (state == osEnd)
        return osEnd;

    bool hadSubMenu = HasSubMenu();
    state = cMenuSetupPage::ProcessKey(Key);
    if (hadSubMenu && Key == kOk) {
        Store();
    }

    if (!hadSubMenu && (Key == kOk || Key == kUp || Key == kDown || Key == kLeft || Key == kRight || Key == kRed || Key == kYellow)) {
        SetHelp(NULL, NULL, NULL, NULL);
        cOsdItem *current = Get(Current());
        cSkinMenuItem *skinMenuItem = dynamic_cast<cSkinMenuItem*>(current);
        if (!skinMenuItem)
            return state;
        eItemType type = skinMenuItem->Type();
        currentSkin = skinMenuItem->GetSkinName();
        // KEY OK
        if ((Key == kOk)) {
            if (type == itSkinSetup) {
                state = AddSubMenu(new cSkindesignerSkinSetup(currentSkin, "", ""));
            } else if (type == itNoSkinSetup) {
                state = osContinue;
            } else if (type == itSkinRepo) {
                Skins.Message(mtStatus, tr("Downloading Skin Screenshots..."));
                cSkindesignerSkinPreview *prev = new cSkindesignerSkinPreview(currentSkin, skinPreviewStruct);
                Skins.Message(mtStatus, NULL);
                state = AddSubMenu(prev);
            }
        }
        // Menu Moves
        if (Key == kUp || Key == kDown || Key == kLeft || Key == kRight) {
            if (type == itSkinRepo) {
                SetHelp(tr("Install Skin"), NULL, NULL, NULL);
            } else if (type == itSkinSetup || type == itNoSkinSetup) {
                cSkinRepo *repo = config.GetSkinRepo(currentSkin);
                if (repo) {
                    if (repo->Type() == rtGit)
                        SetHelp(tr("Update"), NULL, tr("Delete Skin"), NULL);
                    else
                        SetHelp(NULL, NULL, tr("Delete Skin"), NULL);
                }
            }
        }
        // KEY RED
        if (Key == kRed) {
            string versionNeeded = "";
            bool versionOk = config.CheckVersion(currentSkin, versionNeeded);
            if (type == itSkinRepo) {
                if (!versionOk) {
                    cString error = cString::sprintf("%s %s %s %s %s", 
                                                     tr("Skin Designer"),
                                                     tr("version"), 
                                                     versionNeeded.c_str(),
                                                     tr("or higher"),
                                                     tr("needed"));
                    Skins.Message(mtError, *error);
                    return state;
                }
                Skins.Message(mtStatus, *cString::sprintf("%s ...", tr("Installing Skin")));
                StartInstallation(currentSkin);
            } else if (type == itSkinSetup || type == itNoSkinSetup) {
                bool gitAvailable = StartUpdate(currentSkin);
                if (gitAvailable) {
                    if (!versionOk) {
                        cString error = cString::sprintf("%s %s %s %s %s", 
                                                         tr("Skin Designer"),
                                                         tr("version"), 
                                                         versionNeeded.c_str(),
                                                         tr("or higher"),
                                                         tr("needed"));
                        Skins.Message(mtError, *error);
                        return state;
                    }                    
                    Skins.Message(mtStatus, *cString::sprintf("%s ...", tr("Updating Skin from Git")));
                } else {
                    Skins.Message(mtStatus, tr("No Git Repository available"));
                }
            }
        }
        // KEY YELLOW
        if (Key == kYellow) {
            if (type == itSkinSetup || type == itNoSkinSetup) {
                if (config.SkinActive(currentSkin)) {
                    Skins.Message(mtError, tr("Skin is running and can't be deleted"));
                } else if (Interface->Confirm(*cString::sprintf("%s?", tr("Really delete skin")))) {
                    config.DeleteSkin(currentSkin);
                    Skins.Message(mtStatus, tr("Skin deleted"));
                    cCondWait::SleepMs(1000);
                    return osEnd;
                }
                state = osContinue;
            }
        }
    }
    return state;
}

void cSkinDesignerSetup::Store(void) {
    config.numLogosPerSizeInitial = numLogosPerSizeInitial;
    config.cacheImagesInitial = cacheImagesInitial;
    config.limitLogoCache = limitLogoCache;
    config.numLogosMax = numLogosMax;
    config.debugImageLoading = debugImageLoading;
    config.rerunAmount = rerunAmount;
    config.rerunDistance = rerunDistance;
    config.rerunMaxChannel = rerunMaxChannel;
    config.numCustomTokens = numCustomTokens;
    config.FPS = FPS;

    config.InitSetupIterator();
    cSkinSetup *skinSetup = NULL;
    while (skinSetup = config.GetNextSkinSetup()) {
        string skin = skinSetup->GetSkin();
        skinSetup->InitParameterIterator();
        cSkinSetupParameter *param = NULL;
        while (param = skinSetup->GetNextParameter()) {
            cString paramName = cString::sprintf("%s.%s", skin.c_str(), param->name.c_str());
            SetupStore(*paramName, param->value);
            config.UpdateSkinSetupParameter(*paramName, param->value);
        }
    }
    config.UpdateGlobals();

    SetupStore("DebugImageLoading", debugImageLoading);
    SetupStore("CacheImagesInitial", cacheImagesInitial);
    SetupStore("LimitChannelLogoCache", limitLogoCache);
    SetupStore("NumberLogosInitially", numLogosPerSizeInitial);
    SetupStore("NumberLogosMax", numLogosMax);
    SetupStore("RerunAmount", rerunAmount);
    SetupStore("RerunDistance", rerunDistance);
    SetupStore("RerunMaxChannel", rerunMaxChannel);
    SetupStore("NumCustomTokens", numCustomTokens);
    SetupStore("FPS", FPS);
}

cOsdItem *cSkinDesignerSetup::InfoItem(const char *label) {
    cOsdItem *item;
    item = new cOsdItem(cString::sprintf("---------------- %s ----------------", tr(label)));
    item->SetSelectable(false);
    return item;
}

void cSkinDesignerSetup::PluginSetup(void) {
    Add(InfoItem(tr("Plugin Setup")));

    Add(new cMenuEditIntItem(tr("Frames per Second to display animations"), &FPS, 10, 60));
    Add(new cMenuEditIntItem(tr("Maximum number of custom tokens"), &numCustomTokens, 0, 100));

    Add(InfoItem(tr("Reruns")));
    Add(new cMenuEditIntItem(tr("Maximum number of reruns to display"), &rerunAmount, 1, 100));
    Add(new cMenuEditIntItem(tr("Minimum timely distance of rerun (in hours)"), &rerunDistance, 0, 1000));
    Add(new cMenuEditIntItem(tr("Limit Channel Numbers"), &rerunMaxChannel, 0, 1000, tr("no limit")));

    Add(InfoItem(tr("Image Loading")));
    Add(new cMenuEditBoolItem(tr("Debug Image Loading"), &debugImageLoading));
    Add(new cMenuEditBoolItem(tr("Cache icons, skinparts and logos at start"), &cacheImagesInitial));
    Add(new cMenuEditBoolItem(tr("Limit Channel Logo Cache"), &limitLogoCache));
    Add(new cMenuEditIntItem(tr("Number to cache initially (per size)"), &numLogosPerSizeInitial, 0, 1000));
    Add(new cMenuEditIntItem(tr("Number to cache in maximum"), &numLogosMax, 0, 1000));
}

void cSkinDesignerSetup::ImageCacheStatistics(void) {
    if (!imgCache) {
        return;
    }

    Add(InfoItem(tr("Cache Statistics")));
    float sizeIconCacheInt = 0;
    float sizeIconCacheExt = 0;
    int numIcons = 0;
    imgCache->GetIconCacheSize(numIcons, sizeIconCacheInt, sizeIconCacheExt);
    cString iconCacheInfo = cString::sprintf("%s %d %s - %s %.2f%s %s, %.2f%s %s", tr("cached"), numIcons, tr("icons"), tr("size"), sizeIconCacheInt, tr("MB"), tr("int. memory"), sizeIconCacheExt, tr("MB"), tr("high level memory"));
    Add(new cOsdItem(*iconCacheInfo));
    cList<cOsdItem>::Last()->SetSelectable(false);
    
    float sizeLogoCache = 0;
    int numLogos = 0;
    imgCache->GetLogoCacheSize(numLogos, sizeLogoCache);
    cString logoCacheInfo = cString::sprintf("%s %d %s - %s %.2f%s %s", tr("cached"), numLogos, tr("logos"), tr("size"), sizeLogoCache, tr("MB"), tr("int. memory"));
    Add(new cOsdItem(*logoCacheInfo));
    cList<cOsdItem>::Last()->SetSelectable(false);

    float sizeSkinpartCacheInt = 0;
    float sizeSkinpartCacheExt = 0;
    int numSkinparts = 0;
    imgCache->GetSkinpartsCacheSize(numSkinparts, sizeSkinpartCacheInt, sizeSkinpartCacheExt);
    cString skinpartCacheInfo = cString::sprintf("%s %d %s - %s %.2f%s %s, %.2f%s %s", tr("cached"), numSkinparts, tr("skinparts"), tr("MB"), sizeSkinpartCacheInt, tr("MB"), tr("int. memory"), sizeSkinpartCacheExt, tr("MB"), tr("high level memory"));
    Add(new cOsdItem(*skinpartCacheInfo));
    cList<cOsdItem>::Last()->SetSelectable(false);
}

void cSkinDesignerSetup::SkinSetup(void) {
    Add(InfoItem(tr("Skin Setup")));

    config.InitSkinIterator();
    string skin = "";
    while (config.GetSkin(skin)) {
        cSkinSetup *skinSetup = config.GetSkinSetup(skin);
        if (!skinSetup) {
            Add(new cSkinMenuItem(skin.c_str(), *cString::sprintf("%s %s\t(%s)", tr("Skin"), skin.c_str(), tr("has no setup")), itNoSkinSetup));
        } else {
            Add(new cSkinMenuItem(skin.c_str(), *cString::sprintf("%s %s", tr("Skin"), skin.c_str()), itSkinSetup));
        }
    }
}

void cSkinDesignerSetup::InstallSkins(void) {
#ifndef DO_NOT_USE_SKININSTALLER
    Add(InfoItem(tr("Install new skins")));

    config.InitSkinRepoIterator();
    cSkinRepo *repo = NULL;
    while (repo = config.GetNextSkinRepo()) {
        if (config.SkinInstalled(repo->Name()))
            continue;
        Add(new cSkinMenuItem(repo->Name(), *cString::sprintf("%s %s", tr("Preview Skin"), repo->Name().c_str()), itSkinRepo));
    }
#endif
}

// --- cSkinMenuItem -----------------------------------------------------------
cSkinMenuItem::cSkinMenuItem(string skinName, string displayText, eItemType type) : cOsdItem(displayText.c_str()) {
    this->skinName = skinName;
    this->type = type;
}

// --- cSkinSetupSubMenu -----------------------------------------------------------
cSkinSetupSubMenu::cSkinSetupSubMenu(string name, string displayText) : cOsdItem(displayText.c_str()) {
    this->name = name;
    this->displayText = displayText;
}

// --- cSkindesignerSkinSetup -----------------------------------------------------------

cSkindesignerSkinSetup::cSkindesignerSkinSetup(string skin, string menu, string header) : 
cOsdMenu(*cString::sprintf("%s: %s \"%s\" %s", trVDR("Setup"), tr("Skin"), skin.c_str(), header.c_str()), 30) {
    SetMenuCategory(mcPluginSetup);
    this->skin = skin;
    this->menu = menu;
    buttonRed = tr("Update");
    buttonGreen = tr("Help");
    buttonYellow = tr("Delete Skin");
    showRed = false;
    showYellow = false;
    hadHelp = false;
    Set();
}

cSkindesignerSkinSetup::~cSkindesignerSkinSetup() {
}

eOSState cSkindesignerSkinSetup::ProcessKey(eKeys Key) {
    eOSState state = ProcessInstallationStatus();
    if (state == osEnd)
        return osEnd;
    state = cOsdMenu::ProcessKey(Key);
    if (Key == kUp || Key == kDown) {
        ShowButtons(Current());
    }
    if (state == osUnknown) {
        switch (Key) {
            case kOk: {
                cOsdItem *current = Get(Current());
                cSkinSetupSubMenu *subMenuItem = dynamic_cast<cSkinSetupSubMenu*>(current);
                if (subMenuItem) {
                    state = AddSubMenu(new cSkindesignerSkinSetup(skin, subMenuItem->GetName(), subMenuItem->GetDisplayText()));
                    break;
                } else {
                    return osBack;
                }
            }
            case kRed: {
                string versionNeeded = "";
                bool versionOk = config.CheckVersion(skin, versionNeeded);
                bool gitAvailable = StartUpdate(skin);
                if (gitAvailable) {
                    if (!versionOk) {
                        cString error = cString::sprintf("%s %s %s %s %s", 
                                                         tr("Skin Designer"),
                                                         tr("version"), 
                                                         versionNeeded.c_str(),
                                                         tr("or higher"),
                                                         tr("needed"));
                        Skins.Message(mtError, *error);
                        return osContinue;
                    }
                    Skins.Message(mtStatus, *cString::sprintf("%s ...", tr("Updating Skin from Git")));
                } else {
                    Skins.Message(mtStatus, tr("No Git Repository available"));
                }
                break;
            }
            // KEY YELLOW
            case kYellow: {
                if (config.SkinActive(skin)) {
                    Skins.Message(mtError, tr("Skin is running and can't be deleted"));
                } else if (Interface->Confirm(*cString::sprintf("%s?", tr("Really delete skin")))) {
                    config.DeleteSkin(skin);
                    Skins.Message(mtStatus, tr("Skin deleted"));
                    cCondWait::SleepMs(1000);
                    return osEnd;
                }
                state = osContinue;
                break;
            }
            case kGreen: {
                string helpText = helpTexts[Current()];
                if (helpText.size() == 0) {
                    state = osContinue;
                    break;
                }
                const char* ItemText = Get(Current())->Text();
                string title = *cString::sprintf("%s - %s", buttonGreen.c_str(), ItemText);
                state = AddSubMenu(new cMenuText(title.c_str(), helpText.c_str()));
                break;
            }
            default:
                break;
        }
    }
    return state;
}

void cSkindesignerSkinSetup::Set(void) {
    cSkinSetupMenu *setupMenu = config.GetSkinSetupMenu(skin, menu);
    if (!setupMenu) {
        return;
    }
    
    cSkinRepo *repo = config.GetSkinRepo(skin);
    if (repo && repo->Type() == rtGit)
        showRed = true;
    if (repo)
        showYellow = true;
 
    setupMenu->InitParameterIterator();
    cSkinSetupParameter *param = NULL;
    while (param = setupMenu->GetNextParameter(false)) {
        if (param->type == sptInt) {
            Add(new cMenuEditIntItem(param->displayText.c_str(), &param->value, param->min, param->max));
        } else if (param->type == sptBool) {
            Add(new cMenuEditBoolItem(param->displayText.c_str(), &param->value));
        } else if (param->type == sptString) {
            Add(new cMenuEditStraItem(param->displayText.c_str(), &param->value, param->numOptions, param->optionsTranslated));
        } else if (param->type == sptSeparator) {
            cOsdItem *item = new cOsdItem(param->displayText.c_str());
            item->SetSelectable(false);
            Add(item);            
        }
        helpTexts.push_back(param->helpText);
    }

    setupMenu->InitSubmenuIterator();
    cSkinSetupMenu *subMenu = NULL;
    while (subMenu = setupMenu->GetNextSubMenu(false)) {
        Add(new cSkinSetupSubMenu(subMenu->GetName(), subMenu->GetDisplayText()));
        helpTexts.push_back("");
    }

    ShowButtons(0, true);
}

void cSkindesignerSkinSetup::ShowButtons(int current, bool force) {
    bool showGreen = helpTexts[current].size() > 0 ? true : false;
    bool changed = false;
    if ((hadHelp && !showGreen) || (!hadHelp && showGreen))
        changed = true;
    hadHelp = showGreen;
    if (changed || force)
        SetHelp(showRed ? buttonRed.c_str() : "", showGreen ? buttonGreen.c_str() : "", showYellow ? buttonYellow.c_str() : "", NULL);
}

// --- cSkindesignerSkinPreview -----------------------------------------------------------

cSkindesignerSkinPreview::cSkindesignerSkinPreview(string skin, skindesignerapi::cPluginStructure *plugStruct)  : 
cSkindesignerOsdMenu(plugStruct, *cString::sprintf("%s: %s \"%s\"", tr("Preview"), tr("Skin"), skin.c_str())) {
    currentSkin = skin;
    FirstCallCleared();
    Set();
}

cSkindesignerSkinPreview::~cSkindesignerSkinPreview() {
}

eOSState cSkindesignerSkinPreview::ProcessKey(eKeys Key) {
    eOSState state = ProcessInstallationStatus();
    if (state == osEnd)
        return osEnd;
    state = cOsdMenu::ProcessKey(Key);
    switch (Key) {
        case kOk:
        case kBack:
            state = osBack;
            break;
        case kLeft: {
            TextKeyLeft();
            state = osContinue;
            break;
        } case kRight: {
            TextKeyRight();
            state = osContinue;            
            break;
        } case kUp: {
            TextKeyUp();
            state = osContinue;            
            break;
        } case kDown: {
            TextKeyDown();
            state = osContinue;
            break;
        } case kRed: {
            string versionNeeded = "";
            bool versionOk = config.CheckVersion(currentSkin, versionNeeded);
            if (!versionOk) {
                cString error = cString::sprintf("%s %s %s %s %s", 
                                                 tr("Skin Designer"),
                                                 tr("version"), 
                                                 versionNeeded.c_str(),
                                                 tr("or higher"),
                                                 tr("needed"));
                Skins.Message(mtError, *error);
            } else {
                StartInstallation(currentSkin);
            }
            state = osContinue;
            break;
        } default:
            break;
    }
    return state;
}

void cSkindesignerSkinPreview::Display(void) {
    SetHelp(tr("Install Skin"), NULL, NULL, NULL);
    skindesignerapi::cSkindesignerOsdMenu::Display();
}

void cSkindesignerSkinPreview::Set(void) {
    SetPluginMenu(0, skindesignerapi::mtText);
    Clear();

    skindesignerapi::cTokenContainer *tk = GetTokenContainer(0);
    SetTokenContainer(tk);
    ClearTokens();

    cSkinRepo *skinRepo = config.GetSkinRepo(currentSkin);
    if (!skinRepo) {
        esyslog("skindesigner: no valid skin repository found for skin %s", currentSkin.c_str());
        return;
    }

    
    int fontsIndex = GetLoopIndex("fonts");
    int pluginIndex = GetLoopIndex("plugins");
    int screenshotIndex = GetLoopIndex("screenshots");

    vector<string> *specialFonts = skinRepo->SpecialFonts();
    vector<string> *supportedPlugins = skinRepo->SupportedPlugins();
    vector< pair < string, string > > *screenshots = skinRepo->Screenshots();

    vector<int> loopInfo;        
    loopInfo.push_back((int)specialFonts->size());
    loopInfo.push_back((int)supportedPlugins->size());
    loopInfo.push_back((int)screenshots->size());
    SetLoop(loopInfo);

    AddStringToken((int)eDmSkinPreviewST::menuheader, *cString::sprintf("%s: %s \"%s\"", trVDR("Preview"), tr("Skin"), currentSkin.c_str()));
    AddStringToken((int)eDmSkinPreviewST::skinname, currentSkin.c_str());
    AddStringToken((int)eDmSkinPreviewST::author, skinRepo->Author().c_str());

    stringstream plainText;
    plainText << *cString::sprintf("%s: %s \"%s\"", trVDR("Preview"), tr("Skin"), currentSkin.c_str()) << "\n\n";
    plainText << tr("Author") << ": " << skinRepo->Author() << "\n";
    plainText << tr("Used Fonts") << ": \n";
    
    int i = 0;
    for (vector<string>::iterator it = specialFonts->begin(); it != specialFonts->end(); it++) {
        AddLoopToken(fontsIndex, i, (int)eDmSkinPreviewFontsLT::name, (*it).c_str());
        AddLoopToken(fontsIndex, i, (int)eDmSkinPreviewFontsLT::installed, CheckFontInstalled(*it));
        plainText << *it << "\n";
        i++;
    }
    
    plainText << tr("Supported Plugins") << ": \n";
    i = 0;
    for (vector<string>::iterator it = supportedPlugins->begin(); it != supportedPlugins->end(); it++) {
        AddLoopToken(pluginIndex, i, (int)eDmSkinPreviewPluginsLT::name, (*it).c_str());
        plainText << *it << "\n";
        i++;
    }

    SetText(plainText.str().c_str());

    i = 0;
    for (vector< pair < string, string > >::iterator it = screenshots->begin(); it != screenshots->end(); it++) {
        string url = it->second;
        string imgType = ".jpg";
        if (url.find(".png") != string::npos)
            imgType = ".png";
        cString tempName = cString::sprintf("/tmp/screenshot_%s_%d%s", currentSkin.c_str(), i, imgType.c_str());
        dsyslog("skindesigner: download screenshot name %s url %s", *tempName, url.c_str());
        CurlGetUrlFile(url.c_str(), *tempName);
        AddLoopToken(screenshotIndex, i, (int)eDmSkinPreviewScreenshotsLT::desc, (it->first).c_str());
        AddLoopToken(screenshotIndex, i, (int)eDmSkinPreviewScreenshotsLT::path, *tempName);
        i++;
    }
}

void cSkindesignerSkinPreview::DefineTokens(skindesignerapi::cTokenContainer *tk) {
    tk->DefineStringToken("{menuheader}", (int)eDmSkinPreviewST::menuheader);
    tk->DefineStringToken("{skinname}", (int)eDmSkinPreviewST::skinname);
    tk->DefineStringToken("{author}", (int)eDmSkinPreviewST::author);
    tk->DefineLoopToken("{fonts[name]}", (int)eDmSkinPreviewFontsLT::name);
    tk->DefineLoopToken("{fonts[installed]}", (int)eDmSkinPreviewFontsLT::installed);
    tk->DefineLoopToken("{plugins[name]}", (int)eDmSkinPreviewPluginsLT::name);
    tk->DefineLoopToken("{screenshots[desc]}", (int)eDmSkinPreviewScreenshotsLT::desc);
    tk->DefineLoopToken("{screenshots[path]}", (int)eDmSkinPreviewScreenshotsLT::path);
}

const char *cSkindesignerSkinPreview::CheckFontInstalled(string fontName) {
    if (fontManager->FontInstalled(fontName))
        return "1";
    return "0";
}

