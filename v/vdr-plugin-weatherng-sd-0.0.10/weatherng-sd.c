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



static const char *VERSION        = "0.0.10.SD-FF";
static const char *DESCRIPTION    = trNOOP("Weather.com: Weather forecast");
static const char *MAINMENUENTRY  = trNOOP("Weatherng-SD-FF");

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
        static char *help_str = NULL;

       free(help_str);	
       asprintf(&help_str,
			  "  -D DIR    --data=DIR    DIR to datafiles of plugin.  e.g. /var/lib/vdr/plugins/weatherng-sd\n"
			  "  -I DIR    --images=DIR  DIR to imagefolder of plugin e.g. /var/lib/vdr/plugins/weatherng-sd\n"
			  "  -C DIR    --config=DIR  DIR to own configuration     e.g. /var/lib/vdr/plugins/weatherng-sd\n"
			  "  -S DIR    --script=DIR  DIR to weatherng-sd.sh       e.g. /var/lib/vdr/plugins/weatherng-sd\n");
  return help_str;         
}

bool cPluginWetter::ProcessArgs(int argc, char *argv[]) {
  static struct option long_options[] = {
       { "data",	required_argument, NULL, 'D' },
       { "images",	required_argument, NULL, 'I' },
       { "config",	required_argument, NULL, 'C' },
       { "script",	required_argument, NULL, 'S' },
       { NULL }
     };
     
  int c, option_index = 0;
  while ((c = getopt_long(argc, argv, "D:I:C:S:", long_options, &option_index)) != -1) {
          switch (c) {
	  case 'D': DataDir        = optarg; break;
	  case 'I': ImageDir       = optarg; break;
	  case 'C': config_path    = optarg; break;
	  case 'S': ScriptDir      = optarg; break;
	  default: return false;
	  }		    
	}
  return true;
}

bool cPluginWetter::Initialize(void)
{
  if (DataDir == NULL) {
          DataDir = strdup(ConfigDirectory(PLUGIN_NAME_I18N));	         
          esyslog("%s: Parameter '-D' not set.\n"
	          " Set default value : %s\n", PLUGIN_NAME_I18N, DataDir);
  }
  if (ImageDir == NULL) {
          ImageDir = strdup(ConfigDirectory(PLUGIN_NAME_I18N));	         
          esyslog("%s: Parameter '-I' not set.\n"
	          " Set default value : %s\n", PLUGIN_NAME_I18N, ImageDir);
  }

  if (config_path == NULL) {
          config_path = strdup(ConfigDirectory(PLUGIN_NAME_I18N));	         
          esyslog("%s: Parameter '-C' not set.\n"
	          " Set default value : %s\n", PLUGIN_NAME_I18N, config_path);
  }

  if (ScriptDir == NULL) {
          ScriptDir = strdup(ConfigDirectory(PLUGIN_NAME_I18N));	         
          esyslog("%s: Parameter '-S' not set.\n"
	          " Set default value : %s\n", PLUGIN_NAME_I18N, ImageDir);
  }


  if(!WeatherSkin.ParseSkin("current.colors")) {
    esyslog("weatherng-sd: WARNING: no themefile found ! Using default colors");
    printf("weatherng-sd: no themefile found !\n");
    }
  else isyslog("weatherng-sd: Loading themefile\n");    
  
#ifdef USE_MAGICK
  dsyslog("DEBUG : %s: defined USE_MAGICK\n",PLUGIN_NAME_I18N);  
#else
  dsyslog("DEBUG : %s: defined USE_IMLIB2\n",PLUGIN_NAME_I18N);  
#endif

  if(wetterSetup.w_hiquality)
    dsyslog("DEBUG : %s: use HiQuality\n",PLUGIN_NAME_I18N);  
  else
    dsyslog("DEBUG : %s: use LowQuality\n",PLUGIN_NAME_I18N);  
  
  return true;
}

bool cPluginWetter::Start(void)
{
  if ( wetterSetup.w_update == true ) {
     FILE *cmd;
     char *buffer;

       asprintf(&buffer, "%s%s '%s' '%s' '%s'",ScriptDir,"/weatherng-sd.sh", wetterSetup.w_id1, wetterSetup.w_id2, wetterSetup.w_id3);	
//	asprintf(&buffer, "%s%s &", ScriptDir, "/weatherng-sd.sh");
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
  if      (!strcasecmp(Name, "AutoUpdate"))    wetterSetup.w_update      = atoi(Value);
  else if (!strcasecmp(Name, "HiQuality"))     wetterSetup.w_hiquality   = atoi(Value);
  else if (!strcasecmp(Name, "RadarLeft"))     wetterSetup.w_left        = atoi(Value);
  else if (!strcasecmp(Name, "RadarTop"))      wetterSetup.w_top         = atoi(Value);
  else if (!strcasecmp(Name, "RadarWidth"))    wetterSetup.w_width       = atoi(Value);
  else if (!strcasecmp(Name, "RadarHeight"))   wetterSetup.w_height      = atoi(Value);
  else if (!strcasecmp(Name, "OSDOffsetX"))    wetterSetup.w_osdoffset_x = atoi(Value);
  else if (!strcasecmp(Name, "OSDOffsetY"))    wetterSetup.w_osdoffset_y = atoi(Value);
  else if (!strcasecmp(Name, "ImgDither"))     wetterSetup.w_dither      = atoi(Value);
  else if (!strcasecmp(Name, "ImgAlpha"))      wetterSetup.w_alpha       = atoi(Value);
#ifdef USE_MAGICK
  else if (!strcasecmp(Name, "ImgTreeDepth"))     wetterSetup.w_treedepth   = atoi(Value);
#endif

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
  Add(new cMenuEditIntItem(tr("Radarbitmap left"),                 &wetterSetup.w_left, 1, 480));
  Add(new cMenuEditIntItem(tr("Radarbitmap top"),                  &wetterSetup.w_top, 1, 270));
  if(wetterSetup.w_hiquality) {
    // 4 MB FF-CARD
    Add(new cMenuEditIntItem(tr("Radarbitmap width"),              &wetterSetup.w_width, 100, 600));
    Add(new cMenuEditIntItem(tr("Radarbitmap height"),             &wetterSetup.w_height, 100, 360));
    }
  else {
    // 2 MB FF-CARD
    Add(new cMenuEditIntItem(tr("Radarbitmap width"),              &wetterSetup.w_width, 100, 600));
    Add(new cMenuEditIntItem(tr("Radarbitmap height"),             &wetterSetup.w_height, 100, 360));
  }
  Add(new cMenuEditIntItem(tr("Offset horizontal"),                &wetterSetup.w_osdoffset_x, -50, 50));
  Add(new cMenuEditIntItem(tr("Offset vertical"),                  &wetterSetup.w_osdoffset_y, -50, 0));
  Add(new cMenuEditBoolItem(tr("Use dithering for images"),        &wetterSetup.w_dither));
  Add(new cMenuEditIntItem(tr("Transparency for images"),          &wetterSetup.w_alpha, 1, 255));
#ifdef USE_MAGICK
  Add(new cMenuEditIntItem(tr("Image quality >=slower"),           &wetterSetup.w_treedepth, 1, 6));
#endif

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
#ifdef USE_MAGICK
  SetupStore("ImgTreeDepth",    wetterSetup.w_treedepth);
#endif

  SetupStore("ID1",		wetterSetup.w_id1);
  SetupStore("ID2",		wetterSetup.w_id2);
  SetupStore("ID3",		wetterSetup.w_id3);
  SetupStore("ID1_NAME",	wetterSetup.w_id1_name);
  SetupStore("ID2_NAME",	wetterSetup.w_id2_name);
  SetupStore("ID3_NAME",	wetterSetup.w_id3_name);


}

        
VDRPLUGINCREATOR(cPluginWetter); // Don't touch this!
