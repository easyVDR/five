/*
 * dm140vfd plugin for VDR (C++)
 *
 * (C) 2010 Andreas Brachold <vdr07 AT deltab de>
 *
 * This dm140vfd plugin is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 3 of the License.
 *
 * See the files README and COPYING for details.
 *
 */

#include <vdr/plugin.h>
#include <getopt.h>
#include <string.h>

#include "dm140vfd.h"
#include "vfd.h"
#include "watch.h"
#include "status.h"
#include "setup.h"

static const char *VERSION        = "0.0.6";

cPluginDm140VFD::cPluginDm140VFD(void)
{
  m_bSuspend = true;
  statusMonitor = NULL;
  m_szIconHelpPage = NULL;
}

cPluginDm140VFD::~cPluginDm140VFD()
{
  //Cleanup if'nt throw housekeeping
  if(statusMonitor) {
    delete statusMonitor;
    statusMonitor = NULL;
  }

  if(m_szIconHelpPage) {
    free(m_szIconHelpPage);
    m_szIconHelpPage = NULL;
  }
}

const char* cPluginDm140VFD::Version(void) { 
  return VERSION; 
}

const char* cPluginDm140VFD::Description(void) { 
  return tr("Control a dm140 vfd"); 
}

const char *cPluginDm140VFD::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "";
}

bool cPluginDm140VFD::ProcessArgs(int argc, char *argv[])
{
  return true;
}

bool cPluginDm140VFD::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginDm140VFD::resume() {

  if(m_bSuspend
      && m_dev.open()) {
        m_bSuspend = false;
      return true;
  }
  return false;
}

bool cPluginDm140VFD::suspend() {
  if(!m_bSuspend) {
    m_dev.shutdown(eOnExitMode_BLANKSCREEN);
    m_bSuspend = true;
    return true;
  }
  return false;
}

bool cPluginDm140VFD::Start(void)
{
  if(resume()) {
      statusMonitor = new cVFDStatusMonitor(&m_dev);
      if(NULL == statusMonitor){
        esyslog("DM140VFD: can't create cVFDStatusMonitor!");
        return (false);
      }
  }
  return true;
}

void cPluginDm140VFD::Stop(void)
{
  if(statusMonitor) {
    delete statusMonitor;
    statusMonitor = NULL;
  }
  m_dev.shutdown(theSetup.m_nOnExit);
}

void cPluginDm140VFD::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginDm140VFD::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginDm140VFD::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginDm140VFD::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginDm140VFD::MainMenuAction(void)
{
  return NULL;
}

cMenuSetupPage *cPluginDm140VFD::SetupMenu(void)
{
  return new cVFDMenuSetup(&m_dev);
}

bool cPluginDm140VFD::SetupParse(const char *szName, const char *szValue)
{
  return theSetup.SetupParse(szName,szValue);
}

const char* cPluginDm140VFD::SVDRPCommandOn(const char *Option, int &ReplyCode)
{
    if(!m_bSuspend) {
      ReplyCode=251; 
      return "driver already resumed";
    }
    if(resume()) {
      ReplyCode=250; 
      return "driver resumed";
    } else {
      ReplyCode=554; 
      return "driver could not resumed";
    }
}

const char* cPluginDm140VFD::SVDRPCommandOff(const char *Option, int &ReplyCode)
{
    if(suspend()) {
      ReplyCode=250; 
      return "driver suspended";
    } else {
      ReplyCode=251; 
      return "driver already suspended";
    }
}

static const struct  {
    unsigned int nIcon;
    const char* szIcon;    
} icontable[] = {
    { eIconPLAY  , "Play" },
    { eIconPAUSE  , "Pause" },
    { eIconRECORD  , "Record" },
    { eIconMESSAGE  , "Msg" },
    { eIconMSGAT  , "MsgAt" },
    { eIconMUTE  , "Mute" },
    { eIconWLAN1  , "WLAN1" },
    { eIconWLAN2  , "WLAN2" },
    { eIconWLAN3  , "WLAN3" },
    { eIconWLAN4  , "WLAN4" },
    { eIconVOLUME  , "Volume" },
    { eIconVOL1  , "Level1" },
    { eIconVOL2  , "Level2" },
    { eIconVOL3  , "Level3" },
    { eIconVOL4  , "Level4" },
    { eIconVOL5  , "Level5" },
    { eIconVOL6  , "Level6" },
    { eIconVOL7  , "Level7" },
    { eIconVOL8  , "Level8" },
    { eIconVOL9  ,  "Level9" },
    { eIconVOL10  , "Level10" },
    { eIconVOL11  , "Level11" },
    { eIconVOL12  , "Level12" }
//    { eIconVOL12  , "Level12" },
//    { eIconVOL13  , "Level13" },
//    { eIconVOL14  , "Level14" }
};

const char* cPluginDm140VFD::SVDRPCommandIcon(const char *Option, int &ReplyCode)
{
  if(m_bSuspend) {
      ReplyCode=251; 
      return "driver suspended";
  }
  if( Option && strlen(Option) < 256) {
    char* request = strdup(Option);
    char* cmd = NULL;
    if( request ) {
      char* tmp = request;
      eIconState m = eIconStateQuery;
      cmd = strtok_r(request, " ", &tmp);
      if(cmd) {
        char* param = strtok_r(0, " ", &tmp);
        if(param) {
          if(!strcasecmp(param,"ON")) {
            m = eIconStateOn;
          } else if(!strcasecmp(param,"OFF")) {
            m = eIconStateOff;
          } else if(!strcasecmp(param,"AUTO")) {
            m = eIconStateAuto;
          } else {
            ReplyCode=501; 
            free(request);
            return "unknown icon state";
          }
        }
      }

      ReplyCode=501; 
      const char* szReplay = "unknown icon title";
      if(cmd) {
        unsigned int i;
        for(i = 0; i < (sizeof(icontable)/sizeof(*icontable));++i)
        {
            if(0 == strcasecmp(cmd,icontable[i].szIcon))    
            {
                eIconState r = m_dev.ForceIcon(icontable[i].nIcon, m);
                switch(r) {
                  case eIconStateAuto:
                    ReplyCode=250; 
                    szReplay = "icon state 'auto'";
                    break;
                  case eIconStateOn:
                    ReplyCode=251; 
                    szReplay = "icon state 'on'";
                    break;
                  case eIconStateOff:
                    ReplyCode=252; 
                    szReplay = "icon state 'off'";
                    break;
                  default:
                    break;
                }
                break;
            }
        }
      }
      free(request);
      return szReplay;
    }
  }
  ReplyCode=501; 
  return "wrong parameter";
}

cString cPluginDm140VFD::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  ReplyCode=501; 
  const char* szReplay = "unknown command";

  if(!strcasecmp(Command, "ON")) {
    szReplay = SVDRPCommandOn(Option,ReplyCode);
  } else if(!strcasecmp(Command, "OFF")) {
    szReplay = SVDRPCommandOff(Option,ReplyCode);
  } else if(!strcasecmp(Command, "ICON")) {
    szReplay = SVDRPCommandIcon(Option,ReplyCode);
  } 

  dsyslog("DM140VFD:  SVDRP %s %s - %d (%s)", Command, Option, ReplyCode, szReplay);
  return szReplay;
}

const char **cPluginDm140VFD::SVDRPHelpPages(void)
{
  if(!m_szIconHelpPage) {
    unsigned int i,l,k;
    for(i = 0,l = 0, k = (sizeof(icontable)/sizeof(*icontable)); i < k;++i) {
      l += strlen(icontable[i].szIcon);
      l += 2;
    }
    l += 80;

    m_szIconHelpPage = (char*) calloc(l + 1,1);
    if(m_szIconHelpPage) {
      strncat(m_szIconHelpPage, "ICON [name] [on|off|auto]\n    Force state of icon. Names of icons are:", l);
      for(i = 0; i < k;++i) {
        if((i % 7) == 0) {
          strncat(m_szIconHelpPage, "\n    ", l - (strlen(m_szIconHelpPage) + 5));
        } else {
          strncat(m_szIconHelpPage, ",", l - (strlen(m_szIconHelpPage) + 1));
        }
        strncat(m_szIconHelpPage, icontable[i].szIcon, 
          l - (strlen(m_szIconHelpPage) + strlen(icontable[i].szIcon)));
      }
    }
  }

  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "ON\n"
    "    Resume driver of display.\n",
    "OFF\n"
    "    Suspend driver of display.\n",
    "ICON [name] [on|off|auto]\n"
    "    Force state of icon.\n",
    NULL
    };
  if(m_szIconHelpPage)
    HelpPages[2] = m_szIconHelpPage;
  return HelpPages;
}
VDRPLUGINCREATOR(cPluginDm140VFD); // Don't touch this!
