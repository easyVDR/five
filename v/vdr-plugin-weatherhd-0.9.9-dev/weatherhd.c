/*
 * wetter.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: wetter.c,v 1.3 2005/01/23 00:56:49 beejay Exp $
 */

#include <getopt.h>
#include <string>

#include <vdr/plugin.h>

#include "setup.h"
#include "OsdWeather.h"
#include "skin.h"
#include "vars.h"



static const char *VERSION        = "0.9.9-dev1";
static const char *DESCRIPTION    = trNOOP("Accuweather.com: Weather forecast");
static const char *MAINMENUENTRY  = trNOOP("Weather-HD");

class cPluginWetter : public cPlugin {
private:

 // Add any member variables or functions you may need here.
public:
  cPluginWetter(void);
  virtual ~cPluginWetter();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

class cMenuWetterSetup : public cMenuSetupPage {
 protected:
   virtual void Store(void);
 public:
   cMenuWetterSetup(void);
};


cPluginWetter::cPluginWetter(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginWetter::~cPluginWetter()
{
    
  // Clean up after yourself!
}

const char *cPluginWetter::CommandLineHelp(void)
{
  return  "  -C DIR,   --config=DIR      set VDRs plugin directory to DIR\n";
}

bool cPluginWetter::ProcessArgs(int argc, char *argv[]) {
  static struct option long_options[] = {
       { "config",	required_argument, NULL, 'C' },
       { NULL }
     };
     
  int c, option_index = 0;
  while ((c = getopt_long(argc, argv, "C", long_options, &option_index)) != -1) {
          switch (c) {
	  case 'C': config_path    = optarg; break;
	  default: return false;
	  }		    
	}
  return true;
}

bool cPluginWetter::Initialize(void)
{
  if (config_path == NULL) {
          config_path = strdup(ConfigDirectory());	         
          esyslog("%s: Parameter '-C' not set.\n"
	          " Set default value : %s\n", PLUGIN_NAME_I18N, config_path);
  }


  if(!WeatherSkin.ParseSkin("current.skin")) {
    esyslog("weather: WARNING: no themefile found ! Using default colors");
    printf("weather: no themefile found !\n");
    }
  else isyslog("weather: Loading themefile\n");    
  
  return true;
}

bool cPluginWetter::Start(void)
{
  if ( wetterSetup.w_update == true ) {
    FILE *cmd;
    char *buffer;

    asprintf(&buffer, "%s%s '%s' '%s' '%s'",config_path,"/moronsuite/weather/scripts/weather.sh", wetterSetup.w_id1, wetterSetup.w_id2, wetterSetup.w_id3);	
    cmd = popen(buffer, "r");
    dsyslog("DEBUG : %s: Executing as autoupdate: '%s'", PLUGIN_NAME_I18N, buffer);
    free(buffer);
    pclose(cmd);
  }	
  return true;
}

void cPluginWetter::Housekeeping(void)
{
}

cOsdObject *cPluginWetter::MainMenuAction(void)
{
  return new cWetterOsd();
}

cMenuSetupPage *cPluginWetter::SetupMenu(void)
{
    return new cMenuWetterSetup;
}

bool cPluginWetter::SetupParse(const char *Name, const char *Value)
 {
  if      (!strcasecmp(Name, "AutoUpdate"))     wetterSetup.w_update             = atoi(Value);
  else if (!strcasecmp(Name, "RadarLeft"))      wetterSetup.w_left               = atoi(Value);
  else if (!strcasecmp(Name, "RadarTop"))       wetterSetup.w_top                = atoi(Value);
  else if (!strcasecmp(Name, "RadarWidth"))     wetterSetup.w_width              = atoi(Value);
  else if (!strcasecmp(Name, "RadarHeight"))    wetterSetup.w_height             = atoi(Value);
  else if (!strcasecmp(Name, "OSDOffsetX"))     wetterSetup.w_osdoffset_x        = atoi(Value);
  else if (!strcasecmp(Name, "OSDOffsetY"))     wetterSetup.w_osdoffset_y        = atoi(Value);
  else if (!strcasecmp(Name, "ImgDither"))      wetterSetup.w_dither             = atoi(Value);
  else if (!strcasecmp(Name, "ImgTreeDepth"))   wetterSetup.w_treedepth          = atoi(Value);

  else if (!strcasecmp(Name, "DayForecast"))    wetterSetup.w_dayf               = atoi(Value);
  else if (!strcasecmp(Name, "ID1"))            strn0cpy(wetterSetup.w_id1,      Value, sizeof(wetterSetup.w_id1));
  else if (!strcasecmp(Name, "ID2"))            strn0cpy(wetterSetup.w_id2,      Value, sizeof(wetterSetup.w_id2));
  else if (!strcasecmp(Name, "ID3"))            strn0cpy(wetterSetup.w_id3,      Value, sizeof(wetterSetup.w_id3));
  else if (!strcasecmp(Name, "ID1_NAME"))       strn0cpy(wetterSetup.w_id1_name, Value, sizeof(wetterSetup.w_id1_name));
  else if (!strcasecmp(Name, "ID2_NAME"))       strn0cpy(wetterSetup.w_id2_name, Value, sizeof(wetterSetup.w_id2_name));
  else if (!strcasecmp(Name, "ID3_NAME"))       strn0cpy(wetterSetup.w_id3_name, Value, sizeof(wetterSetup.w_id3_name));

  else 
       return false;

  return true;  
}


cMenuWetterSetup::cMenuWetterSetup(void)
{

  static const char char_allowed[] = { "abcdefghijklmnopqrstuvwxyz0123456789-_@~!.:\\/" };
  
  Add(new cMenuEditBoolItem(tr("Update data on start"),            &wetterSetup.w_update));
  Add(new cMenuEditBoolItem(tr("Use one 8-bit area"),              &wetterSetup.w_hiquality));
  Add(new cMenuEditIntItem(tr("Radarbitmap left"),                 &wetterSetup.w_left, 1, 200));
  Add(new cMenuEditIntItem(tr("Radarbitmap top"),                  &wetterSetup.w_top, 1, 200));
  Add(new cMenuEditIntItem(tr("Radarbitmap width"),              &wetterSetup.w_width, 100, 672));
  Add(new cMenuEditIntItem(tr("Radarbitmap height"),             &wetterSetup.w_height, 100, 567));
  Add(new cMenuEditIntItem(tr("Offset horizontal"),                &wetterSetup.w_osdoffset_x, -50, 50));
  Add(new cMenuEditIntItem(tr("Offset vertical"),                  &wetterSetup.w_osdoffset_y, -50, 0));
  Add(new cMenuEditBoolItem(tr("Use dithering for images"),        &wetterSetup.w_dither));
  Add(new cMenuEditIntItem(tr("Transparency for images"),          &wetterSetup.w_alpha, 1, 255));
  Add(new cMenuEditIntItem(tr("Image quality >=slower"),           &wetterSetup.w_treedepth, 1, 6));
  Add(new cMenuEditIntItem(tr("Days to forecast"),                 &wetterSetup.w_dayf, 1, 7));
  Add(new cMenuEditStrItem(tr("Location 1 (ID)"),                  wetterSetup.w_id1,      64, char_allowed));
  Add(new cMenuEditStrItem(tr("Location 1 (Name)"),                wetterSetup.w_id1_name, 64, char_allowed));
  Add(new cMenuEditStrItem(tr("Location 2 (ID)"),                  wetterSetup.w_id2,      64, char_allowed));
  Add(new cMenuEditStrItem(tr("Location 2 (Name)"),                wetterSetup.w_id2_name, 64, char_allowed));
  Add(new cMenuEditStrItem(tr("Location 3 (ID)"),                  wetterSetup.w_id3,      64, char_allowed));
  Add(new cMenuEditStrItem(tr("Location 3 (Name)"),                wetterSetup.w_id3_name, 64, char_allowed));

}    


void cMenuWetterSetup::Store(void)
{
  SetupStore("AutoUpdate",	wetterSetup.w_update);
  SetupStore("HiQuality",	wetterSetup.w_hiquality);
  SetupStore("RadarLeft",       wetterSetup.w_left);
  SetupStore("RadarTop",        wetterSetup.w_top);
  SetupStore("RadarWidth",      wetterSetup.w_width);
  SetupStore("RadarHeight",     wetterSetup.w_height);
  SetupStore("OSDOffsetX",      wetterSetup.w_osdoffset_x);
  SetupStore("OSDOffsetY",      wetterSetup.w_osdoffset_y);
  SetupStore("ImgDither",       wetterSetup.w_dither);
  SetupStore("ImgAlpha",        wetterSetup.w_alpha);
  SetupStore("ImgTreeDepth",    wetterSetup.w_treedepth);
  SetupStore("DayForecast",     wetterSetup.w_dayf);
  SetupStore("ID1",		wetterSetup.w_id1);
  SetupStore("ID2",		wetterSetup.w_id2);
  SetupStore("ID3",		wetterSetup.w_id3);
  SetupStore("ID1_NAME",	wetterSetup.w_id1_name);
  SetupStore("ID2_NAME",	wetterSetup.w_id2_name);
  SetupStore("ID3_NAME",	wetterSetup.w_id3_name);


}

        
VDRPLUGINCREATOR(cPluginWetter); // Don't touch this!
