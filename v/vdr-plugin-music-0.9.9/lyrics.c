#include <string.h>
#include <fstream>

#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>

#include "lyrics.h"
#include "setup-mp3.h"
#include "i18n.h"
#include "vars.h"
#include "decoder.h"

#define TEMPFILE "/tmp/temp.lyrics"



//---------------- cMP3LoadLyrics --------------------------------------------------------------------------------------------------------------------------
// Load local lyrics

cMP3LoadLocalLyrics::cMP3LoadLocalLyrics(void)
:cOsdMenu(tr("Music: Local lyrics"))
{
  DisplayLocalLyrics();
  Display();
}


char *cMP3LoadLocalLyrics::CheckLocalLyrics(char *fileName) {

  static char tmpFile[1024];
  char *result=NULL;
  
  FILE *fp = fopen(fileName, "rb");
  if(fp) {
    fclose(fp);
    d(printf("music[%d]: lyrics: lyrics %s found\n", getpid (), fileName));
    strcpy(tmpFile,fileName);
    result = tmpFile;
    }
  else
    d(printf("music[%d]: lyrics: no lyrics found\n", getpid ()));
      
  return result;
}  


void cMP3LoadLocalLyrics::DisplayLocalLyrics() {

  using namespace std;
  ifstream lyrics;
  std::string line;
  
  size_t i;
  char *p, *q = NULL;
  char songFile[1024];
  const char *fileSuffixes[] = { "lyrics" };      

  SetHelp(tr("Parent"),tr("Track-"),tr("Track+"),NULL);

  d(printf("music[%d]: lyrics: checking %s for lyrics\n", getpid (), Songname));

  strcpy (songFile , Songname);

  p = strrchr(songFile, '.');
  if(p) {
    for (i=0; i < sizeof(fileSuffixes) / sizeof(fileSuffixes[0]); i++) {
      strcpy (p +1, fileSuffixes[i]);
      d(printf("music[%d]: lyrics: %s\n", getpid(), songFile));

      q=CheckLocalLyrics(songFile);
      
      if(q) {
	lyrics.open(q);
        if(lyrics) {
	  while(getline(lyrics, line , '\n')) {
            cOsdItem *lines = new cOsdItem(hk(line.c_str()));
	    Add(lines, osUnknown);
            if(!strcmp(lines->Text(),""))
	      lines->SetSelectable(false);
	    else  
	      lines->SetSelectable(true);
          }	    
	}
        else {
          Skins.Message(mtError, tr("No lyrics found !"));
        }
      }
    }
  }
}


eOSState cMP3LoadLocalLyrics::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
       case kGreen:
          mgr->Prev();
          break;
       case kYellow:
          mgr->Next();
          break;
       case kOk:
       case kRed:
       case kBack:
          return osBack;
       default: state = osContinue;
    }     
  }
  return state;
}


//---------------- cMP3LoadExternLyrics --------------------------------------------------------------------------------------------------------------------------
// Get Lyrics from extern Source with posibility to save (i.e. from internet) 

cMP3LoadExternLyrics::cMP3LoadExternLyrics(void)
:cOsdMenu(tr("Music: Extern lyrics"))
{
  DisplayExternLyrics();

  Display();
}


char *cMP3LoadExternLyrics::CheckExternLyrics(const char *fileName) {

  static char tmpFile[1024];
  char *result=NULL;
  
  FILE *fp = fopen(fileName, "r");
  if(fp) {
    fclose(fp);
    d(printf("music[%d]: lyrics: file '%s' found\n", getpid (), fileName));
    strcpy(tmpFile,fileName);
    result = tmpFile;
    }
  else
    d(printf("music[%d]: lyrics: file '%s' not found\n", getpid (), fileName));
  
  return result;
}  

int cMP3LoadExternLyrics::RunCommand(const char *cmd) {
  int res=-1;

  std::string Artist;
  std::string Title;
  cSongInfo *si = mgr->Current()->Info(false);
  if(!si)    si = mgr->Current()->Info();
  if(si && si->Artist) Artist = si->Artist; else Artist = "";
  if(si && si->Title)  Title  = si->Title; else Title = "";

  if(cmd) {
    std::string tmp("");
    tmp = cmd;
    tmp = tmp + " -a \"" + Artist + "\"" + " -t \"" + Title + "\"";
    
////    char *tmp=0;
////    asprintf(&tmp,"%s \"%s\" \"%s\"",cmd ,Artist.c_str(),Title.c_str());
    d(printf("music[%d]: lyrics: executing '%s'\n",getpid (), tmp.c_str()));
    res=SystemExec(tmp.c_str());
////    free(tmp);
  }
  return res;
}


eOSState cMP3LoadExternLyrics::SaveExternLyrics() {
//TODO
  bool res;
  bool res2;

  res    = false;
  res2   = false;
  std::string::size_type pos;

  std::string fullfilename;
  std::string filename;
  std::string Base;
  std::string FileName;
  std::string MoveToFile;
  
  if( Interface->Confirm(tr("Save lyrics ?")) ) {
    fullfilename = Songname;
  
    pos = fullfilename.rfind("/");
    if(pos!=std::string::npos) {
      //BaseDir with slash
      Base       = fullfilename.substr(0,pos +1);
      d(printf("music[%d]: lyrics: BaseDir '%s'\n",getpid(), Base.c_str()));
      res = true;
    }    

    //filename with extension
    filename = fullfilename.substr(Base.length(), fullfilename.length());
    d(printf("music[%d]: lyrics: Filename with extension '%s'\n",getpid() ,filename.c_str()));

    pos = filename.rfind(".");
    if(pos!=std::string::npos) {
      //filename w/o extension
      FileName   = filename.substr(0, filename.length() - (filename.length() -pos));
      d(printf("music[%d]: lyrics: Filename without extension '%s'\n",getpid(), FileName.c_str()));

      MoveToFile = Base;
      MoveToFile = MoveToFile + FileName;
      MoveToFile = MoveToFile + ".lyrics";
      d(printf("music[%d]: lyrics: Move lyrics for %s to '%s'\n", getpid(), Songname, MoveToFile.c_str()));

      res2 = true;
    }    

    if(res && res2) {
      std::string cmd("");
      cmd = "mv ";
      cmd = cmd + "\"" + TEMPFILE + "\"" + " \"" + MoveToFile + "\"";
////      char *cmd=0;
////      asprintf(&cmd, "mv \"%s\" \"%s\"", TEMPFILE, MoveToFile.c_str());
      d(printf("music[%d]: lyrics: Executing %s\n",getpid(), cmd.c_str()));
      SystemExec(cmd.c_str());
////      free(cmd);
    }
  }

  return osContinue;
}


void cMP3LoadExternLyrics::DisplayExternLyrics() {
  
  using namespace std;
  ifstream lyrics;
  std::string line;
  std::string datei;
    
  char *q = NULL;
  char songFile[1024];

  datei = config;
  datei = datei + langdir;
  datei = datei + "/scripts/music_getlyrics.pl";

#ifdef DEBUG
  isyslog("music: lyrics: executing '%s'",datei.c_str());
#endif

  SetHelp(tr("Parent"),tr("Track-"),tr("Track+"),tr("Save"));

  q=CheckExternLyrics(datei.c_str());
  if(q) { 
    Skins.Message(mtStatus,tr("Try to get lyrics..."));
    RunCommand(q);

    d(printf("music[%d]: lyrics: checking %s for lyrics\n", getpid (), TEMPFILE));

    strcpy (songFile , TEMPFILE);
    
    q=CheckExternLyrics(songFile);
      
    if(q) {
      lyrics.open (songFile);
      if(lyrics) {
        while(getline(lyrics, line , '\n')) {
	  cOsdItem *lines = new cOsdItem(hk(line.c_str()));
	  Add(lines, osUnknown);
          if(!strcmp(lines->Text(),""))
	    lines->SetSelectable(false);
	  else  
	    lines->SetSelectable(true);
        }
      }	
    }
    Skins.Message(mtStatus,NULL);
  }
}


eOSState cMP3LoadExternLyrics::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  
  if (state == osUnknown) {
    switch (Key) {
      case kGreen:
        mgr->Prev();
        break;
      case kYellow:
        mgr->Next();
        break;
      case kBlue:
        if(MP3Setup.user_is_admin)
	  SaveExternLyrics(); 
	else
	  Skins.Message(mtError, tr("Operation denied !"));
  	break;
      case kOk:
      case kRed:
      case kBack:
        return osBack;
      default: state = osContinue;
    }
  }
  return state;
}


//---------------- cMP3Lyrics --------------------------------------------------------------------------------------------------------------------------

cMP3Lyrics::cMP3Lyrics(void)
:cOsdMenu(tr("Music: Lyrics"))
{
  SetHelp();

  Add(new cOsdItem(hk(tr("Load lyrics from local source"))));                   //  0
  Add(new cOsdItem(hk(tr("Load lyrics from external source"))));                //  1

  Display();
}

cMP3Lyrics::~cMP3Lyrics()
{
}


void cMP3Lyrics::SetHelp() {
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


eOSState cMP3Lyrics::Execute() {
  int current = Current();

  if (current==0) 
    return AddSubMenu(new cMP3LoadLocalLyrics());
  else
    return AddSubMenu(new cMP3LoadExternLyrics());
}
	 

eOSState cMP3Lyrics::ProcessKey(eKeys Key)
{
  bool hadSubmenu = HasSubMenu();
  
  eOSState state = cOsdMenu::ProcessKey(Key);
  
  if(hadSubmenu && !HasSubMenu()) return osBack; 
  
  if (state == osUnknown) {
    switch (Key) {
        case kBack:
        case kRed:
	  state = osBack; 
          break;
        case kOk:
	  return Execute();
	  break;
        default: state = osContinue;
    }
  }
  return state;
}


