#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <typeinfo>
#include <string>
#include <fstream>

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


// --- cMP3Control -------------------------------------------------------------
cMP3Control::cMP3Control(void)
:cControl(player=new cMP3Player)
,cmdMenu(NULL),rateMenu(NULL),trackList(NULL)
{
  jumpactive=false;
  skipfwd=skiprew=visible=selecting=refresh=muted=showmessage=false;
  ShowSA=MP3Setup.EnableVis;
  showbuttons=0;  
  lastkeytime=number=vol=laststatus=0;
  visualization=channelsSA=bandsSA=cw=rows=0;
  lastMode=0;
  framesPerSecond=SecondsToFrames(1);
  if(!osd) osd=0;


  MP3Setup.EnableShutDown = 0;

#if APIVERSNUM >= 10338
  cStatus::MsgReplaying(this,"MP3",0,true);
#else
  cStatus::MsgReplaying(this,"MP3");
#endif
  MP3Setup.showcoveronly = false;
  coversize = 0;
  
// GraphTFT stuff
  cPlugin* Plugin = cPluginManager::GetPlugin("graphtft");
  if (Plugin) {
    gtft_exist = true;
    d(printf("music-sd: ctrl: graphtft exist\n"));
	}
  else {
    gtft_exist = false;   
    d(printf("music-sd: ctrl: graphtft dont exist\n"));
  }

  SetFonts();

   
}

cMP3Control::~cMP3Control()
{
  if(cmdMenu) {delete cmdMenu;}
  cmdMenu = NULL;

  if(rateMenu) {delete rateMenu;}
  rateMenu = NULL;

  if(trackList) {delete trackList;}
  trackList = NULL;

  if(pFontTop)       delete pFontTop;
  if(pFontInterpret) delete pFontInterpret;
  if(pFontGenre)     delete pFontGenre;
  if(pFontStatus)    delete pFontStatus;
  if(pFontInfo)      delete pFontInfo;
  if(pFontTitle)     delete pFontTitle;
  if(pFontList)      delete pFontList;
  if(pFontButton)    delete pFontButton;
  if(pFontSymbol)    delete pFontSymbol;

  delete lastMode;
  Hide();
  InfoCache.Save(true);
  Stop();
}

void cMP3Control::Stop(void)
{
  if(MP3Setup.RecordStream) {cmdMenu->StopRecord();}
#if APIVERSNUM >= 10338
  cStatus::MsgReplaying(this,0,0,false);
#else
  cStatus::MsgReplaying(this,0);
#endif
  delete player; player=0;
  mgr->Halt();
  mgr->Flush(); //XXX remove later
}

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


void cMP3Control::Hide(void)
{
  if (cmdMenu) {
    delete cmdMenu;
    cmdMenu=NULL;
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
#if VDRVERSNUM >= 10500
    visible = false;
    SetNeedsFastResponse(visible);
#else    
    needsFastResponse=false;
    visible=false;
#endif
    }
}


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
    GetBarHeights.name			 = "Music-SD";
    GetBarHeights.falloff		 = MP3Setup.FalloffSA;
    GetBarHeights.barPeaksBothChannels	 = barPeaksBothChannels;
    GetBarHeights.barPeaksLeftChannel	 = barPeaksLeftChannel;
    GetBarHeights.barPeaksRightChannel	 = barPeaksRightChannel;

    if ( cPluginManager::CallFirstService(SPAN_GET_BAR_HEIGHTS_ID, &GetBarHeights )) {
        int i;
        int barPeakHeight = 3;
        int barPeak       = 0;
        int covertop      = MP3Skin.covery;
        int coverbottom   = MP3Skin.covery + MP3Skin.coverh;

        if(player !=NULL) {
            switch(visualization) {
              case 1: // MONO 
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar      = (barHeights[i]*(coverh -10))/100;
                    barPeak  = (barPeaksBothChannels[i]*(coverh -10))/100;
                    if(barPeak > 0) {
                      osd->DrawRectangle(channel1left +(i*barwidth), covertop, channel1left + (i*barwidth) +barwidth -1, covertop + (coverh -6) - barPeak -barPeakHeight, clrCoverBar);
                      osd->DrawRectangle(channel1left +(i*barwidth), covertop +(coverh -6) -barPeak +1, channel1left + (i*barwidth) +barwidth -1, covertop + (coverh -6) - bar, clrCoverBar);
                      }
                    else {
                      osd->DrawRectangle(channel1left +(i*barwidth), covertop, channel1left + (i*barwidth) +barwidth -1, covertop +(coverh -6) - bar, clrCoverBar);
                    }  
                  }
                  break;

              case 2:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar = barHeightsLeftChannel[i]*(coverh -10)/100;
                    barPeak = barPeaksLeftChannel[i]*(coverh -10)/100;
                    if(barPeak > 0) {
                      osd->DrawRectangle(channel1left +(i*barwidth)    , covertop, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - barPeak -barPeakHeight, clrCoverBar);
                      osd->DrawRectangle(channel1left +(i*barwidth)    , covertop +(coverh -6) -barPeak +1, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - bar, clrCoverBar);
                      }
                    else {
                      osd->DrawRectangle(channel1left +(i*barwidth)    , covertop, channel1left +(i*barwidth) +barwidth -1    , covertop + (coverh -6) - bar, clrCoverBar);
                    }		       	 
                    bar = barHeightsRightChannel[i]*(coverh -10)/100;
                    barPeak = barPeaksRightChannel[i]*(coverh -10)/100;
                    if( barPeak > 0) {
                      osd->DrawRectangle(channel2left +(i*barwidth), covertop, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - barPeak -barPeakHeight, clrCoverBar);
                      osd->DrawRectangle(channel2left +(i*barwidth), covertop +(coverh -6) -barPeak +1, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - bar, clrCoverBar);
                      }
                    else {
                      osd->DrawRectangle(channel2left +(i*barwidth), covertop, channel2left +(i*barwidth) +barwidth -1, covertop + (coverh -6) - bar, clrCoverBar);
                    }
                  }
                  break;

              case 3:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar      = (volumeLeftChannel*(coverh -10))/100;
                    osd->DrawRectangle(channel1left , covertop, channel1left + barwidth , covertop + (coverh -6) - bar,  clrCoverBar);
                    bar      = (volumeRightChannel*(coverh -10))/100;
                    osd->DrawRectangle(channel2left , covertop, channel2left + barwidth , covertop + (coverh -6) - bar, clrCoverBar);
                  }
                  break;





/*


              case 4:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    bar = barHeightsLeftChannel[i]*(coverh -10)/100;
                    osd->DrawRectangle(channel1left ,coverbottom -6 -(i*barwidth) -barwidth, channel1left +(coverw/2) -(bar/2), coverbottom -(i*barwidth), clrCoverBar);
                    bar = barHeightsRightChannel[i]*(coverh -10)/100;
                    osd->DrawRectangle(channel2left ,coverbottom -6 -(i*barwidth) -barwidth, coverright  , coverbottom -(i*barwidth), clrCoverBar);
                  }
                  break;

              case 5:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    barWidth = 1;
                    bar = barHeightsLeftChannel[i]*118/100;
                    osd->DrawRectangle(coverleft            ,coverbottom -6 -(i*barWidth) -barWidth, coverleft  +64 -(bar/2), coverbottom -(i*barWidth), clrCoverBar);
                    bar = barHeightsRightChannel[i]*118/100;
                    osd->DrawRectangle(coverright -64 +(bar/2),coverbottom -6 -(i*barWidth) -barWidth, coverright   , coverbottom -(i*barWidth), clrCoverBar);
                  }
                  break;
          
              case 6:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    barWidth = 128/bandsSA;
                    bar      = (barHeights[i]*118)/100;
                    barPeak  = barPeaksBothChannels[i]*118/100;
                    if(barPeak > 0) {
                      osd->DrawRectangle(coverleft +(i*barWidth), covertop, coverleft + (i*barWidth) +barWidth -1, covertop + 122 - barPeak -barPeakHeight, clrCoverBar);
                      osd->DrawRectangle(coverleft +(i*barWidth), covertop +122 -barPeak +1, coverleft + (i*barWidth) +barWidth -1, coverbottom, clrCoverBar);
                      }
                    else {
                      osd->DrawRectangle(coverleft +(i*barWidth), covertop, coverleft + (i*barWidth) +barWidth -1, coverbottom, clrCoverBar);
                    }  
                  }
                  break;

              case 7:
                  LoadVisCover();
                  for ( i=0; i < bandsSA; i++ ) {
                    barWidth = 128/bandsSA;
                    bar      = (barHeights[i]*118)/100;
                    osd->DrawRectangle(coverleft +(i*barWidth), covertop, coverleft + (i*barWidth) +barWidth -1, covertop +122 -bar, clrCoverBar);
                    osd->DrawRectangle(coverleft +(i*barWidth), covertop + 122 -bar +4, coverleft + (i*barWidth) +barWidth -1, coverbottom, clrCoverBar);
                  }
                  break;
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
    osd->DrawRectangle(MP3Skin.coverx, MP3Skin.covery, MP3Skin.coverx + MP3Skin.coverw, MP3Skin.covery + MP3Skin.coverh, clrCoverBG);
    flush = true;
  }
}



void cMP3Control::LoadVisCover(void)
{
  cMP3Bitmap *bmp;

  if((bmp = cMP3Bitmap::Load(visimagefile.c_str(), osdalpha, coverh, coverw, covercolor)) !=NULL) {
    osd->DrawBitmap(coverx , covery, bmp->Get(), clrTransparent, clrTransparent, true);
  }
}


void cMP3Control::LoadCover(void)
{
  int bmpcolors=covercolor;
  int w1=coverw;
  int h1=coverh;

  if (coverpicture == NULL) {
	esyslog("music-sd: ctrl: Oops cant load coverpicture : %s",coverpicture);
	}
  else {
    if(MP3Setup.showcoveronly) {
      switch(coversize) {
        case 0:
          bmpcolors = MP3Skin.bigcovercolor;
          w1 = 120;
          h1 = 120;
////          osd->DrawRectangle(0 , 0, w1 -1, h1 -1, clrBlack);
          Flush();
          break;  
        case 1:  
          bmpcolors = MP3Skin.bigcovercolor;
          w1 = 274;
          h1 = 274;
////          osd->DrawRectangle(0 , 0, w1 -1, h1 -1, clrBlack);
          Flush();
          break;
        case 2:
          bmpcolors = MP3Skin.bigcovercolor;
          w1 = 300;
          h1 = 256;
////          osd->DrawRectangle(0 , 0, w1 -1, h1 -1, clrBlack);
          Flush();
          break;
        case 3:
          bmpcolors = MP3Skin.bigcovercolor;
          w1 = MP3Skin.bigcoverw;
          h1 = MP3Skin.bigcoverh;
////          osd->DrawRectangle(0 , 0, w1 -1, h1 -1, clrBlack);
          Flush();
          break;
      }	        
    }

    cMP3Bitmap *bmp;

    if((bmp = cMP3Bitmap::Load(coverpicture, osdalpha, h1, w1, bmpcolors)) !=NULL) {

      if(MP3Setup.showcoveronly) {
        osd->DrawRectangle( 0, 0, w1 -1, h1 -1, clrTransparent);
        osd->DrawBitmap( 0, 0, bmp->Get(), clrTransparent, clrTransparent, true);
        }
      else {
        if(!MP3Skin.UseOneArea) {
          osd->GetBitmap(0)->SetColor(0, clrBG);
          osd->GetBitmap(0)->SetColor(1, clrCoverBG);
#ifdef  DEBUG_important
          printf("pal_color_0  : %08X\n", osd->GetBitmap(0)->Color(0));
          printf("pal_color_1  : %08X\n", osd->GetBitmap(0)->Color(1));
#endif
          for(int i = 0; i < 14; i++) {
            osd->GetBitmap(0)->SetColor(i+2, bmp->Get().Color(i));
#ifdef  DEBUG_important
            printf("pal_color_%i  : %08X\n", i+2, osd->GetBitmap(0)->Color(i+2));
#endif
            }    
          }
        else {
//	  int a = 0;
//          for(int i = 240; i < 254; i++) {
//            osd->GetBitmap(0)->SetColor(i, bmp->Get().Color(a));
//	    a++;
//          }    
#ifdef  DEBUG_important
          for(int i2 = 0; i2 < 256; i2++) {
            printf("pal_color_%i  : %08X\n", i2, osd->GetBitmap(0)->Color(i2));
          }
#endif
	}
	
        osd->DrawBitmap(coverx , covery, bmp->Get(), clrTransparent, clrTransparent, true, false);
      }
    }
  }
  MP3Setup.CanLoadCover = 0;
}



void cMP3Control::LoadBackground(void)
{
  std::string background;
 
  background = config;
  background = background + MP3Skin.osdbackground;

#ifdef  DEBUG_important
  printf("music-sd: Load Background ='%s' with values=%ix%i colors=%i\n", background.c_str(), osdwidth, osdheight,MP3Skin.osdbackgroundcolor);
#endif

  cMP3Bitmap *bmp;
  if((bmp = cMP3Bitmap::Load(background.c_str(), osdalpha, osdheight, osdwidth, MP3Skin.osdbackgroundcolor)) !=NULL) {
    osd->DrawBitmap( 0, 0, bmp->Get() ,clrTransparent ,clrTransparent, true);

  }
}


void cMP3Control::ShowCoverOnly(bool open)
{
  coversize = MP3Setup.Coversize;
  int x =0;
  int y =0;
  int w =120;
  int h =120;
          
  switch(coversize) {
    case 0:
          x =Setup.OSDLeft + Setup.OSDWidth -120;
          y =Setup.OSDTop;
          w =120;
          h =120;
          break;  
    case 1:  
          x =(Setup.OSDWidth  /2 - 136)  + Setup.OSDLeft;
          y =(Setup.OSDHeight  /2 - 136) + Setup.OSDTop;
          w =274;
          h =274;
          break;
    case 2:
          x =(Setup.OSDWidth  /2 - 150)  + Setup.OSDLeft;
          y =(Setup.OSDHeight  /2 - 128) + Setup.OSDTop;
          w =300;
          h =256;
          break;
    case 3:
          x =MP3Skin.bigcoverx;
          y =MP3Skin.bigcovery;
          w =MP3Skin.bigcoverw;
          h =MP3Skin.bigcoverh;
          break;
  }	        


  if(!cOsd::IsOpen()) {
    if(!visible && !open) {
      open=true;
    }
  }

  if(!visible && open) {

    if(MP3Setup.ShowMessage)
      osd=cOsdProvider::NewOsd(x , y, 20);
    else
      osd=cOsdProvider::NewOsd(x , y);
	
    if(!osd) return;

    tArea Area[] = {{ 0, 0, w -1, h -1, MP3Skin.bigcoverdepth}, };
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

      esyslog("music-sd: ctrl: ERROR! osd open failed! can't handle areas (%d)-%s\n", result, errormsg);
      if(osd){ delete osd; osd=0;}
      return;
    }

    mgr->playnow = false;

    osd->DrawRectangle(0 , 0, w -1, h -1, clrTransparent); // Cover

    visible=true;

    force = true;

    delete lastMode; lastMode=0;
  }

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

      player->LoadImage(mode->Filename, mode->Artist, true);
      LoadCover();
      flush = true;
    }
    if(flush) Flush();
  }

#if VDRVERSNUM >= 10500
  SetNeedsFastResponse(false);
#else  
  needsFastResponse=false;
#endif
  delete lastMode; lastMode=mode;
  refresh = false;

}      



void cMP3Control::Flush(void)
{
  if(osd) osd->Flush();
}


void cMP3Control::ShowHelpButtons(int ShowButtons) {
  int align;

  showbuttons = ShowButtons;
  align       = MP3Skin.buttonalign;
  
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
         blue = tr("Parent");
         break; 
      case 3:  	
         red = tr("Parent");
         green = tr("<<");
         yellow = tr(">>");
         blue = tr("Min/Sec");
         break;  
  }
 
  if (MP3Skin.drawButtons) { 
    osd->DrawEllipse(      MP3Skin.elipseredx    , MP3Skin.elipseredy    , MP3Skin.elipseredx    + MP3Skin.elipseh , MP3Skin.elipseredy    + MP3Skin.elipseh , clrStatusRed    , 0);
    osd->DrawEllipse(      MP3Skin.elipsegreenx  , MP3Skin.elipsegreeny  , MP3Skin.elipsegreenx  + MP3Skin.elipseh , MP3Skin.elipsegreeny  + MP3Skin.elipseh , clrStatusGreen  , 0);
    osd->DrawEllipse(      MP3Skin.elipseyellowx , MP3Skin.elipseyellowy , MP3Skin.elipseyellowx + MP3Skin.elipseh , MP3Skin.elipseyellowy + MP3Skin.elipseh , clrStatusYellow , 0);
    osd->DrawEllipse(      MP3Skin.elipsebluex   , MP3Skin.elipsebluey   , MP3Skin.elipsebluex   + MP3Skin.elipseh , MP3Skin.elipsebluey   + MP3Skin.elipseh , clrStatusBlue   , 0);
  }

  osd->DrawText(         MP3Skin.buttontext1posx , MP3Skin.buttontext1posy , red    , clrStatusFG1, clrStatusBG, pFontButton, MP3Skin.button1w, fh, align);
  osd->DrawText(         MP3Skin.buttontext2posx , MP3Skin.buttontext2posy , green  , clrStatusFG2, clrStatusBG, pFontButton, MP3Skin.button2w, fh, align);
  osd->DrawText(         MP3Skin.buttontext3posx , MP3Skin.buttontext3posy , yellow , clrStatusFG3, clrStatusBG, pFontButton, MP3Skin.button3w, fh, align);
  osd->DrawText(         MP3Skin.buttontext4posx , MP3Skin.buttontext4posy , blue   , clrStatusFG4, clrStatusBG, pFontButton, MP3Skin.button4w, fh, align);

  sendHelpButtons(red, green, yellow, blue);	

  refresh = true;
}



void cMP3Control::ConvertRatingToChar( int Posx, int Posy, int Value, int bgClr, int fgClr, int fontheight, int fontwidth ) {

  int bg=bgClr;
  int fg=fgClr;
  int posx=Posx;
  int posy=Posy;
  int value=Value;
  int fonth=fontheight;
  int fontw=fontwidth;

  if(value <0) value=0;


  osd->DrawRectangle(posx, posy, posx + (5*fontw), posy + fonth , bg);
  osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), 0xFF202020, bg, pFontSymbol, fontw, fonth, taLeft);
  osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), 0xFF202020, bg, pFontSymbol, fontw, fonth, taLeft);
  osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Full_Star(), 0xFF202020, bg, pFontSymbol, fontw, fonth, taLeft);
  osd->DrawText( posx + 3*fontw, posy, Icons::OSD_Full_Star(), 0xFF202020, bg, pFontSymbol, fontw, fonth, taLeft);
  osd->DrawText( posx + 4*fontw, posy, Icons::OSD_Full_Star(), 0xFF202020, bg, pFontSymbol, fontw, fonth, taLeft);



  switch(value) {
     case 0: // Empty
       break;
     case 3: // Delete
       osd->DrawText( posx, posy, Icons::OSD_Bomb(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 28: // D
       osd->DrawText( posx, posy, Icons::OSD_Half_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 53: // O
       osd->DrawText( posx, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 78: // OD
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw, posy, Icons::OSD_Half_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 104: // OO
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 129: // OOD
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Half_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 154: // OOO
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 179: // OOOD
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 3*fontw, posy, Icons::OSD_Half_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break; 
     case 205: // OOOO
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 3*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 230: // OOOOD
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 3*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 4*fontw, posy, Icons::OSD_Half_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 255: // OOOOO
       osd->DrawText( posx          , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + fontw  , posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 2*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 3*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       osd->DrawText( posx + 4*fontw, posy, Icons::OSD_Full_Star(), fg, bg, pFontSymbol, fontw, fonth, taLeft);
       break;
     case 300: // Nothing
       osd->DrawRectangle(posx, posy, posx + (5*fontw), posy + fonth , bg);
       break;
     
     default: // Empty
       break;  
  }

  flush = true;
}


void cMP3Control::SetVars(void)
{
  artistfirst           = MP3Setup.ArtistFirst;
  // size of areas
  cw                    = (MP3Skin.area_coverw) & ~0x07;
  ch                    = MP3Skin.area_coverh;
  th                    = MP3Skin.area_toph;
  ih                    = MP3Skin.area_infoh;
  lh                    = MP3Skin.area_listh;
  sh                    = MP3Skin.area_symbolh;
  // fontheight
  fh                    = MP3Skin.fontheight;
  sfh                   = MP3Skin.smallfontheight;

  rows                  = MP3Skin.trackcount;
  sw                    = MP3Skin.symbolxy;  // height/width of symobls
  coverw                = MP3Skin.coverw;
  coverh                = MP3Skin.coverh;
  coverx                = MP3Skin.coverx;
  covery                = MP3Skin.covery;
  covercolor            = MP3Skin.covercolor;
  clrBG                 = MP3Skin.clrBG;
  clrTopBG              = MP3Skin.clrTopBG;
  clrTopFG              = MP3Skin.clrTopFG;
  clrArtistBG           = MP3Skin.clrArtistBG;
  clrArtistFG           = MP3Skin.clrArtistFG;
  clrRatingFG           = MP3Skin.clrRatingFG;
  clrPlayStatusBG       = MP3Skin.clrPlayStatusBG;
  clrPlayStatusFG       = MP3Skin.clrPlayStatusFG;
  clrInfoBG             = MP3Skin.clrInfoBG;
  clrInfoFG             = MP3Skin.clrInfoFG;
  clrProgressbarBG      = MP3Skin.clrProgressbarBG;
  clrProgressbarFG      = MP3Skin.clrProgressbarFG;
  clrListTitle          = MP3Skin.clrListTitle;
  clrListBG             = MP3Skin.clrListBG;
  clrListFG             = MP3Skin.clrListFG;
  clrListRating         = MP3Skin.clrListRating;
  clrSymbolBG           = MP3Skin.clrSymbolBG;
  clrSymbolFG           = MP3Skin.clrSymbolFG;
  clrSymbolActive       = MP3Skin.clrSymbolActive;
  clrRecordingActive    = MP3Skin.clrRecordingActive;
  clrVolumeActive        = MP3Skin.clrVolumeActive;
  clrVolumeProgressbarBG = MP3Skin.clrVolumeProgressbarBG;
  clrVolumeProgressbarFG = MP3Skin.clrVolumeProgressbarFG;


  clrStatusBG            = MP3Skin.clrStatusBG;
  clrStatusFG1           = MP3Skin.clrStatusFG1;
  clrStatusFG2           = MP3Skin.clrStatusFG2;
  clrStatusFG3           = MP3Skin.clrStatusFG3;
  clrStatusFG4           = MP3Skin.clrStatusFG4;
  clrStatusRed           = MP3Skin.clrStatusRed;
  clrStatusGreen         = MP3Skin.clrStatusGreen;
  clrStatusYellow        = MP3Skin.clrStatusYellow;
  clrStatusBlue          = MP3Skin.clrStatusBlue;
  
  clrCoverBG             = MP3Skin.clrCoverBG;
  clrCoverBar            = MP3Skin.clrCoverBar;
//  imgalpha               = MP3Setup.ImgAlpha;
  osdalpha               = MP3Skin.OSDAlpha;
  visimagefile           = MP3VisLoader.VisLoaderBackground();
  
  osdheight              = MP3Skin.osdheight;
  osdwidth               = MP3Skin.osdwidth;
  osdtop                 = MP3Skin.osdtop;
  osdleft                = MP3Skin.osdleft;

  if ( player->IsStream() )
    clrProgressbarBG    = 0xFF000000;
  else
    clrProgressbarBG    = MP3Skin.clrProgressbarBG;
  
  
  visualization         = MP3Setup.Visualization; 
  channel1left          = MP3Setup.Channel1Left;
  channel2left          = MP3Setup.Channel2Left;
  barwidth              = MP3Setup.BarWidth;
  channelsSA            = MP3Setup.ChannelsSA;
  bandsSA               = MP3Setup.BandsSA;
  
}


void cMP3Control::SetFonts(void)
{


  if (MP3Skin.ReloadFonts) {
    printf("MUSIC-SD : DELETE FONTS\n");

    if(pFontTop)       delete pFontTop;
    if(pFontInterpret) delete pFontInterpret;
    if(pFontGenre)     delete pFontGenre;
    if(pFontStatus)    delete pFontStatus;
    if(pFontInfo)      delete pFontInfo;
    if(pFontTitle)     delete pFontTitle;
    if(pFontList)      delete pFontList;
    if(pFontButton)    delete pFontButton;
    if(pFontSymbol)    delete pFontSymbol;
    MP3Skin.ReloadFonts=0;
  }
  
  printf("MUSIC-SD : CREATE FONTS\n");

  pFontTop        = cFont::CreateFont(MP3Skin.FONTTOPNAME.c_str()       , MP3Skin.FONTTOPSIZE       , 0);
  pFontInterpret  = cFont::CreateFont(MP3Skin.FONTINTERPRETNAME.c_str() , MP3Skin.FONTINTERPRETSIZE , 0);
  pFontGenre      = cFont::CreateFont(MP3Skin.FONTGENRENAME.c_str()     , MP3Skin.FONTGENRESIZE     , 0);
  pFontStatus     = cFont::CreateFont(MP3Skin.FONTSTATUSNAME.c_str()    , MP3Skin.FONTSTATUSSIZE    , 0);
  pFontInfo       = cFont::CreateFont(MP3Skin.FONTINFONAME.c_str()      , MP3Skin.FONTINFOSIZE      , 0);
  pFontTitle      = cFont::CreateFont(MP3Skin.FONTTITLENAME.c_str()     , MP3Skin.FONTTITLESIZE     , 0);
  pFontList       = cFont::CreateFont(MP3Skin.FONTLISTNAME.c_str()      , MP3Skin.FONTLISTSIZE      , 0);
  pFontButton     = cFont::CreateFont(MP3Skin.FONTBUTTONNAME.c_str()    , MP3Skin.FONTBUTTONSIZE    , 0);
  pFontSymbol     = cFont::CreateFont(MP3Skin.FONTSYMBOLNAME.c_str()    , MP3Skin.FONTSYMBOLSIZE    , 0);

  printf("MUSIC-SD : FONTS CREATED\n");

}


void cMP3Control::SetVol(void)
{
  vol   = player->CurrentVolume();
  muted = player->Muted();

  if(muted || vol == 0)
    osd->DrawText( MP3Skin.mutex, MP3Skin.mutey, Icons::OSD_Mute(), clrVolumeActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
  else
    osd->DrawText( MP3Skin.mutex , MP3Skin.mutey, Icons::OSD_Volume(), clrVolumeActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);

  cProgressBar VolumeBar(MP3Skin.volbarw, MP3Skin.volbarh, vol, 255, clrVolumeProgressbarFG , clrVolumeProgressbarBG);
  osd->DrawBitmap(MP3Skin.volbarx, MP3Skin.volbary, VolumeBar);
  
  flush = true;
}

bool cMP3Control::BuildAreas(void)
{
  tArea Area[] = { { 0, 0, osdwidth -1,osdheight -1 , MP3Skin.OneAreaDepth} };
    if (MP3Skin.UseOneArea && osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea)) == oeOk) {
      osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
	  return true;
      }
    else {	 
      tArea Area[] = {
        {	0   ,th +1              ,cw -1        ,ch               ,MP3Skin.coverdepth},     //  cover
        {	cw  ,th +1              ,osdwidth -1  ,ch               ,MP3Skin.trackinfodepth}, //  trackinfo
        {	0   ,0                  ,osdwidth -1  ,th               ,MP3Skin.topdepth},       //  border top
        {	0   ,ch +1              ,osdwidth -1  ,ih               ,MP3Skin.statusdepth},    //  info,Progress
        {	0   ,ih +1              ,osdwidth -1  ,lh               ,MP3Skin.tracksdepth},    //  titel + next tracks
        {	0   ,lh +1              ,osdwidth -1  ,sh               ,MP3Skin.symboldepth},    //  symbolbar
        {	0   ,sh +1              ,osdwidth -1  ,osdheight -1     ,MP3Skin.buttondepth},    //  bottombar
      };

      eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
      if (result == oeOk) {
        osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
		return true;
        }
      else  {
        const char *errormsg = NULL;
        switch (result) {
          case oeTooManyAreas:
            errormsg = "music-sd: Too many OSD areas"; break;
          case oeTooManyColors:
            errormsg = "music-sd: Too many colors"; break;
          case oeBppNotSupported:
            errormsg = "music-sd: Depth not supported"; break;	
          case oeAreasOverlap:
            errormsg = "music-sd: Areas are overlapped"; break;
          case oeWrongAlignment:
            errormsg = "music-sd: Areas not correctly aligned"; break;
          case oeOutOfMemory:
            errormsg = "music-sd: OSD memory overflow"; break;
          case oeUnknown:
            errormsg = "music-sd: Unknown OSD error"; break;
          default:
            break;
        }	
        esyslog("music-sd: ctrl: ERROR! osd open failed! can't handle areas (%d)-%s\n", result, errormsg);
        if(osd){ delete osd; osd=0;}

	    return false;
      }
    }
}



void cMP3Control::BuildDefault(void)
{
  if(MP3Skin.isOSDTheme) {      
    // load the backgroundpicture
    LoadBackground();
    // Draw bars on top and bottom
    if (MP3Skin.drawBars) {
      osd->DrawRectangle(0 , 0                 , osdwidth -1 , fh -3        , clrTopBG);    // topbar             
      osd->DrawRectangle(0 , osdheight -1 -fh  , osdwidth -1 , osdheight -1 , clrStatusBG); // bottombar

    }
    // draw transparent elipses at corner
    if (MP3Skin.roundedCorner) {
      osd->DrawEllipse( 0                 , 0                  , fh/2         , fh/2         , clrTransparent, -2);
      osd->DrawEllipse( osdwidth -1 -fh/2 , 0                  , osdwidth -1  , fh/2         , clrTransparent, -1);
      osd->DrawEllipse( 0                 , osdheight -1 -fh/2 , fh/2         , osdheight -1 , clrTransparent, -3);
      osd->DrawEllipse( osdwidth -1 -fh/2 , osdheight -1 -fh/2 , osdwidth -1  , osdheight -1 , clrTransparent, -4);
    }
  }
  else {
    osd->DrawRectangle(0, 0, osdwidth -1, osdheight -1, clrBG);
  }

// osd->SaveRegion(0, th +1, cw -1, ch);




	  
  // Loop Shuffle
  if(MP3Skin.symrect) {
    osd->DrawRectangle( MP3Skin.shufflex, MP3Skin.shuffley, MP3Skin.shufflex +sw, MP3Skin.shuffley +sw, clrSymbolFG);
    // Loop symbol
    osd->DrawRectangle( MP3Skin.loopx, MP3Skin.loopy, MP3Skin.loopx +sw, MP3Skin.loopy +sw, clrSymbolFG);
    // Lyrics
    osd->DrawRectangle( MP3Skin.lyricsx, MP3Skin.lyricsy, MP3Skin.lyricsx +sw, MP3Skin.lyricsy +sw, clrSymbolFG);
    // Copy symbol
    osd->DrawRectangle( MP3Skin.copyx, MP3Skin.copyy, MP3Skin.copyx +sw, MP3Skin.copyy +sw, clrSymbolFG);
    // Shutdown symbol
    if(MP3Setup.EnableShutDown) 
       osd->DrawRectangle( MP3Skin.shutdownx, MP3Skin.shutdowny, MP3Skin.shutdownx +sw, MP3Skin.shutdowny +sw, clrSymbolActive);
    else	
       osd->DrawRectangle( MP3Skin.shutdownx, MP3Skin.shutdowny, MP3Skin.shutdownx +sw, MP3Skin.shutdowny +sw, clrSymbolFG);
    // Timer
    if(MP3Setup.TimerEnabled)
      osd->DrawRectangle( MP3Skin.timerx, MP3Skin.timery, MP3Skin.timerx +sw, MP3Skin.timery +sw, clrSymbolActive);
    else		
      osd->DrawRectangle( MP3Skin.timerx, MP3Skin.timery, MP3Skin.timerx +sw, MP3Skin.timery +sw, clrSymbolFG);
    }
  else {
    osd->DrawText( MP3Skin.shufflex, MP3Skin.shuffley, Icons::OSD_Shuffle(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    // Loop symbol
    osd->DrawText( MP3Skin.loopx, MP3Skin.loopy, Icons::OSD_Replay(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    // Lyrics
    osd->DrawText( MP3Skin.lyricsx, MP3Skin.lyricsy, Icons::OSD_Lyrics(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    // Copy symbol
    osd->DrawText( MP3Skin.copyx, MP3Skin.copyy, Icons::OSD_Copy(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    // Shutdown symbol
    if(MP3Setup.EnableShutDown) 
      osd->DrawText( MP3Skin.shutdownx, MP3Skin.shutdowny, Icons::OSD_Shutdown(), clrSymbolActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    else	
      osd->DrawText( MP3Skin.shutdownx, MP3Skin.shutdowny, Icons::OSD_Shutdown(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    // Timer
    if(MP3Setup.TimerEnabled)
      osd->DrawText( MP3Skin.timerx, MP3Skin.timery, Icons::OSD_Timer(), clrSymbolActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
    else		
      osd->DrawText( MP3Skin.timerx, MP3Skin.timery, Icons::OSD_Timer(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
  }		
		
  SetVol();
// SetKey buttons
  ShowHelpButtons(showbuttons);	
}


void cMP3Control::ShowProgress(bool open)
{
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
      if (MP3Skin.isMpeg == 1) {
        MP3Skin.reloadmpeg = true;
      }
      if(MP3Setup.ShowMessage)
        osd=cOsdProvider::NewOsd(osdleft + MP3Setup.OSDoffsetx, osdtop + MP3Setup.OSDoffsety, 20);
      else
        osd=cOsdProvider::NewOsd(osdleft + MP3Setup.OSDoffsetx, osdtop + MP3Setup.OSDoffsety);

      if(!osd) return;
      if (!BuildAreas()) return;

      mgr->playnow = false;

// SetFont
      if(MP3Skin.ReloadFonts) SetFonts();

      BuildDefault();


#if VDRVERSNUM >= 10500
      visible = true;
      if(MP3Setup.EnableVis && ShowSA)
        SetNeedsFastResponse(true);
      else
        SetNeedsFastResponse(false);
#else      
      visible=true;
      if(MP3Setup.EnableVis && ShowSA)
        needsFastResponse=true;
      else
        needsFastResponse=false;
#endif
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

        if(mode->Filename && !(strcmp(mode->Filename,lastfile) ==0)) {
          printf("music-sd: straight before cStatus::MsgReplaying(this.......)\n");
          cStatus::MsgReplaying(this,buf,mode->Filename[0]?mode->Filename:0,true);
          printf("music-sd: straight after cStatus::MsgReplaying(this.......)\n");
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
      if ( mode->Filename && !(strcmp(mode->Filename,lastfile) ==0) || force || MP3Setup.ShowStatus != laststatus || changed) {
        strncpy(lastfile, mode->Filename,sizeof(lastfile));


        if(mode->Filename && !(strcmp(mode->Filename,lastfile) ==0)) printf("mode->Filename\n");
        if(force) printf("force\n");
        if(MP3Setup.ShowStatus != laststatus) printf("mp3setup.showstatus\n");
        if(changed) printf("changed\n");


        playstatus = -1;
        gtftChanges++;


		
// Load Cover
        player->LoadImage(mode->Filename, mode->Artist, true);
        if(!MP3Setup.EnableVis) {
          LoadCover();
          flush = true;
        }		
		
// Rating
        if(MP3Setup.EnableRating) {
          ConvertRatingToChar(MP3Skin.ratingx, MP3Skin.ratingy, mode->Rating, clrArtistBG, clrRatingFG, fh, fh); 
        }

// Infofield        
        buf[0]=0;
        if(mode->Artist[0]) {
          snprintf(buf,sizeof(buf),"%s",mode->Artist);
          osd->DrawText( MP3Skin.artistx, MP3Skin.artisty, buf, clrArtistFG, clrArtistBG, pFontInterpret, MP3Skin.artistw, fh, taLeft);
          }          
        else {
          snprintf(buf,sizeof(buf),"%s: %s",tr("Artist"), tr("Unknown"));
          osd->DrawText( MP3Skin.artistx, MP3Skin.artisty, buf, clrArtistFG, clrArtistBG, pFontInterpret, MP3Skin.artistw, fh, taLeft);
        }
        
        if(mode->Album[0]) {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Album"), mode->Album);
          osd->DrawText( MP3Skin.albumx, MP3Skin.albumy, buf, clrArtistFG, clrArtistBG, pFontGenre, MP3Skin.albumw, fh, taLeft);
          }
        else {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Album"),tr("Unknown"));
          osd->DrawText( MP3Skin.albumx, MP3Skin.albumy, buf, clrArtistFG, clrArtistBG, pFontGenre, MP3Skin.albumw, fh, taLeft);
        }

        if(mode->Genre[0]) {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Genre"), mode->Genre);
          osd->DrawText( MP3Skin.genrex, MP3Skin.genrey, buf, clrArtistFG, clrArtistBG, pFontGenre, MP3Skin.genrew, fh, taLeft);
          }          
        else {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Genre"), tr("Unknown"));
          osd->DrawText( MP3Skin.genrex, MP3Skin.genrey, buf, clrArtistFG, clrArtistBG, pFontGenre, MP3Skin.genrew, fh, taLeft);
        }
          
        if(mode->Year > 2) {
          snprintf(buf,sizeof(buf),"%s: %d", tr("Year"), mode->Year);
          osd->DrawText( MP3Skin.yearx, MP3Skin.yeary, buf, clrArtistFG, clrArtistBG, pFontGenre, MP3Skin.yearw, fh, taLeft);
          }          
        else {
          snprintf(buf,sizeof(buf),"%s: %s", tr("Year"), tr("Unknown"));
          osd->DrawText( MP3Skin.yearx, MP3Skin.yeary, buf, clrArtistFG, clrArtistBG, pFontGenre, MP3Skin.yearw, fh, taLeft);
        }

// Lyrics exists or not symbol
        if(MP3Skin.symrect) {
          if(player->ExistsLyrics(mode->Filename)) {
            osd->DrawRectangle( MP3Skin.lyricsx, MP3Skin.lyricsy, MP3Skin.lyricsx +sw, MP3Skin.lyricsy +sw, clrSymbolActive);
            flush=true;
            }
          else {
            osd->DrawRectangle( MP3Skin.lyricsx, MP3Skin.lyricsy, MP3Skin.lyricsx +sw, MP3Skin.lyricsy +sw, clrSymbolFG);
            flush=true;
            }
          }  
        else {
          if(player->ExistsLyrics(mode->Filename)) {
            osd->DrawText( MP3Skin.lyricsx, MP3Skin.lyricsy, Icons::OSD_Lyrics(), clrSymbolActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
            flush=true;
            }
          else {
            osd->DrawText( MP3Skin.lyricsx, MP3Skin.lyricsy, Icons::OSD_Lyrics(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
            flush=true;
          }
        }  
        force = false;

      } // End of NewSong

      // track changed or show status
      if(changed || refresh || MP3Setup.ShowStatus != laststatus) {

        // Recording symbol
        if(MP3Skin.symrect) {	  
          if(MP3Setup.RecordStream) {
            osd->DrawRectangle( MP3Skin.recordx, MP3Skin.recordy, MP3Skin.recordx +sw, MP3Skin.recordy +sw, clrRecordingActive);
            flush=true;
            }
          else {
            osd->DrawRectangle( MP3Skin.recordx, MP3Skin.recordy, MP3Skin.recordx +sw, MP3Skin.recordy +sw, clrSymbolFG);
            flush=true;
            }
          }             
        else {
          if(MP3Setup.RecordStream) {
            osd->DrawText( MP3Skin.recordx, MP3Skin.recordy, Icons::OSD_Record(), clrRecordingActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
            flush=true;
            }
          else {
            osd->DrawText( MP3Skin.recordx, MP3Skin.recordy, Icons::OSD_Record(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
            flush=true;
          }
        }          

        // Infofield
        if(!selecting) {
          gtftChanges++;

          if(mode->MaxBitrate>0) {
            snprintf(buf,sizeof(buf),"%.1f kHz, %d-%d kbps, %s", mode->SampleFreq/1000.0,mode->Bitrate/1000,mode->MaxBitrate/1000,mode->SMode);
            osd->DrawText( MP3Skin.modex, MP3Skin.modey, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.modew, sfh, taLeft);
            }
          else {
            snprintf(buf,sizeof(buf),"%.1f kHz, %d kbps, %s", mode->SampleFreq/1000.0,mode->Bitrate/1000,mode->SMode);
            osd->DrawText( MP3Skin.modex, MP3Skin.modey, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.modew, sfh, taLeft);
          }

          snprintf(buf,sizeof(buf),"%s %i %s %i","No.",mode->Num,tr("of"),mode->MaxNum);
          osd->DrawText( MP3Skin.numberx, MP3Skin.numbery, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.numberw, sfh, taLeft);
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
          if(buf[0]) DisplayInfo(buf);
     
          flush = true;
        }



// Tracklist		
      if (mode->Num < 1) {
        osd->DrawRectangle(MP3Skin.titlex , MP3Skin.titley, MP3Skin.titlex + MP3Skin.titlew , MP3Skin.tracksy +(rows *sfh), clrListBG);
        osd->DrawText( MP3Skin.titlex, MP3Skin.titley, tr("Please add some tracks..."), clrListTitle, clrListBG, pFontTitle, MP3Skin.titlew, fh, taLeft);
        sendPlaylistItem(tr("Please add some tracks..."), 0, 1);
        }
      // last track is playing...
      else if (mode->Num == mode->MaxNum) {
        osd->DrawRectangle(MP3Skin.tracksx , MP3Skin.tracksy, MP3Skin.titlex + MP3Skin.titlew , MP3Skin.tracksy +(rows *sfh), clrListBG);
        osd->DrawText( MP3Skin.tracksx, MP3Skin.tracksy, tr("End of playlist ============================"), clrListFG, clrListBG, pFontList, MP3Skin.tracksw, sfh, taLeft);
        sendPlaylistItem("-EOL-", 0, 1);
      }

      // more than one track to play...
      if( (mode->MaxNum - mode->Num) >= 1 ) {
        osd->DrawRectangle(MP3Skin.tracksx , MP3Skin.tracksy, MP3Skin.tracksx + MP3Skin.tracksw  , MP3Skin.tracksy +(rows *sfh), clrListBG);

        cMP3PlayInfo pi;

        // the graphTFT should display more than configured rows ... TODO , this isnt perfect ..if you load many tracks  and scan is active , OSD show up after a long time
        // int maxNum = max(mgr->InfoCount()-mode->Num, 0);
        // maxNum = min(maxNum, 1000);  // not realy needed ;)

        for(int i=0 ; i < rows; i++) {
          mgr->Info(mode->Num +i +1, &pi);
          if(pi.Title[0]) {
            if(artistfirst && pi.Artist[0])
              snprintf(buf,sizeof(buf),pi.Artist[0]?"%s  -  %s":"%s",pi.Artist, pi.Title);
            else 
              snprintf(buf,sizeof(buf),pi.Artist[0]?"%s  -  %s":"%s",pi.Title,pi.Artist);

            if(i < rows) {
              if(MP3Setup.EnableRating) {
                osd->DrawText( MP3Skin.tracksx, MP3Skin.tracksy +(sfh *i), buf, clrListFG, clrListBG, pFontList, MP3Skin.tracksw , sfh, taLeft);
                ConvertRatingToChar( MP3Skin.ratesmx , MP3Skin.ratesmy +(sfh*i), pi.Rating, clrListBG, clrListRating, sfh, sfh);
                }
              else {
                osd->DrawText( MP3Skin.tracksx, MP3Skin.tracksy +(sfh *i), buf, clrListFG, clrListBG, pFontList, MP3Skin.tracksw, sfh, taLeft);
              }
            }	

            sendPlaylistItem(buf, i, rows);
          }
          else {
            osd->DrawText( MP3Skin.tracksx, MP3Skin.tracksy +(sfh *i), "", clrListFG, clrListBG, pFontList, MP3Skin.tracksw, sfh, taLeft);
            ConvertRatingToChar( MP3Skin.ratesmx , MP3Skin.ratesmy +(sfh*i), 300, clrListBG, clrListRating, sfh, sfh);
            sendPlaylistItem("", i, rows);
          }
        }
      }


      // Loof Shuffle
      if(MP3Skin.symrect) {      
        if( !lastMode || mode->Shuffle!=lastMode->Shuffle) {
          if(mode->Shuffle)
            osd->DrawRectangle( MP3Skin.shufflex, MP3Skin.shuffley, MP3Skin.shufflex +sw, MP3Skin.shuffley +sw, clrSymbolActive);
          else
            osd->DrawRectangle( MP3Skin.shufflex, MP3Skin.shuffley, MP3Skin.shufflex +sw, MP3Skin.shuffley +sw, clrSymbolFG);
          flush = true;  
        }

        // Loop symbol
        if( !lastMode || mode->Loop!=lastMode->Loop) {
          if(mode->Loop)
            osd->DrawRectangle( MP3Skin.loopx, MP3Skin.loopy, MP3Skin.loopx +sw, MP3Skin.loopy +sw, clrSymbolActive);
          else
            osd->DrawRectangle( MP3Skin.loopx, MP3Skin.loopy, MP3Skin.loopx +sw, MP3Skin.loopy +sw, clrSymbolFG);
          flush = true;  
          }
        }  
      else {
        if( !lastMode || mode->Shuffle!=lastMode->Shuffle) {
          if(mode->Shuffle)
            osd->DrawText( MP3Skin.shufflex, MP3Skin.shuffley, Icons::OSD_Shuffle(), clrSymbolActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
          else
            osd->DrawText( MP3Skin.shufflex, MP3Skin.shuffley, Icons::OSD_Shuffle(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
          flush = true;  
        }

        // Loop symbol
        if( !lastMode || mode->Loop!=lastMode->Loop) {
          if(mode->Loop)
            osd->DrawText( MP3Skin.loopx, MP3Skin.loopy, Icons::OSD_Replay(), clrSymbolActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
          else
            osd->DrawText( MP3Skin.loopx, MP3Skin.loopy, Icons::OSD_Replay(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);
          flush = true;  
        }
      }  

      }	

      // Playstatus   
      if(mgr->Scanning() && mgr->maxIndex >0) {
        gtftChanges++;	  
     	osd->DrawText( MP3Skin.statusx, MP3Skin.statusy, tr("SCANNING"), clrPlayStatusFG, clrPlayStatusBG, pFontStatus, MP3Skin.statusw, sfh, taCenter);
        gtftStatus.status = tr("SCANNING");
        }
      else {		
        if(playstatus != player->StatusMode()) {
          playstatus = player->StatusMode();
		  gtftChanges++;
		  
		  switch(playstatus) {
            case 1:
	            osd->DrawText( MP3Skin.statusx, MP3Skin.statusy, tr("STOPPED"), clrPlayStatusFG, clrPlayStatusBG, pFontStatus, MP3Skin.statusw, sfh, taCenter);
                gtftStatus.status = tr("STOPPED");
	            break;
	        case 2:
                osd->DrawText( MP3Skin.statusx, MP3Skin.statusy, tr("PLAYING"), clrPlayStatusFG, clrPlayStatusBG, pFontStatus, MP3Skin.statusw, sfh, taCenter);
                gtftStatus.status = tr("PLAYING");
                break;
            case 3:
                osd->DrawText( MP3Skin.statusx, MP3Skin.statusy, tr("PAUSED") , clrPlayStatusFG, clrPlayStatusBG, pFontStatus, MP3Skin.statusw, sfh, taCenter);
                gtftStatus.status = tr("PAUSED");
                break;
          }
        }
      }

      // Progressbar      
      if(!player->IsStream()) {
        if(index!=lastIndex || total!=lastTotal) {
          index/=framesPerSecond; total/=framesPerSecond;
          if(total>0) {
            cProgressBar ProgressBar(MP3Skin.progressw, MP3Skin.progressh, index, total, clrProgressbarFG , clrProgressbarBG);
            osd->DrawBitmap(MP3Skin.progressx, MP3Skin.progressy, ProgressBar);
          }
          snprintf(buf, sizeof(buf),"%02d:%02d", index/60, index%60);
          osd->DrawText( MP3Skin.timex, MP3Skin.timey, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.timew, sfh, taLeft);
      
          snprintf(buf, sizeof(buf),"%02d:%02d", total/60, total%60);
          osd->DrawText( MP3Skin.totalx, MP3Skin.totaly, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.totalw, sfh, taRight);
          
          flush=true;
        }
      }  
      else {
        osd->DrawRectangle(MP3Skin.progressx, MP3Skin.progressy, MP3Skin.progressx + MP3Skin.progressw, MP3Skin.progressy + MP3Skin.progressh, clrProgressbarBG);
      }
     
      
      
      // Volumebar
      if ( vol != player->CurrentVolume() || muted != player->Muted()) {
        SetVol();
      }  
      
      // Spectrum Analyzer visualization begins here...
      if ( MP3Setup.EnableVis && ShowSA)	{
	    ShowSpectrumAnalyzer();
      }

      if(flush) {
        Flush();
      }
      	  
      // update graphtft info ...
      if (gtft_exist && gtftChanges) {
        cPluginManager::CallFirstService(GRAPHTFT_STATUS_ID, &gtftStatus);
        d(printf("music-sd: ctrl: status transmission succeeded (gtft)\n"));
      }
  
    }
    skiprew=0;
    skipfwd=0;
    lastIndex=index; lastTotal=total;
    delete lastMode; lastMode=mode;
    refresh=false;
  }
}


void cMP3Control::DisplayInfo(const char *s)
{
  if(osd && !MP3Setup.showcoveronly) {

    if(s)
      osd->DrawText( MP3Skin.titlex, MP3Skin.titley, s, clrListTitle, clrListBG, pFontTitle, MP3Skin.titlew, fh, taLeft);
    else
      osd->DrawText( MP3Skin.titlex, MP3Skin.titley, "", clrListBG, clrListBG, pFontTitle, MP3Skin.titlew, fh, taLeft);

    printf("titlew = %i\n", MP3Skin.titlew);

	//transmit current playing title from playlist to graphtft
    if(gtft_exist) {
      MusicServiceInfo_1_0 info;
      info.info = s;
      cPluginManager::CallFirstService(GRAPHTFT_INFO_ID, &info);
      d(printf("music-sd: player: transmission of info succeeded (gtft)\n"));
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
  
  if(MP3Skin.symrect)
    osd->DrawRectangle( MP3Skin.copyx, MP3Skin.copyy, MP3Skin.copyx +sw, MP3Skin.copyy +sw, clrSymbolActive);
  else
    osd->DrawText( MP3Skin.copyx, MP3Skin.copyy, Icons::OSD_Copy(), clrSymbolActive, clrSymbolBG, pFontSymbol, fh, fh, taLeft);

  Flush();
  asprintf(&buffer, "cp -f \"%s\"  \"%s\"", Songname, MP3Setup.CopyDir);
  copyscript = popen(buffer, "r");
  dsyslog("music-sd: ctrl: executing: %s", buffer);
  free(buffer);
  pclose(copyscript);

  if(MP3Skin.symrect)
    osd->DrawRectangle( MP3Skin.copyx, MP3Skin.copyy, MP3Skin.copyx +sw, MP3Skin.copyy +sw, clrSymbolFG);
  else
    osd->DrawText( MP3Skin.copyx, MP3Skin.copyy, Icons::OSD_Copy(), clrSymbolFG, clrSymbolBG, pFontSymbol, fh, fh, taLeft);

  refresh = true;
}


void cMP3Control::ShutDown(void) {
  std::string shutdowncmd;
  
  shutdowncmd = config;
  shutdowncmd = shutdowncmd + "/" + langdir;
  shutdowncmd = shutdowncmd + "/scripts/music_shutdown.sh &";
  
  isyslog("music-sd: ctrl: player stopped. Initiate shutdown...");
  system(shutdowncmd.c_str());
  isyslog("music-sd: ctrl: Executing '%s'", shutdowncmd.c_str());
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
      esyslog("music-sd: ctrl: ERROR! osd open failed! can't handle areas (%d)-%s\n", result, errormsg);
      if(osd){ delete osd; osd=0;}
      return;
    }
    osd->DrawRectangle(0 , y2, osdwidth -1, y2 + 4*fh, clrBG); //complete  Background
    osd->DrawRectangle(fh/2 , y2 +fh/2, osdwidth -1 -fh/2, y2 + 4*fh -fh/2, clrListBG); //Textfield
    visible=true;
    switch (message) {
          case 1:
              osd->DrawText( 0 +fh, y2 + fh , tr("Be patient..."), clrListTitle, clrListBG, pFontTitle, osdwidth - 2*fh, fh, taCenter);
              osd->DrawText( 0 +fh, y2 + 2*fh , tr("TRACKS become straight scanned"), clrListFG, clrListBG, pFontTitle, osdwidth - 2*fh, fh, taCenter);
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
	            cRemote::CallPlugin("music-sd");
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
	            cRemote::CallPlugin("music-sd");
	            return(osPlugin);
	            }   
              else {
			    if(mgr->maxIndex >0) {
	              ShowHelpButtons(0);
                  player->DeleteTrack(false, mgr->currIndex);
				}  
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
//	            ShowProgress(false);
      	      }
	        }
	        else if(showbuttons==2) {
              ShowHelpButtons(0);
              if(mgr->maxIndex <0) break;
              player->DeleteTrack(true, mgr->currIndex);
              cRemote::CallPlugin("music-sd");
              return(osPlugin);
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
              ShowHelpButtons(0);
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
	        cRemote::CallPlugin("music-sd");
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
              osd->DrawText( MP3Skin.numberx, MP3Skin.numbery, buf, clrInfoFG, clrInfoBG, pFontInfo, MP3Skin.numberw, sfh, taLeft);
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
            Hide();
#if APIVERSNUM >= 10332
	        cRemote::CallPlugin("music-sd");
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
      d(printf("music-sd: player: transmission of help buttons succeeded (gtft)\n"));
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
      d(printf("music-sd: player: transmission succeeded (gtft)\n"));
   }
}
