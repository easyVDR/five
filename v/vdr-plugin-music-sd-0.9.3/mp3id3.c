#ifdef HAVE_ICONPATCH
#include <vdr/iconpatch.h>
#endif

#include "mp3id3.h"
#include "i18n.h"
#include "player-mp3.h"
#include "decoder.h"
#include "icons.h"

// ------------ cMP3SearchID3Info --------------------------------------------------------------------------------------------------- //
cMP3SearchID3Info::cMP3SearchID3Info(cSong *song)
:cOsdMenu(tr("Music-SD: ID3 Information"),16)
{
  Build(song->Info(false),song->Fullname());
}

cMP3SearchID3Info::cMP3SearchID3Info(cSongInfo *si, const char *name)
:cOsdMenu(tr("Music-SD: ID3 Information"),16)
{
  Build(si,name);
}

void cMP3SearchID3Info::Build(cSongInfo *si, const char *name)
{
  if(si) {
    Item(tr("Filename"),name);
    if(si->HasInfo() && si->Total>0) {
      char *buf1=0;
      char *buf2=0;
      int totaltime = mgr->GetListLength();
      asprintf(&buf1,"%02d:%02d",si->Total/60,si->Total%60);
      asprintf(&buf2,"%02d:%02d", totaltime/60, totaltime%60);
      Item(tr("Length"),buf1);
      free(buf1);
      Item(tr("Title"),si->Title);
      Item(tr("Artist"),si->Artist);
      Item(tr("Album"),si->Album);
      Item(tr("Year"),0,(float)si->Year);
      Item(tr("Genre"),si->Genre);
	  
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
      Item(tr("Samplerate"),"%.1f kHz",si->SampleFreq/1000.0);
      Item(tr("Bitrate"),"%.f kbit/s",si->Bitrate/1000.0);
      Item(tr("Channels"),0,(float)si->Channels);
      Item(tr("Comment"),si->Comment);
      Item(tr("Playlist time"), buf2);
      Item(tr("Playlist tracks"),0,(float)(mgr->maxIndex +1));
      free(buf2);
      }
    Display();
    }
}


cMP3SearchID3Info::~cMP3SearchID3Info()
{

}

cOsdItem *cMP3SearchID3Info::Item(const char *name, const char *format, const float num)
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

cOsdItem *cMP3SearchID3Info::Item(const char *name, const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s\t%s",name,text?text:"");
  cOsdItem *item = new cOsdItem(buf,osBack);
//  item->SetSelectable(false);
  free(buf);
  Add(item); return item;
}


eOSState cMP3SearchID3Info::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osUnknown) {
     switch(Key) {
       case kRed:
       case kGreen:
       case kYellow:
       case kOk:
       case kBlue:   state = osBack;
       default: state = osContinue;;
       }
     }
  return state;
}
