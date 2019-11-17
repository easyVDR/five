
#include <string>
#include <fstream>

#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include <vdr/status.h>

#include "commands.h"
#include "i18n.h"
#include "player-mp3.h"
#include "decoder.h"
#include "skin.h"
#include "search.h"
#include "mp3id3.h"
#include "mp3id3tag.h"
#include "lyrics.h"
#include "cover.h"
#include "visual.h"
#include "options.h"
#include "vars.h"

#define X_DISPLAY_MISSING
#define MAXLENGTH 256
#define THEMESEXT "*.theme"
#define VISEXT "*.visual"
#define FINDCMD "cd '%s' && find '%s' -iname '%s' -printf '%%p\n' | sort -f"

char urlname[256];

//---------------- cMP3UpdateWebStreams --------------------------------------------------------------------------------------------------------------------------


class cMP3UpdateWebStreams : public cOsdMenu {
private:
  cCommands commands;
//  cOsdItem *Item(const char *name, const char *text);
  void SetHelp();
  void ShowDisplay(void);
  void UpdateGenre();
  eOSState Execute(void);
  int limits, bitrate;
  const char *bitrates[10];
public:
  cMP3UpdateWebStreams(void);
  virtual ~cMP3UpdateWebStreams();
  virtual eOSState ProcessKey(eKeys Key);
  void LoadCommand();
};


cMP3UpdateWebStreams::cMP3UpdateWebStreams(void)
:cOsdMenu(tr("Music: Update webstreams"),32)
{
  limits   = MP3Setup.ShoutcastLimit;
  bitrate  = 192;
//  bitrates = MP3Setup.ShoutcastBitrate;
  
  ShowDisplay();
/*  
  Add(new cMenuEditIntItem(tr("Limit for stations"),   &limits,   0 , 100));
  Add(new cMenuEditIntItem(tr("Bitrate for stations"), &bitrates, 0 , 100));

  SetHelp();

  LoadCommand();

  for (cCommand *command = commands.First(); command; command = commands.Next(command)) {
    cOsdItem *item = new cOsdItem(hk(command->Title()));
    Add(item, osUnknown);
    if(strstr(item->Text(), "----------------")) 
      item->SetSelectable(false);
    else
      item->SetSelectable(true);
  }
*/  
}

cMP3UpdateWebStreams::~cMP3UpdateWebStreams()
{
}

void cMP3UpdateWebStreams::ShowDisplay(void)
{
  Clear();
  Add(new cMenuEditIntItem(tr("Limit for Shoutcast"),   &limits,   0 , 100));

  bitrates[0]="32";
  bitrates[1]="48";
  bitrates[2]="56";
  bitrates[3]="64";
  bitrates[4]="96";
  bitrates[5]="128";
  bitrates[6]="160";
  bitrates[7]="192";
  bitrates[8]="256";
  bitrates[9]="320";
  Add(new cMenuEditStraItem(tr("Bitrate for Shoutcast"), &MP3Setup.ShoutcastBitrate, 10 , bitrates));
  Add(new cOsdItem(hk(tr("Update Genres"))));


  LoadCommand();

  for (cCommand *command = commands.First(); command; command = commands.Next(command)) {
    cOsdItem *item = new cOsdItem(hk(command->Title()));
    Add(item, osUnknown);
    if(strstr(item->Text(), "----------------")) 
      item->SetSelectable(false);
    else
      item->SetSelectable(true);
  }

  SetHelp();
  Display();
}

void cMP3UpdateWebStreams::SetHelp() {
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


void cMP3UpdateWebStreams::LoadCommand() {

  std::string lingual;

  lingual = langdir;
  lingual = lingual + "/data/shoutcast.dat";

  commands.Load(AddDirectory(config, lingual.c_str()), true);
#ifdef DEBUG
  isyslog("music: commands: load  shoutcast.dat '%s%s'",config, lingual.c_str());
#endif
}


void cMP3UpdateWebStreams::UpdateGenre()
{
  std::string datei;
  
  datei = config;
  datei = datei + langdir;
  datei = datei + "/scripts/music_updategenre.pl '";
  datei = datei + MP3Setup.WebstreamDat + "'  '10'  '192'";

  Skins.Message(mtStatus, tr("Refreshing Genres"));
  FILE *updatecmd;
  updatecmd = popen(datei.c_str(), "r");
  pclose(updatecmd);

#ifdef DEBUG
  isyslog("music: commands: update genres '%s'", datei.c_str());
#endif

  Display();
}


eOSState cMP3UpdateWebStreams::Execute(void) {

    int ItemIndex = Current();
    if (ItemIndex <= 1) return osContinue;

    if (ItemIndex == 2) {
      UpdateGenre();
      return osBack;
    }


    int parbit = MP3Setup.ShoutcastBitrate;

    switch(parbit) {
      case  0: bitrate = 32; break;
      case  1: bitrate = 48; break;
      case  2: bitrate = 56; break;
      case  3: bitrate = 64; break;
      case  4: bitrate = 96; break;
      case  5: bitrate = 128; break;
      case  6: bitrate = 160; break;
      case  7: bitrate = 192; break;
      case  8: bitrate = 256; break;
      case  9: bitrate = 320; break;
      default : break;
    }  


    cCommand *command = commands.Get(Current() -3);
    if (command) {
      char *buffer = NULL;
      bool confirmed = true;

      if (command->Confirm()) {
        if (asprintf(&buffer, "%s?", command->Title()) == -1) {
         fprintf(stderr, "Failed to allocate string for command-Title");
         return osBack;
        }
        confirmed = Interface->Confirm(buffer);
        free(buffer);
      }
      if (confirmed) {
        if (asprintf(&buffer, "%s...", command->Title()) == -1)
          fprintf(stderr, "Failed to allocate string for command-Title");

        Skins.Message(mtStatus, buffer);
        free(buffer);

        char *parameters;
        if (asprintf(&parameters, " '%d' '%d' '%s';", limits, bitrate, MP3Setup.WebstreamDat) == -1)
          fprintf(stderr, "Failed to allocate string");

        const char *Result = command->Execute(parameters);
        free(parameters);

        Skins.Message(mtStatus, NULL);
        if(Result) return AddSubMenu(new cMenuText(command->Title(), Result, fontFix));
	return osBack;
      }
    }

  return osContinue;
}


eOSState cMP3UpdateWebStreams::ProcessKey(eKeys Key) {

  bool hadSubmenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (hadSubmenu && !HasSubMenu())
    return osBack;

  if (state == osUnknown) {
    switch (Key) {
        case kRed:   state = osBack;
	case kGreen:
	case kYellow:
	case kBlue:  state = osContinue;
      case kOk:      state = Execute(); 
      default:       state = osContinue;
    }
  }
  
  return state;
}        		



// ------------ cMP3ThemesItem ----------------------------------------------------------------------------------------------- //

class cMP3ThemesItem : public cOsdItem {
private:
  cFileObjItem *item;
  virtual void Set(void);
  int idx;
public:
  cMP3ThemesItem(cFileObjItem *Item, int index);
  cFileObjItem *Item(void) {return item;}
  ~cMP3ThemesItem();
};


cMP3ThemesItem::cMP3ThemesItem(cFileObjItem *Item, int index) {
  idx = index; 
  item = Item;
  Set();  
}

cMP3ThemesItem::~cMP3ThemesItem(){
}

void cMP3ThemesItem::Set(void) {
  char *Name;

  if (asprintf(&Name,"%s", item->Name()) == -1)
    fprintf(stderr, "Failed to allocate string for item->Name()");

  SetText(Name,false);
}


// ------------ cMP3Themes --------------------------------------------------------------------------------------------------- //

class cMP3Themes : public cOsdMenu {
private:
  void SetHelp();
  void LoadSkinFiles();
  eOSState SetTheme(void);
  cFileObjItem *newitem;
protected:
  cFileObjItem *CurrentFileObjItem(void);  
public:
  cMP3Themes(void);
  virtual eOSState ProcessKey(eKeys Key);
};


cMP3Themes::cMP3Themes(void)
:cOsdMenu(tr("Music: Appearance"),4)
{

  LoadSkinFiles();
  SetHelp();
}

void cMP3Themes::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


void cMP3Themes::LoadSkinFiles()
{
  int count = 0;

  char *cmd = NULL;
  cReadLine reader;

  std::string plugindir;
  std::string themename;
  std::string themefile;
  std::string Base;
   
  plugindir = config;
  plugindir = plugindir + "/themes/";

#ifdef DEBUG
  isyslog("music: commands: load themes from '%s'", plugindir.c_str());
#endif

  int  len2 = plugindir.length()+1;

   
  if (asprintf(&cmd, FINDCMD, plugindir.c_str(), plugindir.c_str(), THEMESEXT) == -1)
    fprintf(stderr, "Failed to allocate string");

  FILE *p = popen(cmd, "r");

  if(p) {
    char *s;
    while ((s = reader.Read(p)) != NULL) {
      count++;
      themefile = s;
      int len = themefile.length();
      std::string::size_type pos = themefile.rfind('/',len);
      if(pos!=std::string::npos) {
        // Filename
        themename = themefile.substr(pos+1,len);
        // Basedir
        int sublen = len - len2 - themename.length()-1;
        Base = themefile.substr(len2,sublen);
      }
      newitem = new cFileObjItem(plugindir.c_str(), themename.c_str(), otFile);
      if(newitem && newitem->Type()==otFile) {
        Add(new cMP3ThemesItem(newitem, count));
      }
    }
    pclose(p);
  }
  else
    Skins.Message(mtError, tr("ERROR: Havent found any themes !"));
    
  free(cmd);
}          
      
cFileObjItem *cMP3Themes::CurrentFileObjItem(void)
{
  cMP3ThemesItem *item = (cMP3ThemesItem *)Get(Current());
  return item ? item->Item():0;
}  

  
eOSState cMP3Themes::SetTheme(void)
{
  cFileObjItem *item=CurrentFileObjItem();

  if(item && item->Type()==otFile) {
    if(MP3Skin.StoreSkin(item->Name())) {
      MP3Skin.ParseSkin("current.colors");
      MP3Skin.ReloadFonts=1;
      return osBack;
      }
    else    
      return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not store/load new themefile !"), fontFix));
  }
  
  return osContinue;
}


eOSState cMP3Themes::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osUnknown) {
     switch(Key) {
       case kOk:
          state = SetTheme();
	  break;     
       case kBlue:
       case kGreen:
       case kYellow:
       case kRed: return osBack;
       default: return osContinue;;
       }
     }
  return state;
}



// ------------ cMP3VisualItem --------------------------------------------------------------------------------------------- //

class cMP3VisualItem : public cOsdItem {
private:
  cFileObjItem *item;
  virtual void Set(void);
  int idx;
public:
  cMP3VisualItem(cFileObjItem *Item, int index);
  cFileObjItem *Item(void) {return item;}
  ~cMP3VisualItem();
};


cMP3VisualItem::cMP3VisualItem(cFileObjItem *Item, int index)
{
  idx = index; 
  item = Item;
  Set();  
}

cMP3VisualItem::~cMP3VisualItem()
{
}

void cMP3VisualItem::Set(void)
{
  char *Name;

  if(asprintf(&Name,"%s", item->Name()) == -1)
    fprintf(stderr, "Failed to allocate string for item->Name()");

  SetText(Name,false);
}


// ------------ cMP3Visual ------------------------------------------------------------------------------------------------- //

class cMP3Visual : public cOsdMenu {
private:
  void SetHelp();
  void LoadVisFiles();
  eOSState SetVis(void);
  cFileObjItem *newitem;
protected:
  cFileObjItem *CurrentFileObjItem(void);  
public:
  cMP3Visual(void);
  virtual eOSState ProcessKey(eKeys Key);
};


cMP3Visual::cMP3Visual(void)
:cOsdMenu(tr("Music: Visualizations"),4)
{
  Add(new cOsdItem(hk(tr("Disable visualization"))));
  LoadVisFiles();
  SetHelp();
}

void cMP3Visual::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


void cMP3Visual::LoadVisFiles()
{
  int count = 0;

  char *cmd = NULL;
  cReadLine reader;

  std::string plugindir;
  std::string visname;
  std::string visfile;
  std::string visBase;
   
  plugindir = config;
  plugindir = plugindir + "/visual/themes/";

#ifdef DEBUG
  isyslog("music: commands: load visfiles from '%s'", plugindir.c_str());
#endif

  int  len2 = plugindir.length()+1;

   
  if(asprintf(&cmd, FINDCMD, plugindir.c_str(), plugindir.c_str(), VISEXT) == -1)
    fprintf(stderr, "Failed to allocate string");
  
  FILE *p = popen(cmd, "r");

  if(p) {
    char *s;
    while ((s = reader.Read(p)) != NULL) {
      count++;
      visfile = s;
      int len = visfile.length();
      std::string::size_type pos = visfile.rfind('/',len);
      if(pos!=std::string::npos) {
        // Filename
        visname = visfile.substr(pos+1,len);
        // Basedir
        int sublen = len - len2 - visname.length()-1;
        visBase = visfile.substr(len2,sublen);
      }
      newitem = new cFileObjItem(plugindir.c_str(), visname.c_str(), otFile);
      if(newitem && newitem->Type()==otFile) {
        Add(new cMP3VisualItem(newitem, count));
      }
    }
    pclose(p);
  }
  else
    Skins.Message(mtError, tr("ERROR: Cant find any visualization!"));
    
  free(cmd);
}          
      
cFileObjItem *cMP3Visual::CurrentFileObjItem(void)
{
  cMP3VisualItem *item = (cMP3VisualItem *)Get(Current());
  return item ? item->Item():0;
}  

  
eOSState cMP3Visual::SetVis(void)
{
  int ItemIndex = Current();
  cFileObjItem *item=CurrentFileObjItem();

  if(ItemIndex == 0) {
    MP3Setup.EnableVis = false;
    std::string file;
    if(!MP3Setup.isWebstream) {
      file = config;
      file = file + "/themes/defaultcover/music-default-cover.png";
      strncpy(coverpicture,file.c_str(),sizeof(coverpicture));
      }
    else {
      file = config;
      file = file + "/themes/defaultcover/music-default-stream.png";
      strncpy(coverpicture,file.c_str(),sizeof(coverpicture));
    }
    return osBack;
  }
  else if (ItemIndex >= 1) {    
    if(item && item->Type()==otFile) {
      if(MP3VisLoader.LoadVis(item->Name())) {
        MP3Setup.EnableVis = true;
        MP3VisLoader.SetVis();
	strncpy(coverpicture,MP3VisLoader.VisLoaderBackground(),sizeof(coverpicture));

        if(MP3VisLoader.StoreVis(item->Name()))
	  isyslog("music: stored '%s' as 'current.vis'\n", item->Name());
        else
	  esyslog("music: could not store '%s' as 'current.vis'\n", item->Name());

        return osBack;
        }
      else    
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not load new visualization !"), fontFix));
    }
  }

  return osContinue;
}


eOSState cMP3Visual::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osUnknown) {
     switch(Key) {
       case kOk:
          state = SetVis();
	  break;     
       case kBlue:
       case kGreen:
       case kYellow:
       case kRed: return osBack;
       default: return osContinue;;
       }
     }
  return state;
}


// ------------ cMP3User --------------------------------------------------------------------------------------------------- //

class cMP3User : public cOsdMenu {
private:
  void SetHelp();
  void LoadUsername();
  eOSState SetUser(void);
  cOsdItem *Item(const char *text);
public:
  cMP3User(void);
  virtual eOSState ProcessKey(eKeys Key);
};


cMP3User::cMP3User(void)
:cOsdMenu(tr("Music: User"),4)
{
  LoadUsername();
  SetHelp();
}

void cMP3User::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


cOsdItem *cMP3User::Item(const char *text)
{
  char *buf=0;
  if(asprintf(&buf, "%s",text?text:"") == -1)
    fprintf(stderr, "Failed to allocate string for text");
  
  cOsdItem *item = new cOsdItem(buf,osUnknown,true);
  free(buf);
  Add(item); return item;
}  

void cMP3User::LoadUsername()
{
  std::ifstream filestr;
  std::string userfile;
  std::string line;
   
  userfile = config;
  userfile = userfile + "/data/user.dat";

#ifdef DEBUG
  isyslog("music: commands: load user.dat from '%s'", userfile.c_str());
#endif

  filestr.open(userfile.c_str());
  
  if(filestr) {
    while ((getline(filestr,line,'\n'))) {
      Item(line.c_str());
    } 
    filestr.close(); 
  }
}
      

eOSState cMP3User::SetUser(void)
{
  cOsdItem *item=(cOsdItem*)Get(Current());

  std::string datei;
  datei = config;
  datei = datei + "/data/current_user.dat";

#ifdef DEBUG
  isyslog("music: commands: load current_user.dat from '%s'", datei.c_str());
#endif


  if(item) {
    if(Interface->Confirm(tr("Change user (Music will be stopped) ?"))  ) {
      InfoCache.Save(true);
      if( FILE *f = fopen ( datei.c_str(), "w")) {
        std::string user;
        user = item->Text();
        fprintf(f, "%s\n", user.c_str());
        isyslog("music: change to user <%s>\n", user.c_str());
        fclose(f); 
        InfoCache.Load(true);
      } 
      return osEnd;
    }

  }
  else    
    return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not change user !"), fontFix));
  
  return osContinue;
}


eOSState cMP3User::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osUnknown) {
     switch(Key) {
       case kOk:
          state = SetUser();
	  break;     
       case kBlue:
       case kGreen:
       case kYellow:
       case kRed: return osBack;
       default: return osContinue;;
       }
     }
  return state;
}



// ------------ cMP3EditPlaylist ---------------------------------------------------------------------------------------------- //

cMP3EditPlaylist::cMP3EditPlaylist(const char *sz_Title)
:cOsdMenu(tr(sz_Title))
{
//0
  Add(new cOsdItem(hk(tr("Remove track from playlist"))));
//1
  Add(new cOsdItem(hk(tr("Remove all from playlist"))));
//2
  Add(new cOsdItem(hk(tr("Save active playlist"))));

  if(MP3Setup.AdminMode >= 2) {

//3
    cOsdItem *item = new cOsdItem(hk(""));
    Add(item, osUnknown);
    item->SetSelectable(false);

//4
    Add(new cOsdItem(hk(tr("Delete selected track from medium"))));
//5
    Add(new cOsdItem(hk(tr("Delete all tracks in playlist from medium"))));
  }


  SetHelp();
}


void cMP3EditPlaylist::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


eOSState cMP3EditPlaylist::Execute(void)
{

  int ItemIndex = Current();
  
//0/ Delete Track from playlist and save as current playlist in ConfigDir;;
  if (ItemIndex == 0) {

    if(mgr->maxIndex <= -1)
      return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not remove track !"), fontFix));


    if(MP3Setup.AdminMode >= 1) {

      if (mgr->maxIndex==0)
        MP3Setup.DeleteTracks = 1;
      else
        MP3Setup.DeleteTrack = 1;

        return osBack;
      }
    else {	
      if(Interface->Confirm(tr("Remove track from playlist ?"))  && Interface->Confirm(tr("Are you sure?"))  ) {
        if (mgr->maxIndex==0)
          MP3Setup.DeleteTracks = 1;
	      else
	        MP3Setup.DeleteTrack = 1;    

          return osBack;
      }
    }
  }      



//1/ Empty playlist;;
  if (ItemIndex == 1) {
    if(MP3Setup.AdminMode >= 1) {
        MP3Setup.DeleteTracks = 1;
        return osBack;
      }
    else {	
      if(Interface->Confirm(tr("Remove all tracks from playlist ?")) ) {
          Skins.Message(mtStatus, tr("Remove all tracks from playlist..."));
          MP3Setup.DeleteTracks = 1;
          Skins.Message(mtStatus, NULL);
          return osBack;
      }
    }
  }


//2/ Save playlist for burn..something with full path in ConfigDir;;
  if (ItemIndex == 2) {
    if(MP3Setup.AdminMode >= 1) {
      Skins.Message(mtStatus, tr("Save playlist..."));
      char *buf=0;
      if(asprintf(&buf, "%s%s", config, "/playlists/burnlist.m3u") == -1)
        fprintf(stderr, "Failed to allocate string");
#ifdef DEBUG
        isyslog("music: commands: save playlust to '%s'", buf);
#endif
      bool Result = mgr->SaveList(buf, true);
      Skins.Message(mtStatus, NULL);
      free(buf);
      if (Result)
        return AddSubMenu(new cMenuText(tr("Save playlist"), tr("Playlist saved as burnlist.m3u !"), fontFix));
      else
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not save playlist burnlist.m3u !"), fontFix));
      }
    else {
      if(Interface->Confirm(tr("Overwrite last playlist ?"))  ) {
        Skins.Message(mtStatus, tr("Save playlist..."));
        char *buf=0;
        if(asprintf(&buf, "%s%s", config, "/playlists/burnlist.m3u") == -1)
          fprintf(stderr, "Failed to allocate string");
#ifdef DEBUG
        isyslog("music: commands: save playlist to '%s'", buf);
#endif
        bool Result = mgr->SaveList(buf, true);
        Skins.Message(mtStatus, NULL);
        free(buf);
        if (Result)
          return AddSubMenu(new cMenuText(tr("Save playlist"), tr("Playlist saved as burnlist.m3u !"), fontFix));
        else
          return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not save playlist burnlist.m3u !"), fontFix));
      }
    }
  }


//3/ Delete file from medium
  if(MP3Setup.AdminMode >=1) {
    if (ItemIndex == 4) {

      if(mgr->maxIndex <= -1)
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not remove track !"), fontFix));

      if(MP3Setup.AdminMode == 2) {
        if (mgr->maxIndex==0) {
          RemoveFileOrDir(Songname, false);
          MP3Setup.DeleteTracks = 1;
          return osBack;
          }
	else {
          RemoveFileOrDir(Songname, false);
          MP3Setup.DeleteTrack = 1;
          return osBack;
          }
        }
      else {
        if(Interface->Confirm(tr("Delete file from medium ?"))  && Interface->Confirm(tr("Are you sure?"))  ) {
          if (mgr->maxIndex==0) {
            RemoveFileOrDir(Songname, false);
            MP3Setup.DeleteTracks = 1;
            return osBack;
            }
          else {
            RemoveFileOrDir(Songname, false);
            MP3Setup.DeleteTrack = 1;
            return osBack;
	  }
        }
      }
    }


//4/ Delete everything in playlist from medium
    if (ItemIndex == 5) {
      if(mgr->maxIndex <= -1)
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not remove tracks !"), fontFix));


      if(Interface->Confirm(tr("Delete all in playlist from medium ?"))  && Interface->Confirm(tr("Are you sure?"))  ) {
        Skins.Message(mtStatus, tr("Delete all tracks from medium..."));
        bool Result = DeleteList();
        Skins.Message(mtStatus, NULL);
        if (Result) {
          return AddSubMenu(new cMenuText(tr("Delete"), tr("Tracks terminated !"), fontFix));
          MP3Setup.DeleteTracks = 1;
          }
        else
          return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not delete tracks !"), fontFix));
      }
    }

  }

  return osContinue;
}

eOSState cMP3EditPlaylist::ProcessKey(eKeys Key)
{

  bool hadSubmenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (hadSubmenu && !HasSubMenu())
    return osBack;

  if (state == osUnknown) {
    switch (Key) {
        case kRed:
	  return osBack;
	break;
      case kOk:
          return Execute();
	break;
      default:
           state = osContinue;
    }
  }

  return state;
}


bool cMP3EditPlaylist::DeleteList(void)
{
  std::ifstream filestr;
  std::string datei;
  std::string line;
  int count=0;
  bool res = false;

  datei=config;
  datei=datei + "/playlists/deletelist.m3u";

#ifdef DEBUG
  isyslog("music: commands: get deletelist.m3u from '%s'", datei.c_str());
#endif

  if(mgr->SaveList(datei.c_str(), true)) {
    filestr.open (datei.c_str());
    while ((getline(filestr,line,'\n'))) {
      count++;
      isyslog("music: delete file : %i  %s\n",count,line.c_str());
      RemoveFileOrDir(line.c_str(), false);
    }
    filestr.close();
    res = true;
  }

  return res;
}



// ------------ cMP3Commands --------------------------------------------------------------------------------------------------- //

cMP3Commands::cMP3Commands(void)
:cOsdMenu(tr("Music: Commands"))
{
/* 0*/  Add(new cOsdItem(hk(tr("Edit playlist..."))));
/* 1*/  Add(new cOsdItem(hk(tr("Search for songs..."))));
/* 2*/  Add(new cOsdItem(hk(tr("Show ID3 information of song"))));
/* 3*/  Add(new cOsdItem(hk(tr("Edit ID3 information"))));
/* 4*/  Add(new cOsdItem(hk(tr("Quick settings..."))));
/* 5*/  Add(new cOsdItem(hk(tr("Search for lyric..."))));
/* 6*/  Add(new cOsdItem(hk(tr("Download coverpicture..."))));
/* 7*/  Add(new cOsdItem(hk(tr("Update webstreams..."))));
/* 8*/  Add(new cOsdItem(hk(tr("Start imageviewer"))));
/* 9*/  if(!MP3Setup.RecordStream)
          Add(new cOsdItem(hk(tr("Start Recording"))));
        else
          Add(new cOsdItem(hk(tr("Stop Recording"))));
/* 10*/  Add(new cOsdItem(hk(tr("Change Appearance..."))));
/* 11*/ Add(new cOsdItem(hk(tr("Visualizations..."))));
/* 12*/ Add(new cOsdItem(hk(tr("Change user..."))));


  SetHelp();
  LoadCommands();

  for (cCommand *command = commands.First(); command; command = commands.Next(command)) {
    cOsdItem *item = new cOsdItem(hk(command->Title()));
    Add(item, osUnknown);
    if(strstr(item->Text(), "----------"))
      item->SetSelectable(false);
    else
      item->SetSelectable(true);
  }

    Display();
}


cMP3Commands::~cMP3Commands()
{
}

void cMP3Commands::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}

void cMP3Commands::LoadCommands()
{
    std::string datei;
    datei = langdir;
    datei = datei + "/data/musiccmds.dat";
#ifdef DEBUG
    isyslog("music: commands: load commands from '%s%s'", config, datei.c_str());
#endif
    commands.Load(AddDirectory(config, datei.c_str()), true);
}


eOSState cMP3Commands::Execute(void)
{
  int ItemIndex = Current();

//0/ Edit Playlist;;
    if (ItemIndex == 0) {
      if(mgr->maxIndex <0)
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));
      else
        return AddSubMenu(new cMP3EditPlaylist("Edit playlist..."));
    }


//1/ Search;;
    if (ItemIndex == 1) {
      if(mgr->maxIndex >0) {
        std::string artist ="";
        cSongInfo *songinfo = mgr->Current()->Info(false);
        if(!songinfo) { songinfo = mgr->Current()->Info(); }
        if(songinfo && songinfo->Artist) { artist = songinfo->Artist; }
	return AddSubMenu(new cMP3Search(artist.c_str()));
	}
      else {
	return AddSubMenu(new cMP3Search(""));
      }
    }


//2/ Show ID3Info;;
    if (ItemIndex == 2) {
      if(mgr->maxIndex <0)
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));

      if(mgr->Current()) {
        return AddSubMenu(new cMP3SearchID3Info(mgr->curr));
	}
      else {
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not get songinfo !"), fontFix));
      }
    }	
    

//3/ Edit ID3 Tags
    if (ItemIndex == 3) {
      if(mgr->maxIndex <0) 
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));

      if(mgr->Current() && MP3Setup.user_is_admin && (MP3Setup.AdminMode >=2)) {
        return AddSubMenu(new cMP3id3Tag(mgr->curr));
	}
      else {
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: no songinfo or not allowed !"), fontFix));
      }

    }	


//4/ Some options/commands
    if (ItemIndex == 4) {
      if(mgr->maxIndex <0)
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));
      else
        return AddSubMenu(new cMP3Options());
    }


//5/ Show Lyrics;;
    if (ItemIndex == 5) {
      if(mgr->maxIndex <0) 
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));

      return AddSubMenu(new cMP3Lyrics());
    }


    if (ItemIndex == 1) {
      std::string artist ="";
      if(mgr->maxIndex >0) {
        cSongInfo *songinfo = mgr->Current()->Info(false);
        if(!songinfo) { songinfo = mgr->Current()->Info(); }
        if(songinfo && songinfo->Artist) { artist = songinfo->Artist; }
	return AddSubMenu(new cMP3Search(artist.c_str()));
	}
      else {
        return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not get songinfo !"), fontFix));
      }
    }


//6/ Download Cover;;
    if (ItemIndex == 6) {
      std::string::size_type pos;
      std::string artist   = "";
      std::string album    = "";
      std::string title    = "";
      std::string baseDir  = "";
      std::string filename = "";
      std::string Filenoext= "";

      if(mgr->maxIndex >=0) {
        cSongInfo *songinfo = mgr->Current()->Info(false);
        if(!songinfo) { 
          songinfo = mgr->Current()->Info();
        }

        if(songinfo) {
	  if(songinfo->Artist) {
	    artist = songinfo->Artist;
	  }
	  if(songinfo->Album) {
	    album  = songinfo->Album;
	  }
	  if(songinfo->Title) {
	    title  = songinfo->Title;
	  }

	  cSong *s = mgr->curr;
	
          if(s) {
	    filename = s->Fullname();
            // get dir
            int len = filename.length();
	    pos = filename.rfind("/",len);
	    if(pos != std::string::npos) {
	      baseDir = filename.substr(0, pos+1);
	    }  
            // get file w/o ext.
	    pos = filename.rfind(".",len);
	    if(pos != std::string::npos) {
	      Filenoext = filename.substr(0, pos+1);
            } 
	  }
          return AddSubMenu( new cMP3Cover(artist.c_str(), album.c_str(), title.c_str(), baseDir.c_str(), Filenoext.c_str()) );
          }
	}
//      else {
//	    return AddSubMenu( new cMP3Cover(artist.c_str(), album.c_str(), title.c_str(), baseDir.c_str(), Filenoext.c_str()) );
//      }
    }


//7/ Update Webstreams;;
    if (ItemIndex == 7) {
      return AddSubMenu(new cMP3UpdateWebStreams());
    }

//8/ Open Imageviewer;;
    if (ItemIndex == 8) {
      cRemote::CallPlugin("picselshow");
      return (osPlugin);
    }  


//9/ Start<->Stop Recording;;
    if (ItemIndex == 9) {

        if (MP3Setup.AdminMode >= 2) {
          if (MP3Setup.RecordStream) {
            StopRecord();
            return AddSubMenu(new cMenuText(tr("Stop Recording"), tr("Recording stopped !"), fontFix));
            }
          else {
            if (MP3Setup.isWebstream) {
              if(mgr->maxIndex <0) 
                return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));
              StartRecord();
              return AddSubMenu(new cMenuText(tr("Start Recording"), tr("Recording started !"), fontFix));
            }
	    else
              return AddSubMenu(new cMenuText(tr("ERROR:"), tr("What you want to record ?!"), fontFix));
          }
        }
        else {	
          if(MP3Setup.RecordStream) {
            if(Interface->Confirm(tr("Stop recording ?"))  ) {
              StopRecord();
              return AddSubMenu(new cMenuText(tr("Stop Recording"), tr("Recording stopped !"), fontFix));
            }
	  }    
          else {
            if (MP3Setup.isWebstream) {
              if(Interface->Confirm(tr("Start recording ?"))  ) {
                if(mgr->maxIndex <0) 
                  return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));
                StartRecord();
                return AddSubMenu(new cMenuText(tr("Start Recording"), tr("Recording started !"), fontFix));
              }
            }
            else
              return AddSubMenu(new cMenuText(tr("ERROR:"), tr("What you want to record ?!"), fontFix));
          }
        }
    }


//10/ Themes;;
    if (ItemIndex == 10) {
      return AddSubMenu(new cMP3Themes());
    }


//11/ Visualization;;
    if (ItemIndex == 11) {
      return AddSubMenu(new cMP3Visual());
    }


//12/ User;;
    if (ItemIndex == 12) {
      return AddSubMenu(new cMP3User());
    }



// From NOW on execute usercommands;;
  if (ItemIndex >= 13) {
    cCommand *command = commands.Get(ItemIndex-13);
    if (command) {
      char *buffer = NULL;
      bool confirmed = true;

      std::string Artist ="";
      std::string Album  =""; 
      std::string Coverdir ="";
    
      cSongInfo *si = mgr->Current()->Info(false);
      if(!si) si    = mgr->Current()->Info();
      if(si && si->Artist)Artist = si->Artist;
      if(si && si->Album) Album  = si->Album;
      if (command->Confirm()) {
        if(asprintf(&buffer, "%s?", command->Title()) == -1)
          fprintf(stderr, "Failed to allocate string for command->Title()");
        confirmed = Interface->Confirm(buffer);
        free(buffer);
      }
      if (confirmed) {
        if(asprintf(&buffer, "%s...", command->Title()) == -1)
          fprintf(stderr, "Failed to allocate string for command->Title()");
        Skins.Message(mtStatus, buffer);
        free(buffer);

        if(asprintf(&buffer, "\"%s\"  \"%s\"  \"%s\"  \"%s\"  \"%s\" \"%s\"", Songname, MP3Setup.CopyDir, Artist.c_str(), Album.c_str(), MP3Setup.CoverDir, config) == -1)
          fprintf(stderr, "Failed to allocate string");

        const char *Result = command->Execute(buffer);

        d(printf("music: commands: Executed: '%s'\n", buffer));
#ifdef DEBUG
	isyslog("music: commands: executed: '%s'\n", buffer);
#endif
        free(buffer);
        Skins.Message(mtStatus, NULL);
        if(Result) return AddSubMenu(new cMenuText(command->Title(), Result, fontFix));
	return osBack;
      }
    }
  }

  return osContinue;
}

// ------------------ STARTRECORD -----------------------//
void cMP3Commands::StartRecord(void)
{
  FILE *recordcmd;
  char *buffer;

  std::string url;
  url = "\"";
  url = url +  urlname;
  url = url + "\"";


  d(printf("music: commands: Started Recording\n"));
  if(asprintf(&buffer, "screen -A -m -d -S RECORD /usr/bin/streamripper %s -d '%s' -w %s%s/data/parse_rules.txt %s", url.c_str(), MP3Setup.RecordDir, config, langdir, MP3Setup.RecordOpts) == -1)
    fprintf(stderr, "Failed to allocate string");
  
  recordcmd = popen(buffer, "r");
  d(printf("music: commands: Starting recording '%s'\n", buffer));
#ifdef DEBUG
  isyslog("music: commands: starting recording '%s'", buffer);
#endif
  pclose(recordcmd);
  
  free(buffer);
  MP3Setup.RecordStream = 1;
}

// ------------------ STOPRECORD -----------------------//
void cMP3Commands::StopRecord(void)
{
  FILE *stoprecordcmd;
  char *buffer;

  d(printf("music: commands: Stopped Recording\n"));
  if(asprintf(&buffer, "killall -15 streamripper") == -1)
    fprintf(stderr, "Failed to allocate string");
  
  stoprecordcmd = popen(buffer, "r");
  d(printf("music: commands: Stop recording '%s'\n", buffer));
#ifdef DEBUG
  isyslog("music: commands: stop recording '%s'", buffer);
#endif
  pclose(stoprecordcmd);
      
  free(buffer);
  MP3Setup.RecordStream = 0;
}

eOSState cMP3Commands::ProcessKey(eKeys Key)
{
  bool hadSubmenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (hadSubmenu && !HasSubMenu())
    return osBack;

  if (state == osUnknown) {
    switch (Key) {
        case kRed:
	  return osBack;
	break;  
      case kOk:
          return Execute();
	break;  
      default:
           state = osContinue;
    }
  }
  
  return state;
}        		
