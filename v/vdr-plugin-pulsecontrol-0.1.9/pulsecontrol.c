/*
 * pulsecontrol.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <pulse/pulseaudio.h>

#include "action_getinfo.h"
#include "action_listcards.h"
#include "action_listdevices.h"
#include "action_listsinks.h"
#include "action_listsinkinputs.h"
#include "action_movesinkinput.h"
#include "action_setcardprofile.h"
#include "action_setdefaultsink.h"
#include "loop.h"
#include "menu.h"
#include "script.h"

static const char *VERSION        = "0.1.9";
static const char *DESCRIPTION    = trNOOP("control settings of pulseaudio");
static const char *MAINMENUENTRY  = "Pulsecontrol";


class cPluginPulsecontrol : public cPlugin {
private:
  // Add any member variables or functions you may need here.

public:
  cPluginPulsecontrol(void);
  virtual ~cPluginPulsecontrol();
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
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginPulsecontrol::cPluginPulsecontrol(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginPulsecontrol::~cPluginPulsecontrol()
{
  // Clean up after yourself!
}

const char *cPluginPulsecontrol::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginPulsecontrol::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginPulsecontrol::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginPulsecontrol::Start(void)
{
  // Start any background activities the plugin shall perform.
  cString filename = AddDirectory(cPlugin::ConfigDirectory(Name()), "startup.script");
  cPulseScript *startup = cPulseScript::FromFile(*filename);
  if (startup) {
     isyslog("pulsecontrol: executing startup.script");
     int ret = startup->Run();
     if (ret != 0)
        esyslog("pulsecontrol: startup script error %d", ret);
     delete startup;
     }
  return true;
}

void cPluginPulsecontrol::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginPulsecontrol::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginPulsecontrol::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginPulsecontrol::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginPulsecontrol::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginPulsecontrol::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cPulsecontrolMainMenu();
}

cMenuSetupPage *cPluginPulsecontrol::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginPulsecontrol::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginPulsecontrol::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginPulsecontrol::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "EXEC / execute filename\n"
    "    Execute a script, \"filename\" must be absolute.",
    "INFO\n"
    "    Prints some info about the pulseaudio server",
    "LCRD / list-cards\n"
    "    Lists the available cards",
    "LDEV / list-devices [type]\n"
    "    Lists the available devices",
    "LFOR / list-formats\n"
    "    Lists the available formats",
    "LSNK / list-sinks\n"
    "    Lists the available sinks",
    "LSKI / list-sink-inputs\n"
    "    Lists the available sink inputs",
    "MSKI / move-sink-input inputname_or_index sinkname_or_index\n"
    "    Moves the given input to the chosen sink\n"
    "    If the input is not given and there is only one active\n"
    "    sink input, this input will be moved.\n"
    "    The special sink name \"next\" selects the next sink"
    "    from the list returned by list-sinks.",
    "SCPR / set-card-profile cardname_or_index profilename\n"
    "    Set the active profile of the given card",
    "SDSK / set-default-sink sinkname_or_index\n"
    "    Set the given sink as default",
    "SSKF / set-sink-formats sinkname_or_index format1;format2;...\n"
    "    Set the passthrough formats of the given sink",
    NULL
  };
  return HelpPages;
}

cString cPluginPulsecontrol::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  cPulseLoop loop;
  cPulseScript *script = NULL;
  if ((strcasecmp(Command, "EXEC") == 0) || (strcasecmp(Command, "execute") == 0)) {
     if (!Option || !*Option) {
        ReplyCode = 501;
        return "missing filename of script";
        }
     cPulseScript *script = cPulseScript::FromFile(Option);
     if (!script) {
        ReplyCode = 550;
        return cString::sprintf("can't read script %s", Option);
        }
     int ret = script->Run();
     delete script;
     if (ret != 0) {
        ReplyCode = 550;
        return cString::sprintf("error %d", ret);
        }
     return cString::sprintf("executed script %s", Option);
     }
  if (strcasecmp(Command, "INFO") == 0) {
     cPulseGetInfoAction action(loop);
     int ret = loop.Run();
     if (ret != 0) {
        ReplyCode = 550;
        return cString::sprintf("error %d", ret);
        }
     return action.Info();
     }
  else if ((strcasecmp(Command, "LCRD") == 0) || (strcasecmp(Command, "list-cards") == 0)) {
     cPulseListCardsAction action(loop);
     int ret = loop.Run();
     if (ret != 0) {
        ReplyCode = 550;
        return cString::sprintf("error %d", ret);
        }
     return action.Info();
     }
  else if ((strcasecmp(Command, "LDEV") == 0) || (strcasecmp(Command, "list-devices") == 0)) {
     pa_device_type_t type = (pa_device_type_t)PA_INVALID_INDEX;
     if (Option && *Option) {
        if (strcasecmp(Option, "sink") == 0)
           type = PA_DEVICE_TYPE_SINK;
        else if (strcasecmp(Option, "source") == 0)
           type = PA_DEVICE_TYPE_SOURCE;
        }
     cPulseListDevicesAction action(loop, type);
     int ret = loop.Run();
     if (ret != 0) {
        ReplyCode = 550;
        return cString::sprintf("error %d", ret);
        }
     return action.Info();
     }
  else if ((strcasecmp(Command, "LFOR") == 0) || (strcasecmp(Command, "list-formats") == 0)) {
     cList<cPulseFormat> all;
     cPulseFormat::AllFormats(all);
     cString formats = "";
     for (const cPulseFormat *f = all.First(); f; f = all.Next(f))
         formats = cString::sprintf("%s%s\n", *formats, f->Name());
     return formats;
     }
  else if ((strcasecmp(Command, "LSNK") == 0) || (strcasecmp(Command, "list-sinks") == 0)) {
     cPulseGetInfoAction getinfo(loop);
     cPulseListSinksAction sinks(loop);
     int ret = loop.Run();
     if (ret != 0) {
        ReplyCode = 550;
        return cString::sprintf("error %d", ret);
        }
     const char *defsink = getinfo.DefaultSink();
     cString msg = "";
     for (const cPulseSink *s = sinks.Sinks().First(); s; s = sinks.Sinks().Next(s)) {
         msg = cString::sprintf("%s%ssink %d: %s\r\n", *msg, (strcmp(s->Name(), defsink)) ? "*" : " ", s->Index(), s->Name());
         }
     return msg;
     }
  else if ((strcasecmp(Command, "LSKI") == 0) || (strcasecmp(Command, "list-sink-inputs") == 0)) {
     cPulseListSinksAction sinks(loop);
     cPulseListSinkInputsAction inputs(loop);
     int ret = loop.Run();
     if (ret != 0) {
        ReplyCode = 550;
        return cString::sprintf("error %d", ret);
        }
    cString msg = "";
    for (const cPulseSinkInput *i = inputs.SinkInputs().First(); i; i = inputs.SinkInputs().Next(i)) {
        const cPulseSink *s = cListHelper<cPulseSink>::Find(sinks.Sinks(), i->Sink());
        msg = cString::sprintf("%ssink-input %d: %s -> %s\r\n", *msg, i->Index(), i->Name(), s ? s->Name() : "");
        }
     return msg;
     }
  else if ((strcasecmp(Command, "MSKI") == 0) || (strcasecmp(Command, "move-sink-input") == 0)) {
     script = cPulseScript::FromLine(cString::sprintf("%s%s", cPulseScript::cmdMoveSinkInput, Option));
     if (!script) {
        ReplyCode = 501;
        return "error in parameters";
        }
     }
  else if ((strcasecmp(Command, "SCPR") == 0) || (strcasecmp(Command, "set-card-profile") == 0)) {
     script = cPulseScript::FromLine(cString::sprintf("%s%s", cPulseScript::cmdSetCardProfile, Option));
     if (!script) {
        ReplyCode = 501;
        return "error in parameters";
        }
     }
  else if ((strcasecmp(Command, "SDSK") == 0) || (strcasecmp(Command, "set-default-sink") == 0)) {
     script = cPulseScript::FromLine(cString::sprintf("%s%s", cPulseScript::cmdSetDefaultSink, Option));
     if (!script) {
        ReplyCode = 501;
        return "error in parameters";
        }
     }
  else if ((strcasecmp(Command, "SSKF") == 0) || (strcasecmp(Command, "set-sink-formats") == 0)) {
     script = cPulseScript::FromLine(cString::sprintf("%s%s", cPulseScript::cmdSetSinkFormats, Option));
     if (!script) {
        ReplyCode = 501;
        return "error in parameters";
        }
     }

  if (script) {
     int ret = script->Run();
     delete script;
     if (ret != 0) {
         ReplyCode = 550;
         return cString::sprintf("error %d", ret);
         }
     return "Ok";
     }

  return NULL;
}

VDRPLUGINCREATOR(cPluginPulsecontrol); // Don't touch this!
