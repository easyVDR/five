/*
 * tvonscreen.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include "magazine.h"
#include "config.h"
#include "i18n.h"

#if defined(MAINMENUHOOKSVERSNUM) & !defined(MAINMENUHOOKSVERSION)
#undef  MAINMENUHOOKSVERSNUM
#define MAINMENUHOOKSVERSNUM 10000
#endif

static const char *VERSION        = "1.1.142";
static const char *DESCRIPTION    = trNOOP("Shows the EPG info in form of a typical TV magazine");
static const char *MAINMENUENTRY  = trNOOP("TV-OnScreen");

class cPluginTvOnscreen : public cPlugin
{
private:

public:
    cPluginTvOnscreen(void);
    virtual ~cPluginTvOnscreen();
    virtual const char *Version(void)
    {
        return VERSION;
    }
    virtual const char *Description(void)
    {
        return tr(DESCRIPTION);
    }
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Housekeeping(void);
    virtual const char *MainMenuEntry(void);
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
#if MAINMENUHOOKSVERSNUM == 10001
    virtual bool Service(const char *Id, void *Data);
#endif
};

cPluginTvOnscreen::cPluginTvOnscreen(void)
{
    // Initialize any member variables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginTvOnscreen::~cPluginTvOnscreen()
{
    // Clean up after yourself!
}

const char *cPluginTvOnscreen::CommandLineHelp(void)
{
    // Return a string that describes all known command line options.
    return tvonscreenCfg.CommandLineHelp();
}

bool cPluginTvOnscreen::ProcessArgs(int argc, char *argv[])
{
    return tvonscreenCfg.ProcessArgs(argc,argv);
}

bool cPluginTvOnscreen::Initialize(void)
{
    // Initialize any background activities the plugin shall perform.
#if (APIVERSNUM < 10507)
    RegisterI18n(tvoPhrases);
#endif
    return true;
}

bool cPluginTvOnscreen::Start(void)
{
    // Start any background activities the plugin shall perform.
    return true;
}

void cPluginTvOnscreen::Housekeeping(void)
{
    // Perform any cleanup or other regular tasks.
}

const char *cPluginTvOnscreen::MainMenuEntry(void)
{
#if MAINMENUHOOKSVERSNUM == 10001
    if (tvonscreenCfg.replaceorgschedule) return NULL;
#endif
    return tr(MAINMENUENTRY);
}

cOsdObject *cPluginTvOnscreen::MainMenuAction(void)
{
    // Perform the action when selected from the main VDR menu.

    return new magazine(this);
}

cMenuSetupPage *cPluginTvOnscreen::SetupMenu(void)
{
    // Return a setup menu in case the plugin supports one.
    return new tvonscreenConfigPage();
}

bool cPluginTvOnscreen::SetupParse(const char *Name, const char *Value)
{
    // Parse your own setup parameters and store their values.
    return tvonscreenCfg.SetupParse(Name,Value);
}

#if MAINMENUHOOKSVERSNUM == 10001
bool cPluginTvOnscreen::Service(const char *Id, void *Data)
{
    if (tvonscreenCfg.replaceorgschedule &&
            strcmp(Id, "MainMenuHooksPatch-v1.0::osSchedule")==0)
    {
        if (!Data) return true;
        cOsdObject **osd = (cOsdObject **) Data;
        if (osd)
            *osd = (cOsdObject*) MainMenuAction();
        return true;
    }
    return false;
}
#endif

VDRPLUGINCREATOR(cPluginTvOnscreen) // Don't touch this!
