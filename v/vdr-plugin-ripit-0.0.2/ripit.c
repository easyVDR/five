/*
 * mcrip.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/interface.h>
#include <vdr/menu.h>
#include "setup.h"
#include "ripit.h"
#include "help.h"

cPluginRipit::cPluginRipit(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRipit::~cPluginRipit()
{
  // Clean up after yourself!
}


const char *cPluginRipit::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRipit::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRipit::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
} 


bool cPluginRipit::Start(void)
{
  // Start any background activities the plugin shall perform. 
  return true;
}


void cPluginRipit::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}


cString cPluginRipit::Active(void)
{
   // Return a message string if shutdown should be postponed
   if ( ripitosd->Rip_On() )
      return cString( tr( "ripit is running" ) );
      
   return NULL;
}

const char *cPluginRipit::MainMenuEntry(void)
{
  return RipitSetup.Ripit_hidden ? 0 : tr(MAINMENUENTRY);
}

cOsdObject *cPluginRipit::MainMenuAction(void)
{
  return new cRipitOsd;
}  


#if APIVERSNUM >= 10331
const char **cPluginRipit::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {

    "START\n"
    "	'START'  Start a new rip process",
    "ABORT\n"
    "	'ABORT'  Abort a running rip process",
    "STATUS\n"
    "	'STATUS' Shows status of ripit",
    NULL
    };
  return HelpPages;    
}

cString cPluginRipit::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if(!strcasecmp(Command,"START")) {
    if(ripitosd->Rip_On()) {
      ReplyCode=550;
      return "Another rip process is running, Can't start new one";
      }
    else {
      ripitosd->Start_Encode();
      return "New rip process started";
    }
  }
  else if(!strcasecmp(Command,"ABORT")) {
    if(!ripitosd->Rip_On()) {
      ReplyCode=550;
      return "Can't abort process because no one is running";
      }
    else {  
      ripitosd->Abort_Encode(1);
      return "Rip process aborted";
      }
    }
  else if(!strcasecmp(Command,"STATUS")) {
    if(ripitosd->Rip_On())
      return "A rip process is running";
    else
      return "No rip process is running";  
    }
  else {
    ReplyCode=502; return "Wrong command";
  }    
  return NULL;
}
#endif


cMenuSetupPage *cPluginRipit::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuRipitSetup;
} 

bool cPluginRipit::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(Name, "Ripit_hidden"))                   RipitSetup.Ripit_hidden     = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_lastlog"))             RipitSetup.Ripit_lastlog     = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_excludespecialchars")) RipitSetup.Ripit_excludespecialchars = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_underscores"))         RipitSetup.Ripit_underscores = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_playlist"))            RipitSetup.Ripit_playlist = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_verbosity"))           RipitSetup.Ripit_verbosity = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_fastrip"))             RipitSetup.Ripit_fastrip    = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_lowbitrate"))          RipitSetup.Ripit_lowbitrate = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_maxbitrate"))          RipitSetup.Ripit_maxbitrate = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_encoding_type"))       RipitSetup.Ripit_encoding_type     = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_oggquality"))          RipitSetup.Ripit_oggquality     = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_crc"))                 RipitSetup.Ripit_crc        = atoi(Value);
  else if (!strcasecmp(Name, "Ripit_ripopts"))             strn0cpy(RipitSetup.Ripit_ripopts, Value, sizeof(RipitSetup.Ripit_ripopts));
  else if (!strcasecmp(Name, "Ripit_encopts1"))            strn0cpy(RipitSetup.Ripit_encopts1, Value, sizeof(RipitSetup.Ripit_encopts1));
  else if (!strcasecmp(Name, "Ripit_encopts2"))            strn0cpy(RipitSetup.Ripit_encopts2, Value, sizeof(RipitSetup.Ripit_encopts2));
  else if (!strcasecmp(Name, "Ripit_encopts3"))            strn0cpy(RipitSetup.Ripit_encopts3, Value, sizeof(RipitSetup.Ripit_encopts3));
  else if (!strcasecmp(Name, "Ripit_encopts4"))            strn0cpy(RipitSetup.Ripit_encopts4, Value, sizeof(RipitSetup.Ripit_encopts4));
  else if (!strcasecmp(Name, "Ripit_encopts5"))            strn0cpy(RipitSetup.Ripit_encopts5, Value, sizeof(RipitSetup.Ripit_encopts5));
  else if (!strcasecmp(Name, "Ripit_dir"))                 strn0cpy(RipitSetup.Ripit_dir, Value, sizeof(RipitSetup.Ripit_dir));
  else if (!strcasecmp(Name, "Ripit_nice"))                RipitSetup.Ripit_nice       = atoi(Value);

  else return false;
    return true;
}

cMenuRipitSetup::cMenuRipitSetup(void)
{

  oldCurrent=-1;
	Setup();
/*  static const char allowed[] = { "abcdefghijklmnopqrstuvwxyz0123456789-_/" };
  int current = Current();

  Clear();

  Add(new cMenuEditBoolItem( tr("Shutdown after finished"),       &RipitSetup.Ripit_halt));
  Add(new cMenuEditBoolItem( tr("Verbose output"),                &RipitSetup.Ripit_noquiet));
  Add(new cMenuEditBoolItem( tr("Eject after finished"),          &RipitSetup.Ripit_eject));
  Add(new cMenuEditBoolItem( tr("Fast mode"),                      &RipitSetup.Ripit_fastrip));

  lowbitrate[0] =  "32";
  lowbitrate[1] =  "64";
  lowbitrate[2] =  "96";
  lowbitrate[3] = "112";
  lowbitrate[4] = "128";
  lowbitrate[5] = "160";
  lowbitrate[6] = "192";
  lowbitrate[7] = "224";
  lowbitrate[8] = "256";
  lowbitrate[9] = "320";
  Add(new cMenuEditStraItem( tr("Min. bitrate"),                         &RipitSetup.Ripit_lowbitrate, 10, lowbitrate));

  maxbitrate[0] =  "32";
  maxbitrate[1] =  "64";
  maxbitrate[2] =  "96";
  maxbitrate[3] = "112";
  maxbitrate[4] = "128";
  maxbitrate[5] = "160";
  maxbitrate[6] = "192";
  maxbitrate[7] = "224";
  maxbitrate[8] = "256";
  maxbitrate[9] = "320";
  Add(new cMenuEditStraItem( tr("Max. bitrate"),                         &RipitSetup.Ripit_maxbitrate, 10, maxbitrate));

  preset[0] = tr("none");
  preset[1] = tr("low");
  preset[2] = tr("standard");
  preset[3] = tr("great");
  preset[4] = tr("best");
  Add(new cMenuEditStraItem( tr("Presets"),                         &RipitSetup.Ripit_encoding_type, 5, preset));

  Add(new cMenuEditBoolItem( tr("CRC-check"),                             &RipitSetup.Ripit_crc));
  Add(new cMenuEditStrItem( tr("More encoder settings"),            RipitSetup.Ripit_encopts, 255, allowed));
  Add(new cMenuEditStrItem( tr("Device"),                           RipitSetup.Ripit_dev, 255, allowed));
  Add(new cMenuEditStrItem( tr("Directory for ripped tracks"),      RipitSetup.Ripit_dir, 255, allowed));
  Add(new cMenuEditIntItem( tr("Priority of task (nice)"),         &RipitSetup.Ripit_nice, -20, 19));

  Add(new cMenuEditBoolItem( tr("Encode remotely"),                &RipitSetup.Ripit_remote));
  if(RipitSetup.Ripit_remote)
    Add(new cMenuEditStrItem( tr("Options for remote encoding"),    RipitSetup.Ripit_remotevalue, 255, allowed));

  SetCurrent(Get(current));
  Display();
*/
} 

void cMenuRipitSetup::Setup(void)
{
  static const char allowedcmd[] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-\"" };
  int current = Current();

  Clear();
  help.Clear();

  Add(new cOsdItem(cString::sprintf("%s%s%s", "---- ",tr("Plugin Options")," ----"),osUnknown,false));
  help.Append("");
	Add(new cMenuEditBoolItem( tr("Hide mainmenu entry"),               &RipitSetup.Ripit_hidden));
  help.Append(tr(HELP_Ripit_hidden));
  Add(new cMenuEditIntItem( tr("Show last x lines of log"),               &RipitSetup.Ripit_lastlog, 1, 100));
  help.Append(tr(HELP_Ripit_lastlog));
  Add(new cMenuEditIntItem( tr("Priority of task (nice)"),            &RipitSetup.Ripit_nice, -20, 19));
  help.Append(tr(HELP_Ripit_nice));
  Add(new cMenuEditIntItem( tr("Verbosity"),                          &RipitSetup.Ripit_verbosity, 0, 5));
  help.Append(tr(HELP_Ripit_verbosity));
  if(RipitSetup.Ripit_encoding_type == 0 ){
    Add(new cOsdItem(cString::sprintf("%s%s%s", "---- ",tr("Rip script Options")," ----"),osUnknown,false));
    help.Append("");
  }
  else{
    Add(new cOsdItem(cString::sprintf("%s%s%s", "---- ",tr("Encoder Options")," ----"),osUnknown,false));
    help.Append("");
  }
  preset[0] = tr("Free Setting");
  preset[1] = tr("mp3 Constant bitrate (CBR)");
  preset[2] = tr("mp3 Average bitrate (ABR)");
  preset[3] = tr("mp3 Variable bitrate (VBR)");
  preset[4] = tr("flac (lossless)");
  preset[5] = tr("ogg vorbis");
  Add(new cMenuEditStraItem( tr("Encoding type"),                           &RipitSetup.Ripit_encoding_type, 6, preset));
  help.Append(tr(HELP_Ripit_encoding_type));

  if (RipitSetup.Ripit_encoding_type == 5){
    Add(new cMenuEditIntItem( tr("Quality"),                               &RipitSetup.Ripit_oggquality, -1, 10));
    help.Append(tr(HELP_Ripit_oggquality));
  }

  lowbitrate[0] =  "32";
  lowbitrate[1] =  "64";
  lowbitrate[2] =  "96";
  lowbitrate[3] = "112";
  lowbitrate[4] = "128";
  lowbitrate[5] = "160";
  lowbitrate[6] = "192";
  lowbitrate[7] = "224";
  lowbitrate[8] = "256";
  lowbitrate[9] = "320";
  if(RipitSetup.Ripit_encoding_type == 1 || RipitSetup.Ripit_encoding_type == 2 ){
    Add(new cMenuEditStraItem( tr("Bitrate"),                      &RipitSetup.Ripit_lowbitrate, 10, lowbitrate));
    help.Append(tr(HELP_Ripit_lowbitrate_mp3_CBR_ABR));
  }
  else{
    if (RipitSetup.Ripit_encoding_type == 3){
      Add(new cMenuEditStraItem( tr("Min. bitrate"),                      &RipitSetup.Ripit_lowbitrate, 10, lowbitrate));
      help.Append(tr(HELP_Ripit_lowbitrate_mp3_VBR));
    }
  }

  if(RipitSetup.Ripit_encoding_type == 3 ) {
    maxbitrate[0] =  "32";
    maxbitrate[1] =  "64";
    maxbitrate[2] =  "96";
    maxbitrate[3] = "112";
    maxbitrate[4] = "128";
    maxbitrate[5] = "160";
    maxbitrate[6] = "192";
    maxbitrate[7] = "224";
    maxbitrate[8] = "256";
    maxbitrate[9] = "320";
    Add(new cMenuEditStraItem( tr("Max. bitrate"),                      &RipitSetup.Ripit_maxbitrate, 10, maxbitrate));
    help.Append(tr(HELP_Ripit_maxbitrate_mp3_VBR));
  }
  if(RipitSetup.Ripit_encoding_type == 0 ) {
    Add(new cMenuEditStrItem( tr("Rip script extra Options"),              RipitSetup.Ripit_ripopts, 255, allowedcmd));
    help.Append(tr(HELP_Ripit_ripopts));
  }
  else if (RipitSetup.Ripit_encoding_type == 1 ) {
    Add(new cMenuEditStrItem( tr("Extra encoder options"),              RipitSetup.Ripit_encopts1, 255, allowedcmd));
    help.Append(tr(HELP_Ripit_encopts1));
    Add(new cMenuEditBoolItem( tr("Add CRC-code in each frame"),                         &RipitSetup.Ripit_crc));
    help.Append(tr(HELP_Ripit_crc));
  }
  else if (RipitSetup.Ripit_encoding_type == 2 ) {
    Add(new cMenuEditStrItem( tr("Extra encoder options"),              RipitSetup.Ripit_encopts2, 255, allowedcmd));
    help.Append(tr(HELP_Ripit_encopts2));
    Add(new cMenuEditBoolItem( tr("Add CRC-code in each frame"),                         &RipitSetup.Ripit_crc));
    help.Append(tr(HELP_Ripit_crc));
  }
  else if (RipitSetup.Ripit_encoding_type == 3 ) {
    Add(new cMenuEditStrItem( tr("Extra encoder options"),              RipitSetup.Ripit_encopts3, 255, allowedcmd));
    help.Append(tr(HELP_Ripit_encopts3));
    Add(new cMenuEditBoolItem( tr("Add CRC-code in each frame"),                         &RipitSetup.Ripit_crc));
    help.Append(tr(HELP_Ripit_crc));
  }
  else if (RipitSetup.Ripit_encoding_type == 4 ) {
    Add(new cMenuEditStrItem( tr("Extra encoder options"),              RipitSetup.Ripit_encopts4, 255, allowedcmd));
    help.Append(tr(HELP_Ripit_encopts4));
  }
  else if (RipitSetup.Ripit_encoding_type == 5 ) {
    Add(new cMenuEditStrItem( tr("Extra encoder options"),              RipitSetup.Ripit_encopts5, 255, allowedcmd));
    help.Append(tr(HELP_Ripit_encopts5));
  }
  Add(new cOsdItem(cString::sprintf("%s%s%s", "---- ",tr("Ripper Options")," ----"),osUnknown,false));
  help.Append("");
  Add(new cMenuEditBoolItem( tr("Fast ripping"),                         &RipitSetup.Ripit_fastrip));
  help.Append(tr(HELP_Ripit_fastrip));
  Add(new cOsdItem(cString::sprintf("%s%s%s", "---- ",tr("Target Options")," ----"),osUnknown,false));
  help.Append("");
  Add(new cMenuEditStrItem( tr("Directory for ripped tracks"),        RipitSetup.Ripit_dir, 255, NULL)); //NULL = VDR use tr(FileNameChars)
  help.Append(tr(HELP_Ripit_dir));
  excludespecialchars[0] = trVDR("no");
  excludespecialchars[1] = ("NTFS");
  excludespecialchars[2] = ("HFS");
  Add(new cMenuEditStraItem( tr("Exclude special Filesystem chars"),            &RipitSetup.Ripit_excludespecialchars, 3, excludespecialchars));
  help.Append(tr(HELP_Ripit_excludespecialchars));
  Add(new cMenuEditBoolItem( tr("Use underscores instead of spaces"), &RipitSetup.Ripit_underscores));
  help.Append(tr(HELP_Ripit_underscores));
  Add(new cMenuEditBoolItem( tr("Create playlist"),               &RipitSetup.Ripit_playlist));
  help.Append(tr(HELP_Ripit_playlist));


  SetCurrent(Get(current));


  // Common are:
  // Red: <Save&Exit (Yacoto)><About (ripit)>
  // Green: <New>|<Save (Yacoto)>
  // Yellow: <Help>
  // Blue: <open Submenu>
  // kInfo: <Help>
  if (Current()>-1)
    if (strlen(tr(HELP_ABOUT)) > 0)
      if (strlen(help[Current()])>0)
        SetHelp( tr("About"), NULL, tr("Help"), NULL);
      else
        SetHelp( tr("About"), NULL, NULL, NULL);
    else
      if (strlen(help[Current()])>0)
        SetHelp( NULL, NULL, tr("Help"), NULL);
      else
        SetHelp( NULL, NULL, NULL, NULL);
  else
    if (strlen(tr(HELP_ABOUT)) > 0)
      if (strlen(help[1])>0)
        SetHelp( tr("About"), NULL, tr("Help"), NULL);
      else
        SetHelp( tr("About"), NULL, NULL, NULL);
    else
      if (strlen(help[1])>0)
        SetHelp( NULL, NULL, tr("Help"), NULL);
      else
        SetHelp( NULL, NULL, NULL, NULL);

  Display();

}


void cMenuRipitSetup::Store(void)
{
  SetupStore("Ripit_hidden",              RipitSetup.Ripit_hidden);
  SetupStore("Ripit_lastlog",             RipitSetup.Ripit_lastlog);
  SetupStore("Ripit_excludespecialchars", RipitSetup.Ripit_excludespecialchars);
  SetupStore("Ripit_underscores",         RipitSetup.Ripit_underscores);
  SetupStore("Ripit_playlist",            RipitSetup.Ripit_playlist);
  SetupStore("Ripit_verbosity",           RipitSetup.Ripit_verbosity);
  SetupStore("Ripit_fastrip",             RipitSetup.Ripit_fastrip);
  SetupStore("Ripit_lowbitrate",          RipitSetup.Ripit_lowbitrate);
  SetupStore("Ripit_maxbitrate",          RipitSetup.Ripit_maxbitrate);
  SetupStore("Ripit_crc",                 RipitSetup.Ripit_crc);
  SetupStore("Ripit_encoding_type",       RipitSetup.Ripit_encoding_type);
  SetupStore("Ripit_oggquality",          RipitSetup.Ripit_oggquality);
  SetupStore("Ripit_ripopts",             RipitSetup.Ripit_ripopts);
  SetupStore("Ripit_encopts1",            RipitSetup.Ripit_encopts1);
  SetupStore("Ripit_encopts2",            RipitSetup.Ripit_encopts2);
  SetupStore("Ripit_encopts3",            RipitSetup.Ripit_encopts3);
  SetupStore("Ripit_encopts4",            RipitSetup.Ripit_encopts4);
  SetupStore("Ripit_encopts5",            RipitSetup.Ripit_encopts5);
  SetupStore("Ripit_dir",                 RipitSetup.Ripit_dir);
  SetupStore("Ripit_nice",                RipitSetup.Ripit_nice);
} 


eOSState cMenuRipitSetup::ProcessKey(eKeys Key)
{

  int oldvalue = RipitSetup.Ripit_encoding_type;

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (oldCurrent != Current()) {
    oldCurrent = Current();
    if (strlen(tr(HELP_ABOUT)) > 0)
      if (strlen(help[Current()])>0)
        SetHelp( tr("About"), NULL, tr("Help"), NULL);
      else
        SetHelp( tr("About"), NULL, NULL, NULL);
    else
      if (strlen(help[Current()])>0)
        SetHelp( NULL, NULL, tr("Help"), NULL);
      else
        SetHelp( NULL, NULL, NULL, NULL);
  }

  if( (Key != kNone) && (RipitSetup.Ripit_encoding_type != oldvalue) ) {
    Setup();
  }

  if ((Key == kInfo || Key == kYellow ) && (state == osUnknown) && (strlen(help[Current()])>0))
     return AddSubMenu(new cMenuText(cString::sprintf("%s - %s '%s'", tr("Help"), trVDR("Plugin"), PLUGIN_NAME_I18N), help[Current()]));

  if (strlen(tr(HELP_ABOUT)) > 0)
    if ((Key == kRed) && (state == osUnknown))
      return AddSubMenu(new cMenuText(cString::sprintf("%s - %s '%s'", tr("About"), trVDR("Plugin"), PLUGIN_NAME_I18N), tr(HELP_ABOUT) ));

  return state;
}      



VDRPLUGINCREATOR(cPluginRipit); // Don't touch this!
