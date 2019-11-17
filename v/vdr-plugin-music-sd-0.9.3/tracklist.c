#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>

#include "decoder.h"
#include "tracklist.h"
#include "i18n.h"
#include "mp3id3.h"
#include "mp3id3tag.h"
#include "vars.h"
#include "icons.h"
#include "skin.h"

#define INPUTTIMEOUT 1000
//  pFontSymbol      = MusicConfig.GetFont(FONT_SYMBOL);
// ------------ cTracklistSave ----------------------------------------------------------------------------------------------- //

cTracklistSave::cTracklistSave(void)
:cOsdMenu(tr("Save tracklist as"), 15)
{
  newname=0;
  data[0]=0;
  Add(new cMenuEditStrItem( tr("New name"), data, sizeof(data)-1, tr(FileNameChars)),true);
  SetHelp();
}

void cTracklistSave::SetHelp() {
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Execute"));
  Display();
}


eOSState cTracklistSave::Rename(void) {
  if(newname) {
    std::string filename("");
	std::string base("");
    std::string basedir("");
	
    cSong *s = mgr->curr;

	filename = s->Fullname();
	int len1 = filename.length();
	basedir  = s->BASEDIR();
	int len2 = basedir.length();
	int len3 = len1 - len2 ;
	base     = filename.substr(0,len3);

	base = base + newname;
	base = base + ".m3u";

	bool Result = mgr->SaveList(base.c_str(), false);
	if (!Result) {
		esyslog("music-sd: tracklist: could not save playlist : '%s'", base.c_str());
		return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not save playlist"), fontFix));
	    }
	else
	    isyslog("music-sd: tracklist: saved tracklist to '%s'", base.c_str());

  }
  
  return osBack;
}



eOSState cTracklistSave::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk: 
       case kBlue:		if(data[0]) newname=strdup(data);    return Rename();

       case kRed:		return osBack; break;
	   
       case kGreen:
       case kYellow:	return osContinue;

       default: break;
       }
     }
  return state;
}

// ------------ cTrackItem ----------------------------------------------------------------------------------------------- //

cTrackItem::eTrackSortMode cTrackItem::sortMode = tsNumber;
  
cTrackItem::cTrackItem(cSong *Song) {
  song=Song;
  si=song->Info(false);
  if(!si) si=song->Info();
  Set();
}

cTrackItem::~cTrackItem(){
}

int cTrackItem::Compare(const cListObject &ListObject) const
{
  cTrackItem *t= (cTrackItem *)&ListObject;
  std::string artist1("");
  std::string artist2("");
  std::string title1("");
  std::string title2("");
  int r = -1;
  
  if(si && si->HasInfo()) {
    if(si->Artist)
	  artist1 = si->Artist;
	else
      artist1 = song->Name();

    if(si->Title)
      title1 = si->Title;
    else
      title1 = song->Name();
    }
  else { artist1 = song->Name(); title1 = song->Name(); }	

  if(t->si && t->si->HasInfo()) {
    if(t->si->Artist)
	  artist2 = t->si->Artist;
	else
      artist2 = t->song->Name();

    if(t->si->Title)
      title2 = t->si->Title;
    else
      title2 = t->song->Name();
    }
  else { artist2 = t->song->Name();	title2 = t->song->Name(); }
  
  if (sortMode == tsArtist)	r = strcoll(artist1.c_str(), artist2.c_str());
  
  if (sortMode == tsTitle)    r = strcoll(title1.c_str(), title2.c_str());

  if (sortMode == tsNumber)   r = song->Index(), t->song->Index();

  return r;
}  


void cTrackItem::Set(void) {
  char *buf=0;

  if(si && si->HasInfo()) {

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
/*  
    if (sortMode == tsArtist) {
      asprintf(&buf,"%d.\t %s - %s\t", song->Index() +1, si->Artist?si->Artist:tr("Unknown"), si->Title?si->Title:tr("No title"));
      }
	else if(sortMode == tsNumber) {
      asprintf(&buf,"%d.\t %s - %s\t", song->Index() +1, si->Artist?si->Artist:tr("Unknown"), si->Title?si->Title:tr("No title"));
      }   
    else if(sortMode == tsTitle) {
      asprintf(&buf,"%d.\t %s - %s\t", song->Index() +1, si->Title?si->Title:tr("No title"), si->Artist?si->Artist:tr("Unknown"));
      }
    else printf("Set: What the heck is the sortmode ???\n");
*/

    if (sortMode == tsArtist) {
//      asprintf(&buf,"%d.\t %s - %s\t \t %s", song->Index() +1, si->Artist?si->Artist:tr("Unknown"), si->Title?si->Title:tr("No title"), rate2text.c_str());
      asprintf(&buf,"%d.\t %s \t \t %s - %s", song->Index() +1, rate2text.c_str() , si->Artist?si->Artist:tr("Unknown"), si->Title?si->Title:tr("No title"));
      }
	else if(sortMode == tsNumber) {
//      asprintf(&buf,"%d.\t %s - %s\t \t %s", song->Index() +1, si->Artist?si->Artist:tr("Unknown"), si->Title?si->Title:tr("No title"), rate2text.c_str());
      asprintf(&buf,"%d.\t %s \t \t %s - %s", song->Index() +1, rate2text.c_str() , si->Artist?si->Artist:tr("Unknown"), si->Title?si->Title:tr("No title"));
      }   
    else if(sortMode == tsTitle) {
//      asprintf(&buf,"%d.\t %s - %s\t \t %s", song->Index() +1, si->Title?si->Title:tr("No title"), si->Artist?si->Artist:tr("Unknown"), rate2text.c_str());
      asprintf(&buf,"%d.\t %s \t \t %s - %s", song->Index() +1, rate2text.c_str() , si->Title?si->Title:tr("No title"), si->Artist?si->Artist:tr("Unknown"));
      }
    else printf("Set: What the heck is the sortmode ???\n");
	
	
  }  
  else {
    asprintf(&buf, "%d.\t %s \t \t %s",song->Index()+1,"",song->Name());
  }
  SetText(buf,false);
}

	
// ------ Tracklist ------------------------------------------------------------------------------
cTrackList::cTrackList(cList<cSong> *Tracklist, cMP3Player *player)
:cOsdMenu(tr("Music-SD: Playlist"),GetTab(1),GetTab(2),GetTab(3),GetTab(4))
{
  tracklist = Tracklist;
  mp3player = player;

  Rebuild();
  
  buttons=number=0;
  SetHelp();
  
  Display();  

}

cTrackList::~cTrackList()
{

}

int cTrackList::GetTab(int Tab)
{
  int a = MP3Skin.Tracklist_tab1;
  int b = MP3Skin.Tracklist_tab2;
  int c = MP3Skin.Tracklist_tab3;
  int d = MP3Skin.Tracklist_tab4;

  if (Tab==1)    return  a;  // Number
  if (Tab==2)    return  b;  // Rating
  if (Tab==3)    return  c;  // Space
  if (Tab==4)    return  d;  // Title

/*
  if (Tab==1)    return  6;  // Number
  if (Tab==2)    return  29; // Title
  if (Tab==3)    return  1;  // Space
  if (Tab==4)    return  9;  // Rating
*/

  return 0;
} 


void cTrackList::SetHelp()
{

    switch(buttons) {
        case 0:  // OK
            cOsdMenu::SetHelp(tr("Parent"),tr("Sort"),tr("Play now"),tr("More.."));
			break;

        case 1:			
            cOsdMenu::SetHelp(tr("Copy"),tr("ID3info"),tr("Edit ID3"),tr("More.."));
            break;
	
        case 2:			
            cOsdMenu::SetHelp(tr("Delete"),tr("Add"),tr("Mark"),tr("More.."));
            break;

        case 3:			
            cOsdMenu::SetHelp(tr("Empty"),tr("Save PL"),tr("Save BL"),tr("More.."));
            break;
	
	}  

	Display();
}


void cTrackList::Refresh(bool all)
{
  cTrackItem *cur=(cTrackItem *)((all || Count()<2) ? First() : Get(Current()));
  while(cur) {
    cur->Set();
    cur=(cTrackItem *)Next(cur);
    }
}

void cTrackList::Rebuild(void) {
  cTrackItem *currentItem = NULL;
  Clear();
  
  for(cSong *track = tracklist->First(); track; track=tracklist->cList<cSong>::Next(track)) {
    cTrackItem *item = new cTrackItem(track);
	Add(item);
	if(track == mgr->curr)
	  currentItem = item;
  }

  if(cTrackItem::SortMode() != cTrackItem::tsNumber)
    Sort();  
  SetCurrent(currentItem);
  
  int totaltime = mgr->GetListLength();
  char *buf=0;
  std::string sorting;
  if (cTrackItem::SortMode() == cTrackItem::tsNumber)     sorting =tr("Number");
  else if(cTrackItem::SortMode() == cTrackItem::tsArtist) sorting=tr("Artist");
  else if(cTrackItem::SortMode() == cTrackItem::tsTitle)  sorting=tr("Title");
  else sorting=tr("Error while sorting");

  asprintf(&buf,"%s %s - %s [%02d:%02d]", tr("Sortmode:"), sorting.c_str(), tr("Total:"), totaltime/60, totaltime%60);
  strcpy(title,buf);
  free(buf);

  SetTitle(title);
  
}  


void cTrackList::Move(int From, int To)
{
  char *buf;

  tracklist->Move(From,To);
  cOsdMenu::Move(From,To);
//1
  if(From < mgr->currIndex && To >= mgr->currIndex  ) {
	mgr->currIndex = mgr->currIndex -1;
    }
//2
  else if( From > mgr->currIndex && (To == mgr->currIndex || To < mgr->currIndex) ) {
	mgr->currIndex = mgr->currIndex +1;
    }
//3
  else if ( From == mgr->currIndex ) {
	mgr->currIndex = Current();
  }  
  //save current playlist to file
  asprintf(&buf, "%s/%s", BaseSource, "@current.m3u");
  if(mgr->SaveList(buf, false)) {
    isyslog("music-sd: ctrl: refreshed playlist to file: %s\n", buf);
    }
  else
    esyslog("music-sd: ctrl: couldn't save current playlist to file");
  free(buf);
	
  Rebuild();
  Display();
 
}



eOSState cTrackList::SavePlayList(bool burn)
{
  if(burn) {
	if(Interface->Confirm(tr("Save playlist as burnlist ?"))  ) {  
		Skins.Message(mtStatus, tr("Save playlist..."));
		char *buf=0;
		asprintf(&buf, "%s/%s", config, "playlists/burnlist.m3u");
		bool Result = mgr->SaveList(buf, true);
		Skins.Message(mtStatus, NULL);
		free(buf);
		if (Result)
			return AddSubMenu(new cMenuText(tr("Save playlist"), tr("Playlist saved as burnlist.m3u !"), fontFix));
		else
			return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not save playlist burnlist.m3u !"), fontFix));
		}
	}
  else {
	if(Interface->Confirm(tr("Save tracklist as playlist ?"))  ) { 
		return AddSubMenu(new cTracklistSave());
	}

  }

  return osContinue;
}
 

eOSState cTrackList::Remove(bool all)
{
    if(!all) {
		if(mgr->maxIndex ==0) {
			mp3player->DeleteTrack(true, Current());
			}   
		else if(mgr->maxIndex >=1) {
            cTrackItem *current = (cTrackItem *)Get(Current());
			printf("Tracklist: Remove: [%i] - %s\n", current->Song()->Index(), current->Song()->Fullname());
			mp3player->DeleteTrack(false, current->Song()->Index());
			Rebuild();
			Display();
		}
	}
	else {
		if(mgr->maxIndex >=0) {
			mp3player->DeleteTrack(true, Current());
		}   
	}
	return osContinue;
}	


eOSState cTrackList::CopyFile(void)
{
  FILE *copyscript;
  char *buffer;

  if(Interface->Confirm(tr("Copy track to goal listing ?"))  ) {  
    Skins.Message(mtStatus, tr("Copy..."));

	cTrackItem *current = (cTrackItem *)Get(Current());
	asprintf(&buffer, "cp -f \"%s\"  \"%s\"", current->Song()->Fullname(), MP3Setup.CopyDir);
	copyscript = popen(buffer, "r");
	dsyslog("music-sd: tracklist: executing: %s", buffer);
	free(buffer);
	pclose(copyscript);

    Skins.Message(mtStatus, NULL);
  }
  return osContinue;
}



eOSState cTrackList::GotoTrack(void)
{
  cTrackItem *current = (cTrackItem *)Get(Current());
  mgr->Goto(current->Song()->Index() +1);

  return osBack;
}


eOSState cTrackList::Number(eKeys Key)
{
  if (HasSubMenu())
    return osContinue;
  if (numberTimer.TimedOut())
    number = 0;

  number = number * 10 + Key - k0;

  if(number == 0) {
    Rebuild();
    Display();    
    }
  else {   
    for (cTrackItem *item = (cTrackItem *)First(); item; item = (cTrackItem *)item->Next()) {
      if(item->Song()->Index() == number -1) {
        SetCurrent(item);
        Display();
        break;
      }
    }
  }
  numberTimer.Set(INPUTTIMEOUT);
     
  return osContinue;
}


eOSState cTrackList::ProcessKey(eKeys Key)
{
  bool hadSubmenu = HasSubMenu();
  
  eOSState state = cOsdMenu::ProcessKey(Key);
  
  if(hadSubmenu && !HasSubMenu())  return osContinue;
	
  if (state == osUnknown) {
  
    switch (Key) {
		case kBack:
            state = osBack;
            break;

		case kOk:
			printf("Tracklist: Pressed OK\n");
            return GotoTrack();
			break;
			
		case kRed:
            if(buttons==0) {
                state = osBack;
		    }
	        else if(buttons==1) {
				CopyFile();
	        }
	        else if(buttons==2) {
				//loeschen

				if(mgr->maxIndex ==0) {
					if(Interface->Confirm(tr("Empty playlist ?"))  ) {
						Remove(true);
						cRemote::CallPlugin("music-sd");
						return (osPlugin);
	                }
				}
                else if(mgr->maxIndex >=1) {
					if(Interface->Confirm(tr("Delete this track from playlist ?"))  ) {
						return Remove(false);
					}
				}	
	        }   
            else if(buttons==3) {
				if(Interface->Confirm(tr("Empty playlist ?"))  ) {  
					Remove(true);
					cRemote::CallPlugin("music-sd");
					return (osPlugin);
				}	
			}
			break;
			
	case kGreen:
            if(buttons==0) {
				cTrackItem::IncSortMode();
				Rebuild();
				Display();
			}
	        else if(buttons==1) {
				// id3info
				cTrackItem *current = (cTrackItem *)Get(Current());
                if(current)
					return AddSubMenu(new cMP3SearchID3Info(current->Song()));
				else
                    return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));
	        }
	        else if(buttons==2) {
				//hizufuegen
                buttons = 0;
			    SetHelp();
	            cRemote::CallPlugin("music-sd");
                return (osPlugin);
	        }   
            else if(buttons==3) {
		        //Save burnlist
				return SavePlayList(false);
	        }      
            break;
 

	case kYellow:
            if(buttons==0) {
				cTrackItem *current = (cTrackItem *)Get(Current());
				mgr->Goto(current->Song()->Index() +1);
				if(mp3player->StatusMode() ==1 || mp3player->StatusMode() ==3) mp3player->Play();
		    }
	        else if(buttons==1) {
			    // edit id3tag
					cTrackItem *current = (cTrackItem *)Get(Current());
					if(current)
						return AddSubMenu(new cMP3id3Tag(current->Song()));
					else
						return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: No track(s) loaded !"), fontFix));
	        }
	        else if(buttons==2) {
				//verschieben
  				if(cTrackItem::SortMode() == cTrackItem::tsNumber) {
					Mark();
					return osContinue;
					}
				else
					return AddSubMenu(new cMenuText(tr("ERROR:"), tr("Sorry... but first change to sortmode = number !"), fontFix));
	        }   
            else if(buttons==3) {
		        //Save burnlist
				return SavePlayList(true);
			}      
            break;
 

	case kBlue:
            if(buttons==0) {
                buttons = 1;
			    SetHelp();
		    }
	        else if(buttons==1) {
                buttons = 2;
			    SetHelp();
	        }
	        else if(buttons==2) {
                buttons = 3;
			    SetHelp();
	        }   
            else if(buttons==3) {
                buttons = 0;
			    SetHelp();
	        }      
            break;

    case k0 ... k9:
            return Number(Key);            
	        break;

//    default: break;
      default: state = osContinue;
    }
  }
  return state;
}
