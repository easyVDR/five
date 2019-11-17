#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <typeinfo>
#include <string>
#include <fstream>
#include <glob.h>
#include <vdr/menuitems.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include <vdr/osd.h>
#include <vdr/interface.h>
#include <vdr/skins.h>
#include <vdr/audio.h>
#include <vdr/font.h>


#include "mp3control.h"
#include "common.h"
#include "skin.h"
#include "setup.h"
#include "setup-mp3.h"
#include "data-mp3.h"
#include "player-mp3.h"
#include "menu.h"
#include "decoder.h"
#include "decoder-mp3.h"
#include "i18n.h"
#include "stream.h"
#include "vars.h"
#include "config.h"
#include "visual.h"
#include "icons.h"


#ifdef DEBUG
#include <mad.h>
#endif


// OK
// --- cMP3Control -------------------------------------------------------------
cMP3Control::cMP3Control(void)
:cControl(player=new cMP3Player)
,cmdMenu(NULL),id3infoMenu(NULL),rateMenu(NULL),trackList(NULL)
{
  showbuttons = 0;

  jumpactive=false;
  skipfwd=skiprew=visible=selecting=refresh=muted=showmessage=isstream=false;
  ShowSA=MP3Setup.EnableVis;
  lastkeytime=number=vol=laststatus=0;
  visualization=channelsSA=bandsSA=cw=rows=0;
  lastMode=0;
  framesPerSecond=SecondsToFrames(1);
  osd=0;

  BgrdPixmap = NULL;
  PlaystatusPixmap = NULL;
  CoverPixmap = NULL;
  VisPixmap = NULL;
  RatingPixmap = NULL;
  AlbumPixmap = NULL;
  GenrePixmap = NULL;
  YearPixmap = NULL;
  ModePixmap = NULL;
  NumberPixmap = NULL;
  VolumeBGPixmap = NULL;
  VolumeFGPixmap = NULL;
  VolumePixmap = NULL;
  IconsPixmap = NULL;
  TracksPixmap = NULL;
  TracksRatingPixmap = NULL;
  ArtistPixmap = NULL;
  TitlePixmap = NULL;
  ProgressBGPixmap = NULL;
  ProgressFGPixmap = NULL;
  ProgressPixmap = NULL;
  ButtonPixmap = NULL;
  CoverOnlyPixmap = NULL;


  MP3Setup.EnableShutDown = 0;

  cStatus::MsgReplaying(this,"Music",0,true);

  MP3Setup.showcoveronly = false;
  coversize = 0;

// GraphTFT stuff
  cPlugin* Plugin = cPluginManager::GetPlugin("graphtft");
  if (Plugin) {
    gtft_exist = true;
    d(printf("music: ctrl: graphtft exist\n"));
	}
  else {
    gtft_exist = false;   
    d(printf("music: ctrl: graphtft dont exist\n"));
  }

  SetFonts();

   
}


// OK
cMP3Control::~cMP3Control()
{
  if(cmdMenu) {delete cmdMenu;}
  cmdMenu = NULL;

  if(id3infoMenu) {delete id3infoMenu;}
  id3infoMenu = NULL;

  if(rateMenu) {delete rateMenu;}
  rateMenu = NULL;

  if(trackList) {delete trackList;}
  trackList = NULL;

  if(pFontStatus)     delete pFontStatus;
  if(pFontRating)     delete pFontRating;
  if(pFontGenre)      delete pFontGenre;
  if(pFontInfo)       delete pFontInfo;
  if(pFontSymbol)     delete pFontSymbol;
  if(pFontList)       delete pFontList;
  if(pFontListRating) delete pFontListRating;
  if(pFontInterpret)  delete pFontInterpret;
  if(pFontTitle)      delete pFontTitle;
  if(pFontButton)     delete pFontButton;

  delete lastMode;
  Hide();
  InfoCache.Save(true);
  Stop();
}

// OK
// Convert to UpperCase
int cMP3Control::Tupper(char *q)
{
  unsigned char c = NULL;

  while (*q) { c = *q; *q = toupper(c); q++;}

  return c;
}


// OK
void cMP3Control::Stop(void)
{
  if(MP3Setup.RecordStream) {cmdMenu->StopRecord();}

  cStatus::MsgReplaying(this,0,0,false);

  delete player; player=0;
  mgr->Halt();
  mgr->Flush(); //XXX remove later
}


// OK
bool cMP3Control::SetPlayList(cPlayList *plist)
{
  bool res;
  cControl *control=cControl::Control();
  // is there a running MP3 player?
  if(control && typeid(*control)==typeid(cMP3Control)) {
    // add songs to running playlist
    mgr->Add(plist);
    res=true;
    }    
  else {
    mgr->Flush();
    mgr->Add(plist);
    cControl::Launch(new cMP3Control);
    res=false;
  }
  delete plist;
  return res;
}


// OK
void cMP3Control::Hide(void)
{
	
#ifdef BUGHUNT
  printf("OSD HIDE\n");	
#endif
  BgrdPixmap = NULL;
  PlaystatusPixmap = NULL;
  CoverPixmap = NULL;
  VisPixmap = NULL;
  RatingPixmap = NULL;
  AlbumPixmap = NULL;
  GenrePixmap = NULL;
  YearPixmap = NULL;
  ModePixmap = NULL;
  NumberPixmap = NULL;
  VolumeBGPixmap = NULL;
  VolumeFGPixmap = NULL;
  VolumePixmap = NULL;
  IconsPixmap = NULL;
  TracksPixmap = NULL;
  TracksRatingPixmap = NULL;
  ArtistPixmap = NULL;
  TitlePixmap = NULL;
  ProgressBGPixmap = NULL;
  ProgressFGPixmap = NULL;
  ProgressPixmap = NULL;
  ButtonPixmap = NULL;
  CoverOnlyPixmap = NULL;

	
  if (cmdMenu) {
    delete cmdMenu;
    cmdMenu=NULL;
    }

  if (id3infoMenu) {
    delete id3infoMenu;
    id3infoMenu=NULL;
    }

  if (rateMenu) {
    delete rateMenu;
    rateMenu=NULL;
    }

  if (trackList) {
    delete trackList;
    trackList=NULL;
    }

  if(visible) {
    delete osd; osd=0;
    visible = false;
    SetNeedsFastResponse(visible);
    }
}


// OK
void cMP3Control::ShowSpectrumAnalyzer(void)
{
  if (cPluginManager::CallFirstService(SPAN_GET_BAR_HEIGHTS_ID, NULL))	{
    Span_GetBarHeights_v1_0 GetBarHeights;
    unsigned int bar;
    unsigned int *barHeights             = new unsigned int[bandsSA];
    unsigned int *barHeightsLeftChannel  = new unsigned int[bandsSA];
    unsigned int *barHeightsRightChannel = new unsigned int[bandsSA];
    unsigned int volumeLeftChannel;
    unsigned int volumeRightChannel;
    unsigned int volumeBothChannels;
    unsigned int *barPeaksBothChannels   = new unsigned int[bandsSA];
    unsigned int *barPeaksLeftChannel    = new unsigned int[bandsSA];
    unsigned int *barPeaksRightChannel   = new unsigned int[bandsSA];
 								
    GetBarHeights.bands 		 = bandsSA;
    GetBarHeights.barHeights		 = barHeights;
    GetBarHeights.barHeightsLeftChannel  = barHeightsLeftChannel;
    GetBarHeights.barHeightsRightChannel = barHeightsRightChannel;
    GetBarHeights.volumeLeftChannel	 = &volumeLeftChannel;
    GetBarHeights.volumeRightChannel	 = &volumeRightChannel;
    GetBarHeights.volumeBothChannels	 = &volumeBothChannels;
    GetBarHeights.name			 = "Music";
    GetBarHeights.falloff		 = barfalloff;
    GetBarHeights.barPeaksBothChannels	 = barPeaksBothChannels;
    GetBarHeights.barPeaksLeftChannel	 = barPeaksLeftChannel;
    GetBarHeights.barPeaksRightChannel	 = barPeaksRightChannel;

    if ( cPluginManager::CallFirstService(SPAN_GET_BAR_HEIGHTS_ID, &GetBarHeights )) {
        int i;
        int barPeak       = 0;
        int covertop      = covery;

        if(player !=NULL) {
            switch(visualization) {
              case 1: // MONO x-Fach
                  for ( i=0; i < bandsSA; i++ ) {
                    bar      = (barHeights[i]*(barheight))/100;
                    barPeak  = (barPeaksBothChannels[i]*(barheight))/100;
                    if(barPeak > 0) {
                      VisPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)  , 0                       , channel1left + (i*barwidth) +barwidth -1 , 0 + (barheight) - barPeak -barpeakheight), barcolor);
                      VisPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)  , 0 +(barheight) -barPeak +1 , channel1left + (i*barwidth) +barwidth -1 , 0 + (barheight) - bar)                   , barcolor);
                      }
                    else {
                      VisPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)  , 0                       , channel1left + (i*barwidth) +barwidth -1 , 0 +(barheight) - bar)                    , barcolor);
                    }  
                  }
                  break;

              case 2: // Stereo x-Fach
                    for ( i=0; i < bandsSA; i++ ) {
                      // Left channel
                      bar = barHeightsLeftChannel[i]*(barheight)/100;
                      if(bar > 0) {
                        VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , 0                   , barwidth  , (barheight) - bar)   , barcolor);
                        VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , barheight - bar +1  , barwidth  , bar -1)              , barcolor2);
                        }
                      else {
                        VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , 0                   , barwidth  ,  barheight)          , barcolor);
                      }		       	 

                      // right channel
                      bar = barHeightsRightChannel[i]*(barheight)/100;
                      if( bar > 0) {
                        VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)  , 0                , barwidth  , (barheight) - bar)   , barcolor);
                        VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)  , barheight - bar  , barwidth  , bar)                 , barcolor2);
                        }
                      else {
                        VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)  , 0                , barwidth  ,  barheight)          , barcolor);
                      }
                    }
                  break;

              case 3: // Stereo 2-Fach
                  for ( i=0; i < bandsSA; i++ ) {

                    // left
                    bar      = (volumeLeftChannel*(barheight))/100;
                    VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)   , 0                , barwidth  , barheight - bar)     , barcolor);
                    VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)   , barheight - bar  , barwidth  , bar)                 , barcolor2);

                    // right
                    bar      = (volumeRightChannel*(barheight))/100;
                    VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)   , 0                , barwidth  , barheight - bar)     , barcolor);
                    VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)   , barheight - bar  , barwidth  , bar)                 , barcolor2);
                  }
                  break;


              case 4: // Stereo x-Fach with PEAK
                  for ( i=0; i < bandsSA; i++ ) {

                    // Left channel
                    bar = barHeightsLeftChannel[i]*(barheight)/100;
                    barPeak = barPeaksLeftChannel[i]*(barheight)/100;
//                    printf("bar           = %i\n",bar);
//                    printf("barPeak       = %i\n",barPeak);
//                    printf("barpeakheight = %i\n",barpeakheight);
                    if(barPeak > 0) {
                      VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , 0                         , barwidth    , barheight -1)                        , barcolor2);

                      VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , 0                         , barwidth    , barheight -barPeak -barpeakheight)   , barcolor);
                      VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , barheight - barPeak       , barwidth    ,  barpeakheight)                      , barcolor);
                      }
                    else {
                      VisPixmap->DrawRectangle(cRect(channel1left + (i*barwidth)  , 0                         , barwidth    , barheight -bar)                      , barcolor);
                    }		       	 

                    // right channel
                    bar = barHeightsRightChannel[i]*(barheight)/100;
                    barPeak = barPeaksRightChannel[i]*(barheight)/100;
                    if( barPeak > 0) {
                      VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)  , 0                       , barwidth  , (barheight) - bar)   , barcolor);
                      VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)  , barheight - bar         , barwidth  ,  bar)                , barcolor2);
                      }
                    else {
                      VisPixmap->DrawRectangle(cRect(channel2left + (i*barwidth)  , 0                       , barwidth  ,  barheight)          , barcolor);
                    }

                  }
                  break;





/*
############

                    if(barPeak > 0) {
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)    , covertop                         , channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - barPeak -barPeakHeight), clrBlack);
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)    , covertop +(coverh -6) -barPeak +1, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - bar)                   , clrBlack);
                      }
                    else {
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)    , covertop, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - bar), clrBlack);
                    }		       	 
                    bar = barHeightsRightChannel[i]*(coverh -10)/100;
                    barPeak = barPeaksRightChannel[i]*(coverh -10)/100;
                    if( barPeak > 0) {
                      CoverPixmap->DrawRectangle(cRect(channel2left +(i*barwidth), covertop, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - barPeak -barPeakHeight), clrBlack);
                      CoverPixmap->DrawRectangle(cRect(channel2left +(i*barwidth), covertop +(coverh -6) -barPeak +1, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - bar), clrBlack);
                      }




############
*/






                default: break;
            }
        }

            flush = true;
    }
    delete [] barHeights;
    delete [] barHeightsLeftChannel;
    delete [] barHeightsRightChannel;
    delete [] barPeaksBothChannels;
    delete [] barPeaksLeftChannel;
    delete [] barPeaksRightChannel;
  }
  else {
//    CoverPixmap->DrawRectangle(cRect(coverx, covery, coverx + coverw, covery + coverh), barcolor);
    VisPixmap->DrawRectangle(cRect(0, 0, coverw, coverh), barcolor);
    flush = true;
  }
}




/*
// OK
void cMP3Control::ShowSpectrumAnalyzer(void)
{
  if (cPluginManager::CallFirstService(SPAN_GET_BAR_HEIGHTS_ID, NULL))	{
    Span_GetBarHeights_v1_0 GetBarHeights;
    unsigned int bar;
    unsigned int *barHeights             = new unsigned int[bandsSA];
    unsigned int *barHeightsLeftChannel  = new unsigned int[bandsSA];
    unsigned int *barHeightsRightChannel = new unsigned int[bandsSA];
    unsigned int volumeLeftChannel;
    unsigned int volumeRightChannel;
    unsigned int volumeBothChannels;
    unsigned int *barPeaksBothChannels   = new unsigned int[bandsSA];
    unsigned int *barPeaksLeftChannel    = new unsigned int[bandsSA];
    unsigned int *barPeaksRightChannel   = new unsigned int[bandsSA];
 								
    GetBarHeights.bands 		 = bandsSA;
    GetBarHeights.barHeights		 = barHeights;
    GetBarHeights.barHeightsLeftChannel  = barHeightsLeftChannel;
    GetBarHeights.barHeightsRightChannel = barHeightsRightChannel;
    GetBarHeights.volumeLeftChannel	 = &volumeLeftChannel;
    GetBarHeights.volumeRightChannel	 = &volumeRightChannel;
    GetBarHeights.volumeBothChannels	 = &volumeBothChannels;
    GetBarHeights.name			 = "Music";
    GetBarHeights.falloff		 = MP3Setup.FalloffSA;
    GetBarHeights.barPeaksBothChannels	 = barPeaksBothChannels;
    GetBarHeights.barPeaksLeftChannel	 = barPeaksLeftChannel;
    GetBarHeights.barPeaksRightChannel	 = barPeaksRightChannel;

    if ( cPluginManager::CallFirstService(SPAN_GET_BAR_HEIGHTS_ID, &GetBarHeights )) {
        int i;
        int barPeakHeight = 3;
        int barPeak       = 0;
        int covertop      = covery;

        if(player !=NULL) {
            switch(visualization) {
              case 1: // MONO 
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar      = (barHeights[i]*(coverh -10))/100;
                    barPeak  = (barPeaksBothChannels[i]*(coverh -10))/100;
                    if(barPeak > 0) {
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth), covertop, channel1left + (i*barwidth) +barwidth -1, covertop + (coverh -6) - barPeak -barPeakHeight), clrBlack);
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth), covertop +(coverh -6) -barPeak +1, channel1left + (i*barwidth) +barwidth -1, covertop + (coverh -6) - bar), clrBlack);
                      }
                    else {
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth), covertop, channel1left + (i*barwidth) +barwidth -1, covertop +(coverh -6) - bar), clrBlack);
                    }  
                  }
                  break;

              case 2:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar = barHeightsLeftChannel[i]*(coverh -10)/100;
                    barPeak = barPeaksLeftChannel[i]*(coverh -10)/100;
                    if(barPeak > 0) {
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)    , covertop                         , channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - barPeak -barPeakHeight), clrBlack);
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)    , covertop +(coverh -6) -barPeak +1, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - bar)                   , clrBlack);
                      }
                    else {
                      CoverPixmap->DrawRectangle(cRect(channel1left +(i*barwidth)    , covertop, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - bar), clrBlack);
                    }		       	 
                    bar = barHeightsRightChannel[i]*(coverh -10)/100;
                    barPeak = barPeaksRightChannel[i]*(coverh -10)/100;
                    if( barPeak > 0) {
                      CoverPixmap->DrawRectangle(cRect(channel2left +(i*barwidth), covertop, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - barPeak -barPeakHeight), clrBlack);
                      CoverPixmap->DrawRectangle(cRect(channel2left +(i*barwidth), covertop +(coverh -6) -barPeak +1, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - bar), clrBlack);
                      }
                    else {
                      CoverPixmap->DrawRectangle(cRect(channel2left +(i*barwidth), covertop, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - bar), clrBlack);
                    }
                  }
                  break;

              case 3:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar      = (volumeLeftChannel*(coverh -10))/100;
                    CoverPixmap->DrawRectangle(cRect(channel1left , covertop, channel1left + barwidth , covertop + (coverh -6) - bar),  clrBlack);
                    bar      = (volumeRightChannel*(coverh -10))/100;
                    CoverPixmap->DrawRectangle(cRect(channel2left , covertop, channel2left + barwidth , covertop + (coverh -6) - bar), clrBlack);
                  }
                  break;

                default: break;
            }
        }

            flush = true;
    }
    delete [] barHeights;
    delete [] barHeightsLeftChannel;
    delete [] barHeightsRightChannel;
    delete [] barPeaksBothChannels;
    delete [] barPeaksLeftChannel;
    delete [] barPeaksRightChannel;
  }
  else {
//    CoverPixmap->DrawRectangle(cRect(coverx, covery, coverx + coverw, covery + coverh), clrBlack);
    CoverPixmap->DrawRectangle(cRect(0, 0, coverw, coverh), clrBlack);
    flush = true;
  }
}
*/

// OK
void cMP3Control::LoadVisCover(void)
{
//  if ( Load(visimagefile.c_str()) )
//    DrawCover(visimagefile.c_str(), coverw, coverh, false);
#ifdef DEBUG_COVER
  printf("LoadVisCover started\n");
#endif

  cImageloadBitmap osdbitmap;
  if(osdbitmap.Load( visimagefile.c_str())) {
    osdbitmap.LoadImage(visimagefile.c_str(), coverw, coverh, false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    CoverPixmap->Clear();
    CoverPixmap->DrawImage(cPoint(0,0), img);
  }
  
#ifdef DEBUG_COVER
  printf("LoadVisCover ended\n");
#endif

}


// OK
void cMP3Control::LoadCoverOnly(void)
{
#ifdef DEBUG_COVER
  printf("Start LoadCoverOnly\n");
#endif

  int w1=120;
  int h1=120;

  if (coverpicture == NULL) {
    esyslog("music: ctrl: Oops cant load coverpicture : %s",coverpicture);
    }
  else {
    switch(coversize) {
        case 0:
          w1 = 120;
          h1 = 120;
          Flush();
          break;  
        case 1:  
          w1 = 274;
          h1 = 274;
          Flush();
          break;
        case 2:
          w1 = 300;
          h1 = 256;
          Flush();
          break;
        case 3:
          w1 = MP3Skin.CoverOnly_Width;
          h1 = MP3Skin.CoverOnly_Height;
          Flush();
          break;
    }


    cImageloadBitmap osdbitmap;
    if((osdbitmap.Load(coverpicture))) {
      osdbitmap.LoadImage(coverpicture, w1 , h1 , true, 0, false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      CoverOnlyPixmap->Clear();
      CoverOnlyPixmap->DrawImage(cPoint( osdbitmap.newWidth() < w1 ? (w1 - osdbitmap.newWidth()) / 2 : 0,0), img);
    }
  }


#ifdef DEBUG_COVER
  printf("LoadCoverOnly ended\n");
#endif

}


// OK
void cMP3Control::LoadCover(void)
{

  cImageloadBitmap osdbitmap;
  if((osdbitmap.Load(coverpicture))) {
    osdbitmap.LoadImage(coverpicture, coverw , coverh , false);
#ifdef DEBUG_COVER
    printf("music: mp3control.c: cImage img(cSize(%d,%d))\n", osdbitmap.newWidth(), osdbitmap.newHeight() );
#endif
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    CoverPixmap->Clear();
    CoverPixmap->DrawImage(cPoint(0,0), img);
  }

}




// OK
void cMP3Control::LoadBackground(void)
{

  std::string background;

  background = MP3Skin.SkinPath + MP3Skin.OsdBgrd;


#ifdef DEBUG_COVER
  printf("Load Background '%s'\n", background.c_str());
#endif

  cImageloadBitmap osdbitmap;
  if((osdbitmap.Load(background.c_str()))) {
    osdbitmap.LoadImage(background.c_str(), MP3Skin.Bgrd_Width , MP3Skin.Bgrd_Height , false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    BgrdPixmap->DrawImage(cPoint(0,0), img);
  }
}


// OK
void cMP3Control::ShowCoverOnly(bool open)
{
  int index, total;
  MusicServicePlayerInfo_1_0 gtftStatus;
  int gtftChanges = 0;

  if(player->GetIndex(index,total) && total>=0) {
    if(!cOsd::IsOpen() && !visible && !open) {
      open=true;
    }

    
    if(!visible && open) {
#ifdef DEBUG_COVER
      printf("Not visible but open\n");
#endif

      if(MP3Setup.ShowMessage)
        osd=cOsdProvider::NewOsd(osdleft, osdtop, 20);
      else
        osd=cOsdProvider::NewOsd(osdleft, osdtop);

      if(!osd) return;
      if (!BuildAreas()) return;

      mgr->playnow = false;

      BuildDefault();

      visible = true;
      
      force = true;
      
      delete lastMode; lastMode=0;      
    }
	
	
	
/*	
	
  if(!cOsd::IsOpen()) {
    if(!visible && !open) {
      open=true;
    }
  }
*/
 
    cMP3PlayInfo *mode=new cMP3PlayInfo;
    bool valid=mgr->Info(-1,mode);
    bool changed=(!lastMode || mode->Hash!=lastMode->Hash || refresh);
    char buf[256];

    if(valid) { // send progress to status monitor
      if(changed || mode->Loop!=lastMode->Loop || mode->Shuffle!=lastMode->Shuffle) {
        snprintf(buf,sizeof(buf),mode->Artist[0]?"[%c%c] (%d/%d) %s - %s":"[%c%c] (%d/%d) %s",
                 mode->Loop?'L':'.',mode->Shuffle?'S':'.',mode->Num,mode->MaxNum,mode->Title,mode->Artist);

        if(mode->Filename && !(strcmp(mode->Filename,lastfile) ==0)) {
          cStatus::MsgReplaying(this,buf,mode->Filename[0]?mode->Filename:0,true);
        }
      }
    }

    if(visible) { // refresh the OSD display
      flush=false;

      if ( mode->Filename && !(strcmp(mode->Filename,lastfile) ==0) || force) {
        strncpy(lastfile, mode->Filename,sizeof(lastfile));
        force = false;

        LoadImage(mode->Filename, mode->Artist, true);
        LoadCoverOnly();
        flush = true;
      }
      if(flush) Flush();
    }

    SetNeedsFastResponse(false);
    delete lastMode; lastMode=mode;
    refresh = false;
  }
}


void cMP3Control::Flush(void)
{
  if(osd) osd->Flush();
}


// OK
void cMP3Control::ShowHelpButtons(int ShowButtons) {
  int align;
  align       = MP3Skin.Button_Align;
  showbuttons = ShowButtons;


  const char* red = "";
  const char* green = "";
  const char* yellow = "";
  const char* blue = "";  
  
  switch(showbuttons) {
       case 0:
         red = MP3Setup.RatingFirst ? tr("Rating") : tr("Commands");
         green = tr("Track-");
         yellow = tr("Track+");
         blue = tr("More..");
         break;
      case 1:
         red = MP3Setup.RatingFirst ? tr("Commands") : tr("Rating") ;
         green = tr("Cover");
         yellow = tr("Jump");
         blue = MP3Setup.AdminMode ? tr("More..") : tr("Parent");
         break;
      case 2:  	
         red = tr("Delete");
         green = tr("Clear");
         yellow = tr("Copy");
         blue = tr("More..");
         break; 
      case 3:  	
         red = tr("Parent");
         green = tr("<<");
         yellow = tr(">>");
         blue = tr("Min/Sec");
         break;
      case 4:
         red = tr("Visuals");
         green = tr("ID3Info");
         yellow = tr("Shoutcast");
         blue = tr("Parent");
         break;
  }

  ButtonPixmap->Clear();

  ButtonPixmap->DrawText( cPoint(MP3Skin.ButtonText1posx , MP3Skin.ButtonText1posy) , red    , clrStatusRed    , clrTextBG, pFontButton, MP3Skin.Button1w, MP3Skin.Button_Height, align);
  ButtonPixmap->DrawText( cPoint(MP3Skin.ButtonText2posx , MP3Skin.ButtonText2posy) , green  , clrStatusGreen  , clrTextBG, pFontButton, MP3Skin.Button2w, MP3Skin.Button_Height, align);
  ButtonPixmap->DrawText( cPoint(MP3Skin.ButtonText3posx , MP3Skin.ButtonText3posy) , yellow , clrStatusYellow , clrTextBG, pFontButton, MP3Skin.Button3w, MP3Skin.Button_Height, align);
  ButtonPixmap->DrawText( cPoint(MP3Skin.ButtonText4posx , MP3Skin.ButtonText4posy) , blue   , clrStatusBlue   , clrTextBG, pFontButton, MP3Skin.Button4w, MP3Skin.Button_Height, align);

  sendHelpButtons(red, green, yellow, blue);

  refresh = true;
#ifdef BUGHUNT
  printf("ATTENTION: Buttons drawed\n");
#endif
}



// OK
void cMP3Control::ConvertRatingToChar_Tracks( int Posx, int Posy, int Value, int bgClr, int fgClr, int imagewidth, int imageheight ) {

  int bg=bgClr;
  int fg=fgClr;
  int posx=Posx;
  int posy=Posy;
  int value=Value;
  int w=imagewidth;
  int h=imageheight;

  std::string listrating_file;
  listrating_file = MP3Skin.SkinPath + "ratingstars/";

  if(value <0) value=0;

#ifdef DEBUG
  isyslog("music: ctrl: loading trackrating images from '%s'", listrating_file.c_str());
#endif

  switch(value) {
     case 0: // Empty
       listrating_file = listrating_file + "list_rating_00.png";
       break;
     case 3: // Delete
       listrating_file = listrating_file + "list_rating_00.png";
       break;
     case 28: // D
       listrating_file = listrating_file + "list_rating_05.png";
       break;
     case 53: // O
       listrating_file = listrating_file + "list_rating_10.png";
       break;
     case 78: // OD
       listrating_file = listrating_file + "list_rating_15.png";
       break;
     case 104: // OO
       listrating_file = listrating_file + "list_rating_20.png";
       break;
     case 129: // OOD
       listrating_file = listrating_file + "list_rating_25.png";
       break;
     case 154: // OOO
       listrating_file = listrating_file + "list_rating_30.png";
       break;
     case 179: // OOOD
       listrating_file = listrating_file + "list_rating_35.png";
       break; 
     case 205: // OOOO
       listrating_file = listrating_file + "list_rating_40.png";
       break;
     case 230: // OOOOD
       listrating_file = listrating_file + "list_rating_45.png";
       break;
     case 255: // OOOOO
       listrating_file = listrating_file + "list_rating_50.png";
       break;
     case 300: // Nothing
       TracksRatingPixmap->DrawRectangle(cRect(posx, posy, posx + w, posy + h), bg);
       break;

     default: // Empty
       break;
  }

/*
#ifdef BUGHUNT
  printf("ConvertRatingToChar: Left        : %i\n",posx);
  printf("ConvertRatingToChar: Top         : %i\n",posy);
  printf("ConvertRatingToChar: Value       : %i\n",value);
  printf("ConvertRatingToChar: ImageWidth  : %i\n",w);
  printf("ConvertRatingToChar: ImageHeight : %i\n",h);
  printf("ConvertRatingToChar: File        : %s\n",listrating_file.c_str());

#endif
*/


//  TracksRatingPixmap->Clear();
  cImageloadBitmap osdbitmap;
  if(osdbitmap.Load(listrating_file.c_str())) {
    osdbitmap.LoadImage(listrating_file.c_str(), w , h , false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    TracksRatingPixmap->DrawImage(cPoint(posx,posy), img);
  }

  flush = true;
}


// OK
void cMP3Control::ConvertRatingToChar_Rating( int Posx, int Posy, int Value, int bgClr, int fgClr, int imagewidth, int imageheight ) {

  int bg=bgClr;
  int fg=fgClr;
  int posx=Posx;
  int posy=Posy;
  int value=Value;
  int w=imagewidth;
  int h=imageheight;

  std::string rating_file;
  rating_file = MP3Skin.SkinPath + "ratingstars/";

#ifdef DEBUG
  isyslog("music: ctrl: loading rating image from '%s'", rating_file.c_str());
#endif


  if(value <0) value=0;

  switch(value) {
     case 0: // Empty
       rating_file = rating_file + "list_rating_00.png";
       break;
     case 3: // Delete
       rating_file = rating_file + "list_rating_00.png";
       break;
     case 28: // D
       rating_file = rating_file + "list_rating_05.png";
       break;
     case 53: // O
       rating_file = rating_file + "list_rating_10.png";
       break;
     case 78: // OD
       rating_file = rating_file + "list_rating_15.png";
       break;
     case 104: // OO
       rating_file = rating_file + "list_rating_20.png";
       break;
     case 129: // OOD
       rating_file = rating_file + "list_rating_25.png";
       break;
     case 154: // OOO
       rating_file = rating_file + "list_rating_30.png";
       break;
     case 179: // OOOD
       rating_file = rating_file + "list_rating_35.png";
       break; 
     case 205: // OOOO
       rating_file = rating_file + "list_rating_40.png";
       break;
     case 230: // OOOOD
       rating_file = rating_file + "list_rating_45.png";
       break;
     case 255: // OOOOO
       rating_file = rating_file + "list_rating_50.png";
       break;
     case 300: // Nothing
       RatingPixmap->DrawRectangle(cRect(posx, posy, posx + w, posy + h), bg);
       break;

     default: // Empty
       break;
  }

  cImageloadBitmap osdbitmap;
  if(osdbitmap.Load(rating_file.c_str())) {
    osdbitmap.LoadImage(rating_file.c_str(), w , h , false, 0, false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    RatingPixmap->DrawImage(cPoint(posx,posy), img);
  }

  flush = true;
}




// OK
void cMP3Control::SetVars(void)
{
  artistfirst            = MP3Setup.ArtistFirst;
  // fontheight
  fh                     = MP3Skin.FontHeight;
  sfh                    = MP3Skin.SmallFontHeight;

  rows                   = MP3Skin.TrackCount;
  coverw                 = MP3Skin.Cover_Width;
  coverh                 = MP3Skin.Cover_Height;
  coverx                 = MP3Skin.Cover_Left;
  covery                 = MP3Skin.Cover_Top;

  // Colors
  clrTextBG              = MP3Skin.clrTextBG;
  clrArtist              = MP3Skin.clrArtist;
  clrRating              = MP3Skin.clrRating;
  clrRatingBG            = MP3Skin.clrRatingBG;

  clrPlayStatus          = MP3Skin.clrPlayStatus;
  clrInfo                = MP3Skin.clrInfo;

  if ( player->IsStream() )
    clrProgressbarBG    = 0xFF000000;
  else
    clrProgressbarBG    = MP3Skin.clrProgressbarBG;

  clrProgressbarFG       = MP3Skin.clrProgressbarFG;
  clrListTitle           = MP3Skin.clrListTitle;
  clrList                = MP3Skin.clrList;
  clrListRating          = MP3Skin.clrListRating;
  clrListRatingBG        = MP3Skin.clrListRatingBG;
  clrSymbol              = MP3Skin.clrSymbol;
  clrSymbolActive        = MP3Skin.clrSymbolActive;
  clrRecordingActive     = MP3Skin.clrRecordingActive;
  clrVolumeActive        = MP3Skin.clrVolumeActive;
  clrVolumeProgressbarBG = MP3Skin.clrVolumeProgressbarBG;
  clrVolumeProgressbarFG = MP3Skin.clrVolumeProgressbarFG;

  clrStatusRed           = MP3Skin.clrStatusRed;
  clrStatusGreen         = MP3Skin.clrStatusGreen;
  clrStatusYellow        = MP3Skin.clrStatusYellow;
  clrStatusBlue          = MP3Skin.clrStatusBlue;


  osdheight              = MP3Skin.OsdHeight;
  osdwidth               = MP3Skin.OsdWidth;
  osdtop                 = MP3Skin.OsdTop;
  osdleft                = MP3Skin.OsdLeft;
  osdalpha               = MP3Skin.OsdAlpha;


  visimagefile           = MP3VisLoader.VisLoaderBackground();
  visualization          = atoi(MP3VisLoader.VisLoaderVisualization()); 
  channel1left           = atoi(MP3VisLoader.VisLoaderChannel1Left());
  channel2left           = atoi(MP3VisLoader.VisLoaderChannel2Left());
  barwidth               = atoi(MP3VisLoader.VisLoaderBarWidth());
  barheight              = atoi(MP3VisLoader.VisLoaderBarHeight());
  barpeakheight          = atoi(MP3VisLoader.VisLoaderBarPeakHeight());
  barfalloff             = atoi(MP3VisLoader.VisLoaderBarFallOff());
  channelsSA             = atoi(MP3VisLoader.VisLoaderChannelsSA());
  barcolor               = strtoul(MP3VisLoader.VisLoaderBarColor(), NULL , 16);
  barcolor2              = strtoul(MP3VisLoader.VisLoaderBarColor2(), NULL , 16);
  bandsSA                = atoi(MP3VisLoader.VisLoaderBandsSA());
}



// OK
void cMP3Control::SetFonts(void)
{
  if (MP3Skin.ReloadFonts) {
    if(pFontStatus)      delete pFontStatus;
    if(pFontRating)      delete pFontRating;
    if(pFontGenre)       delete pFontGenre;
    if(pFontInfo)        delete pFontInfo;
    if(pFontSymbol)      delete pFontSymbol;
    if(pFontList)        delete pFontList;
    if(pFontListRating)  delete pFontListRating;
    if(pFontInterpret)   delete pFontInterpret;
    if(pFontTitle)       delete pFontTitle;
    if(pFontButton)      delete pFontButton;

    MP3Skin.ReloadFonts=0;
  }
  pFontStatus     = cFont::CreateFont(MP3Skin.FONTSTATUSNAME.c_str()    , MP3Skin.FONTSTATUSSIZE    , 0);
  pFontRating     = cFont::CreateFont(MP3Skin.FONTRATINGNAME.c_str()    , MP3Skin.FONTRATINGSIZE    , 0);
  pFontGenre      = cFont::CreateFont(MP3Skin.FONTGENRENAME.c_str()     , MP3Skin.FONTGENRESIZE     , 0);
  pFontInfo       = cFont::CreateFont(MP3Skin.FONTINFONAME.c_str()      , MP3Skin.FONTINFOSIZE      , 0);
  pFontSymbol     = cFont::CreateFont(MP3Skin.FONTSYMBOLNAME.c_str()    , MP3Skin.FONTSYMBOLSIZE    , 0);
  pFontList       = cFont::CreateFont(MP3Skin.FONTLISTNAME.c_str()      , MP3Skin.FONTLISTSIZE      , 0);
  pFontListRating = cFont::CreateFont(MP3Skin.FONTLISTRATINGNAME.c_str(), MP3Skin.FONTLISTRATINGSIZE, 0);
  pFontInterpret  = cFont::CreateFont(MP3Skin.FONTINTERPRETNAME.c_str() , MP3Skin.FONTINTERPRETSIZE , 0);
  pFontTitle      = cFont::CreateFont(MP3Skin.FONTTITLENAME.c_str()     , MP3Skin.FONTTITLESIZE     , 0);
  pFontButton     = cFont::CreateFont(MP3Skin.FONTBUTTONNAME.c_str()    , MP3Skin.FONTBUTTONSIZE    , 0);

}



// OK
void cMP3Control::SetVol(void)
{

  std::string mutebmp_file;
  std::string volfgbmp_file;
  std::string volbgbmp_file;

  vol   = player->CurrentVolume();
  muted = player->Muted();

  VolumeFGPixmap->Clear();

  mutebmp_file = MP3Skin.SkinPath + "icons/";
  if(muted || vol == 0)
    mutebmp_file = mutebmp_file + "mute.png";
  else
    mutebmp_file = mutebmp_file + "no_mute.png";

  volfgbmp_file = MP3Skin.SkinPath + "icons/";
  volfgbmp_file = volfgbmp_file + "volfgbmp.png";

  volbgbmp_file = MP3Skin.SkinPath + "icons/";
  volbgbmp_file = volbgbmp_file + "volbgbmp.png";


  int p = vol * MP3Skin.VolumeFG_Scale / 255;

  cImageloadBitmap osdbitmap;
  if(osdbitmap.Load(volbgbmp_file.c_str())) {
    osdbitmap.LoadImage(volbgbmp_file.c_str(), MP3Skin.VolumeBG_Width, MP3Skin.VolumeBG_Height, false, 0, true);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    img.Clear();
    osdbitmap.Render(img);
    VolumeBGPixmap->DrawImage(cPoint(0, 0), img);
  }


  if (MP3Skin.VolumeFG_Slider > 0) {
    if(osdbitmap.Load(volfgbmp_file.c_str())) {
      osdbitmap.LoadImage(volfgbmp_file.c_str(), MP3Skin.VolumeFG_Width , MP3Skin.VolumeFG_Height , false, 0, true);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      img.Clear();
      osdbitmap.Render(img);
      VolumeFGPixmap->DrawImage(cPoint(p , 0), img);
      }
    }
  else {
    if(osdbitmap.Load(volfgbmp_file.c_str())) {
      osdbitmap.LoadImage(volfgbmp_file.c_str(), MP3Skin.VolumeFG_Width , MP3Skin.VolumeFG_Height , false, p, true);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      img.Clear();
      osdbitmap.Render(img);
      VolumeFGPixmap->DrawImage(cPoint(p , 0), img);
    }
  }


  if(osdbitmap.Load(mutebmp_file.c_str())) {
    osdbitmap.LoadImage(mutebmp_file.c_str(), MP3Skin.Volume_Width , MP3Skin.Volume_Height , false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    img.Clear();
    osdbitmap.Render(img);
    VolumePixmap->DrawImage(cPoint(0,0), img);
  }

  flush = true;
}


// OK
bool cMP3Control::BuildAreas(void)
{
#ifdef BUGHUNT
  printf("build areas\n");
#endif

  tArea Area[] = { { 0, 0, osdwidth -1,osdheight -1 , 32} };

  eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
  if (result == oeOk) {
    osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
#ifdef BUGHUNT
    printf("areas built\n");
#endif

    return true;
    }
  else  {
      const char *errormsg = NULL;
        switch (result) {
          case oeTooManyAreas:
            errormsg = "music: Too many OSD areas"; break;
          case oeTooManyColors:
            errormsg = "music: Too many colors"; break;
          case oeBppNotSupported:
            errormsg = "music: Depth not supported"; break;	
          case oeAreasOverlap:
            errormsg = "music: Areas are overlapped"; break;
          case oeWrongAlignment:
            errormsg = "music: Areas not correctly aligned"; break;
          case oeOutOfMemory:
            errormsg = "music: OSD memory overflow"; break;
          case oeUnknown:
            errormsg = "music: Unknown OSD error"; break;
          default:
            break;
        }
      esyslog("music: ctrl: ERROR! osd open failed! can't handle areas (%d)-%s\n", result, errormsg);
      if(osd){ delete osd; osd=0;}

      return false;
  }
}

// OK
void cMP3Control::BuildDefault(void)
{
#ifdef BUGHUNT
  printf("Build default\n");
#endif


  if (!MP3Setup.showcoveronly) {
  
    BgrdPixmap = NULL;
    PlaystatusPixmap = NULL;
    CoverPixmap = NULL;
    VisPixmap = NULL;
    RatingPixmap = NULL;
    AlbumPixmap = NULL;
    GenrePixmap = NULL;
    YearPixmap = NULL;
    ModePixmap = NULL;
    NumberPixmap = NULL;
    VolumeBGPixmap = NULL;
    VolumeFGPixmap = NULL;
    VolumePixmap = NULL;
    IconsPixmap = NULL;
    TracksPixmap = NULL;
    TracksRatingPixmap = NULL;
    ArtistPixmap = NULL;
    TitlePixmap = NULL;
    ProgressBGPixmap = NULL;
    ProgressFGPixmap = NULL;
    ProgressPixmap = NULL;
    ButtonPixmap = NULL;


    // load the backgroundpicture on Layer 0 , Pixmap will be created there
    if(!BgrdPixmap) {
#ifdef BUGHUNT
      printf("BgrdPixmap not exists: Creating now\n");
#endif
    	 BgrdPixmap = osd->CreatePixmap(MP3Skin.Bgrd_Layer, cRect(MP3Skin.Bgrd_Left, MP3Skin.Bgrd_Top, MP3Skin.Bgrd_Width, MP3Skin.Bgrd_Height) );
  //  	 BgrdPixmap->Clear();
    }



#ifdef BUGHUNT
    printf("Load Background\n");
#endif
    LoadBackground();

    // Create Pixmap on Layer 1 for Playstatus
    if(!PlaystatusPixmap) {
#ifdef BUGHUNT
      printf("PlaystatusPixmap not exists: Creating now\n");
#endif
      PlaystatusPixmap = osd->CreatePixmap(MP3Skin.Playstatus_Layer, cRect(MP3Skin.Playstatus_Left, MP3Skin.Playstatus_Top, MP3Skin.Playstatus_Width, MP3Skin.Playstatus_Height) );
      PlaystatusPixmap->Clear();
    }

    // Create Pixmap on Layer 2 for Cover
    if(!CoverPixmap) {
#ifdef BUGHUNT
      printf("CoverPixmap not exists: Creating now\n");
#endif
    CoverPixmap = osd->CreatePixmap(MP3Skin.Cover_Layer, cRect(coverx, covery, coverw, coverh));
    CoverPixmap->Clear();
    }


    // Create Pixmap on Layer 3 for Visbar
    if(!VisPixmap) {
#ifdef BUGHUNT
      printf("VisPixmap not exists: Creating now\n");
#endif
    VisPixmap = osd->CreatePixmap(MP3Skin.Cover_Layer, cRect(coverx, covery, coverw, coverh));
    VisPixmap->Clear();
    }


    // Create Pixmap on Layer 3 for Rating
    if(!RatingPixmap) {
#ifdef BUGHUNT
  	   printf("RatingPixmap not exists: Creating now\n");
#endif
      RatingPixmap = osd->CreatePixmap(MP3Skin.Rating_Layer, cRect(MP3Skin.Rating_Left, MP3Skin.Rating_Top, MP3Skin.Rating_Width, MP3Skin.Rating_Height) );
      RatingPixmap->Clear();
    }


    // Create Pixmap om Layer 3 for Album
    if(!AlbumPixmap) {
#ifdef BUGHUNT
  	   printf("AlbumPixmap not exists: Creating now\n");
#endif
      AlbumPixmap = osd->CreatePixmap(MP3Skin.Album_Layer, cRect(MP3Skin.Album_Left, MP3Skin.Album_Top, MP3Skin.Album_Width, MP3Skin.Album_Height) );
      AlbumPixmap->Clear();
    }

    // Create Pixmap on Layer 3 for Genre
    if(!GenrePixmap) {
#ifdef BUGHUNT
  	   printf("GenrePixmap not exists: Creating now\n");
#endif
      GenrePixmap = osd->CreatePixmap(MP3Skin.Genre_Layer, cRect(MP3Skin.Genre_Left, MP3Skin.Genre_Top, MP3Skin.Genre_Width, MP3Skin.Genre_Height) );
      GenrePixmap->Clear();
    }

    // Create Pixmap on Layer 3 for Year
    if(!YearPixmap) {
#ifdef BUGHUNT
  	   printf("YearPixmap not exists: Creating now\n");
#endif
      YearPixmap = osd->CreatePixmap(MP3Skin.Year_Layer, cRect(MP3Skin.Year_Left, MP3Skin.Year_Top, MP3Skin.Year_Width, MP3Skin.Year_Height) );
      YearPixmap->Clear();
    }

    // Create Pixmap on Layer 3 for Mode
    if(!ModePixmap) {
#ifdef BUGHUNT
  	   printf("ModePixmap not exists: Creating now\n");
#endif
      ModePixmap = osd->CreatePixmap(MP3Skin.Mode_Layer, cRect(MP3Skin.Mode_Left, MP3Skin.Mode_Top, MP3Skin.Mode_Width, MP3Skin.Mode_Height) );
      ModePixmap->Clear();
    }

    // Create Pixmap on Layer 3 for Number
    if(!NumberPixmap) {
#ifdef BUGHUNT
  	   printf("NumberPixmap not exists: Creating now\n");
#endif
      NumberPixmap = osd->CreatePixmap(MP3Skin.Number_Layer, cRect(MP3Skin.Number_Left, MP3Skin.Number_Top, MP3Skin.Number_Width, MP3Skin.Number_Height) );
      NumberPixmap->Clear();
    }

    // Create Pixmap on Layer 1 for VolumebarBackground
    if(!VolumeBGPixmap) {
#ifdef BUGHUNT
  	   printf("VolumeBGPixmap not exists: Creating now\n");
#endif
      VolumeBGPixmap = osd->CreatePixmap(MP3Skin.VolumeBG_Layer, cRect(MP3Skin.VolumeBG_Left, MP3Skin.VolumeBG_Top, MP3Skin.VolumeBG_Width, MP3Skin.VolumeBG_Height) );
      VolumeBGPixmap->Clear();
    }

    // Create Pixmap on Layer 3 for VolumebarForeground
    if(!VolumeFGPixmap) {
#ifdef BUGHUNT
  	   printf("VolumeFGPixmap not exists: Creating now\n");
#endif
      VolumeFGPixmap = osd->CreatePixmap(MP3Skin.VolumeFG_Layer, cRect(MP3Skin.VolumeFG_Left, MP3Skin.VolumeFG_Top, MP3Skin.VolumeFG_Width, MP3Skin.VolumeFG_Height) );
      VolumeFGPixmap->Clear();
    }

    // Create Pixmap on Layer 6 for Volume/MuteButton
    if(!VolumePixmap) {
#ifdef BUGHUNT
  	   printf("VolumePixmap not exists: Creating now\n");
#endif
      VolumePixmap = osd->CreatePixmap(MP3Skin.Volume_Layer, cRect(MP3Skin.Volume_Left, MP3Skin.Volume_Top, MP3Skin.Volume_Width, MP3Skin.Volume_Height) );
      VolumePixmap->Clear();
    }

    // Create Pixmap on Layer 3 for Symbols
    if(!IconsPixmap) {
#ifdef BUGHUNT
  	   printf("IconsPixmap not exists: Creating now\n");
#endif
      IconsPixmap = osd->CreatePixmap(MP3Skin.Symbol_Layer, cRect(MP3Skin.Symbol_Left, MP3Skin.Symbol_Top, MP3Skin.Symbol_Width, MP3Skin.Symbol_Height) );
      IconsPixmap->Clear();
    }

    // Create Pixmap on Layer 4 for Tracks
    if(!TracksPixmap) {
#ifdef BUGHUNT
  	   printf("TracksPixmap not exists: Creating now\n");
#endif
      TracksPixmap = osd->CreatePixmap(MP3Skin.Tracks_Layer, cRect(MP3Skin.Tracks_Left, MP3Skin.Tracks_Top, MP3Skin.Tracks_Width, MP3Skin.Tracks_Height) );
      TracksPixmap->Clear();
    }

    // Create Pixmap on Layer 4 for Tracksrating
    if(!TracksRatingPixmap) {
#ifdef BUGHUNT
  	   printf("TracksRatingPixmap not exists: Creating now\n");
#endif
      TracksRatingPixmap = osd->CreatePixmap(MP3Skin.TracksRating_Layer, cRect(MP3Skin.TracksRating_Left, MP3Skin.TracksRating_Top, MP3Skin.TracksRating_Width, MP3Skin.TracksRating_Height) );
      TracksRatingPixmap->Clear();
    }

    // Create Pixmap on Layer 4 for Artist
    if(!ArtistPixmap) {
#ifdef BUGHUNT
  	   printf("ArtistPixmap not exists: Creating now\n");
#endif
      ArtistPixmap = osd->CreatePixmap(MP3Skin.Artist_Layer, cRect(MP3Skin.Artist_Left, MP3Skin.Artist_Top, MP3Skin.Artist_Width, MP3Skin.Artist_Height) );
      ArtistPixmap->Clear();
    }

    // Create Pixmap on Layer 5 for Title
    if(!TitlePixmap) {
#ifdef BUGHUNT
  	   printf("TitlePixmap not exists: Creating now\n");
#endif
      TitlePixmap = osd->CreatePixmap(MP3Skin.Title_Layer, cRect(MP3Skin.Title_Left, MP3Skin.Title_Top, MP3Skin.Title_Width, MP3Skin.Title_Height) );
      TitlePixmap->Clear();
    }

    // Create Pixmap on Layer 1 for ProgressBarBackground
    if(!ProgressBGPixmap) {
#ifdef BUGHUNT
  	   printf("ProgressBGPixmap not exists: Creating now\n");
#endif
      ProgressBGPixmap = osd->CreatePixmap(MP3Skin.ProgressBG_Layer, cRect(MP3Skin.ProgressBG_Left, MP3Skin.ProgressBG_Top, MP3Skin.ProgressBG_Width, MP3Skin.ProgressBG_Height) );
      ProgressBGPixmap->Clear();
    }	
	
    // Create Pixmap on Layer 3 for ProgressBarForeground
    if(!ProgressFGPixmap) {
#ifdef BUGHUNT
  	   printf("ProgressFGPixmap not exists: Creating now\n");
#endif
      ProgressFGPixmap = osd->CreatePixmap(MP3Skin.ProgressFG_Layer, cRect(MP3Skin.ProgressFG_Left, MP3Skin.ProgressFG_Top, MP3Skin.ProgressFG_Width, MP3Skin.ProgressFG_Height) );
      ProgressFGPixmap->Clear();
    }
	
    // Create Pixmap on Layer 6 for Progress (Text, Buttons, ... , so Layer must be higher than BarBG/BarFG)
    if(!ProgressPixmap) {
#ifdef BUGHUNT
  	   printf("ProgressPixmap not exists: Creating now\n");
#endif
      ProgressPixmap = osd->CreatePixmap(MP3Skin.Progress_Layer, cRect(MP3Skin.Progress_Left, MP3Skin.Progress_Top, MP3Skin.Progress_Width, MP3Skin.Progress_Height) );
      ProgressPixmap->Clear();
    }

    // Create Pixmap on Layer 6 for Button
    if(!ButtonPixmap) {
#ifdef BUGHUNT
  	   printf("ButtonPixmap not exists: Creating now\n");
#endif
      ButtonPixmap = osd->CreatePixmap(MP3Skin.Button_Layer, cRect(MP3Skin.Button_Left, MP3Skin.Button_Top, MP3Skin.Button_Width, MP3Skin.Button_Height) );
      ButtonPixmap->Clear();
    }


#ifdef BUGHUNT
  	 printf("draw default Icons\n");
#endif


    // Shuffle symbol
    DrawIcon(1);
    // Loop symbol
    DrawIcon(3);
    // Lyrics
    DrawIcon(7);
    // Copy symbol
    DrawIcon(9);

    // Shutdown symbol
    if(MP3Setup.EnableShutDown)
      DrawIcon(12);
    else
      DrawIcon(11);
    // Timer
    if(MP3Setup.TimerEnabled)
      DrawIcon(14);
    else
      DrawIcon(13);

#ifdef BUGHUNT
  	 printf("Set Volume\n");
#endif
    SetVol();
    // SetKey buttons

#ifdef BUGHUNT
  	 printf("draw buttons\n");
#endif
    ShowHelpButtons(showbuttons);
    }	
  else {
    CoverOnlyPixmap = NULL; 	 

    // Create Pixmap on Layer 7 for CoverOnly
    if(!CoverOnlyPixmap) {
  	
      coversize = MP3Setup.Coversize;
      int x =0;
      int y =0;
      int w =120;
      int h =120;
          
      switch(coversize) {
        case 0:
          x =MP3Skin.CoverOnly_Width -140;
          y =MP3Skin.CoverOnly_Top + 20;
          w =120;
          h =120;
          break;
        case 1:
          x =(MP3Skin.CoverOnly_Width   /2 - 136);
          y =(MP3Skin.CoverOnly_Height  /2 - 136);
          w =274;
          h =274;
          break;
        case 2:
          x =(MP3Skin.CoverOnly_Width  /2 - 150);
          y =(MP3Skin.CoverOnly_Height  /2 - 128);
          w =300;
          h =256;
          break;
        case 3:
          x =MP3Skin.CoverOnly_Left;
          y =MP3Skin.CoverOnly_Top;
          w =MP3Skin.CoverOnly_Width;
          h =MP3Skin.CoverOnly_Height;
          break;
      }  	
  	
  	   CoverOnlyPixmap = osd->CreatePixmap(MP3Skin.CoverOnly_Layer, cRect(x, y, w, h));
  	   CoverOnlyPixmap->Clear();
    }
  }

#ifdef BUGHUNT
  printf("Default built\n");
#endif
}


void cMP3Control::DrawIcon(int Icon)
{

//  1= no_shuffle    2= lyrics
//  3= no_repeat     4= repeat
//  5= no_record     6= record
//  7= no_lyrics     8= lyrics
//  9= no_copy      10= copy
// 11= no_shutdown  12= shutdown
// 13= no_timer     14= timer

  int icon,w,h,posx,posy;
  std::string iconbmp_file;

  icon=Icon;
  iconbmp_file = MP3Skin.SkinPath + "icons/";
  w            = MP3Skin.symbol_width;
  h            = MP3Skin.symbol_height;
  posx         = 0;
  posy         = 0;


  switch(icon) {
    case 1:
            iconbmp_file = iconbmp_file + "no_shuffle.png";
            posx         = MP3Skin.shufflex;
            posy         = MP3Skin.shuffley;
          break;

    case 2:
            iconbmp_file = iconbmp_file + "shuffle.png";
            posx         = MP3Skin.shufflex;
            posy         = MP3Skin.shuffley;
          break;

    case 3:
            iconbmp_file = iconbmp_file + "no_loop.png";
            posx         = MP3Skin.loopx;
            posy         = MP3Skin.loopy;
          break;

    case 4:
            iconbmp_file = iconbmp_file + "loop.png";
            posx         = MP3Skin.loopx;
            posy         = MP3Skin.loopy;
          break;

    case 5:
            iconbmp_file = iconbmp_file + "no_record.png";
            posx         = MP3Skin.recordx;
            posy         = MP3Skin.recordy;
          break;

    case 6:
            iconbmp_file = iconbmp_file + "record.png";
            posx         = MP3Skin.recordx;
            posy         = MP3Skin.recordy;
          break;

    case 7:
            iconbmp_file = iconbmp_file + "no_lyrics.png";
            posx         = MP3Skin.lyricsx;
            posy         = MP3Skin.lyricsy;
          break;

    case 8:
            iconbmp_file = iconbmp_file + "lyrics.png";
            posx         = MP3Skin.lyricsx;
            posy         = MP3Skin.lyricsy;
          break;

    case 9:
            iconbmp_file = iconbmp_file + "no_copy.png";
            posx         = MP3Skin.copyx;
            posy         = MP3Skin.copyy;
          break;

    case 10:
            iconbmp_file = iconbmp_file + "copy.png";
            posx         = MP3Skin.copyx;
            posy         = MP3Skin.copyy;
          break;

    case 11:
            iconbmp_file = iconbmp_file + "no_shutdown.png";
            posx         = MP3Skin.shutdownx;
            posy         = MP3Skin.shutdowny;
          break;

    case 12:
            iconbmp_file = iconbmp_file + "shutdown.png";
            posx         = MP3Skin.shutdownx;
            posy         = MP3Skin.shutdowny;
          break;

    case 13:
            iconbmp_file = iconbmp_file + "no_timer.png";
            posx         = MP3Skin.timerx;
            posy         = MP3Skin.timery;
          break;

    case 14:
            iconbmp_file = iconbmp_file + "timer.png";
            posx         = MP3Skin.timerx;
            posy         = MP3Skin.timery;
          break;

    default: // Empty
          break;

  }

//  TracksRatingPixmap->Clear();
  cImageloadBitmap osdbitmap;
  if(osdbitmap.Load(iconbmp_file.c_str())) {
    osdbitmap.LoadImage(iconbmp_file.c_str(), w , h , false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    IconsPixmap->DrawImage(cPoint(posx,posy), img);
  }

  flush = true;
}



void cMP3Control::DrawProgressBar (int Index , int Total)
{

  int index = Index;
  int total = Total;

  std::string progressfgbmp_file;
  std::string progressbgbmp_file;

  ProgressFGPixmap->Clear();

  progressbgbmp_file = MP3Skin.SkinPath + "icons/";
  progressbgbmp_file = progressbgbmp_file + "progressbgbmp.png";

  progressfgbmp_file = MP3Skin.SkinPath + "icons/";
  progressfgbmp_file = progressfgbmp_file + "progressfgbmp.png";

  int p = index * MP3Skin.ProgressFG_Scale / total;

  cImageloadBitmap osdbitmap;
  if(osdbitmap.Load(progressbgbmp_file.c_str())) {
    osdbitmap.LoadImage(progressbgbmp_file.c_str(), MP3Skin.ProgressBG_Width, MP3Skin.ProgressBG_Height, false, 0, true);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    img.Clear();
    osdbitmap.Render(img);
    ProgressBGPixmap->DrawImage(cPoint(0, 0), img);
  }

  if (MP3Skin.ProgressFG_Slider > 0) {
    // Get foreground bitmap from left = 0 , good for slider ,...
    if(osdbitmap.Load(progressfgbmp_file.c_str())) {
      osdbitmap.LoadImage(progressfgbmp_file.c_str(), MP3Skin.ProgressFG_Width, MP3Skin.ProgressFG_Height, false, 0, true);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      img.Clear();
      osdbitmap.Render(img);
      ProgressFGPixmap->DrawImage(cPoint(p , 0), img);
	  }
	}
  else {
    // Get foreground bitmap from left = p , if you want normal progressbar,...
    if(osdbitmap.Load(progressfgbmp_file.c_str())) {
      osdbitmap.LoadImage(progressfgbmp_file.c_str(), MP3Skin.ProgressFG_Width, MP3Skin.ProgressFG_Height, false, p, true);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      img.Clear();
      osdbitmap.Render(img);
      ProgressFGPixmap->DrawImage(cPoint(p , 0), img);
	  }
	}

  flush = true;
}




void cMP3Control::ShowProgress(bool open)
{
/*	
#ifdef BUGHUNT
  printf("show ShowProgress\n");
#endif
*/	

  cPixmap::Lock();


  static int playstatus = -1;
  int index, total;
  MusicServicePlayerInfo_1_0 gtftStatus;
  int gtftChanges = 0;

  if(player->GetIndex(index,total) && total>=0) {
    if(!cOsd::IsOpen() && !visible && !open) {
      open=true;
      SetVars();
      vol = 0;
    }
    
    if(!visible && open) {
      if(MP3Setup.ShowMessage)
        osd=cOsdProvider::NewOsd(osdleft, osdtop, 20);
      else
        osd=cOsdProvider::NewOsd(osdleft, osdtop);

      if(!osd) return;
      if (!BuildAreas()) return;

      mgr->playnow = false;

// SetFont
      if(MP3Skin.ReloadFonts) SetFonts();

      BuildDefault();

      visible = true;
      if(MP3Setup.EnableVis && ShowSA) {
        LoadVisCover();
        SetNeedsFastResponse(true);
        }
      else
        SetNeedsFastResponse(false);

      force=true;

      fliptime=listtime=0; flipint=0; flip=-1; lastIndex=lastTotal=-1;
      delete lastMode; lastMode=0;
    }


    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    cMP3PlayInfo *mode=new cMP3PlayInfo;
    bool valid=mgr->Info(-1,mode);
    
    bool changed=(!lastMode || mode->Hash!=lastMode->Hash);
    char buf[256];

    // send progress to status monitor
    if(valid) {
      if(changed || mode->Loop!=lastMode->Loop || mode->Shuffle!=lastMode->Shuffle) {
        snprintf(buf,sizeof(buf),mode->Artist[0]?"[%c%c] (%d/%d) %s - %s":"[%c%c] (%d/%d) %s",
                 mode->Loop?'L':'.',mode->Shuffle?'S':'.',mode->Num,mode->MaxNum,mode->Title,mode->Artist);

        if(mode->Filename && !(strcmp(mode->Filename,lastfile) ==0) || changed) {
          cStatus::MsgReplaying(this,buf,mode->Filename[0]?mode->Filename:0,true);
        }
      }
    }


    // refresh the OSD progress display
    if(visible) {

      // GraphTFT stuff
      gtftStatus.filename     = mode->Filename;
      gtftStatus.artist       = mode->Artist;
      gtftStatus.album        = mode->Album;
      gtftStatus.genre        = mode->Genre;
      gtftStatus.year         = mode->Year;
      gtftStatus.comment      = mode->Comment;
      gtftStatus.frequence    = mode->SampleFreq;
      gtftStatus.bitrate      = mode->Bitrate;
      gtftStatus.smode        = mode->SMode;
      gtftStatus.index        = mode->Num;
      gtftStatus.count        = mode->MaxNum;
      gtftStatus.status       = "";
      gtftStatus.currentTrack = mode->Title;
      gtftStatus.loop         = mode->Loop;
      gtftStatus.shuffle      = mode->Shuffle;
      gtftStatus.shutdown     = MP3Setup.EnableShutDown;
      gtftStatus.recording    = MP3Setup.RecordStream;
      gtftStatus.rating       = mode->Rating;


      //flush the osd explicitly
      if(refresh) flush = true;
      else flush = false;



      // New song
      if ( mode->Filename && !(strcmp(mode->Filename,lastfile) ==0) || force || MP3Setup.ShowStatus != laststatus || changed || player->IsStream() != isstream) {
        strncpy(lastfile, mode->Filename,sizeof(lastfile));
        isstream = player->IsStream();

        playstatus = -1;
        gtftChanges++;


// Load Cover
        LoadImage(mode->Filename, mode->Artist, true);
        if(!MP3Setup.EnableVis) {
          LoadCover();
          flush = true;
        }		
		
// Rating

        if(MP3Setup.EnableRating) {
          ConvertRatingToChar_Rating(0, 0, mode->Rating, clrTextBG, clrRating, MP3Skin.Rating_ImageWidth, MP3Skin.Rating_ImageHeight); 
        }

// Infofield        
        buf[0]=0;


        AlbumPixmap->Clear();
        YearPixmap->Clear();
        GenrePixmap->Clear();



        if(!isstream) {
          if(mode->Artist[0]) {
            snprintf(buf,sizeof(buf),"%s",mode->Artist);
            Tupper(buf);
            ArtistPixmap->Clear();
            ArtistPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontInterpret, MP3Skin.Artist_Width, MP3Skin.Artist_Height, taLeft);
            }
          else {
            snprintf(buf,sizeof(buf),"%s: %s",tr("Artist"), tr("Unknown"));
            Tupper(buf);
            ArtistPixmap->Clear();
            ArtistPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontInterpret, MP3Skin.Artist_Width, MP3Skin.Artist_Height, taLeft);

          }

          if(mode->Album[0]) {
            snprintf(buf,sizeof(buf),"%s: %s", tr("Album"), mode->Album);
            Tupper(buf);
            AlbumPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Album_Width, MP3Skin.Genre_Height, taLeft);

            }
          else {
            snprintf(buf,sizeof(buf),"%s: %s", tr("Album"),tr("Unknown"));
            Tupper(buf);
            AlbumPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Album_Width, MP3Skin.Album_Height, taLeft);

            }
          }

        else {
          snprintf(buf,sizeof(buf),"%s",tr("NETWORKSTREAM"));
          ArtistPixmap->Clear();
          ArtistPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontInterpret, MP3Skin.Artist_Width, MP3Skin.Artist_Height, taLeft);


          if(mode->Album[0]) {
            snprintf(buf,sizeof(buf),"%s: %s", tr("Station"), mode->Album);
            Tupper(buf);
            AlbumPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Album_Width, MP3Skin.Album_Height, taLeft);
            }
          else {
            snprintf(buf,sizeof(buf),"%s: %s", tr("Station"),tr("Unknown"));
            Tupper(buf);
            AlbumPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Album_Width, MP3Skin.Album_Height, taLeft);

           }
        }

        if(mode->Genre[0]) {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Genre"), mode->Genre);
          Tupper(buf);
          GenrePixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Genre_Width, MP3Skin.Genre_Height, taLeft);

          }
        else {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Genre"), tr("Unknown"));
          Tupper(buf);
          GenrePixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Genre_Width, MP3Skin.Genre_Height, taLeft);

        }

        if(mode->Year > 2) {
          snprintf(buf,sizeof(buf),"%s: %d", tr("Year"), mode->Year);
          Tupper(buf);
          YearPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Year_Width, MP3Skin.Year_Height, taLeft);

          }
        else {
          snprintf(buf,sizeof(buf),"%s:  %s", tr("Year"), tr("Unknown"));
          Tupper(buf);

          YearPixmap->DrawText( cPoint(0, 0), buf, clrArtist, clrTextBG, pFontGenre, MP3Skin.Year_Width, MP3Skin.Year_Height, taLeft);

        }

// Lyrics exists or not symbol

        if(player->ExistsLyrics(mode->Filename)) {
          DrawIcon(8);
          }
        else {
          DrawIcon(7);
        }

        force = false;

      } // End of NewSong

      // track changed or show status
      if(changed || refresh || MP3Setup.ShowStatus != laststatus) {

        // Recording symbol
        if(MP3Setup.RecordStream) {
          DrawIcon(6);
          }
        else {
          DrawIcon(5);
        }

        // Infofield
        if(!selecting) {
          gtftChanges++;
          
          ModePixmap->Clear();
          NumberPixmap->Clear();


          if(mode->MaxBitrate>0) {
            snprintf(buf,sizeof(buf),"%.1f kHz, %d-%d kbps, %s", mode->SampleFreq/1000.0,mode->Bitrate/1000,mode->MaxBitrate/1000,mode->SMode);

            ModePixmap->DrawText( cPoint(0, 0), buf, clrInfo, clrTextBG, pFontInfo, MP3Skin.Mode_Width, MP3Skin.Mode_Height, MP3Skin.Mode_Align);

            }
          else {
            snprintf(buf,sizeof(buf),"%.1f kHz, %d kbps, %s", mode->SampleFreq/1000.0,mode->Bitrate/1000,mode->SMode);
            ModePixmap->DrawText( cPoint(0, 0), buf, clrInfo, clrTextBG, pFontInfo, MP3Skin.Mode_Width, MP3Skin.Mode_Height, MP3Skin.Mode_Align);

          }


          snprintf(buf,sizeof(buf),"%s %i %s %i","Track",mode->Num,tr("of"),mode->MaxNum);
          NumberPixmap->DrawText( cPoint(0, 0), buf, clrInfo, clrTextBG, pFontInfo, MP3Skin.Number_Width, MP3Skin.Number_Height, MP3Skin.Number_Align);

          flush=true;
        }

        // Title
        if(!jumpactive) {
          int showstatus = MP3Setup.ShowStatus;

          switch(showstatus) {
            case 0:
              snprintf(buf,sizeof(buf),"%s",mode->Title);
                  break;
            case 1:
              snprintf(buf,sizeof(buf),"%s",tr("Connecting to stream server ..."));
              gtftStatus.currentTrack = tr("Connecting to stream server ...");
                  break;
            case 2:
              snprintf(buf,sizeof(buf),"%s",tr("Remote CDDB lookup..."));
              gtftStatus.currentTrack = tr("Remote CDDB lookup...");
              break;
            default: break;
          }

          laststatus = showstatus;

          if(buf[0]) {
            Tupper(buf);
            DisplayInfo(buf);
          }


          flush = true;
        }





// Tracklist
      if (mode->Num < 1) {
        TitlePixmap->Clear();
        TitlePixmap->DrawText( cPoint(0, 0), tr("Please add some tracks..."), clrListTitle, clrTextBG, pFontTitle, MP3Skin.Title_Width, MP3Skin.Title_Height, MP3Skin.Title_Align);

        sendPlaylistItem(tr("Please add some tracks..."), 0, 1);
        }
      // last track is playing...
      else if (mode->Num == mode->MaxNum) {
        TracksPixmap->Clear();
        TracksPixmap->DrawText( cPoint(0, 0), tr("End of playlist============================"), clrList, clrTextBG, pFontList, MP3Skin.Tracks_TextWidth, MP3Skin.Tracks_TextHeight, taLeft);

        sendPlaylistItem("-EOL-", 0, 1);
      }

      // more than one track to play...
      if( (mode->MaxNum - mode->Num) >= 1 ) {
        TracksPixmap->Clear();
        TracksRatingPixmap->Clear();

        cMP3PlayInfo pi;

        // the graphTFT should display more than configured rows ... TODO , this isnt perfect ..if you load many tracks  and scan is active , OSD show up after a long time
        // int maxNum = max(mgr->InfoCount()-mode->Num, 0);
        // maxNum = min(maxNum, 1000);  // not realy needed ;)

        for(int i=0 ; i < rows; i++) {
          mgr->Info(mode->Num +i +1, &pi);
          if(pi.Title[0]) {
            if(artistfirst && pi.Artist[0])
              snprintf(buf,sizeof(buf),pi.Artist[0]?"%5d.   %s  -  %s":"%975d.   %s",pi.Num,pi.Artist, pi.Title);
            else 
              snprintf(buf,sizeof(buf),pi.Artist[0]?"%5d.   %s  -  %s":"%5d.   %s",pi.Num,pi.Title,pi.Artist);


            if(i < rows) {
              if(MP3Setup.EnableRating) {

                TracksPixmap->DrawText( cPoint(0, (MP3Skin.Tracks_TextHeight *i) + (i * MP3Skin.Tracks_TextPuffer)), buf, clrList, clrTextBG, pFontList, MP3Skin.Tracks_TextWidth, MP3Skin.Tracks_TextHeight, taLeft);
                ConvertRatingToChar_Tracks( 0, (MP3Skin.TracksRating_ImageHeight *i) + (i  *MP3Skin.TracksRating_TextPuffer), pi.Rating, clrTextBG, clrListRating, MP3Skin.TracksRating_ImageWidth, MP3Skin.TracksRating_ImageHeight);

                }
              else {
                TracksPixmap->DrawText( cPoint(0, (MP3Skin.Tracks_TextHeight *i) + (i * MP3Skin.Tracks_TextPuffer)), buf, clrList, clrTextBG, pFontList, MP3Skin.Tracks_Width, MP3Skin.Tracks_TextHeight, taLeft);

              }
            }

            sendPlaylistItem(buf, i, rows);
          }
          else {
            TracksPixmap->DrawText( cPoint(0, (MP3Skin.Tracks_TextHeight *i) + (i * MP3Skin.Tracks_TextPuffer)), "", clrList, clrTextBG, pFontList, MP3Skin.Tracks_TextWidth, MP3Skin.Tracks_TextHeight, taLeft);
            ConvertRatingToChar_Tracks( 0, (MP3Skin.TracksRating_ImageHeight *i) + (i * MP3Skin.TracksRating_TextPuffer), 300, clrTextBG, clrListRating, MP3Skin.TracksRating_ImageWidth, MP3Skin.TracksRating_ImageHeight);

            sendPlaylistItem("", i, rows);
          }
        }
      }


      // Shuffle
      if( !lastMode || mode->Shuffle!=lastMode->Shuffle) {
        if(mode->Shuffle)
          DrawIcon(2);
        else
          DrawIcon(1);
      }

      // Loop symbol
      if( !lastMode || mode->Loop!=lastMode->Loop) {
        if(mode->Loop)
          DrawIcon(4);
        else
          DrawIcon(3);
      }


    }

      // Playstatus 
      if(mgr->Scanning() && mgr->maxIndex >0) {
        gtftChanges++;
        PlaystatusPixmap->Clear();
        PlaystatusPixmap->DrawText( cPoint(0, 0), tr("SCANNING"), clrPlayStatus, clrTextBG, pFontStatus, MP3Skin.Playstatus_Width, MP3Skin.Playstatus_Height, taCenter);

        gtftStatus.status = tr("SCANNING");
        }
      else {		
        if(playstatus != player->StatusMode()) {
          PlaystatusPixmap->Clear();
          playstatus = player->StatusMode();
          gtftChanges++;

          switch(playstatus) {
            case 1:
                PlaystatusPixmap->DrawText( cPoint(0, 0), tr("STOPPED"), clrPlayStatus, clrTextBG, pFontStatus, MP3Skin.Playstatus_Width, MP3Skin.Playstatus_Height, taCenter);
                
                gtftStatus.status = tr("STOPPED");
                break;
            case 2:
                PlaystatusPixmap->DrawText( cPoint(0, 0), tr("PLAYING"), clrPlayStatus, clrTextBG, pFontStatus, MP3Skin.Playstatus_Width, MP3Skin.Playstatus_Height, taCenter);

                gtftStatus.status = tr("PLAYING");
                break;
            case 3:
                PlaystatusPixmap->DrawText( cPoint(0, 0), tr("PAUSED"), clrPlayStatus, clrTextBG, pFontStatus, MP3Skin.Playstatus_Width, MP3Skin.Playstatus_Height, taCenter);

                gtftStatus.status = tr("PAUSED");
                break;
          }
        }
      }

      // Progressbar
      if(!player->IsStream()) {

        if(index!=lastIndex || total!=lastTotal) {
          index/=framesPerSecond; total/=framesPerSecond;
          ProgressPixmap->Clear();
          if(total>0) {
            DrawProgressBar(index, total);
          }
          snprintf(buf, sizeof(buf),"%02d:%02d   %s   %02d:%02d", index/60, index%60, tr("of"), total/60, total%60);
          ProgressPixmap->DrawText( cPoint(MP3Skin.Timex, MP3Skin.Timey), buf, clrInfo, clrTextBG, pFontInfo, MP3Skin.Timew, MP3Skin.Progress_Height, taLeft);
          flush=true;

        }
      }
      else {
        ProgressFGPixmap->Clear();
        ProgressBGPixmap->Clear();
        ProgressPixmap->Clear();
        ProgressPixmap->DrawText( cPoint(MP3Skin.Timex, MP3Skin.Timey), "00:00", clrInfo, clrTextBG, pFontInfo, MP3Skin.Timew, MP3Skin.Progress_Height, taLeft);
        flush=true;
      }

      // Volumebar
      if ( vol != player->CurrentVolume() || muted != player->Muted()) {
        SetVol();
      }

      // Spectrum Analyzer visualization begins here...
      if ( MP3Setup.EnableVis && ShowSA) {
        ShowSpectrumAnalyzer();
      }

      if(flush) {
        Flush();
      }

      // update graphtft info ...
      if (gtft_exist && gtftChanges) {
        cPluginManager::CallFirstService(GRAPHTFT_STATUS_ID, &gtftStatus);
        d(printf("music: ctrl: status transmission succeeded (gtft)\n"));
      }

    }
    skiprew=0;
    skipfwd=0;
    lastIndex=index; lastTotal=total;
    delete lastMode; lastMode=mode;
    refresh=false;
  }


  cPixmap::Unlock();


/*
#ifdef BUGHUNT
  	 printf("ShowProgress ended\n");
#endif
*/
}


void cMP3Control::DisplayInfo(const char *s)
{
  if(osd && !MP3Setup.showcoveronly) {
    TitlePixmap->Clear();
    if(s) {
      TitlePixmap->DrawText( cPoint(0, 0), s, clrListTitle, clrTextBG, pFontTitle, MP3Skin.Title_Width, MP3Skin.Title_Height, MP3Skin.Title_Align);

      }
    else {
      TitlePixmap->DrawText( cPoint(0, 0), "", clrTransparent, clrTransparent, pFontTitle, MP3Skin.Title_Width, MP3Skin.Title_Height, MP3Skin.Title_Align);

    }

    //transmit current playing title from playlist to graphtft
    if(gtft_exist) {
      MusicServiceInfo_1_0 info;
      info.info = s;
      cPluginManager::CallFirstService(GRAPHTFT_INFO_ID, &info);
      d(printf("music: player: transmission of info succeeded (gtft)\n"));
    }

  }
}


void cMP3Control::JumpDisplay(void)
{
  char buf[64];
  const char *j=tr("Jump: "), u=jumpsecs?'s':'m';
  if(!jumpmm)
    sprintf(buf, "%s- %c", j,u);
  else
    sprintf(buf,"%s%d- %c",j,jumpmm,u);

  DisplayInfo(buf);
}


void cMP3Control::JumpProcess(eKeys Key)
{

  int n=Key-k0, d=jumpsecs?1:60;
  switch(Key) {
    case k0 ... k9:
      if(jumpmm*10+n <= lastTotal/d) jumpmm=jumpmm*10+n;
      JumpDisplay();
      if(visible && !MP3Setup.showcoveronly) ShowProgress();
      break;
    case kBlue:
      jumpsecs=!jumpsecs;
      JumpDisplay();
      if(visible && !MP3Setup.showcoveronly) ShowProgress();
      break;
    case kRed:
      jumpactive= false;
      break;
    case kPlay:
    case kUp:
      jumpmm-=lastIndex/d;
      // fall through
    case kFastRew:
    case kFastFwd:
    case kGreen:
    case kYellow:
      player->SkipSeconds(jumpmm*d * ((Key==kGreen || Key==kFastRew) ? -1:1));
      // fall through
    default:
      jumpactive=false;
      break;      
    }
    
  if(!jumpactive) {
    ShowHelpButtons(0);
    if(visible && !MP3Setup.showcoveronly) ShowProgress();
    }
}        

void cMP3Control::Jump(void)
{
  jumpmm=0; jumpsecs=false;
  JumpDisplay();
  jumpactive=true; fliptime=0; flip=-1;
}  


void cMP3Control::CopyTrack(void)
{
FILE *copyscript;
char *buffer;

  DrawIcon(10);

  Flush();
  asprintf(&buffer, "cp -f \"%s\"  \"%s\"", Songname, MP3Setup.CopyDir);
  copyscript = popen(buffer, "r");
#ifdef DEBUG
  dsyslog("music: ctrl: executing: %s", buffer);
#endif
  free(buffer);
  pclose(copyscript);

  DrawIcon(9);

  refresh = true;
}


void cMP3Control::ShutDown(void) {
  std::string shutdowncmd;
  
  shutdowncmd = config;
  shutdowncmd = shutdowncmd + langdir;
  shutdowncmd = shutdowncmd + "/scripts/music_shutdown.sh &";
#ifdef DEBUG  
  isyslog("music: ctrl: player stopped. Initiate shutdown...");
#endif
  system(shutdowncmd.c_str());
#ifdef DEBUG
  isyslog("music: ctrl: executing '%s'", shutdowncmd.c_str());
#endif
}


void cMP3Control::ShowMessage(int Message, bool open) {
  if(!cOsd::IsOpen()) {
    if(!visible && !open) {
      open=true;
    }
  }

  if(!visible && open) {
    if(MP3Setup.ShowMessage)
      osd=cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop, 20);
    else
      osd=cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
	
    if(!osd) return;

    int y2 = (osdheight / 2) +(2*fh);
    int message = Message;
    tArea Area[] = {{ 0, y2, osdwidth -1, y2 + 4*fh, 2}, };
    eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
    if (result == oeOk) {
      osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
      }
    else  {
      const char *errormsg = NULL;
      switch (result) {
            case oeTooManyAreas:
                      errormsg = "Too many OSD areas"; break;
            case oeTooManyColors:
                      errormsg = "Too many colors"; break;
            case oeBppNotSupported:
                      errormsg = "Depth not supported"; break;	
            case oeAreasOverlap:
                      errormsg = "Areas are overlapped"; break;
            case oeWrongAlignment:
                      errormsg = "Areas not correctly aligned"; break;
            case oeOutOfMemory:
                      errormsg = "OSD memory overflow"; break;
            case oeUnknown:
                      errormsg = "Unknown OSD error"; break;
            default:
                      break;
      }	
      esyslog("music: ctrl: ERROR! osd open failed! can't handle areas (%d)-%s\n", result, errormsg);
      if(osd){ delete osd; osd=0;}
      return;
    }
    osd->DrawRectangle(0 , y2, osdwidth -1, y2 + 4*fh, clrBlack); //complete  Background
    osd->DrawRectangle(fh/2 , y2 +fh/2, osdwidth -1 -fh/2, y2 + 4*fh -fh/2, clrYellow); //Textfield
    visible=true;
    switch (message) {
          case 1:
              osd->DrawText( 0 +fh, y2 + fh , tr("Be patient..."), clrBlack, clrYellow, pFontTitle, osdwidth - 2*fh, fh, taCenter);
              osd->DrawText( 0 +fh, y2 + 2*fh , tr("TRACKS become straight scanned"), clrBlack, clrYellow, pFontTitle, osdwidth - 2*fh, fh, taCenter);
//////              TextPixmap->DrawText( cPoint(0 +fh, y2 +fh), tr("Be patient..."), clrListTitle, clrTransparent, pFontTitle);
//////              TextPixmap->DrawText( cPoint(0 +fh, y2 +2*fh), tr("TRACKS become straight scanned"), clrListFG, clrTransparent, pFontTitle);
              break;

          case 2:
              osd->DrawText( 0 +fh, y2 + 2*fh , tr("ERROR: No tracks loaded !"), clrBlack, clrYellow, pFontTitle, osdwidth - 2*fh, fh, taCenter);
              break; 

          case 3:
              osd->DrawText( 0 +fh, y2 + 2*fh , tr("ERROR: Could not load songinfo !"), clrBlack, clrYellow, pFontTitle, osdwidth - 2*fh, fh, taCenter);
              break; 

          default: break;
    }
    Flush();
  }
}

eOSState cMP3Control::MessageProcess(eKeys Key)
{

  if(!mgr->Scanning()) {
	  Hide();
      showmessage = false;
	  
	  if(MP3Setup.showcoveronly) {
        ShowCoverOnly();
        }
      else {
        ShowProgress();
	  }	
  }

  switch(Key) {
    case kUp:
    case kDown:
    case kLeft:
    case kRight:
    case kBlue:
    case kRed:
    case kGreen:
    case kYellow:
    case kPlay:
    case kBack:
    case kOk:
	  Hide();
      showmessage = false;
	  
	  if(MP3Setup.showcoveronly) {
        ShowCoverOnly();
        }
      else {
        ShowProgress();
	  }	

    default:
      break;      
    }

  return osContinue;
}        



eOSState cMP3Control::ProcessKey(eKeys Key)
{
  if(MP3Setup.EnableShutDown == 1 && player->StatusMode() == 1 && mgr->maxIndex == mgr->currIndex ) {
    ShutDown();
    return osEnd;
  }    

  if( (player->StatusMode() == 1) && (mgr->TracksAdded()) && (mgr->NewCurrent()) && (!mgr->ShuffleMode()) ) player->Play();

  if(!player->Active()) return osEnd;

  if(showmessage) { MessageProcess(Key); return osContinue; }

  if(jumpactive && Key!=kNone) { JumpProcess(Key); return osContinue; }


  LOCK_PIXMAPS;
//  LOCK_THREAD;


  if(MP3Setup.showcoveronly) {
    ShowCoverOnly();
    }
  else {
    ShowProgress();
  }

  if(cmdMenu) {
    eOSState eOSRet = cmdMenu->ProcessKey(Key);
    switch(eOSRet) {
      case osBack:
            delete cmdMenu;
            cmdMenu = NULL;

            // Can't call function from commands, so this is the way :(
            if (MP3Setup.DeleteTrack) {
	          MP3Setup.DeleteTrack = 0;
              player->DeleteTrack(false, mgr->currIndex);
	          }
	        else {
	          if(MP3Setup.DeleteTracks) {
	            MP3Setup.DeleteTracks = false;
	            player->DeleteTrack(true, mgr->currIndex);
	            cRemote::CallPlugin("music");
	            return(osPlugin);
	            }	  
            }
            if(MP3Setup.showcoveronly) {
	          ShowCoverOnly();
	          } 	
            else {
              ShowProgress();
            }
	        return osContinue;

      default: return eOSRet;
    }
  }  //cmdMenu ends here    		

  else if(id3infoMenu) {
    eOSState eOSRet = id3infoMenu->ProcessKey(Key);
    switch(eOSRet) {
      case osBack:
            delete id3infoMenu;
	        id3infoMenu = NULL;
            if(MP3Setup.showcoveronly) {
              ShowCoverOnly();
	          } 	
            else {
              ShowProgress();
            }
	        return osContinue;

      default: return eOSRet;
    }
  }  // id3infoMenu ends here

  else if(rateMenu) {
    eOSState eOSRet = rateMenu->ProcessKey(Key);
    switch(eOSRet) {
      case osBack:
            delete rateMenu;
	        rateMenu = NULL;
            if(MP3Setup.showcoveronly) {
              ShowCoverOnly();
	          } 	
            else {
              ShowProgress();
            }
	        return osContinue;

      default: return eOSRet;
    }
  }  // rateMenu ends here

  else if(trackList) {
    eOSState eOSRet = trackList->ProcessKey(Key);
    switch(eOSRet) {
      case osBack:
            delete trackList;
	        trackList = NULL;

            if(MP3Setup.showcoveronly) {
              ShowCoverOnly();
	          } 	
            else {
              ShowProgress();
            }
	        return osContinue;

      default: return eOSRet;
    }
  }  // tracklist ends here

  // player frontend starts here
  else {
    switch(Key) {
      case kMenu:
            break;
/*
      case kInfo:
            Hide();

            if(mgr->maxIndex <0) {
              showmessage=true;
              ShowMessage( 2, false);
              break;
            }


            if(mgr->Current()) {
              id3infoMenu = new cMP3SearchID3Info(mgr->curr);
              printf("SHOW ID3INFO\n");
              }
            else {
              showmessage=true;
              ShowMessage( 3, false);
            }

            break;
*/
      case kUp:
      case kDown:
            if(mgr->maxIndex <0) break;
            Hide();

            if(mgr->Scanning()) {
              showmessage=true;
              ShowMessage( 1, false);
              break;
            }

            trackList = new cTrackList(&(mgr->list), player);
            break;

      case kLeft:	
      case kLeft|k_Repeat:	
      case kFastRew:
      case kFastRew|k_Repeat:
            if(mgr->maxIndex <0) break;
            ShowSA=false;      
            if(!player->IsStream() && player->StatusMode() == 2) player->SkipSeconds(-MP3Setup.Jumptime);
            skiprew=1;
            break;

      case kRight:	
      case kRight|k_Repeat:	
      case kFastFwd:
      case kFastFwd|k_Repeat:
            if(mgr->maxIndex <0) break;
            ShowSA=false;      
            skipfwd=1;
            if(!player->IsStream() && player->StatusMode() == 2) player->SkipSeconds(MP3Setup.Jumptime);
            break;

      case kRecord:
			if(mgr->maxIndex <0) break;
			if(MP3Setup.RecordStream) {
              cmdMenu->StopRecord();
              refresh=true;
              }
            else {
              if(player->IsStream()) {
			    cmdMenu->StartRecord();
                refresh=true;
			  }  
            }
            if(visible && !MP3Setup.showcoveronly) ShowProgress();
			break;
			  
      case kPlay:
            if(mgr->maxIndex <0) break;
            player->Play();
	        refresh = true;
            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;

      case kRed:
            // MENUE  OK
            if(showbuttons==0) {
              if(MP3Setup.RatingFirst) {
                if(mgr->maxIndex <0) break;
                if(!player->IsStream()) {
                  Hide();
                  rateMenu = new cMP3Rating(mgr->curr);
                }
              }
              else {
                MP3Setup.DeleteTrack=MP3Setup.DeleteTracks = 0;
                Hide();
                cmdMenu = new cMP3Commands();
              }
            }
            else if(showbuttons==1) {
              ShowHelpButtons(0);
              if(MP3Setup.RatingFirst) {
                MP3Setup.DeleteTrack=MP3Setup.DeleteTracks = 0;
                Hide();
                cmdMenu = new cMP3Commands();
                }
              else {
                if(mgr->maxIndex <0) break;
                if(!player->IsStream()) {
                  Hide();
                  rateMenu = new cMP3Rating(mgr->curr);
                }
              }
            }
            else if(showbuttons==2) {
              if(mgr->maxIndex==0) {
                ShowHelpButtons(0);
                player->DeleteTrack(true, mgr->currIndex);
                cRemote::CallPlugin("music");
                return(osPlugin);
                }
              else {
                if(mgr->maxIndex >0) {
                  ShowHelpButtons(0);
                  player->DeleteTrack(false, mgr->currIndex);
                }
              }
            }
            else if(showbuttons==4) {
              ShowHelpButtons(0);
              if(!MP3Setup.VizRunning) {
                std::string start_viz_cmd;
                start_viz_cmd = config;
                start_viz_cmd = start_viz_cmd + langdir;
                start_viz_cmd = start_viz_cmd + "/scripts/music_vis_extern_start.sh " + "\"" +  config + "\"" + " &";
                system(start_viz_cmd.c_str());
                MP3Setup.VizRunning = 1;

#ifdef DEBUG
                isyslog("music: ctrl: executing '%s'", start_viz_cmd.c_str());
#endif
                break;
                }
              else {
                std::string kill_viz_cmd;
                kill_viz_cmd = config;
                kill_viz_cmd = kill_viz_cmd + langdir;
                kill_viz_cmd = kill_viz_cmd + "/scripts/music_vis_extern_stop.sh " + "\"" + config + "\"" + " &";
                system(kill_viz_cmd.c_str());
                MP3Setup.VizRunning = 0;

#ifdef DEBUG
                isyslog("music: ctrl: executing '%s'", kill_viz_cmd.c_str());
#endif
                break;
              }
            }

            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;

      case kPrev:
      case kPrev|k_Repeat:
            if(mgr->maxIndex <0) break;
            mgr->Prev();
            if(!player->StatusMode() == 2)
              player->Play();
            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break; 


      case kGreen: 
      case kGreen|k_Repeat:
            if(showbuttons==0) {
              if(mgr->maxIndex <0) break;
              mgr->Prev();
              if(!player->StatusMode() == 2)  player->Play();
              }
            else if(showbuttons==1) {
              if(!MP3Setup.showcoveronly) {
                ShowHelpButtons(0);
                MP3Setup.showcoveronly = 1;
                Hide();
                ShowCoverOnly(false);
                }
              else {
                MP3Setup.showcoveronly = 0;
                Hide();
               }
            }
            else if(showbuttons==2) {
              ShowHelpButtons(0);
              if(mgr->maxIndex <0) break;
              player->DeleteTrack(true, mgr->currIndex);
              cRemote::CallPlugin("music");
              return(osPlugin);
            }
            else if(showbuttons==4) {
              ShowHelpButtons(0);
              if(mgr->maxIndex <0) break;
//              Hide();
//              new cMP3SearchID3Info(mgr->curr);
              printf("INFO GREEN\n");
            }

            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;



      case kNext:
      case kNext|k_Repeat:
            if(mgr->maxIndex <=0) break;
              mgr->Next();
            if(!player->StatusMode() == 2) player->Play();
            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;



      case kYellow:
      case kYellow|k_Repeat:
            if(showbuttons==0) {
              if(mgr->maxIndex <=0) break;
              mgr->Next();
              if(!player->StatusMode() == 2) player->Play();
              }
            else if(showbuttons==1) {
              if(!player->IsStream()){
                ShowHelpButtons(3);
                if(mgr->maxIndex <0) {
                  ShowHelpButtons(0);
                  break;
                }
                Jump();
              }
              else
                ShowHelpButtons(0);
            }
            else if(showbuttons==2) {
              ShowHelpButtons(0);
              if(mgr->maxIndex <0) break;
              CopyTrack();
              }
            else if(showbuttons==4) {
              ShowHelpButtons(0);
              if(mgr->maxIndex <0) break;
//              Hide();
//              rateMenu = new cMP3Shoutcast();
              printf("SHOUTCAST YELLOW\n");
            }


            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;

      case kBlue: //OK
            if(showbuttons==0) {
              if(!MP3Setup.showcoveronly)
                ShowHelpButtons(1);
              else {
                MP3Setup.showcoveronly = 0;
                Hide();
                ShowProgress(false);
                break;
                }
              }
            else if(showbuttons==1) {
              if(MP3Setup.AdminMode)
               ShowHelpButtons(2);
              else
               ShowHelpButtons(0);  
              }
            else if(showbuttons==2) {
              ShowHelpButtons(4);
            } 
            else if(showbuttons==4) {
              ShowHelpButtons(0);
              printf("PARENT BLUE\n");
            }


            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;

      case kPause:
            if(mgr->maxIndex <0) break;
            if(player->StatusMode() == 1)
              player->Play();
			else  
              player->Pause();
	        refresh = true;
            if(visible && !MP3Setup.showcoveronly) ShowProgress();
            break;

      case kOk:
            Hide();
	        cRemote::CallPlugin("music");
            return (osPlugin);
            break;

      case kStop:
            if(mgr->maxIndex <0) break;
            Hide();
            Stop();
            return osEnd;

      case k0 ... k9:
            number=number*10+Key-k0;
            if(lastMode && number>0 && number<=lastMode->MaxNum && !MP3Setup.showcoveronly) {
              selecting=true; lastkeytime=time_ms();
              char buf[32];
              snprintf(buf,sizeof(buf),"%s %d- %s %d","No.",number,tr("of"),lastMode->MaxNum);
//////              osd->DrawText( MP3Skin.numberx, MP3Skin.numbery, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.numberw, sfh, taLeft);
              NumberPixmap->Clear();
              NumberPixmap->DrawText( cPoint(0, 0), buf, clrInfo, clrTextBG, pFontInfo, MP3Skin.Number_Width, MP3Skin.Number_Height, taLeft);
              flush=true;
              if(visible && !MP3Setup.showcoveronly) ShowProgress();
              break;
            }
	    number=0; lastkeytime=0;

      case kNone:
            if(MP3Setup.TimerEnabled && time_ms() - MP3Setup.TimerActiveTime > (MP3Setup.TimerShutdownTime *60000)) {
              MP3Setup.TimerEnabled = false;
              MP3Setup.TimerShutdownTime = 0;
              MP3Setup.TimerActiveTime = 0;
              ShutDown();
              return osEnd;
            }  
		
            if(selecting && time_ms()-lastkeytime>SELECT_TIMEOUT) {
	          if(number>0) {mgr->Goto(number); player->Play(); if(visible && !MP3Setup.showcoveronly) ShowProgress();}
	          if(lastMode) lastMode->Hash=-1;
	          number=0; selecting=false;
	          }
	          ShowSA=true;
            break;

      case kBack:
           if(!MP3Setup.VizRunning) {
             std::string kill_viz_cmd;
             kill_viz_cmd = config;
             kill_viz_cmd = kill_viz_cmd + langdir;
             kill_viz_cmd = kill_viz_cmd + "/scripts/music_vis_extern_stop.sh " + "\"" + config + "\"" + " &";
             system(kill_viz_cmd.c_str());
             MP3Setup.VizRunning = 0;

#ifdef DEBUG
             isyslog("music: ctrl: executing '%s'", kill_viz_cmd.c_str());
#endif
             break;
           }

            Hide();
#if APIVERSNUM >= 10332
            cRemote::CallPlugin("music");
            if(MP3Setup.ExitClose)
              return (osBack);
            else
              return (osPlugin);
#else
            return osEnd;
#endif

      default:
            return osUnknown;
    }

  return osContinue;
  }  // player frontend ends here
}


// GraphTFT stuff
void cMP3Control::sendHelpButtons(const char* red, const char* green, const char* yellow, const char* blue)
{
   // transmit help buttons to graphtft
   if (gtft_exist) 
   {
      MusicServiceHelpButtons_1_0 buttons;

      buttons.red = red;
      buttons.green = green;
      buttons.yellow = yellow;
      buttons.blue = blue;

      cPluginManager::CallFirstService(GRAPHTFT_HELPBUTTONS_ID, &buttons);
      d(printf("music: player: transmission of help buttons succeeded (gtft)\n"));
   }
}

void cMP3Control::sendPlaylistItem(const char* item, int index, int count)
{
   // transmit playlist item to graphtft
   if (gtft_exist) 
   {
      MusicServicePlaylist_1_0 service;

      service.item = item;
      service.index = index;
      service.count = count;

      cPluginManager::CallFirstService(GRAPHTFT_PLAYLIST_ID, &service);
      d(printf("music: ctrl: transmission succeeded (gtft)\n"));
   }
}

char *cMP3Control::CheckImage (char *fileName)
{
  static char tmpFile[256];
  char *result = NULL;
  FILE *fp;


    if ((fp = fopen (fileName, "rb")))
    {
      fclose (fp);
#ifdef DEBUG_COVER
	  printf("music[%d]: ctrl: image found '%s'\n", getpid (), fileName);
#endif
      strncpy(tmpFile,fileName,sizeof(tmpFile));
      result = tmpFile;
    }
  return result;
}

void cMP3Control::DefaultImage(void)
{
  std::string file("");
	
  if (player->IsStream()) {
#ifdef DEBUG_COVER
	printf("music: ctrl: using default cover for streams\n");
#endif
    file = MP3Skin.SkinPath + "cover/" + MP3Skin.StreamCover;
    strncpy(coverpicture, file.c_str(),sizeof(coverpicture));
    }	
  else {
#ifdef DEBUG_COVER
	printf("music: ctrl: using default cover for local track\n");
#endif
    file = MP3Skin.SkinPath + "cover/" + MP3Skin.LocalCover;
    strncpy(coverpicture, file.c_str(),sizeof(coverpicture));
  }
}


void cMP3Control::LoadImage(const char *fullname, const char *artist, bool coveronly)
{
  size_t i;
  char imageFile[256];
  char oldimagefile[256];
  char *p, *q = NULL;
  char Artist[256];
  const char *imageSuffixes[] = { "jpg", "png" };
 
  strncpy (oldimagefile, coverpicture,sizeof(oldimagefile));
  strncpy (imageFile, fullname,sizeof(imageFile));
  strncpy (coverpicture, "",sizeof(coverpicture));

#ifdef DEBUG_COVER
  printf("music[%d]: ctrl: Begin to search Cover for '%s' . artist='%s'\n", getpid (), fullname, artist);
#endif

  //
  // track specific image, e.g. <song>.jpg
  //
  if (!strlen (coverpicture)) {
    p = strrchr (imageFile, '.');
    if (p)
    {
      for (i = 0; i < sizeof (imageSuffixes) / sizeof (imageSuffixes[0]); i++)
      {
        strncpy (p + 1, imageSuffixes[i], sizeof(p +1));
#ifdef DEBUG_COVER
        printf("music[%d]: ctrl: try track specified image: '%s'\n", getpid (), imageFile);
#endif
        q = CheckImage (imageFile);
        if (q)
        {
          strncpy (coverpicture, q, sizeof(coverpicture));
          break;
        }
      }
    }
  }


  //
  // album specific image, e.g. cover-/front-/folder.jpg in song directory
  //
  if (!strlen (coverpicture)) {
    std::string fullfilename;
    std::string Base;

    fullfilename = imageFile;
    int len = fullfilename.length();

    std::string::size_type pos  = fullfilename.rfind('/',len);
    if(pos!=std::string::npos) {
      //BaseDir
      Base     = fullfilename.substr(0,pos+1);
    }  


    std::string jpegs    ="";
    std::string b_jpegs  ="";
    std::string pngs     ="";
    std::string b_pngs   ="";

    jpegs   = Base + "*.jpg";
    b_jpegs = Base + "*.JPG";
    pngs    = Base + "*.png";
    b_pngs  = Base + "*.PNG";

#ifdef DEBUG_COVER
    printf("music[%d]: ctrl: try album specified image (cover-,front-,folder-) in '%s'\n", getpid (), Base.c_str());
#endif
    unsigned int i;
    glob_t glob_buffer;
    glob_buffer.gl_offs = 4;
    glob(  jpegs.c_str(), 0              , NULL, &glob_buffer);
    glob(b_jpegs.c_str(), GLOB_APPEND    , NULL, &glob_buffer);
    glob(  pngs.c_str() , GLOB_APPEND    , NULL, &glob_buffer);
    glob(b_pngs.c_str() , GLOB_APPEND    , NULL, &glob_buffer);

    for (i=0; i < glob_buffer.gl_pathc; ++i) {
#ifdef DEBUG_COVER
      printf("music[%d]: ctrl: look for '%s'\n", getpid(), glob_buffer.gl_pathv[i]);
#endif
      if(strcasestr(glob_buffer.gl_pathv[i], "cover")) {
        q = CheckImage (glob_buffer.gl_pathv[i]);
        }
      else if(strcasestr(glob_buffer.gl_pathv[i], "front")) {
        q = CheckImage (glob_buffer.gl_pathv[i]);
        }
      else if(strcasestr(glob_buffer.gl_pathv[i], "folder")) {
        q = CheckImage (glob_buffer.gl_pathv[i]);
      }

      if (q) {
        strncpy(coverpicture, q, sizeof(coverpicture));
        break;
      }
    }
    globfree(&glob_buffer);
  }

  //
  // Artist specific image, e.g. Acdc.jpg in Artistcover directory
  //
  if (!strlen (coverpicture)) {
    if(artist !=NULL) {
      strncpy(Artist, MP3Setup.CoverDir,sizeof(Artist));
      strcat(Artist, "/");
      strcat(Artist,artist);
      strcat(Artist, ".jpg");
#ifdef DEBUG_COVER
      printf("music[%d]: ctrl: try artist specified image '%s'\n", getpid (), Artist);
#endif
      q = CheckImage (Artist);
      if (q) {
        strncpy (coverpicture, q, sizeof(coverpicture));
      }
    }
  }



  //
  // Default image
  //
  if (!strlen (coverpicture)) {
    DefaultImage();
  }

  // path to cover changed ?
  if (!(strcmp(coverpicture, oldimagefile))==0) {
    cPlugin *Plugin=cPluginManager::GetPlugin("graphtft");
    if (Plugin) {

#ifdef DEBUG_COVER
      printf("'coverpicture' changed , send filename to graphtft \n");
#endif
      MusicService_Covername_1_0 servicecovername;
      servicecovername.name = coverpicture;
      cPluginManager::CallFirstService(GRAPHTFT_COVERNAME_ID, &servicecovername);
    }
  }

#ifdef DEBUG_COVER
      printf("coverpicture = '%s'\n", coverpicture);
#endif


}
