/*
 * soppalusikka.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include "common.h"
#include "config.h"
#include "log.h"
#include "logo.h"
#include "setup.h"
#include "soppalusikka.h"

#if defined(APIVERSNUM) && APIVERSNUM < 20200
#error "VDR-2.2.0 API version or greater is required!"
#endif

#ifndef GITVERSION
#define GITVERSION ""
#endif

static const char VERSION[]     = "2.2.1" GITVERSION;
static const char DESCRIPTION[] = trNOOP("Soppalusikka skin");

class cPluginSkinSoppalusikka : public cPlugin {
private:
  bool isLogoDirSetM;

public:
  cPluginSkinSoppalusikka(void);
  virtual ~cPluginSkinSoppalusikka();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void) {}
  virtual cString Active(void) { return NULL; }
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *nameP, const char *valueP);
  virtual bool Service(const char *idP, void *dataP = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *commandP, const char *optionP, int &replyCodeP);
  };

cPluginSkinSoppalusikka::cPluginSkinSoppalusikka(void)
: isLogoDirSetM(false)
{
  // initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  debug1("%s", __PRETTY_FUNCTION__);
}

cPluginSkinSoppalusikka::~cPluginSkinSoppalusikka()
{
  // clean up after yourself!
}

const char *cPluginSkinSoppalusikka::CommandLineHelp(void)
{
  // return a string that describes all known command line options.
  return "  -l <logodir>, --logodir=<logodir>  define a directory for channel logos\n"
         "  -t <mode>,    --trace=<mode>       set the tracing mode\n";
}

bool cPluginSkinSoppalusikka::ProcessArgs(int argc, char *argv[])
{
  // implement command line argument processing here if applicable.
  static const struct option long_options[] = {
    { "logodir", required_argument, NULL, 'l' },
    { "trace",   required_argument, NULL, 't' },
    { NULL,      no_argument,       NULL, 0   }
    };

  int c;
  while ((c = getopt_long(argc, argv, "l:t:", long_options, NULL)) != -1) {
        switch (c) {
          case 'l':
               SoppalusikkaConfig.SetLogoDir(optarg);
               isLogoDirSetM = true;
               break;
          case 't':
               SoppalusikkaConfig.SetTraceMode(strtol(optarg, NULL, 0));
               break;
          default:
               return false;
          }
        }
  return true;
}

bool cPluginSkinSoppalusikka::Initialize(void)
{
  // initialize any background activities the plugin shall perform.
  debug1("%s", __PRETTY_FUNCTION__);
  return true;
}

bool cPluginSkinSoppalusikka::Start(void)
{
  // start any background activities the plugin shall perform.
  debug1("%s", __PRETTY_FUNCTION__);
  // set logo directory
  if (!isLogoDirSetM) {
     SoppalusikkaConfig.SetLogoDir(cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
     isLogoDirSetM = true;
     }
  // resize logo cache
  SoppalusikkaLogoCache.Resize(SoppalusikkaConfig.GetCacheSize());
  // create skin
  new cSkinSoppalusikka;
  return true;
}

void cPluginSkinSoppalusikka::Stop(void)
{
  // stop any background activities the plugin shall perform.
  debug1("%s", __PRETTY_FUNCTION__);
}

void cPluginSkinSoppalusikka::Housekeeping(void)
{
  // perform any cleanup or other regular tasks.
}

cOsdObject *cPluginSkinSoppalusikka::MainMenuAction(void)
{
  // perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginSkinSoppalusikka::SetupMenu(void)
{
  // return a setup menu in case the plugin supports one.
  debug1("%s", __PRETTY_FUNCTION__);
  return new cSkinSoppalusikkaSetup();
}

bool cPluginSkinSoppalusikka::SetupParse(const char *nameP, const char *valueP)
{
  // parse your own setup parameters and store their values.
  debug1("%s", __PRETTY_FUNCTION__);
  if (!strcasecmp(nameP, "ShowAuxInfo"))
     SoppalusikkaConfig.SetShowAuxInfo(atoi(valueP));
  else if (!strcasecmp(nameP, "ShowProgressBar"))
     SoppalusikkaConfig.SetShowProgressBar(atoi(valueP));
  else if (!strcasecmp(nameP, "ShowSymbols"))
     SoppalusikkaConfig.SetShowSymbols(atoi(valueP));
  else if (!strcasecmp(nameP, "ShowLogo"))
     SoppalusikkaConfig.SetShowLogo(atoi(valueP));
  else if (!strcasecmp(nameP, "ShowVPS"))
     SoppalusikkaConfig.SetShowVps(atoi(valueP));
  else if (!strcasecmp(nameP, "ShowDuration"))
     SoppalusikkaConfig.SetShowDuration(atoi(valueP));
  else if (!strcasecmp(nameP, "CacheSize"))
     SoppalusikkaConfig.SetCacheSize(atoi(valueP));
  else
     return false;

  return true;
}

bool cPluginSkinSoppalusikka::Service(const char *idP, void *dataP)
{
  // handle custom service requests from other plugins
  return false;
}

const char **cPluginSkinSoppalusikka::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "FLSH\n"
    "    Flush logo cache.",
    "TRAC [ <mode> ]\n"
    "    Gets and/or sets used tracing mode.\n",
    NULL
    };
  return HelpPages;
}

cString cPluginSkinSoppalusikka::SVDRPCommand(const char *commandP, const char *optionP, int &replyCodeP)
{
  if (strcasecmp(commandP, "FLSH") == 0) {
     SoppalusikkaLogoCache.Flush();
     return cString("Logo cache flushed");
     }
  else if (strcasecmp(commandP, "TRAC") == 0) {
     if (optionP && *optionP)
        SoppalusikkaConfig.SetTraceMode(strtol(optionP, NULL, 0));
     return cString::sprintf("Tracing mode: 0x%04X\n", SoppalusikkaConfig.GetTraceMode());
     }

  return NULL;
}

VDRPLUGINCREATOR(cPluginSkinSoppalusikka); // don't touch this!
