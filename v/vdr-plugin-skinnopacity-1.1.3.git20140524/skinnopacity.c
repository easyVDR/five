/*
 * skinopacity.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <getopt.h>
#include <vdr/plugin.h>

#define DEFINE_CONFIG 1
#include "config.h"
#include "nopacity.h"
#include "setup.h"


#if defined(APIVERSNUM) && APIVERSNUM < 10734
#error "VDR-1.7.34 API version or greater is required!"
#endif


static const char *VERSION        = "1.1.3";
static const char *DESCRIPTION    = "'nOpacity' Skin";
static const char *MAINMENUENTRY  = "nOpacity";

class cPluginNopacity : public cPlugin {
private:
  cNopacity *nopacity;
  cImageCache *imgCache;
public:
  cPluginNopacity(void);
  virtual ~cPluginNopacity();
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
  virtual const char *MainMenuEntry(void) {return config.mainMenuEntry ? MAINMENUENTRY : NULL;}
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginNopacity::cPluginNopacity(void)
{
  nopacity = NULL;
}

cPluginNopacity::~cPluginNopacity()
{
}

const char *cPluginNopacity::CommandLineHelp(void)
{
  return
         "  -e <EPGIMAGESPATH>, --epgimages=<IMAGESPATH> Set directory where epgimages are stored\n"
         "  -i <ICONSPATH>, --iconpath=<ICONSPATH> Set directory where Menu Icons are stored\n"
         "  -l <LOGOPATH>, --logopath=<LOGOPATH>       Set directory where Channel Logos are stored.\n";
}

bool cPluginNopacity::ProcessArgs(int argc, char *argv[]) {
  // Implement command line argument processing here if applicable.
  static const struct option long_options[] = {
    { "epgimages", required_argument, NULL, 'e' },
    { "logopath", required_argument, NULL, 'l' },
    { "iconpath", required_argument, NULL, 'i' },
    { 0, 0, 0, 0 }
  };

  int c;
  while ((c = getopt_long(argc, argv, "e:l:i:", long_options, NULL)) != -1) {
    switch (c) {
      case 'l':
        config.SetLogoPath(cString(optarg));
        break;
      case 'e':
        config.SetEpgImagePath(cString(optarg));
        break;
      case 'i':
        config.SetIconPath(cString(optarg));
        break;
      default:
        return false;
    }
  }
  return true;
}

bool cPluginNopacity::Initialize(void) {
  return true;
}

bool cPluginNopacity::Start(void) {
    if (!cOsdProvider::SupportsTrueColor()) {
        esyslog("nopacity: No TrueColor OSD found! Aborting!");
        return false;
    } else
        dsyslog("nopacity: TrueColor OSD found");

    imgCache = new cImageCache();
    nopacity = new cNopacity(imgCache);
    return nopacity;
}

void cPluginNopacity::Stop(void) {
    delete imgCache;
    delete geoManager;
    delete fontManager;
}

void cPluginNopacity::Housekeeping(void) {
}

void cPluginNopacity::MainThreadHook(void) {
}

cString cPluginNopacity::Active(void) {
  return NULL;
}

time_t cPluginNopacity::WakeupTime(void) {
  return 0;
}

cOsdObject *cPluginNopacity::MainMenuAction(void) {
  return NULL;
}

cMenuSetupPage *cPluginNopacity::SetupMenu(void) {
  return new cNopacitySetup(imgCache);
}

bool cPluginNopacity::SetupParse(const char *Name, const char *Value) {
  return config.SetupParse(Name, Value);
}

bool cPluginNopacity::Service(const char *Id, void *Data) {
  return false;
}

const char **cPluginNopacity::SVDRPHelpPages(void) {
    return NULL;
}

cString cPluginNopacity::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    return NULL;
}

VDRPLUGINCREATOR(cPluginNopacity); // Don't touch this!
