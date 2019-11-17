
#include <fstream>
#include <string>

#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>

#include "mp3id3tag.h"
#include "setup-mp3.h"
#include "i18n.h"
#include "player-mp3.h"
#include "decoder.h"
#include "common.h"
#include "vars.h"
#include "icons.h"

#define TAG_CMD "%s --artist '%s' --album '%s' --song '%s' --TCON '%s' --genre '%s' --POPM %s:%i --year %i  '%s'"


//---------------- cMP3id3TagYear ---------------------------------------------------------------------------------------------------------------------

char *cMP3id3TagYear::year = NULL;
int iRating = 0;

cMP3id3TagYear::cMP3id3TagYear(void)
:cOsdMenu(tr("Year"))
{
  free(year); year=0;
  LoadItems();
  SetHelp();
}

cMP3id3TagYear::~cMP3id3TagYear()
{
}

void cMP3id3TagYear::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
}

void cMP3id3TagYear::LoadItems(void)
{
  std::ifstream in;
  std::string Datei;
  std::string line;
  
  Datei=config;
  Datei = Datei + "/" + langdir;
  Datei=Datei + "/data/year.dat";

  in.open (Datei.c_str());
  
  if(in) {
    while ((getline(in,line,'\n'))) {
      Add(new cOsdItem(hk(line.c_str())));
    }

    in.close();
  }

  Display();
}


void cMP3id3TagYear::GetValue(void)
{
  cOsdItem *item = (cOsdItem*)Get(Current());

  if(item) {
    year = strdup(item->Text()); 
  }

}


eOSState cMP3id3TagYear::ProcessKey(eKeys Key)
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


//---------------- cMP3id3TagRating ---------------------------------------------------------------------------------------------------------------------

cMP3id3TagRating::cMP3id3TagRating(void)
:cOsdMenu(tr("Rating"),15)
{
  iRating = -1;
  LoadItems();
  SetHelp();
}

cMP3id3TagRating::~cMP3id3TagRating()
{
}

void cMP3id3TagRating::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
}

cOsdItem *cMP3id3TagRating::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osUnknown,true);
  free(buf);
  Add(item); return item;
}

void cMP3id3TagRating::LoadItems(void)
{
  std::string rate2text;

/*
  rate2text = tr("No value");
  Item(rate2text.c_str());
*/
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


void cMP3id3TagRating::GetValue(void)
{
  cOsdItem *item = (cOsdItem*)Get(Current());

  if(item) {
    iRating = Current();
  }

}


eOSState cMP3id3TagRating::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
     
        case kRed:
        case kBack: state = osBack; 
          break;

        case kBlue:
        case kOk:
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


//---------------- cMP3id3TagGenre ---------------------------------------------------------------------------------------------------------------------

char *cMP3id3TagGenre::genre = NULL;

cMP3id3TagGenre::cMP3id3TagGenre(void)
:cOsdMenu(tr("Genre"),5)
{
  free(genre); genre=0;
  LoadItems();
  SetHelp();
}

cMP3id3TagGenre::~cMP3id3TagGenre()
{
}

void cMP3id3TagGenre::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
}

void cMP3id3TagGenre::LoadItems(void)
{
  std::ifstream in;
  std::string Datei;
  std::string line;
  std::string genre_text;
  
  Datei = config;
  Datei = Datei + "/" + langdir;
  Datei = Datei + "/data/genre_set.dat";

  in.open (Datei.c_str());
  
  if(in) {
    while ((getline(in,line,'\n'))) {
      int len = line.length();

      std::string::size_type pos = line.find(":",0);
      if(pos != std::string::npos) {
	genre_text = line.substr(pos +2, len);
        Add(new cOsdItem(hk(genre_text.c_str())));
      }
    }
    in.close();
  }

  Display();
}


void cMP3id3TagGenre::GetValue(void)
{
  cOsdItem *item = (cOsdItem*)Get(Current());

  if(item) {
   genre = strdup(item->Text()); 
  }

}


eOSState cMP3id3TagGenre::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
     
        case kRed:
        case kBack: state = osBack; 
          break;

        case kBlue:
        case kOk:
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


//---------------- cMP3id3Tag --------------------------------------------------------------------------------------------------------------------------

cMP3id3Tag::cMP3id3Tag(cSong *song)
:cOsdMenu(tr("Music-SD: ID3tag editor"),20)
{
  iRating=-1;
  id3_Song = song;

  updateGenre=updateYear=updateRating=false;
  GetRealTags();
}

cMP3id3Tag::~cMP3id3Tag()
{
}

void cMP3id3Tag::ShowDisplay(void)
{
  char *buf=0;
  Clear();

  Add(new cOsdItem(hk(tr("Clear tags"))));
  Add(new cOsdItem(hk(tr("Save tags"))));
  Item("");
  Item(FullFile);
  Add(new cMenuEditStrItem(tr("Artist")          ,Artist      , sizeof(Artist)-1));
  Add(new cMenuEditStrItem(tr("Album")           ,Album       , sizeof(Album)-1));
  Add(new cMenuEditStrItem(tr("Title")           ,Title       , sizeof(Title)-1));
  Add(new cMenuEditStrItem(tr("Genre")           ,Genre       , sizeof(Genre)-1));
  Add(new cMenuEditIntItem(tr("Year")            ,&Year       , -1     , 2030));

  std::string rate2text;
   
  switch(Rating) {
		case 0:
			rate2text = tr("Not rated"); //empty
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
  asprintf(&buf, "%s\t%s", tr("Rating"), rate2text.c_str());
  Add(new cOsdItem(hk(buf)));
  free(buf);

//  Add(new cMenuEditIntItem(tr("Rating")          ,&Rating     , -1     , 10));

  SetCurrent(Get(4));
  SetHelp();
  Display();
}


void cMP3id3Tag::SetHelp()
{
  int ItemIndex = Current();
  
  if(ItemIndex == 0) {
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Execute"));
    }
  else if(ItemIndex ==1) {    
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Execute"));
    }
  else if(ItemIndex ==7) {    
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
    }
  else if(ItemIndex ==8) {    
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
    }
  else if(ItemIndex ==9) {    
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Select"));
    }
  else
    cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


cOsdItem *cMP3id3Tag::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osBack);
  item->SetSelectable(false);
  free(buf);
  Add(item); return item;
}


void cMP3id3Tag::UpdateMenu(int Which) {

  int which = Which;

  switch(which) {
    case 0: // Set Genre
      updateGenre = false;
      SetGenre();           
      break;

    case 1: // Set Year
      updateYear = false;
      SetYear();
      break;

    case 2: // Set Rating
      updateRating = false;
      SetRating();
      break;

    default:
      break;
  }    

}



void cMP3id3Tag::RemoveTagsOnly(void) {
  std::string Datei;

  Datei = config;
  Datei = Datei + "/" + langdir;
  Datei = Datei + "/scripts/music_id3v2";

  if( FILE* fd=fopen(id3_Song->Fullname(), "r")) {
    fclose(fd);

    char *cmd;
    asprintf(&cmd, "%s --delete-all '%s'", Datei.c_str(), id3_Song->Fullname());
    system(cmd);
    free(cmd);
  }    
  else {
    esyslog("music-sd: ERROR while removing id3tags for '%s'", id3_Song->Fullname());
  }

}


void cMP3id3Tag::SaveTags(void) {
  std::string Datei;

  Datei = config;
  Datei = Datei + "/" + langdir;
  Datei = Datei + "/scripts/music_id3v2";

  if(MP3Setup.RateInFile) {
    if( FILE* fd=fopen(id3_Song->Fullname(), "r")) {
      fclose(fd);


      char *cmd;
      asprintf(&cmd, TAG_CMD, Datei.c_str(), Artist, Album, Title, Genre, Genre, MP3Setup.RateCompat ,Rating ,Year, id3_Song->Fullname());
   
      isyslog("music-sd: id3tag: execute '%s'\n",cmd);    
      system(cmd);
      free(cmd);


      Skins.Message(mtStatus, tr("Update id3-tag"));
      mgr->UpdateSong(Artist, Album, Title, Genre, Year, Rating, id3_Song);
      Skins.Message(mtStatus, NULL);
      }    
    else {
      esyslog("music-sd: ERROR while writing id3tags for '%s'", id3_Song->Fullname());
      }
    }
  else {
    mgr->UpdateSong(Artist, Album, Title, Genre, Year, Rating, id3_Song);
//    InfoCache.Modified();
  }
}


void cMP3id3Tag::GetRealTags(void) {

  cSongInfo *id3_songinfo = id3_Song->Info();

  if(id3_songinfo) {
    if(id3_songinfo->HasInfo() && id3_songinfo->Total>0) {
      strcpy(FullFile ,id3_Song->Fullname());

      if(id3_songinfo->Artist != NULL)
        strcpy(Artist   ,id3_songinfo->Artist);
      else
        strcpy(Artist, "");	

      if(id3_songinfo->Album != NULL)
        strcpy(Album    ,id3_songinfo->Album);
      else
        strcpy(Album, "");	

      if(id3_songinfo->Title != NULL)
        strcpy(Title    ,id3_songinfo->Title);
      else
        strcpy(Title, "");	

      if(id3_songinfo->Genre != NULL)
        strcpy(Genre    ,id3_songinfo->Genre);
      else
        strcpy(Genre, "Other");	

      if(id3_songinfo->Year > 1800)
        Year = id3_songinfo->Year;
      else
        Year = -1;	

      Rating = id3_songinfo->Rating;
	  
      ShowDisplay();
    }  
  }
}  


// TODO
void cMP3id3Tag::SetGenre(void) {

  const char *genretext=cMP3id3TagGenre::Genre();
  if(genretext) {
    strcpy(Genre, genretext);
  }

  ShowDisplay();
}


void cMP3id3Tag::SetRating(void) {

  if(iRating >=0) {
    if(iRating==0) Rating = 0;
    else if(iRating==1)  Rating=3;
    else if(iRating==2)  Rating=28;
    else if(iRating==3)  Rating=53;
    else if(iRating==4)  Rating=78;
    else if(iRating==5)  Rating=104;
    else if(iRating==6)  Rating=129;
    else if(iRating==7)  Rating=154;
    else if(iRating==8)  Rating=179;
    else if(iRating==9)  Rating=205;
    else if(iRating==10)  Rating=230;
    else if(iRating==11) Rating=255;
    else Rating = -1;	
  }
  ShowDisplay();
}


void cMP3id3Tag::SetYear(void) {

  const char *newyear=cMP3id3TagYear::Year();
  if(newyear) {
    Year = atoi(newyear);
  }    

  ShowDisplay();
}


eOSState cMP3id3Tag::ProcessKey(eKeys Key)
{
  int current = Current();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if(!HasSubMenu() && state==osContinue) {
    if(updateGenre) UpdateMenu(0);
      else if(updateYear)   UpdateMenu(1);
        else if(updateRating) UpdateMenu(2);
  }

  if (state == osContinue) {
    switch (Key) {
		case kUp:
		case kUp|k_Repeat:
		case kDown:
		case kDown|k_Repeat:
			SetHelp();
			break;
		default: state = osContinue;
    }
  }    	  

  
  if (state == osUnknown) {
    switch (Key) {
        case kOk:
        case kBlue:
			if(current ==0) {
				RemoveTagsOnly();
				state=osBack;
				}
			else if(current==1) {
				SaveTags();
				state=osBack;
				}
			else if(current ==7) {
				updateGenre = true;
				return AddSubMenu(new cMP3id3TagGenre()); 
				} 
			else if(current ==8) {
				updateYear = true;
				return AddSubMenu(new cMP3id3TagYear()); 
				} 
			else if(current ==9) {
				updateRating = true;
				return AddSubMenu(new cMP3id3TagRating()); 
				}
			else
				state = osContinue;
			break;

        case kRed:
		case kBack: state = osBack;
			break;

		case kGreen:
		case kYellow:
        default: state = osContinue;
    }
  }

  return state;
}
