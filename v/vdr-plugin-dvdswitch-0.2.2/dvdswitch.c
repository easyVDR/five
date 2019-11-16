/*
 * dvdswitch.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <unistd.h>
#include <vdr/plugin.h>
#include "helpers.h"
#include "imagelist.h"
#include "setup.h"
#include "menu.h"
#include "dvdplugin.h"

#if defined(APIVERSNUM) && APIVERSNUM < 10600
#error "VDR-1.6.0 API version or greater is required!"
#endif

static const char *VERSION        = "0.2.2";

class cPluginDvdswitch : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  bool CheckError(void);
public:
  cImageList ImageList;

  cPluginDvdswitch(void);
  virtual ~cPluginDvdswitch();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr("Allows playback of DVD images"); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return DVDSwitchSetup.HideMenuEntry ? NULL : DVDSwitchSetup.MenuName; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginDvdswitch::cPluginDvdswitch(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginDvdswitch::~cPluginDvdswitch()
{
  // Clean up after yourself!
  cDVDPlugin::Finish();
}

const char *cPluginDvdswitch::CommandLineHelp(void)
{
     //12345678901234567890123456789012345678901234567890123456789012345678901234567890|
  return
      "  -r SCRIPT,     --readscript=SCRIPT   SCRIPT = scriptname with path for reading\n"
      "                                       DVD as a ISO Image File\n"
      "  -w SCRIPT,     --writescript=SCRIPT  SCRIPT = scriptname with path to writing\n"
      "                                       selected DVD Image\n"
      "  -i PATH,       --imagedir=PATH       Path to DVD-Images. This Option can set\n"
      "                                       in SetupMenu optional\n";
}

bool cPluginDvdswitch::ProcessArgs(int argc, char *argv[])
{
  return DVDSwitchSetup.ProcessArgs(argc,argv);
}

bool cPluginDvdswitch::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  dsyslog("dvdswitch: Initialize plugin");

  DVDSwitchSetup.SetConfDir(ConfigDirectory(Name()));
  cDVDPlugin::Init();
  ImageList.Init();
  DVDSwitchSetup.Init();

  return true;
}

bool cPluginDvdswitch::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginDvdswitch::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginDvdswitch::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginDvdswitch::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  if (CheckError())
  {
    return NULL;
  }
  else
    return new cMainMenu(ImageList);
}

cMenuSetupPage *cPluginDvdswitch::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupDVDSwitch(ImageList);
}

bool cPluginDvdswitch::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return DVDSwitchSetup.SetupParse(Name, Value, ImageList);
}

bool cPluginDvdswitch::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginDvdswitch::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginDvdswitch::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

bool cPluginDvdswitch::CheckError(void)
{
  dsyslog("dvdswitch: Check Image Directory");
  if (access(DVDSwitchSetup.ImageDir, R_OK | X_OK) != 0)
  {
    esyslog("dvdswitch: Image Directory '%s' not readable", DVDSwitchSetup.ImageDir);
    OsdMsg(mtError,tr("Image Directory not readable or not exist"));
    return true;
  }

  dsyslog("dvdswitch: Check DVD Plugin");
  if(!cPluginManager::GetPlugin("dvd"))
  {
    esyslog("dvdswitch: DVD-Plugin not found! Function deactivated!");
    OsdMsg(mtInfo,tr("DVD-Plugin not found! Function deactivated!"));
  } else
    DVDSwitchSetup.DVDPluginExist = true;

  return false;
}

VDRPLUGINCREATOR(cPluginDvdswitch); // Don't touch this!
