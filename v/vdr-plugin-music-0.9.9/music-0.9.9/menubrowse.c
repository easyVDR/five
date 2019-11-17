
#include <string>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <typeinfo>

#include <vdr/menuitems.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/status.h>

#include "mp3control.h"

#include "menubrowse.h"
#include "i18n.h"
#include "data-mp3.h"
#include "decoder.h"
#include "search.h"

#include "player-mp3.h"
#include "vars.h"
#include "icons.h"

//--- cMenuID3Info -------------------------------------------------------------

cMenuID3Info::cMenuID3Info(cSong *song)
:cOsdMenu(tr("Music: ID3 Information"),12)
{
  Build(song->Info(),song->Name());
}

cMenuID3Info::cMenuID3Info(cSongInfo *si, const char *name)
:cOsdMenu(tr("Music: ID3 Information"),12)
{
  Build(si,name);
}

void cMenuID3Info::Build(cSongInfo *si, const char *name)
{
  if(si) {
    Item(tr("Filename"),name);
    if(si->HasInfo() && si->Total>0) {
      char *buf=0;
      asprintf(&buf,"%02d:%02d",si->Total/60,si->Total%60);
      Item(tr("Length"),buf);
      free(buf);
      Item(tr("Title"),si->Title);
      Item(tr("Artist"),si->Artist);
      Item(tr("Album"),si->Album);

	  int rating = si->Rating;
  	  std::string rate2text;
   
      switch(rating) {
		case 0:
			rate2text = ""; //empty
			break;
		case 3:
			rate2text = Icons::Menu_Bomb(); //bombe
			break;
		case 28:
			rate2text = Icons::Menu_Half_Star(); //halb
			break;
		case 53:
			rate2text = Icons::Menu_Full_Star(); //ganz
			break;
		case 78:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Half_Star();
			break;
		case 104:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			break;
		case 129:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Half_Star();
			break;
		case 154:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			break;
		case 179:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Half_Star();
			break;
		case 205:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			break;
		case 230:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Half_Star();
			break;
		case 255:
			rate2text = Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			rate2text = rate2text + Icons::Menu_Full_Star();
			break;
		default:
			rate2text = ""; //empty
			break;  
	  }

      Item(tr("Rating"),rate2text.c_str());
      Item(tr("Year"),0,(float)si->Year);
      Item(tr("Genre"),si->Genre);
      Item(tr("Samplerate"),"%.1f kHz",si->SampleFreq/1000.0);
      Item(tr("Bitrate"),"%.f kbit/s",si->Bitrate/1000.0);
      Item(tr("Channels"),0,(float)si->Channels);
      Item(tr("Comment"),si->Comment);
      }
    Display();
    }
}

cOsdItem *cMenuID3Info::Item(const char *name, const char *format, const float num)
{
  cOsdItem *item;
  if(num>=0.0) {
    char *buf=0;
    asprintf(&buf,format?format:"%.f",num);
    item=Item(name,buf);
    free(buf);
    }
  else item=Item(name,"");
  return item;
}

cOsdItem *cMenuID3Info::Item(const char *name, const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s:\t%s",name,text?text:"");
  cOsdItem *item = new cOsdItem(buf,osBack);
//  item->SetSelectable(false);
  free(buf);
  Add(item); return item;
}

eOSState cMenuID3Info::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osUnknown) {
     switch(Key) {
       case kRed:
       case kGreen:
       case kYellow:
       case kOk:
       case kBlue:   return osBack;
//       case kMenu:   return osEnd;
       default: return osContinue;;
       }
     }
  return state;
}


//--- cMenuInstantBrowse -------------------------------------------------------

cMenuInstantBrowse::cMenuInstantBrowse(cFileSource *Source, const char *Selecttext, const char *Alltext)
:cMenuBrowse(Source,true,true,tr("Directory browser"),excl_br)
{
//  excl=excl_br; // defined in data-mp3.c
  playnow = false;
  selecttext=Selecttext; alltext=Alltext;
  SetButtons();
}

cMenuInstantBrowse::~cMenuInstantBrowse(void)
{
}


void cMenuInstantBrowse::SetButtons(void)
{
//  SetHelp(selecttext, currentdir?tr("Parent"):0, currentdir?0:alltext, tr("ID3 info"));
  SetHelp(selecttext, tr("Search"), currentdir?tr("Play now"):alltext, tr("ID3 info"));
  Display();
}

eOSState cMenuInstantBrowse::ID3Info(void)
{
  cFileObj *item=CurrentItem();
  if(item && item->Type()==otFile) {
    cSong *song=new cSong(item);
    cSongInfo *si;
    if(song && (si=song->Info())) {
      AddSubMenu(new cMenuID3Info(si,item->Path()));
      }
    delete song;
    }
  return osContinue;
}

eOSState cMenuInstantBrowse::PlayNow(void)
{
  int PlayIndex;

  cFileObj *item=CurrentItem();
  if(item && item->Type()==otFile) {
    cInstantPlayList *newpl = new cInstantPlayList(item);
    if(newpl->Load() && newpl->Count()) {
      isyslog("music: now play '%s'", newpl->Name());

      if(!mgr->playnow) {
        std::string real_pl;
        real_pl = config;
	real_pl = real_pl + "/playlists/realpl.m3u";
        mgr->SaveList(real_pl.c_str(),true); //playlist,all
        }
      else {
        printf("music: Playnow: true: DeleteTrack\n");
      }

      mgr->playnow = true;
      cMP3Control::SetPlayList(newpl);

      PlayIndex = mgr->maxIndex +1;
      mgr->Goto(PlayIndex);
//      Info(tr("Playing..."));

      return osContinue;
    }
    delete newpl;
    Error(tr("Error building playlist!"));
    }
  else {
    dsyslog("music: PlayNow: item is not a file\n");    
    Error(tr("Item is not a file"));
  }    
  return osContinue;
}

eOSState cMenuInstantBrowse::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osUnknown) {
    switch (Key) {
        case kYellow:
            if(!currentdir) {
              lastselect=new cFileObj(source,0,0,otBase);
              return osBack;
              }
            else  
              state = PlayNow();
            break;

        case kGreen:
            return AddSubMenu( new cMP3Search("") );

        default: break;
    }
  }

  if(state==osUnknown) state=cMenuBrowse::ProcessStdKey(Key,state);
  return state;
}


// --- cMenuPlayListItem -------------------------------------------------------

cMenuPlayListItem::cMenuPlayListItem(cSong *Song, bool showid3)
{
  song=Song;
  Set(showid3);
}

void cMenuPlayListItem::Set(bool showid3)
{
  showID3=showid3;
  Set();
}

void cMenuPlayListItem::Set(void)
{
  char *buffer=0;
  cSongInfo *si=song->Info(false);
  if(showID3 && !si) si=song->Info();
  if(showID3 && si && si->Title)
    asprintf(&buffer, "%d.\t%s%s%s",song->Index()+1,si->Title,si->Artist?" - ":"",si->Artist?si->Artist:"");
  else
    asprintf(&buffer, "%d.\t<%s>",song->Index()+1,song->Name());
  SetText(buffer,false);
}


// --- cMenuPlayList -----------------------------------------------------------

cMenuPlayList::cMenuPlayList(cPlayList *Playlist)
:cOsdMenu(tr("Playlist editor"),4)
{
  browsing=showid3=false;
  playlist=Playlist;
  if(MP3Setup.EditorMode) showid3=true;

  cSong *mp3 = playlist->First();
  while(mp3) {
    cOsdMenu::Add(new cMenuPlayListItem(mp3,showid3));
    mp3 = playlist->cList<cSong>::Next(mp3);
    }
  Buttons(); Display();
}

void cMenuPlayList::Buttons(void)
{
  SetHelp(tr("Add"), showid3?tr("Filenames"):tr("ID3 names"), tr("Remove"), trVDR(BUTTON"Mark"));
}

void cMenuPlayList::Refresh(bool all)
{
  cMenuPlayListItem *cur=(cMenuPlayListItem *)((all || Count()<2) ? First() : Get(Current()));
  while(cur) {
    cur->Set(showid3);
    cur=(cMenuPlayListItem *)Next(cur);
    }
}

void cMenuPlayList::Add(void)
{
  cFileObj *item=cMenuInstantBrowse::GetSelected();
  if(item) {
    Status(tr("Scanning directory..."));
    cInstantPlayList *newpl=new cInstantPlayList(item);
    if(newpl->Load()) {
      if(newpl->Count()) {
        if(newpl->Count()==1 || Interface->Confirm(tr("Add recursivly?"))) {
          cSong *mp3=newpl->First();
          while(mp3) {
            cSong *n=new cSong(mp3);
            if(Count()>0) {
              cMenuPlayListItem *current=(cMenuPlayListItem *)Get(Current());
              playlist->Add(n,current->Song());
              cOsdMenu::Add(new cMenuPlayListItem(n,showid3),true,current);
              }
            else {
              playlist->Add(n);
              cOsdMenu::Add(new cMenuPlayListItem(n,showid3),true);
              }
            mp3=newpl->cList<cSong>::Next(mp3);
            }
          playlist->Save();
          Refresh(); Display();
          }
        }
      else Error(tr("Empty directory!"));
      }
    else Error(tr("Error scanning directory!"));
    delete newpl;
    Status(0);
    }
}

void cMenuPlayList::Move(int From, int To)
{
  playlist->Move(From,To); playlist->Save();
  cOsdMenu::Move(From,To);
  Refresh(true); Display();
}

eOSState cMenuPlayList::ShowID3(void)
{
  showid3=!showid3;
  Buttons(); Refresh(true); Display();
  return osContinue;
}

eOSState cMenuPlayList::ID3Info(void)
{
  if(Count()>0) {
    cMenuPlayListItem *current = (cMenuPlayListItem *)Get(Current());
    AddSubMenu(new cMenuID3Info(current->Song()));
    }
  return osContinue;
}

eOSState cMenuPlayList::Remove(void)
{
  if(Count()>0) {
    cMenuPlayListItem *current = (cMenuPlayListItem *)Get(Current());
    if(Interface->Confirm(tr("Remove entry?"))) {
      playlist->Del(current->Song()); playlist->Save();
      cOsdMenu::Del(Current());
      Refresh(); Display();
      }
    }
  return osContinue;
}

eOSState cMenuPlayList::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(browsing && !HasSubMenu() && state==osContinue) { Add(); browsing=false; }

  if(state==osUnknown) {
     switch(Key) {
       case kOk:     return ID3Info();
       case kRed:    browsing=true;
                     return AddSubMenu(new cMenuInstantBrowse(MP3Sources.GetSource(),tr("Add"),tr("Add all")));
       case kGreen:  return ShowID3();
       case kYellow: return Remove();
       case kBlue:   Mark(); return osContinue;
       case kMenu:   return osEnd;
       default: break;
       }
     }
  return state;
}


// --- cPlaylistRename ---------------------------------------------------------

char *cPlaylistRename::newname = NULL;

cPlaylistRename::cPlaylistRename(const char *Oldname)
:cOsdMenu(tr("Rename playlist"), 15)
{
  free(newname); newname=0;
  oldname=Oldname;
  char *buf=NULL;
  asprintf(&buf,"%s\t%s",tr("Old name:"),oldname);
  cOsdItem *old = new cOsdItem(buf,osContinue);
  old->SetSelectable(false);
  Add(old);
  free(buf);

  data[0]=0;
  Add(new cMenuEditStrItem( tr("New name"), data, sizeof(data)-1, tr(FileNameChars)),true);
}

eOSState cPlaylistRename::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     if(data[0] && strcmp(data,oldname)) newname=strdup(data);
                     return osBack;
       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;
       default: break;
       }
     }
  return state;
}


// --- cMenuMP3Item ------------------------------------------------------------

cMenuMP3Item::cMenuMP3Item(cPlayList *PlayList)
{
  playlist=PlayList;
  Set();
}

void cMenuMP3Item::Set(void)
{
  char *buffer=0;
  asprintf(&buffer," %s",playlist->BaseName());
  SetText(buffer,false);
}


// --- cMenuMP3 ----------------------------------------------------------------

cMenuMP3::cMenuMP3(void)
:cOsdMenu(tr("Music"))
{
  renaming=sourcing=instanting=false;
  lists=new cPlayLists;
  ScanLists(); SetButtons(1);
  if(MP3Setup.MenuMode) Instant(false);
}

cMenuMP3::~cMenuMP3(void)
{
  delete lists;
}

eOSState cMenuMP3::SetButtons(int num)
{
  switch(num) {
    case 1:
      SetHelp(trVDR("Button$Edit"), tr("Source"), tr("Browse"), ">>");
      break;
    case 2:
      SetHelp("<<", trVDR("Button$New"), trVDR("Button$Delete"), ">>");
      break;
    case 3:
      SetHelp("<<", tr("Convert"), NULL, tr("Rename"));
    }
  buttonnum=num; Display();
  return osContinue;
}

void cMenuMP3::ScanLists(void)
{
  Clear();
  Status(tr("Scanning playlists..."));
//  isyslog("music: scanning playlists...");
  bool res=lists->Load(MP3Sources.GetSource());
  Status(0);
  if(res) {
    cPlayList *plist=lists->First();
    while(plist) {
      Add(new cMenuMP3Item(plist));
      plist=lists->Next(plist);
      }
    }
  else Error(tr("Error scanning playlists!"));
}

eOSState cMenuMP3::Delete(void)
{
  if(Count()>0) {
    if(Interface->Confirm(tr("Delete playlist?")) &&
       Interface->Confirm(tr("Are you sure?")) ) {
      cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
      if(plist->Delete()) {
        lists->Del(plist);
        cOsdMenu::Del(Current());
        Display();
        }
      else Error(tr("Error deleting playlist!"));
      }
    }
  return osContinue;
}

eOSState cMenuMP3::New(void)
{
  cPlayList *plist=new cPlayList(MP3Sources.GetSource(),0,0);
  char name[32];
  int i=0;
  do {
    if(i) sprintf(name,"%s%d",tr("unnamed"),i++);
    else { strcpy(name,tr("unnamed")); i++; }
    } while(plist->TestName(name));

  if(plist->Create(name)) {
    lists->Add(plist);
    Add(new cMenuMP3Item(plist), true);

    isyslog("music: playlist %s added", plist->Name());
    return AddSubMenu(new cMenuPlayList(plist));
    }
  Error(tr("Error creating playlist!"));
  delete plist;
  return osContinue;
}

eOSState cMenuMP3::Convert(void)
{
  if(Count()>0) {
    if(Interface->Confirm(tr("Convert playlist?"))) {

      std::string cmd;
      cmd = config;
      cmd = cmd + langdir;
      cmd = cmd  + "/scripts/music_convertwinamp.sh";

      cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
      isyslog("music: convert playlist '%s'", plist->FullListPath());
  
      char *buff;
      asprintf(&buff, "%s '%s' '%s'", cmd.c_str(), plist->FullListPath(), MP3Setup.ReplaceString);
#ifdef DEBUG
      isyslog("music: executing: '%s'", buff);
#endif
      system(buff);
      free(buff);
#ifdef DEBUG
      dsyslog("music: playlist %s converted", plist->Name());
#endif

      ScanLists();
      Display();
    }
  }
  return osContinue;
}


eOSState cMenuMP3::Rename(bool second)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
  if(!second) {
    renaming=true;
    return AddSubMenu(new cPlaylistRename(plist->BaseName()));
    }
  renaming=false;
  const char *newname=cPlaylistRename::GetNewname();
  if(newname) {
    if(plist->Rename(newname)) {
      RefreshCurrent();
      DisplayCurrent(true);
      }
    else Error(tr("Error renaming playlist!"));
    }
  return osContinue;
}

eOSState cMenuMP3::Edit(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
  if(!plist->Load()) Error(tr("Error loading playlist!"));
  else if(!plist->IsWinAmp()) {
    isyslog("music: editing playlist %s", plist->Name());
    return AddSubMenu(new cMenuPlayList(plist));
    }
  else Error(tr("Can't edit a WinAmp playlist!"));
  return osContinue;
}

eOSState cMenuMP3::Play(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  Status(tr("Loading playlist..."));
  cPlayList *newpl=new cPlayList(((cMenuMP3Item *)Get(Current()))->List());
  if(newpl->Load() && newpl->Count()) {
    isyslog("music: playback started with playlist %s", newpl->Name());
    cMP3Control::SetPlayList(newpl);
    if(MP3Setup.KeepSelect) {
      Status(0);
      return osContinue;
    }
    Status(0);
    return osEnd;
    }
  Status(0);
  delete newpl;
  Error(tr("Error loading playlist!"));
  return osContinue;
}

eOSState cMenuMP3::Source(bool second)
{
  if(HasSubMenu()) return osContinue;

  if(!second) {
    sourcing=true;
    return AddSubMenu(new cMenuSource(&MP3Sources,tr("MP3 source")));
    }
  sourcing=false;
  cFileSource *src=cMenuSource::GetSelected();
  if(src) {
    MP3Sources.SetSource(src);
    ScanLists();
    Display();
    }
  return osContinue;
}

eOSState cMenuMP3::Instant(bool second)
{
  if(HasSubMenu()) return osContinue;

  if(!second) {
    instanting=true;
    return AddSubMenu(new cMenuInstantBrowse(MP3Sources.GetSource(),tr("Add"),tr("Add all")));
    }
  instanting=false;
  cFileObj *item=cMenuInstantBrowse::GetSelected();
  if(item) {
    Status(tr("Building playlist..."));
    cInstantPlayList *newpl = new cInstantPlayList(item);
    if(newpl->Load() && newpl->Count()) {
      isyslog("music: playback started with instant playlist %s", newpl->Name());
      cMP3Control::SetPlayList(newpl);

      if(MP3Setup.KeepSelect) {
        Status(0);
        return Instant(false);
      }
      Status(0);
      return osEnd;
      }
    Status(0);
    delete newpl;
    Error(tr("Error building playlist!"));
    }
  return osContinue;
}

eOSState cMenuMP3::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(!HasSubMenu() && state==osContinue) { // eval the return value from submenus
    if(renaming) return Rename(true);
    if(sourcing) return Source(true);
    if(instanting) return Instant(true);
    }

  if(state == osUnknown) {
    switch(Key) {
      case kOk:     return Play();
      case kRed:    return (buttonnum==1 ? Edit() : buttonnum==2 ? SetButtons(1) : SetButtons(2));
      case kGreen:  return (buttonnum==1 ? Source(false) : buttonnum==2 ? New() : Convert());
      case kYellow: return (buttonnum==1 ? Instant(false) : buttonnum==2 ? Delete() : SetButtons(3));
      case kBlue:   return (buttonnum==1 ? SetButtons(2) : buttonnum==2 ? SetButtons(3) : Rename(false));
      case kMenu:   return osEnd;
      default:      break;
      }
    }
  return state;
}
