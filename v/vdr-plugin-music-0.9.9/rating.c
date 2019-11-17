
#include <string>
#include <fstream>

#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>

#ifdef HAVE_ICONPATCH
#include <vdr/iconpatch.h>
#endif

#include "rating.h"
#include "i18n.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "decoder.h"
#include "mp3control.h"
#include "skin.h"
#include "vars.h"
#include "icons.h"

//---------------- cMP3Rating --------------------------------------------------------------------------------------------------------------------------

cMP3Rating::cMP3Rating(cSong *current)
:cOsdMenu(tr("Music: Rating"),15)
{
  Song = current;
  GetRatingText();
  SetHelp();
  
  Display();
}


cMP3Rating::~cMP3Rating()
{
}


void cMP3Rating::SetHelp() {
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,tr("Execute"));
}


cOsdItem *cMP3Rating::Item(const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s",text?text:"");
  cOsdItem *item = new cOsdItem(buf,osUnknown,true);
  free(buf);
  Add(item); return item;
}


void cMP3Rating::GetRatingText() {
  std::string rate2text;
  
  Item(tr("Clear rating"));
  
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

}
 


eOSState cMP3Rating::SetRating(void) {

  int rating=0;
  int current = Current();
  std::string Datei;
  Datei = "id3v2";

//  Datei = config;
//  Datei = Datei + langdir;
//  Datei = Datei + "/scripts/music_id3v2";

#ifdef DEBUG
  isyslog("music: rating: executing id3v2");
#endif

  switch(current) {
        case 0:
	   rating=0;    // unbewertet
	   break;
        case 1: 
           rating=3;    // kannst loeschen
	   break;
        case 2: 
           rating=28;   // 0,5
	   break;
        case 3: 
           rating=53;   // 1
	   break;
        case 4: 
           rating=78;   // 1,5
	   break;
        case 5: 
           rating=104;  // 2
	   break;
        case 6: 
           rating=129;  // 2,5
	   break;
        case 7: 
           rating=154;  // 3
	   break;
        case 8: 
           rating=179;  // 3,5
	   break;
        case 9: 
           rating=205;  // 4
	   break;
        case 10: 
           rating=230;  // 4,5
	   break;
        case 11: 
           rating=255;  // 5
	   break;
  }

    

  if(MP3Setup.RateInFile) {
    if( FILE* fd=fopen(Song->Fullname(), "r")) {
      fclose(fd);

      
      cSongInfo *id3_songinfo = Song->Info();
  

      if(strcmp(cDecoders::ID2Str(id3_songinfo->DecoderID),"MP3")) {
        isyslog("music: rating: No valid MP3 file");
        return osBack;
      }
  
  
      if(id3_songinfo) {
        if(id3_songinfo->HasInfo() && id3_songinfo->Total>0) {

          if(id3_songinfo->Artist != NULL)
            strcpy(Artist, id3_songinfo->Artist);
          else
            strcpy(Artist, ""); 	

          if(id3_songinfo->Album != NULL)
            strcpy(Album, id3_songinfo->Album);
          else
            strcpy(Album, ""); 	

          if(id3_songinfo->Title != NULL)
            strcpy(Title, id3_songinfo->Title);
          else
            strcpy(Title, ""); 	

          if(id3_songinfo->Genre != NULL)
            strcpy(Genre, id3_songinfo->Genre);
          else
            strcpy(Genre, ""); 	

          if(id3_songinfo->Year >1800)
            Year = id3_songinfo->Year;
          else
            Year = -1; 	
        }
      }            

      char *cmd;
      std::string file;
      file = "\"";
      file = file + Song->Fullname();
      file = file + "\"";

      d(printf("music: rating: Edit ID3Tag 'POPM' to '%s:%i'\n",MP3Setup.RateCompat,rating));
      asprintf(&cmd, "%s --POPM '%s:%i:0' %s", Datei.c_str(), MP3Setup.RateCompat, rating, file.c_str());
      isyslog("music: rating: Executed <%s>\n", cmd);
      system(cmd);
      free(cmd);
      
      mgr->UpdateSong(Artist, Album, Title, Genre, Year, rating, Song);
      d(printf("music: rating: Update Rating to %i \n", rating))
    }
  }
  else {
    mgr->RateSong(rating, Song);    
//    InfoCache.Modified();
  }    
  
 return osBack;
}


eOSState cMP3Rating::ProcessKey(eKeys Key)
{

  bool hadSubmenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (hadSubmenu && !HasSubMenu()) return osBack;

  if (state == osUnknown) {
    switch (Key) {
        case kRed:
	case kBack:   return osBack;
	    break;
    case kBlue:
    case kOk:
        return SetRating();
	    break;
    default:      state = osContinue;
    }
  }
  return state;
}
