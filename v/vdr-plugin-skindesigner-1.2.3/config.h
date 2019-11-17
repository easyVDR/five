#ifndef __DESIGNER_CONFIG_H
#define __DESIGNER_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <vdr/tools.h>
#include <vdr/skins.h>
#include <vdr/plugin.h>
#include "designer.h"
#include "extensions/pluginmanager.h"
#include "extensions/fontmanager.h"
#include "extensions/imagecache.h"
#include "extensions/recfolderinfo.h"
#include "extensions/skinsetup.h"
#include "extensions/skinrepo.h"
#include "libskindesignerapi/skindesignerapi.h"

#define SCRIPTOUTPUTPATH "/tmp/skindesigner"

class cDesignerConfig {
private:
    string version;
    cString CheckSlashAtEnd(string path);
    bool epgImagePathSet;
    bool skinPathSet;
    bool installerSkinPathSet;
    bool logoPathSet;
    cRect osdSize;
    string osdSkin;
    string osdTheme;
    string fontFix;
    string fontOsd;
    string fontSml;
    string osdLanguage;
    cGlobals *tmplGlobals;
    vector<cSkinDesigner*> skinRefs;
    vector<cTheme*> themes;
    vector<cSkinDesigner*>::iterator skinRefsIterator;
    vector<string> deliveredSkins;
    vector<string> installerSkins;
    vector<string> skins;
    vector<string>::iterator skinIterator;
    map < string, cSkinSetup* > skinSetups;
    map < string, cSkinSetup* >::iterator setupIt;
    vector < pair <string, int> > skinSetupParameters;
    cSkinRepos skinRepos;
    void ReadSkinFolder(cString &folder, vector<string> *container);
public:
    cDesignerConfig();
    ~cDesignerConfig();
    void SetVersion(string version) {this->version = version; };
    bool SetupParse(const char *Name, const char *Value);
    void SetPathes(void);
    void SetSkinPath(cString path);
    void SetInstallerSkinPath(cString path);
    void SetLogoPath(cString path);
    void SetEpgImagePath(cString path);
    bool GetThemeColor(string &name, tColor &col);
    void StoreTheme(cTheme *theme);
    void ReadSkins(void);
    void ReadSkinSetup(string skin);
    void InitSkinIterator(void) { skinIterator = skins.begin(); };
    bool GetSkin(string &skin);
    cString GetSkinPath(string skin);
    void AddSkin(cSkinDesigner *skin) { skinRefs.push_back(skin); };
    void AddNewSkinRef(string skin);
    bool SkinActive(string skin);
    void DeleteSkin(string skin);
    void InitSkinRefsIterator(void) { skinRefsIterator = skinRefs.begin(); };
    cSkinDesigner *GetNextSkinRef(void);
    void ClearSkinSetups(void);
    void DebugSkinSetups(void);
    void DebugSkinSetupParameters(void);
    cSkinSetup* GetSkinSetup(string &skin);
    cSkinSetup* GetNextSkinSetup(void);
    cSkinSetupMenu* GetSkinSetupMenu(string &skin, string &menu);
    void InitSetupIterator(void) { setupIt = skinSetups.begin(); };
    void TranslateSetup(void);
    void SetSkinSetupParameters(void);
    void UpdateSkinSetupParameter(string name, int value);
    void ReadSkinRepos(void);
    void InitSkinRepoIterator(void) { skinRepos.InitRepoIterator(); };
    cSkinRepo *GetNextSkinRepo(void) { return skinRepos.GetNextRepo(); };
    cSkinRepo *GetSkinRepo(string name) { return skinRepos.GetRepo(name); };
    bool CheckVersion(string name, string &neededVersion);
    bool SkinInstalled(string name);
    void SetGlobals(cGlobals *globals) { tmplGlobals = globals; };
    void UpdateGlobals(void);
    void CheckDecimalPoint(void);
    void SetSkin(void);
    bool SkinChanged(void);
    void SetOSDSize(void);
    bool OsdSizeChanged(void);
    void SetOSDFonts(void);
    bool OsdFontsChanged(void);
    void SetOsdLanguage(void) { osdLanguage = Setup.OSDLanguage; };
    bool OsdLanguageChanged(void);
    cString GetSkinRessourcePath(void);
    cString skinPath;
    cString installerSkinPath;
    cString logoPath;
    cString epgImagePath;
    string vdrThemesPath;
    bool replaceDecPoint;
    char decPoint;
    //Setup Parameter
    int cacheImagesInitial;
    int numLogosPerSizeInitial;
    int limitLogoCache;
    int numLogosMax;
    int debugImageLoading;
    int rerunAmount;
    int rerunDistance;
    int rerunMaxChannel;
    int numCustomTokens;
    int FPS;
    //TemplateReload on Setup Close
    bool setupCloseDoReload;
};

#ifdef DEFINE_CONFIG
    cDesignerConfig config;
    cSDPluginManager *plgManager = NULL;
    cFontManager *fontManager = NULL;
    cImageCache *imgCache = NULL;
    cRecordingsFolderInfo recFolderInfo;
#else
    extern cDesignerConfig config;
    extern cSDPluginManager *plgManager;
    extern cFontManager *fontManager;
    extern cImageCache *imgCache;
    extern cRecordingsFolderInfo recFolderInfo;
#endif

#endif //__DESIGNER_CONFIG_H
