/*
 * skindesigner.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <getopt.h>
#include <vdr/plugin.h>

#define DEFINE_CONFIG 1
#include "config.h"
#include "designer.h"
#include "setup.h"
#include "libskindesignerapi/skindesignerapi.h"
#include "extensions/globaltimers.h"

#if defined(APIVERSNUM) && APIVERSNUM < 20200
#error "VDR-2.2.0 API version or greater is required!"
#endif


static const char *VERSION        = "1.2.3";
static const char *DESCRIPTION    = trNOOP("Skin Designer");

class cPluginSkinDesigner : public cPlugin, public skindesignerapi::SkindesignerAPI {
private:
    string libskindesignerApiVersion;
    skindesignerapi::cPluginStructure *skinPreviewStruct;
protected:
    bool ServiceRegisterPlugin(skindesignerapi::cPluginStructure *plugStructure);
    skindesignerapi::ISDDisplayMenu *ServiceGetDisplayMenu(void);
    skindesignerapi::ISkinDisplayPlugin *ServiceGetDisplayPlugin(int plugId);
public:
    cPluginSkinDesigner(void);
    virtual ~cPluginSkinDesigner();
    virtual const char *Version(void) { return VERSION; }
    virtual const char *Description(void) { return tr(DESCRIPTION); }
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual void MainThreadHook(void);
    virtual cString Active(void);
    virtual time_t WakeupTime(void);
    virtual const char *MainMenuEntry(void) {return NULL;}
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginSkinDesigner::cPluginSkinDesigner(void) {
    libskindesignerApiVersion = "undefined";
    config.SetVersion(VERSION);
    skinPreviewStruct = NULL;
}

cPluginSkinDesigner::~cPluginSkinDesigner() {
    delete skinPreviewStruct;
}

const char *cPluginSkinDesigner::CommandLineHelp(void) {
  return
         "  -s <SKINPATH>, --skinpath=<SKINPATH> Set directory where xml skins are stored by Package Manager\n"
         "  -i <INSTALLERPATH>, --installerpath=<INSTALLERPATH> Set directory where xml skins are stored by Installer\n"
         "  -l <LOGOPATH>, --logopath=<LOGOPATH> Set directory where a common logo set for all skins is stored\n"
         "  -e <EPGIMAGESPATH>, --epgimages=<IMAGESPATH> Set directory where epgimages are stored\n";
}

bool cPluginSkinDesigner::ProcessArgs(int argc, char *argv[]) {
  // Implement command line argument processing here if applicable.
    static const struct option long_options[] = {
        { "epgimages", required_argument, NULL, 'e' },
        { "logopath", required_argument, NULL, 'l' },
        { "skinpath", required_argument, NULL, 's' },
        { "installerpath", required_argument, NULL, 'i' },
        { 0, 0, 0, 0 }
    };

    int c;
    while ((c = getopt_long(argc, argv, "e:s:l:i:", long_options, NULL)) != -1) {
        switch (c) {
            case 'e':
                config.SetEpgImagePath(cString(optarg));
                break;
            case 'l':
                config.SetLogoPath(cString(optarg));
                break;
            case 's':
                config.SetSkinPath(cString(optarg));
                break;
            case 'i':
                config.SetInstallerSkinPath(cString(optarg));
                break;
            default:
                return false;
        }
    }
    return true;
}

bool cPluginSkinDesigner::Initialize(void) {
    plgManager = new cSDPluginManager();
    return true;
}

bool cPluginSkinDesigner::Start(void) {
    cXmlParser::InitLibXML();
    cImageImporterSVG::InitLibRSVG();
    cGlobalTimers::StartRefreshThread();
    bool trueColorAvailable = true;
    
    if (!cOsdProvider::SupportsTrueColor()) {
        esyslog("skindesigner: No TrueColor OSD found! Using default Skin LCARS!");
        trueColorAvailable = false;
    } else
        dsyslog("skindesigner: TrueColor OSD found");

    libskindesignerApiVersion = LIBSKINDESIGNERAPIVERSION;
    dsyslog("skindesigner: using libskindesigner API Version %s", libskindesignerApiVersion.c_str());

    skinPreviewStruct = new skindesignerapi::cPluginStructure();
    skinPreviewStruct->name = "setup";
    skinPreviewStruct->libskindesignerAPIVersion = LIBSKINDESIGNERAPIVERSION;
    skindesignerapi::cTokenContainer *tkSkinPreview = new skindesignerapi::cTokenContainer();
    cSkindesignerSkinPreview::DefineTokens(tkSkinPreview);
    skinPreviewStruct->RegisterMenu(0, skindesignerapi::mtText, "skinpreview.xml", tkSkinPreview);
    if (RegisterPlugin(skinPreviewStruct)) {
        dsyslog("skindesigner: skinsetup template successfully registered at skindesigner, id %d", skinPreviewStruct->id);
    }

    config.SetOsdLanguage();
    config.SetPathes();
    config.ReadSkins();
    config.InitSkinIterator();
    string skin = "";
    bool skinAvailable = false;
    while (config.GetSkin(skin)) {
        config.ReadSkinSetup(skin);
        cTheme *theme = new cTheme();
        config.StoreTheme(theme);
        cSkinDesigner *newSkin = new cSkinDesigner(skin, theme);
        config.AddSkin(newSkin);
        skinAvailable = true;
        if (!trueColorAvailable) {
            newSkin->ActivateBackupSkin();
        }
    }
    config.TranslateSetup();
    config.SetSkinSetupParameters();

    if (!skinAvailable) {
        esyslog("skindesigner: no skins found! Using default Skin LCARS!");
    }
#ifdef USE_ZAPCOCKPIT
    dsyslog("skindesigner: zapcockpit patch available");
#endif
    return true;
}

void cPluginSkinDesigner::Stop(void) {
    delete imgCache;
    delete fontManager;
    delete plgManager;
    cXmlParser::CleanupLibXML();
    cGlobalTimers::StopRefreshThread();
}

void cPluginSkinDesigner::Housekeeping(void) {
}

void cPluginSkinDesigner::MainThreadHook(void) {
}

cString cPluginSkinDesigner::Active(void) {
    return NULL;
}

time_t cPluginSkinDesigner::WakeupTime(void) {
    return 0;
}

cOsdObject *cPluginSkinDesigner::MainMenuAction(void) {
    return NULL;
}

cMenuSetupPage *cPluginSkinDesigner::SetupMenu(void) {
    return new cSkinDesignerSetup(skinPreviewStruct);
}

bool cPluginSkinDesigner::SetupParse(const char *Name, const char *Value) {
    return config.SetupParse(Name, Value);
}

bool cPluginSkinDesigner::Service(const char *Id, void *Data) {
    return false;
}

const char **cPluginSkinDesigner::SVDRPHelpPages(void) {
    static const char *HelpPages[] = {
        "RELD\n"
        "    force reload of templates and caches",
        "DLIC\n"
        "    delete image cache",
        "SCIT\n"
        "    Set custom Integer Token key = value",
        "SCST\n"
        "    Set custom String Token key = value",
        "LCTK\n"
        "    List custom Tokens",
        "LSTF\n"
        "    List available Fonts",
        0
    };
    return HelpPages;
}

cString cPluginSkinDesigner::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    
    cSkinDesigner *activeSkin = NULL;
    cSkinDesigner *availableSkin = NULL;
    config.InitSkinRefsIterator();
    while (availableSkin = config.GetNextSkinRef()) {
        string activeSkinName = Setup.OSDSkin;
        string currentSkinName = availableSkin->Description();
        if (!currentSkinName.compare(activeSkinName)) {
            activeSkin = availableSkin;
            break;
        }
    }

    if (!activeSkin) {
        ReplyCode = 550;
        return "SKINDESIGNER not active.";
    }

    if (strcasecmp(Command, "RELD") == 0) {
        config.ClearSkinSetups();
        config.InitSkinIterator();
        string skin = "";
        while (config.GetSkin(skin)) {
            config.ReadSkinSetup(skin);
        }
        config.TranslateSetup();
        config.SetSkinSetupParameters();
        activeSkin->Reload();
        ReplyCode = 250;
        return "SKINDESIGNER reload of templates and caches forced.";
    } else if (strcasecmp(Command, "DLIC") == 0) {
        if (imgCache)
            delete imgCache;
        imgCache = new cImageCache();
        imgCache->SetPathes();
        ReplyCode = 250;
        return "SKINDESIGNER Image Cache deleted.";
    } else if (strcasecmp(Command, "LSTF") == 0) {
        activeSkin->ListAvailableFonts();
        ReplyCode = 250;
        return "SKINDESIGNER available fonts listed in syslog.";
    } else if (strcasecmp(Command, "SCIT") == 0) {
        if (!Option) {
            ReplyCode = 501;
            return "SKINDESIGNER SCIK Error: no Token name = value set";
        }
        bool optionOk = activeSkin->SetCustomIntToken(Option);
        if (optionOk) {
            ReplyCode = 250;
            return cString::sprintf("SKINDESIGNER Set custom Int Token %s", Option);
        } else {
            ReplyCode = 501;
            return cString::sprintf("SKINDESIGNER Invalid custom Int Token %s", Option);
        }
    } else if (strcasecmp(Command, "SCST") == 0) {
        if (!Option) {
            ReplyCode = 501;
            return "SKINDESIGNER SCSK Error: no Token name = value set";
        }
        bool optionOk = activeSkin->SetCustomStringToken(Option);
        if (optionOk) {
            ReplyCode = 250;
            return cString::sprintf("SKINDESIGNER Set custom String Token %s", Option);
        } else {
            ReplyCode = 501;
            return cString::sprintf("SKINDESIGNER Invalid custom String Token %s", Option);
        }
    } else if (strcasecmp(Command, "LCTK") == 0) {
        activeSkin->ListCustomTokens();
        ReplyCode = 250;
        return "SKINDESIGNER Custom Tokens listed in Log";
    }
    ReplyCode = 502;
    return "SKINDESIGNER invalid SVDRP command.";
}


bool cPluginSkinDesigner::ServiceRegisterPlugin(skindesignerapi::cPluginStructure *plugStructure) {
    if (plugStructure->menus.size() < 1 && plugStructure->rootview.size() < 1) {
        esyslog("skindesigner: error - plugin without menus or views registered");
        return false;
    }
    //basic plugin interface
    if (plugStructure->menus.size() > 0)
        plgManager->RegisterBasicPlugin(plugStructure);
    //advanced plugin interface
    if (plugStructure->rootview.size() > 0)
        plgManager->RegisterAdvancedPlugin(plugStructure);
    return true;
}

skindesignerapi::ISDDisplayMenu *cPluginSkinDesigner::ServiceGetDisplayMenu(void) {
    cSkin *current = Skins.Current();
    cSkinDesigner *availableSkin = NULL;
    config.InitSkinRefsIterator();
    while (availableSkin = config.GetNextSkinRef()) {
        if (availableSkin == current) {
            cSDDisplayMenu *displayMenu = availableSkin->GetDisplayMenu();
            if (displayMenu) {
                return displayMenu;
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

skindesignerapi::ISkinDisplayPlugin *cPluginSkinDesigner::ServiceGetDisplayPlugin(int plugId) {
    cSkin *current = Skins.Current();
    cSkinDesigner *availableSkin = NULL;
    config.InitSkinRefsIterator();
    while (availableSkin = config.GetNextSkinRef()) {
        if (availableSkin == current) {
            return availableSkin->GetDisplayPlugin(plugId);
        }
    }
    return NULL;
}

VDRPLUGINCREATOR(cPluginSkinDesigner); // Don't touch this!
