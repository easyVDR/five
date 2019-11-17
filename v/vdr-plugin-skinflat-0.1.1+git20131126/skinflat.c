/*
 * skinflat.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <getopt.h>
#include <vdr/plugin.h>

#if defined(APIVERSNUM) && APIVERSNUM < 10734
#error "VDR-1.7.34 API version or greater is required!"
#endif

#include "flat.h"

static const char *VERSION        = "0.1.1";
static const char *DESCRIPTION    = "skin flat";

class cPluginFlat : public cPlugin {
    private:
        cFlat *flat;
    public:
        cPluginFlat(void);
        virtual ~cPluginFlat();
        virtual const char *Version(void) { return VERSION; }
        virtual const char *Description(void) { return DESCRIPTION; }
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

cPluginFlat::cPluginFlat(void) {
    flat = NULL;
}

cPluginFlat::~cPluginFlat() {
}

const char *cPluginFlat::CommandLineHelp(void) {
    return "  -l <LOGOPATH>, --logopath=<LOGOPATH>       Set directory where Channel Logos are stored.\n"; 
}

bool cPluginFlat::ProcessArgs(int argc, char *argv[]) {
    // Implement command line argument processing here if applicable.
    static const struct option long_options[] = {
        { "logopath", required_argument, NULL, 'l' },
        { NULL }
    };

    int c;
    while ((c = getopt_long(argc, argv, "l:", long_options, NULL)) != -1) {
        switch (c) {
            case 'l':
                Config.SetLogoPath(cString(optarg));
                break;
            default:
                return false;
        }
    }
    return true;
}

bool cPluginFlat::Initialize(void) {
    Config.Init();
    return true;
}

bool cPluginFlat::Start(void) {
    if (!cOsdProvider::SupportsTrueColor()) {
        esyslog("skinflat: No TrueColor OSD found! Aborting!");
        return false;
    } else
        dsyslog("skinflat: TrueColor OSD found");
    flat = new cFlat;
    return flat;
}

void cPluginFlat::Stop(void) {
}

void cPluginFlat::Housekeeping(void) {
}

void cPluginFlat::MainThreadHook(void) {
}

cString cPluginFlat::Active(void) {
    return NULL;
}

time_t cPluginFlat::WakeupTime(void) {
    return 0;
}

cOsdObject *cPluginFlat::MainMenuAction(void) {
    return NULL;
}

cMenuSetupPage *cPluginFlat::SetupMenu(void) {
    return NULL;
}

bool cPluginFlat::SetupParse(const char *Name, const char *Value) {
    return true;
}

bool cPluginFlat::Service(const char *Id, void *Data) {
    return false;
}

const char **cPluginFlat::SVDRPHelpPages(void) {
    return NULL;
}

cString cPluginFlat::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    return NULL;
}

VDRPLUGINCREATOR(cPluginFlat); // Don't touch this!
