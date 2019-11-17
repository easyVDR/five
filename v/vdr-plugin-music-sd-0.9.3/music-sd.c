/*
 * music.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <string>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <typeinfo>

#include "music-sd.h"
#include "common.h"
#include "setup.h"
#include "setup-mp3.h"
#include "data-mp3.h"
#include "player-mp3.h"
#include "decoder-mp3.h"
#include "skin.h"
#include "visual.h"
//#include "i18n.h"
#include "vars.h"
#include "icons.h"


const char *sourcesSub=0;
const char *instantplay=0;

static const char *VERSION       = "0.9.3-SD-FF";
static const char *DESCRIPTION   = trNOOP("MP3-/Audioplayer");
static const char *MAINMENUENTRY = trNOOP("Musicplayer SD-FF");

class cPluginMp3 : public cPlugin {
private:
#if APIVERSNUM >= 10330
  bool ExternalPlay(const char *path, bool test);
#endif  
  bool SetLanguage(const char *Lang);
  cMP3Control *mp3control;
public:
  cPluginMp3(void);
  virtual ~cPluginMp3();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual bool Initialize(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
#if APIVERSNUM >= 10330
  virtual bool Service(const char *Id, void *Data);
#if APIVERSNUM >= 10331
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
#endif
#endif    
};

cPluginMp3::cPluginMp3(void)
{
  // Initialize any member varaiables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginMp3::~cPluginMp3()
{
//  MP3Skin.StoreSkin();
//  cMP3Bitmap::FlushCache();
  InfoCache.Save();
  delete mgr;
}

const char *cPluginMp3::CommandLineHelp(void)
{
  static char *help_str=0;

  free(help_str);    //                                     for easier orientation, this is column 80|
  asprintf(&help_str,
	             "  -m CMD,   --mount=CMD    use CMD to mount/unmount/eject mp3 sources\n"
                     "                           (default: %s)\n"
	             "  -I FILE,  --instant=FILE use FILE (full path) to play source instantly\n"
                     "                           (default: %s)\n"
                     "  -C DIR,   --config=DIR   search 'music-sd' files in DIR\n"
	             "                           (default: %s)\n"
                     "  -n CMD,   --network=CMD  execute CMD before & after network access\n"
                     "                           (default: %s)\n"
                     "  -B DIR,   --cddb=DIR     search CDDB files in DIR\n"
                     "                           (default: %s)\n"
                     "  -D DEV,   --dsp=DEV      device for OSS output\n"
                     "                           (default: %s)\n"
	             "  -S SUB,   --sources=SUB  search sources config in SUB subdirectory\n"
	             "                           (default: %s)\n",
#ifdef HAVE_FREETYPE
                     "empty",
#endif
                     mountscript,
                     instantplay,
                     "empty",
		     netscript?netscript:"none",
#ifdef HAVE_SNDFILE
                     cddbpath,
#else
                     "none",
#endif
                     dspdevice,
                     sourcesSub ? sourcesSub:"empty"
                     );
  return help_str;
}

bool cPluginMp3::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
      { "mount",        required_argument, NULL, 'm' },
      { "instantplay",  required_argument, NULL, 'I' },		  
      { "config",       required_argument, NULL, 'C' },
      { "network",      required_argument, NULL, 'n' },
      { "cddb",         required_argument, NULL, 'B' },
      { "dsp",          required_argument, NULL, 'D' },
      { "sources",      required_argument, NULL, 'S' },
      { NULL }
    };

  int c, option_index = 0;
  while((c=getopt_long(argc,argv,"m:I:C:n:B:D:S:",long_options,&option_index))!=-1) {
    switch (c) {
      case 'm': mountscript=optarg; break;
      case 'I': instantplay=optarg; break;
      case 'C': config=optarg; break;
      case 'n': netscript=optarg;   break;
      case 'S': sourcesSub=optarg;  break;
      case 'B':
#ifdef HAVE_SNDFILE
                cddbpath=optarg; break;
#else
                fprintf(stderr, "music-sd: libsndfile support has not been compiled in!\n"); return false;
#endif
      case 'D': dspdevice=optarg; break;

      default:  return false;
      }
    }
  return true;
}

bool cPluginMp3::Start(void)
{

  Icons::InitCharSet();

  if(MP3VisLoader.LoadVis("current.vis")) {
    MP3VisLoader.SetVis();
    isyslog("music-sd: loaded visualization data");
    }
  else {
    MP3Setup.EnableVis = 0;
    esyslog("music-sd: ERROR: problems to load visualization data! -- enable cover");
  }    

  return true;
}


bool cPluginMp3::Initialize(void)
{

  if(!CheckVDRVersion(1,1,29,"music-sd")) return false;

  if(config == NULL) {
    config = strdup(ConfigDirectory(PLUGIN_NAME_I18N));
    isyslog("music-sd: --config not set, use default path '%s'\n", config);
    }
  else
    isyslog("music-sd: --config set: '%s'", config);
    
  if(!MP3Skin.ParseSkin("current.colors", false)) {
     esyslog("music-sd: WARNING: no themefile found ! Using default colors");
     fprintf(stderr,"music-sd: no themefile found !\n");
     }
  else isyslog("music-sd: Loading themefile\n");


  MP3Sources.Load(AddDirectory(config,"musicsources.conf"));
  if(MP3Sources.Count()<1) {
     esyslog("music-sd: ERROR: you should have defined at least one source in musicsources.conf");
     fprintf(stderr,"No source(s) defined in musicsources.conf\n");
     }


//  if(!MP3Setup.First_Time_Run) {
//    isyslog("music-sd: load cache...");
    InfoCache.Load(false);
//  }
  
#if VDRVERSNUM < 10507
  RegisterI18n(Phrases);
#endif

  mgr=new cPlayManager;
  if(!mgr) {
    esyslog("music-sd: ERROR: creating playmanager failed");
    fprintf(stderr,"Creating playmanager failed\n");
    return false;
    }
  d(printf("music-sd: using %s\n",mad_version))
  d(printf("music-sd: compiled with %s\n",MAD_VERSION))

/*  
  if(MP3Setup.First_Time_Run) {

    printf("music-sd: first time run. scanning library...\n");
    cFileSource *src=MP3Sources.GetSource();
    printf("cFileSource.first()\n");	
    if(src) {
      printf("src = true\n");	
      isyslog("music-sd: load cache...");
      cFileObj *item=new cFileObj(src,0,0,otBase);
      printf("item=new FileObj\n");	
      if(item) {
        printf("item->exists()\n");	
	    cInstantPlayList *pl=new cInstantPlayList(item);
        printf("pl=new cInstantPlaylist(item)\n");	
	    if(pl && pl->Load() && pl->Count()) {
          printf("pl && pl->load=true\n");	
		  mgr->Add(pl);
          printf("mgr->Added\n");	
	      delete item;
          printf("delete item and return true\n");
          MP3Setup.First_Time_Run = false;		  
		  return true;
		  }
	    else {
          printf("try to delete pl\n");	
		  delete pl;
          printf("pl deleted\n");	
        }
	  }
      printf("try to delete item\n");	
      delete item;	
      printf("item deleted\n");	
    }  
  }
*/  
  
  
  return true;
}

void cPluginMp3::Housekeeping(void)
{
  InfoCache.Save();
}

const char *cPluginMp3::MainMenuEntry(void)
{
  return MP3Setup.HideMainMenu ? 0 : tr(MAINMENUENTRY);
}

cOsdObject *cPluginMp3::MainMenuAction(void)
{
/*
  if(MP3Setup.First_Time_Run) {
    isyslog("music-sd: music-sd library is scanning");
	Skins.Message(mtInfo, "First time run: Scan music-sd library", 5);
	return false;
  }	
*/  
  if(!SetLanguage(I18nLanguageCode(I18nCurrentLanguage()))) {
    esyslog("music-sd: could not set language: %s . Set default 'eng,dos'", I18nLanguageCode(I18nCurrentLanguage()));
    langdir = strdup("language/english");
    }
  else {
    isyslog("music-sd: language set '%s'", langdir);    
    d(printf("music-sd: path to scripts and data = '%s'\n", langdir))
  }

  if (MP3Setup.InstantPlay && ( instantplay != NULL)) { 
    ExternalPlay(instantplay, false);
    return NULL;
    }
  else
    return new cMenuMP3;
}

cMenuSetupPage *cPluginMp3::SetupMenu(void)
{
  return new cMenuSetupMP3;
}


bool cPluginMp3::SetLanguage(const char *Lang)
{
  char lang[16];
  strcpy(lang, Lang);

  if      (!strcasecmp(lang, "eng,dos"))     langdir = strdup("language/english");      // England
  else if (!strcasecmp(lang, "deu,ger"))     langdir = strdup("language/german");       // Deustchland
  else if (!strcasecmp(lang, "slv,slo"))     langdir = strdup("language/slovenski");    // Slovenien
  else if (!strcasecmp(lang, "ita"))         langdir = strdup("language/italiano");     // Italien
  else if (!strcasecmp(lang, "dut,nla,nld")) langdir = strdup("language/nederland");    // Niederlande
  else if (!strcasecmp(lang, "por"))         langdir = strdup("language/portugese");    // Portugal
  else if (!strcasecmp(lang, "fra,fre"))     langdir = strdup("language/francais");     // Frankreich
  else if (!strcasecmp(lang, "nor"))         langdir = strdup("language/norsk");        // Norwegen ?
  else if (!strcasecmp(lang, "fin,smi"))     langdir = strdup("language/finnish");      // Finland
  else if (!strcasecmp(lang, "pol"))         langdir = strdup("language/polski");       // Polen
  else if (!strcasecmp(lang, "esl,spa"))     langdir = strdup("language/espanol");      // Spanien
  else if (!strcasecmp(lang, "ell,gre"))     langdir = strdup("language/greek");        // Griechenland
  else if (!strcasecmp(lang, "sve,swe"))     langdir = strdup("language/svenska");      // Schweden
  else if (!strcasecmp(lang, "rom,rum"))     langdir = strdup("language/romania");      // Rumaenien
  else if (!strcasecmp(lang, "hun"))         langdir = strdup("language/magyar");       // ?
  else if (!strcasecmp(lang, "cat,cln"))     langdir = strdup("language/catala");       // Catalonien ?
  else if (!strcasecmp(lang, "rus"))         langdir = strdup("language/russia");       // Russland
  else if (!strcasecmp(lang, "hrv"))         langdir = strdup("language/hrvatski");     // ?
  else if (!strcasecmp(lang, "est"))         langdir = strdup("language/eesti");        // ?
  else if (!strcasecmp(lang, "dan"))         langdir = strdup("language/dansk");        // Daenemark
  else if (!strcasecmp(lang, "cze,ces"))     langdir = strdup("language/czech");        // Tschechien
  else if (!strcasecmp(lang, "tur"))         langdir = strdup("language/turkish");      // Turkish
  else if (!strcasecmp(lang, "ukr"))         langdir = strdup("language/ukraine");      // Ukraine

  else return false;
  
  return true;

}


bool cPluginMp3::SetupParse(const char *Name, const char *Value)
{
  if      (!strcasecmp(Name, "InitLoopMode"))     MP3Setup.InitLoopMode    = atoi(Value);
  else if (!strcasecmp(Name, "InitShuffleMode"))  MP3Setup.InitShuffleMode = atoi(Value);
  else if (!strcasecmp(Name, "AudioMode"))        MP3Setup.AudioMode       = atoi(Value);
  else if (!strcasecmp(Name, "BgrScan"))          MP3Setup.BgrScan         = atoi(Value);
  else if (!strcasecmp(Name, "EditorMode"))       MP3Setup.EditorMode      = atoi(Value);
  else if (!strcasecmp(Name, "UseDeviceStillPicture"))       MP3Setup.UseDeviceStillPicture      = atoi(Value);
  else if (!strcasecmp(Name, "MenuMode"))         MP3Setup.MenuMode        = atoi(Value);
  else if (!strcasecmp(Name, "TargetLevel"))      MP3Setup.TargetLevel     = atoi(Value);
  else if (!strcasecmp(Name, "LimiterLevel"))     MP3Setup.LimiterLevel    = atoi(Value);
  else if (!strcasecmp(Name, "Only48kHz"))        MP3Setup.Only48kHz       = atoi(Value);
  else if (!strcasecmp(Name, "ConnectTimeout"))   MP3Setup.ConnectTimeout  = atoi(Value);
  else if (!strcasecmp(Name, "UseProxy"))         MP3Setup.UseProxy        = atoi(Value);
  else if (!strcasecmp(Name, "ProxyHost"))        strn0cpy(MP3Setup.ProxyHost,Value,MAX_HOSTNAME);
  else if (!strcasecmp(Name, "ProxyPort"))        MP3Setup.ProxyPort       = atoi(Value);
  else if (!strcasecmp(Name, "UseCddb"))          MP3Setup.UseCddb         = atoi(Value);
  else if (!strcasecmp(Name, "CddbHost"))         strn0cpy(MP3Setup.CddbHost,Value,MAX_HOSTNAME);
  else if (!strcasecmp(Name, "CddbPort"))         MP3Setup.CddbPort        = atoi(Value);
  else if (!strcasecmp(Name, "AbortAtEOL"))       MP3Setup.AbortAtEOL      = atoi(Value);
  else if (!strcasecmp(Name, "AudioOutMode"))     MP3Setup.AudioOutMode    = atoi(Value);
  else if (!strcasecmp(Name, "HideMainMenu"))     MP3Setup.HideMainMenu              = atoi(Value);
  else if (!strcasecmp(Name, "InstantPlay"))      MP3Setup.InstantPlay               = atoi(Value);
  else if (!strcasecmp(Name, "KeepSelect"))       MP3Setup.KeepSelect                = atoi(Value);
//  else if (!strcasecmp(Name, "AutoPurge"))              MP3Setup.AutoPurge                 = atoi(Value);
  else if (!strcasecmp(Name, "ExitClose"))        MP3Setup.ExitClose                 = atoi(Value);
  else if (!strcasecmp(Name, "BgrMode"))          MP3Setup.BgrMode                   = atoi(Value);
  else if (!strcasecmp(Name, "ShowMessage"))      MP3Setup.ShowMessage               = atoi(Value);
  else if (!strcasecmp(Name, "AdminMode"))        MP3Setup.AdminMode                 = atoi(Value);
  else if (!strcasecmp(Name, "EnableRating"))         	MP3Setup.EnableRating              = atoi(Value);
  else if (!strcasecmp(Name, "RateCompat"))             strn0cpy(MP3Setup.RateCompat,Value,sizeof(MP3Setup.RateCompat));
  else if (!strcasecmp(Name, "RateInFile"))             MP3Setup.RateInFile                = atoi(Value);
  else if (!strcasecmp(Name, "RatingFirst"))            MP3Setup.RatingFirst               = atoi(Value);
  else if (!strcasecmp(Name, "Jumptime"))          	    MP3Setup.Jumptime                  = atoi(Value);
  else if (!strcasecmp(Name, "CopyDir"))           	    strn0cpy(MP3Setup.CopyDir,Value,sizeof(MP3Setup.CopyDir));
  else if (!strcasecmp(Name, "RecordDir"))              strn0cpy(MP3Setup.RecordDir,Value,sizeof(MP3Setup.RecordDir));
  else if (!strcasecmp(Name, "RecordOpts"))           	strn0cpy(MP3Setup.RecordOpts,Value,sizeof(MP3Setup.RecordOpts));
  else if (!strcasecmp(Name, "CoverDir"))               strn0cpy(MP3Setup.CoverDir,Value,sizeof(MP3Setup.CoverDir));
  else if (!strcasecmp(Name, "ReplaceString"))          strn0cpy(MP3Setup.ReplaceString,Value,sizeof(MP3Setup.ReplaceString));
  else if (!strcasecmp(Name, "OSDoffsetx"))           	MP3Setup.OSDoffsetx                = atoi(Value);
  else if (!strcasecmp(Name, "OSDoffsety"))        	    MP3Setup.OSDoffsety                = atoi(Value);
//  else if (!strcasecmp(Name, "UseOneArea"))        	    MP3Setup.UseOneArea                = atoi(Value);
  else if (!strcasecmp(Name, "ArtistFirst"))            MP3Setup.ArtistFirst               = atoi(Value);
  else if (!strcasecmp(Name, "Coversize"))              MP3Setup.Coversize                 = atoi(Value);
  else if (!strcasecmp(Name, "MaxCover"))               MP3Setup.MaxCover                  = atoi(Value);
//  else if (!strcasecmp(Name, "ImgAlpha"))               MP3Setup.ImgAlpha                  = atoi(Value);
  else if (!strcasecmp(Name, "ImgDither"))              MP3Setup.ImgDither                 = atoi(Value);
  else if (!strcasecmp(Name, "EnableVis"))              MP3Setup.EnableVis                 = atoi(Value);
  else if (!strcasecmp(Name, "FalloffSA"))              MP3Setup.FalloffSA                 = atoi(Value);
#ifdef HAVE_MAGICK
  else if (!strcasecmp(Name, "MaxCacheFill"))           MP3Setup.MaxCacheFill              = atoi(Value);
  else if (!strcasecmp(Name, "ImgTreeDepth"))           MP3Setup.ImgTreeDepth              = atoi(Value);
#endif
  else if (!strcasecmp(Name, "First_Time_Run"))         MP3Setup.First_Time_Run            = atoi(Value);
  else return false;

  return true;
}

cMenuSetupMP3::cMenuSetupMP3(void)
{
  
  static const char allowed[] = { "abcdefghijklmnopqrstuvwxyz0123456789-_@~!.:\\/" };

  int numModes=0;
  aout[numModes]=tr("DVB");   amodes[numModes]=AUDIOOUTMODE_DVB; numModes++;
  aout[numModes]=tr("OSS");   amodes[numModes]=AUDIOOUTMODE_OSS; numModes++;


  amode=0;
  for(int i=0; i<numModes; i++)
    if(amodes[i]==MP3Setup.AudioOutMode) { amode=i; break; }

  SetSection(tr("MP3"));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Artist-Title in tracklist"),  &MP3Setup.ArtistFirst));
  userlevel[0]=tr("User");
  userlevel[1]=tr("Admin");
  userlevel[2]=tr("Superadmin");
  Add(new cMenuEditStraItem(tr("Setup.MP3$Userlevel"),                        &MP3Setup.AdminMode,        3   ,userlevel));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Show statusmessages"),              &MP3Setup.ShowMessage));
  Add(new cMenuEditIntItem( tr("Setup.MP3$Jump interval (FFW/FREW)"),         &MP3Setup.Jumptime,         1   ,360));
  Add(new cMenuEditStrItem( tr("Setup.MP3$Where to copy tracks"),              MP3Setup.CopyDir,        255));
  Add(new cMenuEditStrItem( tr("Setup.MP3$Where to record streams"),           MP3Setup.RecordDir,      255));
  Add(new cMenuEditStrItem( tr("Setup.MP3$Options for recording"),             MP3Setup.RecordOpts,     255));
  Add(new cMenuEditStrItem( tr("Setup.MP3$Directory for cover of artists"),    MP3Setup.CoverDir,       255));
  Add(new cMenuEditIntItem(tr("Setup.MP3$OSD Offset X"),                      &MP3Setup.OSDoffsetx,     -50   ,50));
  Add(new cMenuEditIntItem(tr("Setup.MP3$OSD Offset Y"),                      &MP3Setup.OSDoffsety,     -50   ,50));
  coversize[0]=tr("normal");
  coversize[1]=tr("double");
  coversize[2]=tr("4:3");
  coversize[3]=tr("fullscreen");
  Add(new cMenuEditStraItem(tr("Setup.MP3$Size of coverdisplay"),       &MP3Setup.Coversize,4,coversize));
#ifdef HAVE_MAGICK
  Add(new cMenuEditIntItem(tr("Setup.MP3$Max. cache in MB for cover"),  &MP3Setup.MaxCacheFill,1,1024));
  Add(new cMenuEditIntItem(tr("Setup.MP3$Image quality >=slower"),      &MP3Setup.ImgTreeDepth,1 ,6));
#endif
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Use Dithering for cover"),    &MP3Setup.ImgDither));
  Add(new cMenuEditIntItem(tr("Setup.MP3$Max. download of cover"),      &MP3Setup.MaxCover,1,50));

  Add(new cMenuEditBoolItem(tr("Setup.MP3$Enable visualization"),       &MP3Setup.EnableVis));
  Add(new cMenuEditIntItem(tr("Setup.MP3$Visualization bar falloff"),   &MP3Setup.FalloffSA,  0, 20));

  Add(new cMenuEditBoolItem(tr("Setup.MP3$Enable rating"),              &MP3Setup.EnableRating));
  Add(new cMenuEditStrItem(tr("Setup.MP3$Email for rating"),            MP3Setup.RateCompat,255,allowed));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Store rating in file"),       &MP3Setup.RateInFile));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Rating as first red key"),    &MP3Setup.RatingFirst));
  Add(new cMenuEditStraItem(tr("Setup.MP3$Audio output mode"),          &amode,numModes,aout));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Audio mode"),                 &MP3Setup.AudioMode, tr("Round"), tr("Dither")));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Use 48kHz mode only"),        &MP3Setup.Only48kHz));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Live picture in background"), &MP3Setup.BgrMode));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Use DeviceStillPicture"),     &MP3Setup.UseDeviceStillPicture));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Abort player at end of list"),&MP3Setup.AbortAtEOL));
  scan[0]=tr("disabled");
  scan[1]=tr("ID3 only");
  scan[2]=tr("ID3 & Level");
  Add(new cMenuEditStraItem(tr("Setup.MP3$Background scan"),       &MP3Setup.BgrScan, 3, scan));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Editor display mode"),   &MP3Setup.EditorMode, tr("Filenames"), tr("ID3 names")));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Mainmenu mode"),         &MP3Setup.MenuMode, tr("Playlists"), tr("Browser")));

  Add(new cMenuEditStrItem( tr("Setup.MP3$Replace string in winamp playlist"),   MP3Setup.ReplaceString,255));

  Add(new cMenuEditBoolItem(tr("Setup.MP3$Keep selection menu"),   &MP3Setup.KeepSelect));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Exit stop playback"),    &MP3Setup.ExitClose));
  Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"),             &MP3Setup.HideMainMenu));
  Add(new cMenuEditBoolItem(tr("Replay instantly"),                &MP3Setup.InstantPlay));
  Add(new cMenuEditIntItem( tr("Setup.MP3$Normalizer level"),      &MP3Setup.TargetLevel, 0, MAX_TARGET_LEVEL));
  Add(new cMenuEditIntItem( tr("Setup.MP3$Limiter level"),         &MP3Setup.LimiterLevel, MIN_LIMITER_LEVEL, 100));

  Add(new cMenuEditIntItem(tr("Setup.MP3$Connection timeout (s)"), &MP3Setup.ConnectTimeout, 1, 60));

  Add(new cMenuEditBoolItem(tr("Setup.MP3$Use HTTP proxy"),        &MP3Setup.UseProxy));
  Add(new cMenuEditStrItem( tr("Setup.MP3$HTTP proxy host"),       MP3Setup.ProxyHost,MAX_HOSTNAME,allowed));
  Add(new cMenuEditIntItem( tr("Setup.MP3$HTTP proxy port"),       &MP3Setup.ProxyPort,1,65535));
  cddb[0]=tr("disabled");
  cddb[1]=tr("local only");
  cddb[2]=tr("local&remote");
  Add(new cMenuEditStraItem(tr("Setup.MP3$CDDB for CD-Audio"),     &MP3Setup.UseCddb,3,cddb));
  Add(new cMenuEditStrItem( tr("Setup.MP3$CDDB server"),           MP3Setup.CddbHost,MAX_HOSTNAME,allowed));
  Add(new cMenuEditIntItem( tr("Setup.MP3$CDDB port"),             &MP3Setup.CddbPort,1,65535));
  
}

void cMenuSetupMP3::Store(void)
{
#ifdef DEBUG
  printf("music-sd: save setup\n");
  printf("music-sd: InitLoopMode = %d\n", MP3Setup.InitLoopMode);
  printf("music-sd: InitShuffleMode = %d\n", MP3Setup.InitShuffleMode);
#endif

  MP3Setup.AudioOutMode=amodes[amode];

  SetupStore("InitLoopMode",     MP3Setup.InitLoopMode   );
  SetupStore("InitShuffleMode",  MP3Setup.InitShuffleMode);
  SetupStore("AudioMode",        MP3Setup.AudioMode      );
  SetupStore("AudioOutMode",     MP3Setup.AudioOutMode   );
  SetupStore("BgrScan",          MP3Setup.BgrScan        );
  SetupStore("EditorMode",       MP3Setup.EditorMode     );
  SetupStore("UseDeviceStillPicture", MP3Setup.UseDeviceStillPicture );
  SetupStore("MenuMode",         MP3Setup.MenuMode       );
  SetupStore("TargetLevel",      MP3Setup.TargetLevel    );
  SetupStore("LimiterLevel",     MP3Setup.LimiterLevel   );
  SetupStore("Only48kHz",        MP3Setup.Only48kHz      );
  SetupStore("ConnectTimeout",   MP3Setup.ConnectTimeout );
  SetupStore("UseProxy",         MP3Setup.UseProxy       );
  SetupStore("ProxyHost",        MP3Setup.ProxyHost      );
  SetupStore("ProxyPort",        MP3Setup.ProxyPort      );
  SetupStore("UseCddb",          MP3Setup.UseCddb        );
  SetupStore("CddbHost",         MP3Setup.CddbHost       );
  SetupStore("CddbPort",         MP3Setup.CddbPort       );
  SetupStore("AbortAtEOL",       MP3Setup.AbortAtEOL     );
  SetupStore("HideMainMenu",     MP3Setup.HideMainMenu   );
  SetupStore("HideMainMenu",     MP3Setup.HideMainMenu   );
  SetupStore("InstantPlay",      MP3Setup.InstantPlay    );
  SetupStore("KeepSelect",       MP3Setup.KeepSelect     );
  SetupStore("ExitClose",        MP3Setup.ExitClose      );
  SetupStore("BgrMode",          MP3Setup.BgrMode        );
  SetupStore("ShowMessage",      MP3Setup.ShowMessage    );
  SetupStore("AdminMode",        MP3Setup.AdminMode      );
  SetupStore("EnableRating",     MP3Setup.EnableRating   );
  SetupStore("RateCompat",       MP3Setup.RateCompat     );
  SetupStore("RateInFile",       MP3Setup.RateInFile     );
  SetupStore("RatingFirst",      MP3Setup.RatingFirst    );
  SetupStore("Jumptime",         MP3Setup.Jumptime       );
  SetupStore("CopyDir",          MP3Setup.CopyDir        );
  SetupStore("RecordDir",        MP3Setup.RecordDir      );
  SetupStore("RecordOpts",       MP3Setup.RecordOpts     );
  SetupStore("CoverDir",         MP3Setup.CoverDir       );
  SetupStore("ReplaceString",    MP3Setup.ReplaceString  );
  SetupStore("OSDoffsetx",       MP3Setup.OSDoffsetx     );
  SetupStore("OSDoffsety",       MP3Setup.OSDoffsety     );
  SetupStore("ArtistFirst",      MP3Setup.ArtistFirst    );
  SetupStore("Coversize",        MP3Setup.Coversize      );
  SetupStore("MaxCover",         MP3Setup.MaxCover       );
  SetupStore("ImgDither",        MP3Setup.ImgDither      );
  SetupStore("EnableVis",        MP3Setup.EnableVis      );
  SetupStore("FalloffSA",        MP3Setup.FalloffSA);
#ifdef HAVE_MAGICK
  SetupStore("MaxCacheFill",     MP3Setup.MaxCacheFill   );
  SetupStore("ImgTreeDepth",     MP3Setup.ImgTreeDepth   );
#endif
  SetupStore("First_Time_Run",   MP3Setup.First_Time_Run );
}


eOSState cMenuSetupMP3::ProcessKey(eKeys Key)
{
  bool hadSubMenu = HasSubMenu();
  eOSState state = cMenuSetupPage::ProcessKey(Key);
  
  if(hadSubMenu && Key == kOk)
    Store();

  return state;
}      


bool cPluginMp3::ExternalPlay(const char *path, bool test)
{
  char real[PATH_MAX+1];
  if(realpath(path,real)) {
    cFileSource *src=MP3Sources.FindSource(real , true);
    if(src) {
      cFileObj *item=new cFileObj(src,0,0,otFile);
      if(item) {
        item->SplitAndSet(real);
        if(item->GuessType()) {
          if(item->Exists()) {
            cInstantPlayList *pl=new cInstantPlayList(item);
            if(pl && pl->Load() && pl->Count()) {
              if(!test) mp3control->SetPlayList(pl);
	      //mp3control::SetPlayList(pl);
              else delete pl;
              delete item;
              return true;
              }
            else dsyslog("music-sd: service: error building playlist");
            delete pl;
            }
          else dsyslog("music-sd: service: cannot play '%s'",path);
          }
        else dsyslog("music-sd: service: GuessType() failed for '%s'",path);
        delete item;
        }
      }
    else dsyslog("music-sd: service: cannot find source for '%s', real '%s'",path,real);
    }
  else if(errno!=ENOENT && errno!=ENOTDIR)
    esyslog("music-sd: ERROR: realpath: %s: %s",path,strerror(errno));
  return false;
}

bool cPluginMp3::Service(const char *Id, void *Data)
{
  if(strcmp(Id, "Music-SD-Play-v1") == 0) {
    if(Data) {
      struct MPlayerServiceData *msd=(struct MPlayerServiceData *)Data;
      msd->result=ExternalPlay(msd->data.filename,false);
      }
    return true;
    }
  else if(strcmp(Id, "Music-SD-Test-v1") == 0) {
    if(Data) {
      struct MPlayerServiceData *msd=(struct MPlayerServiceData *)Data;
      msd->result=ExternalPlay(msd->data.filename,true);
      }
    return true;
    }
  
  else if(strcmp(Id, SPAN_PROVIDER_CHECK_ID) ==0) {
    if (Data) {
      *((Span_Provider_Check_1_0*)Data)->isActive = true;
    }
    return true;
  }
  else if(strcmp(Id, SPAN_CLIENT_CHECK_ID) == 0) {
    if(MP3Setup.EnableVis && (Data)) {
      *((Span_Client_Check_1_0*)Data)->isActive = true;
    }
    return true;
  }
  
  return false;
}


const char **cPluginMp3::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "PLAY <filename>\n"
    "    Triggers playback of file 'filename'.",
    "TEST <filename>\n"
    "    Tests is playback of file 'filename' is possible.",
    "CURR\n"
    "    Returns filename of song currently being replayed.",
    "CONFIG\n"
    "    Returns music-sd configuration directory.",
    NULL
    };
  return HelpPages;
}

cString cPluginMp3::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if(!strcasecmp(Command,"PLAY")) {
    if(*Option) {
      if(ExternalPlay(Option,false)) return "Playback triggered";
      else { ReplyCode=550; return "Playback failed"; }
      }
    else { ReplyCode=501; return "Missing filename"; }
    }
  else if(!strcasecmp(Command,"TEST")) {
    if(*Option) {
      if(ExternalPlay(Option,true)) return "Playback possible";
      else { ReplyCode=550; return "Playback not possible"; }
      }
    else { ReplyCode=501; return "Missing filename"; }
    }
  else if(!strcasecmp(Command,"CURR")) {
    cControl *control=cControl::Control();
    if(control && typeid(*control)==typeid(cMP3Control)) {
      cMP3PlayInfo mode;
      if(mgr->Info(-1,&mode)) return mode.Filename;
      else return "<unknown>";
      }
    else { ReplyCode=550; return "No running playback"; }
    }
  else if(!strcasecmp(Command,"CONFIG")) {
    if(mgr)
      return config;
    else
       return "<unknown>";
    }

  return NULL;
}

VDRPLUGINCREATOR(cPluginMp3); // Don't touch this!
