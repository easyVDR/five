/*
 * avards.c: A VDR plugin to adapt the WSS signal to the actual picture size
 *
 * Copyright (C) 2007 - 2011 Christoph Haubrich
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
#include <string>

#include <vdr/tools.h>

#include "avards.h"
#if APIVERSNUM < 10507
#include "i18n.h"
#define trNOOP(s) (s)
#endif
#include "setup.h"
#include "detector.h"
#include "avards_services.h"


static const char *VERSION        = "0.2.5";
static const char *DESCRIPTION    = "Automatic Video Aspect Ratio Detection and Signaling";

#define DEFAULT_VIDEO_DEVICE "/dev/video0"
#define DEFAULT_DVB_DEVICE  "/dev/dvb/adapter0"
#define DEFAULT_VBI_DEVICE "/dev/vbi0"

#define DEFAULT_MAX_LOGO_WIDTH_PCT 20
#define DEFAULT_PAN_TONE_TOLERANCE 8
#define DEFAULT_PAN_TONE_BLACK_MAX 0
#define DEFAULT_OVERSCAN_PCT 3
#define DEFAULT_POLL_RATE_MS 80
#define DEFAULT_DELAY_CNT 5

cParameter config;

class cPluginAvards : public cPlugin {
private:
   // Add any member variables or functions you may need here.
   cDetector *detector;
   std::string m_mainMenuEntry;
   cString mainmenuentry;
   bool quirkmode;
   bool grabfromdevice;
public:
   cPluginAvards(void);
   virtual ~cPluginAvards();
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
   virtual const char *MainMenuEntry(void);
   virtual cOsdObject *MainMenuAction(void);
   virtual cMenuSetupPage *SetupMenu(void);
   virtual bool SetupParse(const char *Name, const char *Value);
   virtual bool Service(const char *Id, void *Data = NULL);
   virtual const char **SVDRPHelpPages(void);
   virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginAvards::cPluginAvards(void)
{
   // Initialize any member variables here.
   // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
   // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
   config.pszDevVideo         = (char *)DEFAULT_VIDEO_DEVICE;
   config.pszDevDvbAdapter    = (char *)DEFAULT_DVB_DEVICE;
   config.pszDevVbi           = (char *)DEFAULT_VBI_DEVICE;
   config.ShowInMainMenu      = true;
   config.AutoStart           = false;
   config.Mode                = MODE_AUTO;
   config.Overzoom            = false;
   config.PanToneTolerance    = DEFAULT_PAN_TONE_TOLERANCE;
   config.PanToneBlackMax     = DEFAULT_PAN_TONE_BLACK_MAX;
   config.Overscan_pct        = DEFAULT_OVERSCAN_PCT;
   config.ShowAllLines        = false;
   config.MaxLogoWidth_pct    = DEFAULT_MAX_LOGO_WIDTH_PCT;
   config.PollRate            = DEFAULT_POLL_RATE_MS;
   config.Delay               = DEFAULT_DELAY_CNT;
   config.ShowMsg             = false;
   config.VDROSDisPAL         = true;

   detector = NULL;
   mainmenuentry = NULL;
   quirkmode = false;
   grabfromdevice = true;
}

cPluginAvards::~cPluginAvards()
{
   // Clean up after yourself!
   delete detector;
   detector = NULL;
}

const char *cPluginAvards::CommandLineHelp(void)
{
   // Return a string that describes all known command line options.
   return "  -v, --dev_video=<Device>  Video device\n"
          "                            (default is " DEFAULT_VIDEO_DEVICE ")\n"
          "  -d, --dev_dvb=<Device>    DVB device\n"
          "                            (default is " DEFAULT_DVB_DEVICE ")\n"
          "  -b, --dev_vbi=<Device>    VBI device\n"
          "                            (default is " DEFAULT_VBI_DEVICE ")\n"
          "  -q, --quirk-mode          use 0x0e as default WSS code\n"
          "  -g, --grab                grab pictures from VDR instead directly from device\n";
}


bool cPluginAvards::ProcessArgs(int argc, char *argv[])
{
   // Implement command line argument processing here if applicable.
   static struct option long_options[] = {
      { "dev_video", required_argument, 0, 'v' },
      { "dev_dvb",   required_argument, 0, 'd' },
      { "dev_vbi",   required_argument, 0, 'b' },
      { "quirk-mode",no_argument,       0, 'q' },
      { "grab",      no_argument,       0, 'g' },
      { NULL }
   };

   int c;
   while ((c = getopt_long(argc, argv, "v:d:b:q:g", long_options, NULL)) != -1) {
      switch (c) {
         case 'v': config.pszDevVideo = optarg;
                  break;
         case 'd': config.pszDevDvbAdapter = optarg;
                  break;
         case 'b': config.pszDevVbi = optarg;
                  break;
         case 'q': quirkmode = true;
                  break;
         case 'g': grabfromdevice= false;
                  break;
         default:  return false;
      }
   }

   return true;
}


bool cPluginAvards::Initialize(void)
{
   // Initialize any background activities the plugin shall perform.
   detector = new cDetector(quirkmode, grabfromdevice);
   return detector != NULL;
}

bool cPluginAvards::Start(void)
{
   // Start any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
   RegisterI18n(Phrases);
#endif

   bool result = true;
   if (config.AutoStart)
      result = detector->StartUp();
   return result;
}

void cPluginAvards::Stop(void)
{
   // Stop any background activities the plugin shall perform.
   detector->Stop();
}

void cPluginAvards::Housekeeping(void)
{
   // Perform any cleanup or other regular tasks.
}

void cPluginAvards::MainThreadHook(void)
{
   // Perform actions in the context of the main program thread.
   // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginAvards::Active(void)
{
   // Return a message string if shutdown should be postponed
   return NULL;
}

time_t cPluginAvards::WakeupTime(void)
{
   // Return custom wakeup time for shutdown script
   return 0;
}

const char *cPluginAvards::MainMenuEntry(void)
{
   if (config.ShowInMainMenu) {

      const char *modes[MAX_MODES];
      modes[0] = tr("auto");
      modes[1] = tr("force 4:3");
      modes[2] = tr("force L14:9");
      modes[3] = tr("force L16:9");
      modes[4] = tr("force L>16:9");
      modes[5] = tr("force 16:9");

      if (detector->Active())
         mainmenuentry = cString::sprintf("%s (%s: %s)", tr("Avards stop"), tr("mode"), detector->GetWSSModeString());
      else
         mainmenuentry = cString::sprintf("%s (%s: %s)", tr("Avards start"), tr("detection"), modes[config.Mode]);

      return (const char *)mainmenuentry;
   }
   return NULL;
}

cOsdObject *cPluginAvards::MainMenuAction(void)
{
   // Perform the action when selected from the main VDR menu.
   if (detector->Active())
      detector->Stop();
   else
      detector->StartUp();

   return NULL;
}

cMenuSetupPage *cPluginAvards::SetupMenu(void)
{
   // Return a setup menu in case the plugin supports one.
   return new cMenuSetupAvards();
}

bool cPluginAvards::SetupParse(const char *Name, const char *Value)
{
   // Parse your own setup parameters and store their values.
   if      (!strcasecmp(Name, "ShowInMainMenu"))        config.ShowInMainMenu = atoi(Value);
   else if (!strcasecmp(Name, "AutoStart"))             config.AutoStart = atoi(Value);
   else if (!strcasecmp(Name, "Mode"))                  config.Mode = atoi(Value);
   else if (!strcasecmp(Name, "Overzoom"))              config.Overzoom = atoi(Value);
   else if (!strcasecmp(Name, "PanToneTolerance"))      config.PanToneTolerance = atoi(Value);
   else if (!strcasecmp(Name, "PanToneBlackMax"))       config.PanToneBlackMax = atoi(Value);
   else if (!strcasecmp(Name, "Overscan"))              config.Overscan_pct = atoi(Value);
   else if (!strcasecmp(Name, "ShowAllLines"))          config.ShowAllLines = atoi(Value);
   else if (!strcasecmp(Name, "LogoWidth"))             config.MaxLogoWidth_pct = atoi(Value);
   else if (!strcasecmp(Name, "PollRate"))              config.PollRate = atoi(Value);
   else if (!strcasecmp(Name, "Delay"))                 config.Delay = atoi(Value);
   else if (!strcasecmp(Name, "ShowMsg"))               config.ShowMsg = atoi(Value);
   else if (!strcasecmp(Name, "VDROSDisPAL"))           config.VDROSDisPAL = atoi(Value);
   else
      return false;

   return true;
}

bool cPluginAvards::Service(const char *Id, void *Data)
{
   // Handle custom service requests from other plugins
   #if APIVERSNUM < 10504
   if (strcmp(Id, AVARDS_MAXOSDSIZE_SERVICE_STRING_ID) == 0) {
      if (Data == NULL)
         return true;
      detector->GetMaxOSDSize((avards_MaxOSDsize_v1_0*)Data);
      return true;
   }
   #endif

   if (strcmp(Id, AVARDS_CURRENT_WSSMODE_SERVICE_STRING_ID) == 0) {
      if (Data == NULL)
         return true;
      ((avards_CurrentWssMode_v1_0*)Data)->ModeString = detector->GetWSSModeString();
      return true;
   }

   if (strcmp(Id, AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID) == 0) {
      if (Data == NULL)
         return true;
      detector->GetVideoFormat((avards_CurrentVideoFormat_v1_0*)Data);
      return true;
   }

   return false;
}

const char **cPluginAvards::SVDRPHelpPages(void)
{
   // Return help text for SVDRP commands this plugin implements
   static const char *HelpPages[] = {
      "START\n"
      "    Start AVARDS detection and WSS Signaling",
      "STOP\n"
      "    Stop AVARDS detection and WSS Signaling",
      "STATUS [MODE]\n"
      "    Get current status of AVARDS\n"
      "    If the optional parameter MODE is given it will also show the actual WSS\n"
      "    mode which is one of \"4:3\", \"L14:9\", \"16:9\", \"L16:9\" and \"L>16:9\"",
      NULL
   };
   return HelpPages;
}

cString cPluginAvards::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
   // Process SVDRP commands this plugin implements
   if (strcasecmp(Command, "START") == 0) {
      if (detector->Active()) {
         ReplyCode = 910;
         return "Avards already running";
      } else {
         detector->Start();
         ReplyCode = 900;
         return "Avards started";
      }
   }
   else if (strcasecmp(Command, "STOP") == 0) {
      if (detector->Active()) {
         detector->Stop();
         ReplyCode = 900;
         return "Avards stopped";
      } else {
         ReplyCode = 910;
         return "Avards was not running";
      }
   }
   else if (strcasecmp(Command, "STATUS") == 0) {
      bool mode = false;
      if (*Option) {
         mode = strcasecmp(Option, "MODE") == 0;
         if (!mode) {
         ReplyCode = 504;
         return cString::sprintf("Unknown option: \"%s\"", Option);
         }
      }

      if (detector->Active()) {
         ReplyCode = 901;
         if (mode) {
         return cString::sprintf("Avards is active with mode \"%s\"", detector->GetWSSModeString());
         } else
         return "Avards is active";
      } else {
         ReplyCode = 902;
         return "Avards is not active";
      }
   }

   return NULL;
}

VDRPLUGINCREATOR(cPluginAvards); // Don't touch this!
