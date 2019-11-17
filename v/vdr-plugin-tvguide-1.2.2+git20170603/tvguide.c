/*
 * tvguide.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <time.h>
#include <getopt.h>
#include <vdr/osd.h>
#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/menu.h>

#define DEFINE_CONFIG 1
#include "geometrymanager.h"
#include "fontmanager.h"
#include "imagecache.h"
#include "config.h"
#include "setup.h"
#include "tvguideosd.h"


#if defined(APIVERSNUM) && APIVERSNUM < 20000
#error "VDR-2.0.0 API version or greater is required!"
#endif

static const char *VERSION        = "1.2.2";
static const char *DESCRIPTION    = "A fancy 2d EPG Viewer";
static const char *MAINMENUENTRY  = "Tvguide";

class cPluginTvguide : public cPlugin {
public:
  cPluginTvguide(void);
  virtual ~cPluginTvguide();
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
  virtual const char *MainMenuEntry(void) { return (tvguideConfig.showMainMenuEntry)?MAINMENUENTRY:NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginTvguide::cPluginTvguide(void) {
}

cPluginTvguide::~cPluginTvguide() {
}

const char *cPluginTvguide::CommandLineHelp(void) {
    return 
         "  -e <IMAGESDIR>, --epgimages=<IMAGESDIR> Set directory where epgimages are stored.\n"
	       "  -i <ICONDIR>, --icons=<ICONDIR>         Set directory where icons are stored.\n"
         "  -l <LOGODIR>, --logodir=<LOGODIR>       Set directory where logos are stored.\n";
}

bool cPluginTvguide::ProcessArgs(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "epgimages", required_argument, NULL, 'e' },
    { "iconpath", required_argument, NULL, 'i' },
    { "logopath", required_argument, NULL, 'l' },
    { 0, 0, 0, 0 }
  };
  int c;
  while ((c = getopt_long(argc, argv, "e:i:l:", long_options, NULL)) != -1) {
    switch (c) {
      case 'e':
        tvguideConfig.SetImagesPath(cString(optarg));
        break;
      case 'i':
      	tvguideConfig.SetIconsPath(cString(optarg));
	      break;
      case 'l':
        tvguideConfig.SetLogoPath(cString(optarg));
        break;
      default:
        return false;
    }
  }
  return true;
}

bool cPluginTvguide::Initialize(void) {
    tvguideConfig.SetDefaultPathes();
    tvguideConfig.LoadTheme();
    tvguideConfig.SetStyle();
    tvguideConfig.setDynamicValues();
    geoManager.SetGeometry(cOsd::OsdWidth(), cOsd::OsdHeight());
    fontManager.SetFonts();
    imgCache.CreateCache();
    return true;
}

bool cPluginTvguide::Start(void) {
    return true;
}

void cPluginTvguide::Stop(void) {
}

void cPluginTvguide::Housekeeping(void) {
}

void cPluginTvguide::MainThreadHook(void) {
}

cString cPluginTvguide::Active(void) {
  return NULL;
}

time_t cPluginTvguide::WakeupTime(void) {
  return 0;
}

cOsdObject *cPluginTvguide::MainMenuAction(void) {
    return new cTvGuideOsd;
}

cMenuSetupPage *cPluginTvguide::SetupMenu(void) {
  return new cTvguideSetup();
}

bool cPluginTvguide::SetupParse(const char *Name, const char *Value) {
  return tvguideConfig.SetupParse(Name, Value);
}

bool cPluginTvguide::Service(const char *Id, void *Data) {
  if (strcmp(Id, "MainMenuHooksPatch-v1.0::osSchedule") == 0 && tvguideConfig.replaceOriginalSchedule != 0) {
      if (Data == NULL)
         return true;
      cOsdObject **guide = (cOsdObject**) Data;
      if (guide)
         *guide = MainMenuAction();
      return true;
  }
  return false;
}

const char **cPluginTvguide::SVDRPHelpPages(void) {
  return NULL;
}

cString cPluginTvguide::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
  return NULL;
}

VDRPLUGINCREATOR(cPluginTvguide);
