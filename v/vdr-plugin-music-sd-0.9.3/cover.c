#include <string>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include <vdr/status.h>

#include "cover.h"
#include "i18n.h"
#include "setup-mp3.h"
#include "vars.h"
#include "stringfunct.h"

//---------------- cMP3Cover --------------------------------------------------------------------------------------------------------------------------

cMP3Cover::cMP3Cover(const char *Artist, const char *Album, const char *Basedir, const char *FileNoExt)
:cOsdMenu(tr("Music-SD: Coverpicture"),20)
{
  strncpy(artist, Artist, sizeof(artist));
  strncpy(album, Album, sizeof(album));
  strncpy(basedir, Basedir, sizeof(basedir));
  strncpy(filenoext, FileNoExt, sizeof(filenoext));

  ShowDisplay();
}

cMP3Cover::~cMP3Cover()
{
}

void cMP3Cover::ShowDisplay(void)
{
  Clear();


  std::string sz_artist("");
  std::string sz_album("");
  sz_artist = artist;
  sz_album  = album;

  if(sz_artist > "") {
    replaceAll(sz_artist, "\"", "");
    replaceAll(sz_artist, "/", "");
    replaceAll(sz_artist, "Ä", "Ae");
    replaceAll(sz_artist, "ä", "ae");
    replaceAll(sz_artist, "Ö", "Oe");
    replaceAll(sz_artist, "ö", "oe");
    replaceAll(sz_artist, "Ü", "Ue");
    replaceAll(sz_artist, "ü", "ue");
    replaceAll(sz_artist, "ß", "ss");
    replaceAll(sz_artist, "_", " ");

    replaceAll(sz_album, "\"", "");
    replaceAll(sz_album, "/", "");
    replaceAll(sz_album, "Ä", "Ae");
    replaceAll(sz_album, "ä", "ae");
    replaceAll(sz_album, "Ö", "Oe");
    replaceAll(sz_album, "ö", "oe");
    replaceAll(sz_album, "Ü", "Ue");
    replaceAll(sz_album, "ü", "ue");
    replaceAll(sz_album, "ß", "ss");
    replaceAll(sz_album, "_", " ");
  }

  strncpy(artist   ,sz_artist.c_str() ,sizeof(artist));
  strncpy(album    ,sz_album.c_str()  ,sizeof(album));

  Add(new cOsdItem(hk(tr("Reset query"))));
  Add(new cOsdItem(hk(tr("Execute query"))));
  Item("");
  Add(new cMenuEditStrItem(tr("Artist")         ,artist , sizeof(artist) ));
  Add(new cMenuEditStrItem(tr("Album")          ,album  , sizeof(album) ));

  SetCurrent(Get(1));
  SetHelp();

  Display();
}


void cMP3Cover::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


cOsdItem *cMP3Cover::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osBack);
  item->SetSelectable(false);
  free(buf);
  Add(item); return item;
}


eOSState cMP3Cover::Searching(void) {

  char *buffer = NULL;
  bool confirmed = true;

  if(artist) {
    asprintf(&buffer, "%s?", tr("Search for cover"));
    confirmed = Interface->Confirm(buffer);
    free(buffer);

    if(confirmed) {
      asprintf(&buffer, "%s...", tr("Searching"));
      Skins.Message(mtStatus, buffer);
      free(buffer);

      std::string datei;
      std::string Cmd;
      std::string result;
      datei = config;
      datei = datei + "/downloads/music_cover";
      Cmd   = config;
      Cmd   = Cmd + "/" + langdir;
      Cmd   = Cmd + "/scripts/music_getcover.pl";
      result = datei;
      result = result + "/cover_0.jpg";
    
      asprintf(&buffer, "%s \"%s\" \"%s\" \"%s\" \"%s\" %i \"%s\" \"%s\"" , Cmd.c_str(), artist, album, MP3Setup.CoverDir, datei.c_str(), MP3Setup.MaxCover, basedir, filenoext);
      system(buffer);
      isyslog("music-sd: Executing: %s", buffer);
    
      free(buffer);
      Skins.Message(mtStatus, NULL);

      if(FILE *f = fopen(result.c_str(), "r")) {
        fclose(f);

        if(cRemote::CallPlugin("coverviewer"))
	  return (osPlugin);
        else 
          return AddSubMenu(new cMenuText(tr("ERROR"), tr("ERROR: Could not open Coverviewer or not installed.. !"), fontFix));

        }
      else {
        return AddSubMenu(new cMenuText(tr("ERROR"), tr("ERROR: While getting cover. Watch logs.. !"), fontFix));
      }  
    }
  }
  else {
    return AddSubMenu(new cMenuText(tr("ERROR"), tr("ERROR: Field 'Artist' empty.. !"), fontFix));
  }

  return osContinue;
}


void cMP3Cover::ClearValues(void) {

  strncpy(artist   ,"" ,sizeof(artist));
  strncpy(album    ,"" ,sizeof(album));

  ShowDisplay();
}



eOSState cMP3Cover::ProcessKey(eKeys Key)
{
  int current = Current();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kRed:     
        case kBack: state = osBack; 
          break;

        case kOk:
	  if(current ==0) {
            ClearValues();
	    state = osContinue;
	    }
	  else         
	    state = Searching();
	  break;

	case kGreen:
	case kYellow:

        default: state = osContinue;
    }
  }

  return state;
}
