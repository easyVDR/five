#include <fstream>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include "search.h"
#include "i18n.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "decoder.h"
#include "mp3id3.h"
#include "mp3control.h"
#include "config.h"
#include "skin.h"
#include "vars.h"
#include "icons.h"

#define X_DISPLAY_MISSING

// ------------ cMP3SearchBrowseItem ------------------------------------------------------------------------------------------------ //
int RateFrom;
int RateTo;

cMP3SearchBrowseItem::cMP3SearchBrowseItem(cFileObj *Item, int index, const char *Artist, const char *Title, const char *Bitrate, const char *Total) {
  item      = Item;
  idx       = index +1;
  total     = atoi(Total);
  bitrate   = atoi(Bitrate);
  artist    = Artist;
  title     = Title;

  SetID3();
}

/*
cMP3SearchBrowseItem::~cMP3SearchBrowseItem()
{

}
*/

void cMP3SearchBrowseItem::SetID3(void) {
  char *Name;

  if(MP3Setup.ArtistFirst) 
//    asprintf(&Name, "%s \t %s - %s \t  %3d k    \t%02d:%02d", Icons::Menu_Checkbox(), artist, title, bitrate/1000, total/60, total%60);
//  else
//    asprintf(&Name, "%s \t %s - %s \t  %3d k    \t%02d:%02d", Icons::Menu_Checkbox(), title, artist, bitrate/1000, total/60, total%60);
    asprintf(&Name, "%s \t %02d:%02d \t  %3d k  \t %s - %s", Icons::Menu_Checkbox(), total/60, total%60, bitrate/1000, artist, title);
  else 
    asprintf(&Name, "%s \t %02d:%02d \t  %3d k  \t %s - %s", Icons::Menu_Checkbox(), total/60, total%60, bitrate/1000, title, artist);

  SetText(Name,false);
}




void cMP3SearchBrowseItem::NewName(void) {
  char *Name;
  cSong *song= new cSong(item);
  cSongInfo *si = song->Info(false);

  if(!si) si=song->Info();

  if(si && si->Title) {
    if(MP3Setup.ArtistFirst) 
//      asprintf(&Name, "%s \t %s - %s \t  %3d k    \t%02d:%02d", Icons::Menu_CheckedBox(), si->Artist, si->Title,si->Bitrate/1000, si->Total/60, si->Total%60);
      asprintf(&Name, "%s \t %02d:%02d \t  %3d k  \t %s - %s", Icons::Menu_CheckedBox(), si->Total/60, si->Total%60, si->Bitrate/1000, si->Artist, si->Title);
    else 
//      asprintf(&Name, "%s \t %s - %s \t  %3d k    \t%02d:%02d", Icons::Menu_CheckedBox(), si->Title, si->Artist, si->Bitrate/1000, si->Total/60, si->Total%60);
      asprintf(&Name, "%s \t %02d:%02d \t  %3d k  \t %s - %s", Icons::Menu_CheckedBox(), si->Total/60, si->Total%60, si->Bitrate/1000, si->Title, si->Artist);
    }
  else {
      asprintf(&Name, "%s \t --:-- \t  --- k  \t <%s>", Icons::Menu_CheckedBox(), song->Name());
  } 
 
  SetText(Name,false);
}



// ------------ cMP3SearchResult ------------------------------------------------------------------------------------------------------------

cFileObj *cMP3SearchResult::selected=0;

cMP3SearchResult::cMP3SearchResult(const char *szTitle)
:cOsdMenu(szTitle ,GetTab(1), GetTab(2), GetTab(3), GetTab(4))
{

  if(LoadResult()) {
    SetHelp();
    }
  else
    Skins.Message(mtError, tr("Error while opening @Suchergebnis.m3u !"));

    Display();
}


cMP3SearchResult::~cMP3SearchResult()
{
}


cFileObj *cMP3SearchResult::CurrentItem(void) {
  cMP3SearchBrowseItem *item = (cMP3SearchBrowseItem *)Get(Current());
  return item ? item->Item():0;
}  


void cMP3SearchResult::SetHelp() {
  cOsdMenu::SetHelp(tr("Parent"),tr("Add"),tr("Add all"),tr("Sort"));
}


int cMP3SearchResult::GetTab(int Tab)
{
  int a = MP3Skin.Searchlist_tab1;
  int b = MP3Skin.Searchlist_tab2;
  int c = MP3Skin.Searchlist_tab3;
  int d = MP3Skin.Searchlist_tab4;


  if (Tab==1)    return  a; // Checkbox
  if (Tab==2)    return  b; // Time
  if (Tab==3)    return  c; // Bitrate
  if (Tab==4)    return  d; // Songname
  return 0;
}  


int cMP3SearchResult::LoadResult() {
  
  using namespace std;
  std::string SongTitle("");
  std::string Base("");
  std::string basesource("");
  std::string newbase("");
  std::string datei("");
  std::string FullPath("");
  std::string Artist("");
  std::string Title("");
  std::string Bitrate("");
  std::string Total("");
  bool result = false;
  std::string filename("");
  int  Length;

  basesource = BaseSource;
  Length = basesource.length();
  
  filename = basesource;
  filename = filename + "/@Suchergebnis.m3u";
  
  ofstream out(filename.c_str());

  datei  = basesource + "/@Suche.dat";

  if(FILE* fd = fopen(datei.c_str(), "r")) {
    char line[1024];
    const char* delim = ";";
    char* word = NULL;
    int i=0;
    bool base = false;
  
    while(fgets(line, sizeof(line), fd) != NULL) {
      word = strtok(line,delim);
      for(i=0; word !=NULL; i++) {

        if(i==0) FullPath=word;
          else 
            if(i==1) Artist=word;
              else      
                if(i==2) Title=word;
                  else
                    if(i==3) Bitrate=word;
                      else
                        if(i==4) Total=word;
      
        word = strtok(NULL,delim);
      }


      newbase = FullPath.substr(0, Length);

      // check for same basedir , so substr cant crash
      if(basesource == newbase) {

        string::size_type pos = FullPath.rfind ('/',FullPath.length());

        if(pos!=string::npos) {

	   // Songname
          SongTitle =  FullPath.substr(pos+1,FullPath.length());
          // Basedir
          Base      =  FullPath.substr(Length +1, FullPath.length() - Length - SongTitle.length() -1);
          if(Base.length() > 0) { 
	    Base = Base.substr(0, Base.length() -1);
            base = true;
          }
          else {
            base = false;
          }
        }     

        newsong=new cFileObj(MP3Sources.GetSource(), base ? Base.c_str() : 0, SongTitle.c_str(),otFile);

        if( newsong && (newsong->Type()==otFile) ) {
          Add(new cMP3SearchBrowseItem(newsong, count, Artist.c_str(), Title.c_str(), Bitrate.c_str(), Total.c_str()));
          d(printf("music: search: found '%s'\n", FullPath.c_str()));
          out<<FullPath.c_str()<<endl;
          }
        else {
          delete newsong;       
        }
      }
    }

    out.close();
    fclose(fd);

    result = true;
  }

  return result;

}


eOSState cMP3SearchResult::ID3Info(void) {
  cFileObj *item=CurrentItem();
  if(item && item->Type()==otFile) {
    cSong *song=new cSong(item);
    cSongInfo *si;
    if(song && (si=song->Info())) {
      AddSubMenu(new cMP3SearchID3Info(si, item->Path()));
      }
  }

  return osContinue;
}        


eOSState cMP3SearchResult::AddOne(void) {
  cFileObj *item=CurrentItem();
  
  if((item=CurrentItem())) {
    selected=new cFileObj(item);
    cInstantPlayList *newpl= new cInstantPlayList(item);
    if(newpl->Load() && newpl->Count()) {
      isyslog("music: added track %s to playlist %s", item->Name(),newpl->Name());
      cMP3Control::SetPlayList(newpl);

      cMP3SearchBrowseItem *cur = (cMP3SearchBrowseItem *)Get(Current());
      cur->NewName();
      Display();

      return osContinue;
      }
    else
      delete newpl;
    }

  return osContinue;    
}      



eOSState cMP3SearchResult::AddAll(void) {
  Skins.Message(mtStatus, tr("Add all tracks to playlist..."));

  cFileObj *item=new cFileObj(MP3Sources.GetSource(), 0, "@Suchergebnis.m3u",otFile);
  cPlayList *newpl=new cPlayList(item);

  if(newpl->Load() && newpl->Count()) {
    isyslog("music: added all tracks to playlist %s", newpl->Name());
    cMP3Control::SetPlayList(newpl);
    Skins.Message(mtStatus, NULL);
    return osBack;
    }

  delete newpl;
  Skins.Message(mtStatus, NULL);

  return osContinue;
}      


eOSState cMP3SearchResult::ProcessKey(eKeys Key) {

  bool hadSubmenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (hadSubmenu && !HasSubMenu())
    return osContinue;


  if (state == osUnknown) {
    switch (Key) {

		case kUp:
		case kUp|k_Repeat:
		case kDown:
		case kDown|k_Repeat:
			break;

		case kRed:
			return osBack;
			break;

		case kGreen:	  
			state = AddOne();
			break;

		case kYellow:
			state = AddAll();
			break;

		case kBlue:
			state = ID3Info();
			break;

		case kOk:
			state = ID3Info();
			break; 

		default:
			state = osContinue;
    }
  }
  
  return state;
}        		


//---------------- cMP3RatingTo ---------------------------------------------------------------------------------------------------------------------

char *cMP3SearchRatingTo::ratingto = NULL;

cMP3SearchRatingTo::cMP3SearchRatingTo(void)
:cOsdMenu(tr("Rating to"),15)
{
  free(ratingto); ratingto=0;
  SetHelp();
  LoadItems();
}

cMP3SearchRatingTo::~cMP3SearchRatingTo()
{
}

void cMP3SearchRatingTo::SetHelp(void) {
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
}

cOsdItem *cMP3SearchRatingTo::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osUnknown,true);
  free(buf);
  Add(item); return item;
}


void cMP3SearchRatingTo::LoadItems(void)
{
  std::string rate2text;

  rate2text = tr("No value");
  Item(rate2text.c_str());

  rate2text = tr("Not rated");
  Item(rate2text.c_str());
  
  rate2text = Icons::Menu_Bomb();
  rate2text = rate2text + "\t" + tr("Song can be deleted");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("Makes me nervous");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("Bad song");
  Item(rate2text.c_str());
  
  
  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("Useful for certain cause");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("I hear it from time to time");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("New song, don't know");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("Not bad...not good");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("Good song");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("Very good song");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("I love this song");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("One of my favourites");
  Item(rate2text.c_str());

  Display();
}


void cMP3SearchRatingTo::GetValue(void)
{
  cOsdItem *item = (cOsdItem*)Get(Current());

  if(item) {
    RateTo = Current() -2;
// DEBUG
	printf("RateTo = %d\n", RateTo);
    ratingto = strdup(item->Text()); 
  }

}


eOSState cMP3SearchRatingTo::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kRed:
        case kBack: state = osBack; 
          break;

        case kOk:
	case kBlue:
            GetValue();
            state = osBack;
          break;

	case kGreen:
	case kYellow:

        default: state = osContinue;
    }
  }

  return state;
}


//---------------- cMP3RatingFrom ---------------------------------------------------------------------------------------------------------------------

char *cMP3SearchRatingFrom::ratingfrom = NULL;

cMP3SearchRatingFrom::cMP3SearchRatingFrom(void)
:cOsdMenu(tr("Rating from"),15)
{
  free(ratingfrom); ratingfrom=0;
  SetHelp();
  LoadItems();
}

cMP3SearchRatingFrom::~cMP3SearchRatingFrom()
{
}

void cMP3SearchRatingFrom::SetHelp(void) {
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
}

cOsdItem *cMP3SearchRatingFrom::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osUnknown,true);
  free(buf);
  Add(item); return item;
}

void cMP3SearchRatingFrom::LoadItems(void)
{
  std::string rate2text;

  rate2text = tr("No value");
  Item(rate2text.c_str());

  rate2text = tr("Not rated");
  Item(rate2text.c_str());
  
  rate2text = Icons::Menu_Bomb();
  rate2text = rate2text + "\t" + tr("Song can be deleted");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("Makes me nervous");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("Bad song");
  Item(rate2text.c_str());
  
  
  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("Useful for certain cause");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("I hear it from time to time");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("New song, don't know");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("Not bad...not good");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("Good song");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("Very good song");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Half_Star();
  rate2text = rate2text + "\t" + tr("I love this song");
  Item(rate2text.c_str());

  rate2text = Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + Icons::Menu_Full_Star();
  rate2text = rate2text + "\t" + tr("One of my favourites");
  Item(rate2text.c_str());

  Display();
}


void cMP3SearchRatingFrom::GetValue(void)
{
  cOsdItem *item = (cOsdItem*)Get(Current());

  if(item) {
    RateFrom = Current() -2;
// DEBUG
	  printf("RateFrom = %d\n", RateFrom);
    ratingfrom = strdup(item->Text());
  }

}


eOSState cMP3SearchRatingFrom::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kRed:
        case kBack: state = osBack; 
          break;

        case kOk:
	case kBlue:
            GetValue();
            state = osBack;
          break;


	case kGreen:
	case kYellow:

        default: state = osContinue;
    }
  }

  return state;
}


//---------------- cMP3SearchGenre ---------------------------------------------------------------------------------------------------------------------

char *cMP3SearchGenre::genretext = NULL;

cMP3SearchGenre::cMP3SearchGenre(void)
:cOsdMenu(tr("Genre"))
{
  free(genretext); genretext=0;
  SetHelp();
  LoadItems();
}

cMP3SearchGenre::~cMP3SearchGenre()
{
}

void cMP3SearchGenre::SetHelp(void) {
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
}


void cMP3SearchGenre::LoadItems(void)
{
  std::ifstream in;
  std::string Datei;
  std::string line;
  
  Datei = config;
  Datei = Datei + langdir;
  Datei = Datei + "/data/genre.dat";

#ifdef DEBUG
  isyslog("music: search: loading genre.dat from '%s'", Datei.c_str());
#endif

  in.open (Datei.c_str());
  
  if(in) {
    while ((getline(in,line,'\n'))) {
      Add(new cOsdItem(hk(line.c_str())));
    }

    in.close();
  }

  Display();
}


void cMP3SearchGenre::GetValue(void)
{
  cOsdItem *item = (cOsdItem*)Get(Current());

  if(item) {
   genretext = strdup(item->Text()); 
  }

}


eOSState cMP3SearchGenre::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kRed:
        case kBack: state = osBack; 
          break;

        case kOk:
	case kBlue:
            GetValue();
            state = osBack;
          break;

	case kGreen:
	case kYellow:

        default: state = osContinue;
    }
  }

  return state;
}


//---------------- cMP3Search --------------------------------------------------------------------------------------------------------------------------

cMP3Search::cMP3Search(const char *Name)
:cOsdMenu(tr("Music: Search"),30)
{
  strncpy(Artist, Name, sizeof(Artist));
  updateGenre=updateRatingFrom=updateRatingTo=false;
  UpdateMenu(5);
}

cMP3Search::~cMP3Search()
{
}

void cMP3Search::ShowDisplay(void)
{
  char *buf=0;
  
  Clear();
      
  Add(new cOsdItem(hk(tr("Load previous query"))));
  Add(new cOsdItem(hk(tr("Reset query"))));
  Add(new cOsdItem(hk(tr("Execute query"))));
  Item("");
  Add(new cMenuEditStrItem(tr("Artist")          ,Artist      , sizeof(Artist)-1 ));
  Add(new cMenuEditStrItem(tr("Title")           ,Title       , sizeof(Title)-1  ));
  Add(new cMenuEditStrItem(tr("Genre")           ,Genre       , sizeof(Genre)-1  ));
  Add(new cMenuEditStrItem(tr("Filename")        ,FullFile    , sizeof(FullFile)-1 ));
  Add(new cMenuEditIntItem(tr("Year from")       ,&YearFrom   , -1     , 2030));
  Add(new cMenuEditIntItem(tr("Year to")         ,&YearTo     , -1     , 2030));
  asprintf(&buf, "%s\t%s", tr("Rating from"), RatingFrom);
  Add(new cOsdItem(hk(buf)));
  asprintf(&buf, "%s\t%s", tr("Rating to"), RatingTo);
  Add(new cOsdItem(hk(buf)));
  free(buf);
  //  Add(new cMenuEditStraItem(tr("Rating from")    ,RatingFrom  , sizeof(RatingFrom)-1));
//  Add(new cMenuEditStraItem(tr("Rating to")      ,RatingTo    , sizeof(RatingTo)-1));
  Item("");
  Add(new cMenuEditBoolItem(tr("Only available songs") ,&Exist      ,tr("no"), tr("yes")));

  SetCurrent(Get(4));
  SetHelp();
  Display();
}


void cMP3Search::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


cOsdItem *cMP3Search::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osBack);
  item->SetSelectable(false);
  free(buf);
  Add(item); return item;
}



void cMP3Search::SaveSearch(void) {
  std::string Datei;
  
  Datei = BaseSource;
  Datei = Datei + "/@Search.bak";
  
  if(FILE* fd=fopen(Datei.c_str(), "w")) {
    char *query;
    asprintf(&query, "%s\n%s\n%s\n%s\n%d\n%d\n%s\n%s\n%d\n%d\n%d",Artist,Title,Genre,FullFile,YearFrom,YearTo,RatingFrom,RatingTo,RateFrom,RateTo,Exist);
    fprintf(fd, query);
    d(printf("music: search: Save query: \n%s\n",query));
    free(query);
    
    fclose(fd);
  }    
}


eOSState cMP3Search::Searching(void) {

  if(HasSubMenu()) return osContinue;

  Skins.Message(mtStatus,tr("Searching..."));

  SaveSearch();    

#ifdef DEBUG
printf("Artist: '%s'\n", Artist);
printf("Title: '%s'\n", Title);
printf("Genre: '%s'\n", Genre);
printf("Filename: '%s'\n", FullFile);
printf("Year from: '%d'\n", YearFrom);
printf("Year to: '%d'\n", YearTo);
printf("Rating from: '%d'\n", RateFrom);
printf("Rating to: '%d'\n", RateTo);
printf("File exist: '%d'\n", Exist);

#endif  
  
  if(InfoCache.SearchUp(Artist, Title , Genre, FullFile, YearFrom, YearTo, RateFrom, RateTo, Exist)) {
    char *buf;
    asprintf(&buf,"%s %i", tr("Found:"), InfoCache.expr1);
    d(printf("music: search: %s of %i\n", buf, InfoCache.expr2));
    strncpy(searchresult,buf,sizeof(searchresult));
    free(buf);
    Skins.Message(mtStatus,NULL);
    return AddSubMenu(new cMP3SearchResult(searchresult));
    }
  else {
    Skins.Message(mtStatus,NULL);
    return AddSubMenu(new cMenuText(tr("ERROR:"), tr("ERROR: Could not find anything !"), fontFix));
  }

  return osContinue;
}


void cMP3Search::UpdateMenu(int Which) {

  int which = Which;

  switch(which) {
    case 0: // ClearValues
      d(printf("music: search: clear values\n"));
      ClearValues();    
      break;

    case 1: // PreviousSearch
      d(printf("music: search: load previous search\n"));
      PreviousSearch();    
      break;

    case 2: // Get Genretest
      d(printf("music: search: update value for genre\n"));
      updateGenre = false;
      SetGenre();           
      break;

    case 3: // Get RatingFrom
      d(printf("music: search: update value for rating from\n"));
      updateRatingFrom = false;
      SetRatingFrom();
      break;

    case 4: // Get RatingTo
      d(printf("music: search: update value for rating to\n"));
      updateRatingTo = false;
      SetRatingTo();
      break;

    case 5: // First Search
      d(printf("music: search: first search\n"));
      NewValues();    
      break;


    default:
      ClearValues();
  }    

}


void cMP3Search::PreviousSearch(void) {

  std::ifstream in;
  std::string line;
  std::string pDatei;
  std::string pArtist;
  std::string pTitle;
  std::string pGenre;
  std::string pFullFile;
  std::string pYearFrom;
  std::string pYearTo;
  std::string pRatingFrom;
  std::string pRatingTo;
  std::string pRateFrom;
  std::string pRateTo;
  std::string pPlayable;
  
  pDatei = BaseSource;
  pDatei = pDatei + "/@Search.bak";

  int i = 0;

  in.open(pDatei.c_str());
  if(in) {
    while (getline(in, line, '\n')) {

      if(i==0) pArtist=line;
       else
      if(i==1) pTitle=line;
       else
      if(i==2) pGenre=line;
       else
      if(i==3) pFullFile=line;
       else
      if(i==4) pYearFrom=line;
       else
      if(i==5) pYearTo=line;
       else
      if(i==6) pRatingFrom=line;
       else
      if(i==7) pRatingTo=line;
       else
      if(i==8) pRateFrom=line;
       else
      if(i==9) pRateTo=line;
       else
      if(i==10) pPlayable=line;

      i++;
    }

  in.close();
  }    

  strncpy(Artist   , pArtist.c_str()   ,sizeof(Artist));
  strncpy(Title    , pTitle.c_str()    ,sizeof(Title));
  strncpy(Genre    , pGenre.c_str()    ,sizeof(Genre));
  strncpy(FullFile , pFullFile.c_str() ,sizeof(FullFile));
  YearFrom        = atoi(pYearFrom.c_str());
  YearTo          = atoi(pYearTo.c_str());
  strncpy(RatingFrom, pRatingFrom.c_str() ,sizeof(RatingFrom));
  strncpy(RatingTo  , pRatingTo.c_str()    ,sizeof(RatingTo));
  RateFrom        = atoi(pRateFrom.c_str());
  RateTo          = atoi(pRateTo.c_str());
// Get here the string from file

  Exist           = atoi(pPlayable.c_str());

  ShowDisplay();
}


void cMP3Search::NewValues(void) {

//  strncpy(Artist   ,"" ,sizeof(Artist));
  strncpy(Title    ,"" ,sizeof(Title));
  strncpy(Genre    ,"" ,sizeof(Genre));
  strncpy(FullFile ,"" ,sizeof(FullFile));
  YearFrom        =-1;
  YearTo          =-1;
  strncpy(RatingFrom ,tr("No value") ,sizeof(RatingFrom));
  strncpy(RatingTo   ,tr("No value") ,sizeof(RatingTo));
  RateFrom        =-2;
  RateTo          =-2;
  Exist           =1;

  ShowDisplay();
}
    

void cMP3Search::ClearValues(void) {

  strncpy(Artist   ,"" ,sizeof(Artist));
  strncpy(Title    ,"" ,sizeof(Title));
  strncpy(Genre    ,"" ,sizeof(Genre));
  strncpy(FullFile ,"" ,sizeof(FullFile));
  YearFrom        =-1;
  YearTo          =-1;
  strncpy(RatingFrom ,tr("No value") ,sizeof(RatingFrom));
  strncpy(RatingTo   ,tr("No value") ,sizeof(RatingTo));
  RateFrom        =-2;
  RateTo          =-2;
  Exist           =0;

  ShowDisplay();
}


void cMP3Search::SetGenre(void) {

  const char *genretext=cMP3SearchGenre::GenreText();
  if(genretext) {
    strncpy(Genre    ,genretext ,sizeof(Genre));
  }

  ShowDisplay();
}


void cMP3Search::SetRatingFrom(void) {

  const char *newrating=cMP3SearchRatingFrom::RatingFrom();
  if(newrating) {
    strncpy(RatingFrom, newrating, sizeof(RatingFrom));
  }

  ShowDisplay();
}


void cMP3Search::SetRatingTo(void) {

  const char *newrating=cMP3SearchRatingTo::RatingTo();
  if(newrating) {
    strncpy(RatingTo, newrating, sizeof(RatingTo));
  }    

  ShowDisplay();
}

eOSState cMP3Search::ProcessKey(eKeys Key)
{
  int current = Current();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if(!HasSubMenu() && state==osContinue) {
    if(updateGenre) UpdateMenu(2);
      else if(updateRatingFrom) UpdateMenu(3);
        else if(updateRatingTo)   UpdateMenu(4);
  }

  if (state == osUnknown) {
    switch (Key) {
        case kRed:     
        case kBack: state = osBack; 
          break;

        case kOk:
	  if(current ==0) {
            UpdateMenu(1);
	    state = osContinue;
	    }
	  else if(current==1) {
	    UpdateMenu(0);
	    state = osContinue;
	    }
	  else if(current ==2) {
	    state = Searching();
	    } 
	  else if(current ==6) {
            updateGenre = true;
	    return AddSubMenu(new cMP3SearchGenre()); 
	    } 
	  else if(current ==10) {
            updateRatingFrom = true;
	    return AddSubMenu(new cMP3SearchRatingFrom()); 
	    } 
	  else if(current ==11) {
            updateRatingTo = true;
	    return AddSubMenu(new cMP3SearchRatingTo()); 
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


