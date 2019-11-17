#include <string>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include <vdr/status.h>

#include "cover.h"
#include "i18n.h"
#include "setup-mp3.h"
#include "vars.h"
#include "funct.h"

//---------------- cMP3Cover --------------------------------------------------------------------------------------------------------------------------

cMP3Cover::cMP3Cover(const char *Artist, const char *Album, const char *Title, const char *Basedir, const char *FileNoExt)
:cOsdMenu(tr("Music: Coverpicture"),20)
{
  strncpy(artist, Artist, sizeof(artist));
  strncpy(album, Album, sizeof(album));
  strncpy(title, Title, sizeof(title));
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
  std::string sz_title("");
  sz_artist = artist;
  sz_album  = album;
  sz_title  = title;

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

    replaceAll(sz_title, "\"", "");
    replaceAll(sz_title, "/", "");
    replaceAll(sz_title, "Ä", "Ae");
    replaceAll(sz_title, "ä", "ae");
    replaceAll(sz_title, "Ö", "Oe");
    replaceAll(sz_title, "ö", "oe");
    replaceAll(sz_title, "Ü", "Ue");
    replaceAll(sz_title, "ü", "ue");
    replaceAll(sz_title, "ß", "ss");
    replaceAll(sz_title, "_", " ");
  }

  strncpy(artist   ,sz_artist.c_str() ,sizeof(artist));
  strncpy(album    ,sz_album.c_str()  ,sizeof(album));
  strncpy(title    ,sz_title.c_str()  ,sizeof(title));

  Add(new cOsdItem(hk(tr("Reset query"))));
  Add(new cOsdItem(hk(tr("Execute query for album"))));
  Add(new cOsdItem(hk(tr("Execute query for title"))));
  Item("");
  Add(new cMenuEditStrItem(tr("Artist")         ,artist , sizeof(artist) ));
  Add(new cMenuEditStrItem(tr("Album")          ,album  , sizeof(album) ));
  Add(new cMenuEditStrItem(tr("Title")          ,title  , sizeof(title) ));

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
  std::string m_item("");
  m_item = text;
  cOsdItem *item = new cOsdItem(m_item.c_str(),osBack);

////  char *buf=0;
////  asprintf(&buf,"%s",text?text:"");
////  cOsdItem *item = new cOsdItem(buf,osBack);
  item->SetSelectable(false);
////  free(buf);
  Add(item); return item;
}


eOSState cMP3Cover::SearchForAlbum(void) {

  std::string m_buffer("");

  if(artist) {
      std::string datei;
      std::string Cmd;
      datei = config;
      datei = datei + "/downloads";

      Cmd   = config;
      Cmd   = Cmd + langdir;
      Cmd   = Cmd + "/scripts/music_getcover.pl";

      m_buffer = Cmd;
      m_buffer = m_buffer + " \"" + artist + "\"" + " \"" + album + "\"" + " \"" + MP3Setup.CoverDir + "\"" + " \"" + datei + "\"" + " \"";
      m_buffer = m_buffer + "0";
      m_buffer = m_buffer + "\"" + " \"" + basedir + "\"" + " \"" + filenoext + "\"";



      SystemExec(m_buffer.c_str(), true);
#ifdef DEBUG
      dsyslog("music: Executing: %s", m_buffer.c_str());
#endif
  }
  else {
    return AddSubMenu(new cMenuText(tr("ERROR"), tr("ERROR: Field 'Artist' empty.. !"), fontFix));
  }

  return osBack;
}


eOSState cMP3Cover::SearchForTitle(void) {

  std::string m_buffer("");

  if(artist) {
      std::string datei;
      std::string Cmd;
      datei = config;
      datei = datei + "/downloads";

      Cmd   = config;
      Cmd   = Cmd + langdir;
      Cmd   = Cmd + "/scripts/music_getcover.pl";

      m_buffer = Cmd;
      m_buffer = m_buffer + " \"" + artist + "\"" + " \"" + title + "\"" + " \"" + MP3Setup.CoverDir + "\"" + " \"" + datei + "\"" + " \"";
      m_buffer = m_buffer + "0";
      m_buffer = m_buffer + "\"" + " \"" + basedir + "\"" + " \"" + filenoext + "\"";



      SystemExec(m_buffer.c_str(), true);
#ifdef DEBUG
      dsyslog("music: Executing: %s", m_buffer.c_str());
#endif
  }
  else {
    return AddSubMenu(new cMenuText(tr("ERROR"), tr("ERROR: Field 'Artist' empty.. !"), fontFix));
  }

  return osBack;
}




void cMP3Cover::ClearValues(void) {

  strncpy(artist   ,"" ,sizeof(artist));
  strncpy(album    ,"" ,sizeof(album));
  strncpy(title    ,"" ,sizeof(title));

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
	  else if(current ==2) {
	    state = SearchForTitle();
	    }
	  else {
	    state = SearchForAlbum();
	  }
	  break;

	case kGreen:
	case kYellow:

        default: state = osContinue;
    }
  }

  return state;
}
