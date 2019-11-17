/*
 * skinelchi.c: 'Elchi' skin for the Video Disk Recorder
 *
 * Copyright (C) 2002?- 2004? Andy Grobb, Rolf Ahrenberg, Andreas Kool
 * Copyright (C) 2004?- 2005? sezz @ vdr-portal.de, Christoph Haubrich
 * Copyright (C) 2005?- 2006 _Frank_ @ vdrportal.de
 * Copyright (C) 2007 - 2009 Christoph Haubrich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * See the README file how to reach the author.
 *
 * $Id$
 */
#include <getopt.h>


//#ifdef VIDEOSTATUS
#include "videostatus.h"
//#endif

#if defined(APIVERSNUM) && APIVERSNUM < 10600
#error THIS VERSION OF THE SKINELCHI-PLUGIN REQUIRES AT LEAST VDR 1.6.0
#endif


//#include "service.h"
#include "vdrstatus.h"

#include "setup.h"
#include "config.h"
#include "common.h"
#include "DisplayChannel.h"
#include "DisplayMenu.h"
#include "DisplayReplay.h"
#include "DisplayVolume.h"
#include "DisplayTracks.h"
#include "DisplayMessage.h"
#include "skinelchi.h"

#include <vdr/font.h>
#include <vdr/osd.h>
#include <vdr/menu.h>
#include <vdr/themes.h>
#include <vdr/plugin.h>

static const char *VERSION      = "0.2.8";
static const char *DESCRIPTION  = trNOOP("Elchi skin");
const char *OSDSKIN             = "Elchi";

cVideoStatus *VideoStatus;
cSkinElchiStatus *ElchiStatus;


// --- cSkinElchi ----------------------------------------------------------
cSkinElchi::cSkinElchi(void)
:cSkin(OSDSKIN, &::Theme)//XXX naming problem???
{
}

const char *cSkinElchi::Description(void)
{
   return tr("Elchi");
}

cSkinDisplayChannel *cSkinElchi::DisplayChannel(bool WithInfo)
{
   return new cSkinElchiDisplayChannel(WithInfo);
}

cSkinDisplayMenu *cSkinElchi::DisplayMenu(void)
{
   return new cSkinElchiDisplayMenu;
}

cSkinDisplayReplay *cSkinElchi::DisplayReplay(bool ModeOnly)
{
   return new cSkinElchiDisplayReplay(ModeOnly);
}

cSkinDisplayVolume *cSkinElchi::DisplayVolume(void)
{
   return new cSkinElchiDisplayVolume;
}

cSkinDisplayTracks *cSkinElchi::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
   return new cSkinElchiDisplayTracks(Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cSkinElchi::DisplayMessage(void)
{
   return new cSkinElchiDisplayMessage;
}

// --- cPluginSkinElchi -----------------------------------------------------

class cPluginSkinElchi : public cPlugin {
private:

public:
   cPluginSkinElchi(void);
   virtual ~cPluginSkinElchi();
   virtual const char *Version(void) { return VERSION; }
   virtual const char *Description(void) { return tr(DESCRIPTION); }
   virtual const char *CommandLineHelp(void);
   virtual bool ProcessArgs(int argc, char *argv[]);
   virtual bool Initialize(void);
   virtual bool Start(void);
   virtual void Stop(void);
   virtual void Housekeeping(void);
   //virtual void MainThreadHook(void);
   //virtual cString Active(void);
   //virtual time_t WakeupTime(void);
   //virtual const char *MainMenuEntry(void) { return ((!ElchiConfig.hidemenu || strcasecmp(OSDSKIN, Setup.OSDSkin)) ? NULL : tr(DESCRIPTION)); }
   virtual const char *MainMenuEntry(void) { return NULL; }
   //virtual cOsdObject *MainMenuAction(void);
   virtual cMenuSetupPage *SetupMenu(void);
   virtual bool SetupParse(const char *Name, const char *Value);
   //bool Service(const char *Id, void *Data);
   //virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginSkinElchi::cPluginSkinElchi(void)
{
   // Initialize any member variables here.
   // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
   // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
   DSYSLOG("skinelchi: cPluginSkinElchi is called")
   ElchiStatus = NULL;
   VideoStatus = NULL;
}


bool cPluginSkinElchi::Initialize(void)
{
   // Initialize any background activities the plugin shall perform.
   return true;
}


bool cPluginSkinElchi::Start(void)
{
   // Start any background activities the plugin shall perform.
   //isyslog("skinelchi: cPluginSkinElchi::Start is called");

   // set default logo dir if not supplied by commandline arguments
   if (isempty(ElchiConfig.GetLogoBaseDir()))
      ElchiConfig.SetLogoBaseDir(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));

   isyslog("SkinElchi: using channel logo base dir '%s'", ElchiConfig.GetLogoBaseDir());
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   isyslog("SkinElchi: using EPG image dir '%s'", ElchiConfig.GetEpgImageDir());
#endif

   new cSkinElchi;

   ElchiStatus = new cSkinElchiStatus;

   VideoStatus = new cVideoStatus;

   return true;
}


void cPluginSkinElchi::Stop(void)
{
   // stop any background activities the plugin shall perform.
   //DSYSLOG("skinelchi: Stop() is called")
   //isyslog("skinelchi: Stop() is called");

   if (VideoStatus) VideoStatus->Stop();

   //isyslog("skinelchi: Stop() end");
}


cPluginSkinElchi::~cPluginSkinElchi()
{
   // Clean up after yourself!
   DSYSLOG("skinelchi: ~cPluginSkinElchi is called")
   if(ElchiStatus) {
      //isyslog("skinelchi: deleting ElchiStatus");
      DELETENULL(ElchiStatus);
   }

   if (VideoStatus) {
      //isyslog("skinelchi: deleting VideoStatus");
      DELETENULL(VideoStatus);
   }

   //isyslog("skinelchi: ~cPluginSkinElchi end");
}


void cPluginSkinElchi::Housekeeping(void)
{
   // Perform any cleanup or other regular tasks.
}

const char *cPluginSkinElchi::CommandLineHelp(void)
{
   // Return a string that describes all known command line options.
   DSYSLOG("skinelchi: cPluginSkinElchi::CommandLineHelp is called")
   return
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
          "  -c DIR,         --icache=DIR         optional path for epgimages\n"
          "                                       (default: '/video/epgimages')\n"
#endif
          "  -l PathToLogos, --logos=PathToLogos  optional path for XPM channel logos\n"
          "                                       (default: '<configDir>/skinelchi/logos')\n";
}

bool cPluginSkinElchi::ProcessArgs(int argc, char *argv[])
{
   // Implement command line argument processing here if applicable.
   static struct option long_options[] = {
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
       { "icache", required_argument, NULL, 'c' },
#endif
       { "logos", required_argument, NULL, 'l' },
       { NULL }
   };
   int c, option_index = 0;
   while ((c = getopt_long(argc, argv,
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
    "c:l:",
#else
    "l:",
#endif
       long_options, &option_index)) != -1) {
      switch (c) {
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
        case 'c': ElchiConfig.SetEpgImageDir(optarg);
                  DSYSLOG("skinelchi: PathToEpgImages '%s'", ElchiConfig.GetEpgImageDir())
                  break;
#endif
        case 'l': ElchiConfig.SetLogoBaseDir(optarg);
                  DSYSLOG("skinelchi: PathToLogoDirs '%s'", ElchiConfig.GetLogoBaseDir())
                  break;
        default:
                  ISYSLOG("skinelchi: unknown command-line argument: '%s'", optarg)
                  break;
      }
   }
   return true;
}

cMenuSetupPage *cPluginSkinElchi::SetupMenu(void)
{
   // Return a setup menu in case the plugin supports one.
   return new cSkinElchiSetup();
}

bool cPluginSkinElchi::SetupParse(const char *Name, const char *Value)
{
   // Parse your own setup parameters and store their values.
   return ElchiConfig.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cPluginSkinElchi); // Don't touch this!

