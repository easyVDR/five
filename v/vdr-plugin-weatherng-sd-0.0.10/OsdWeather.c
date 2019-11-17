#include <vector>
//#include <string>
#include <vdr/plugin.h>
#include <vdr/osd.h>
#include <vdr/config.h>
#include "OsdWeather.h"
#include "parsing.h"
#include "bitmap.h"
#include "vars.h"
#include "skin.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>



#define CELLWIDTH 100

#define TRUE 1
#define FALSE 0
#define BOOL int

static int if_exist_file( const char *filename) {
  FILE *f = fopen( filename, "r" );
  if( f != NULL ) {
        fclose(f);
	return TRUE;
      }
  else
      return FALSE;
}            	


cWetterOsd::cWetterOsd(void)
{
// OSD
  osd           = NULL;

  colordepth = 16;
  areadepth  =  4;
  part       = 0;

  show_wait  = false;
  font = cFont::GetFont(fontSml);

  day           = 1;
  IsSatelite    = false;
  data_file     = "/data1.xml";
  
  showbuttons = 0;
  radar = 0;

}

cWetterOsd::~cWetterOsd(void)
{
  cWeatherBitmap::FlushCache();     
  if (osd)
     delete osd;
}




void cWetterOsd::GetData(void)
{
 FILE *script;
 char *buffer;

 asprintf(&buffer, "%s%s '%s' '%s' '%s'",ScriptDir,"/weatherng-sd.sh", wetterSetup.w_id1, wetterSetup.w_id2, wetterSetup.w_id3);
 
 script = popen(buffer, "r");
 dsyslog("DEBUG : weatherng-sd: Executing GetData :'%s'", buffer);

 free(buffer);
 pclose(script);

 cWeatherBitmap::FlushCache();
}


void cWetterOsd::Satelite(void)
{
  delete osd;
  osd = cOsdProvider::NewOsd(Radar_left ,Radar_top );
  if (!osd){
    esyslog("weatherng-sd: ERROR : (satelite): No OSD avaiable");
    return;
  }

    if(wetterSetup.w_hiquality) {
      areadepth   =   8;
      colordepth  = 255;
      }
    else {
      areadepth  = 4;
      colordepth = 16;
    }           


    tArea Area[] = { {1,1,Radar_width,Radar_height,areadepth}, };

    eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
    if (result == oeOk) {
      osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
      }
    else {
        const char *errormsg = NULL;
	switch (result) {
	    case oeTooManyAreas:
		errormsg = "Too many OSD areas!"; break;
	    case oeTooManyColors:
		errormsg = "Too many colors!"; break;
	    case oeBppNotSupported:
		errormsg = "Depth not suppoerted!"; break;
	    case oeAreasOverlap:
		errormsg = "Areas are overlapped!"; break;
	    case oeWrongAlignment:
		errormsg = "Areas not correctly aligned!"; break;
	    case oeOutOfMemory:
		errormsg = "OSD memory overflow!"; break;
	    case oeUnknown:
		errormsg = "Unknown OSD error!"; break;
    	    default:
		break;
	}	
        esyslog("weatherng-sd: ERROR : %s: (satelite): OSD open failed! Can't handle areas (%d)\n",PLUGIN_NAME_I18N, result);
        if(osd) { delete osd; osd=0; }
	
	return;
    }

    sat_dir = DataDir;
    sat_dir = sat_dir  + Radarmap;
    BOOL ex = if_exist_file(sat_dir.c_str());

    if ( ex ) {
      cWeatherBitmap *bmp;
      if((bmp = cWeatherBitmap::Load(sat_dir.c_str(), wetterSetup.w_alpha, Radar_height -2, Radar_width -2 , colordepth )) != NULL)
        osd->DrawBitmap(2, 2, bmp->Get(), clrTransparent, clrTransparent);

      // debug
      dsyslog("DEBUG : weatherng-sd: SATELITE\n");
#ifdef USE_MAGICK
      dsyslog("DEBUG : weatherng-sd: USE IMAGEMAGICK\n");;
#else
      dsyslog("DEBUG : weatherng-sd: USE IMLIB2\n");;
#endif


      dsyslog("DEBUG : weatherng-sd: USE XPM\n");;
      dsyslog("DEBUG : weatherng-sd: SCRIPTDIR  = %s\n", ScriptDir);
      dsyslog("DEBUG : weatherng-sd: IMAGEDIR   = %s\n", ImageDir);
      dsyslog("DEBUG : weatherng-sd: DATADIR   = %s\n", DataDir);
      dsyslog("DEBUG : weatherng-sd: AREADEPTH   = %i\n", areadepth);
      dsyslog("DEBUG : weatherng-sd: COLORDEPTH = %i\n", colordepth);
      dsyslog("DEBUG : weatherng-sd: RADAR_LEFT = %i\n", Radar_left);
      dsyslog("DEBUG : weatherng-sd: RADAR_TOP  = %i\n", Radar_top);
      dsyslog("DEBUG : weatherng-sd: RADAR_WIDTH  = %i\n", Radar_width);
      dsyslog("DEBUG : weatherng-sd: RADAR_HEIGHT  = %i\n", Radar_height);
      }	
    else {	
      perror(sat_dir.c_str());
      sat_dir = ImageDir;
      sat_dir = sat_dir + "/images/nopic.png";
      sat_dir = ImageDir;
      sat_dir = sat_dir + "/images/nopic.png";

      cWeatherBitmap *bmp;
      if((bmp = cWeatherBitmap::Load(sat_dir.c_str(), wetterSetup.w_alpha, Radar_height -2, Radar_width -2 , colordepth )) != NULL)
        osd->DrawBitmap(2, 2, bmp->Get(), clrTransparent, clrTransparent);
    }

    osd->Flush();

}


/* Status buttons */
void cWetterOsd::ShowStatusButtons(int ShowButtons)
{
  int tab;
  tab = osdwidth/4;
  showbuttons = ShowButtons;
  
  switch(showbuttons) {
    case 0:
        osd->DrawEllipse(14     , osdheight -fh + 8,  26 , osdheight -fh +21, clrStatusRed, 0);
        if(part==0)
          osd->DrawText(       34 , osdheight -fh, tr("Night"), clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
	else  
          osd->DrawText(       34 , osdheight -fh, tr("Day"), clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
        osd->DrawEllipse(tab+8  , osdheight -fh + 8, tab + 20 , osdheight -fh  +21, clrStatusGreen, 0);
        osd->DrawText(  tab + 28, osdheight -fh, tr("Refresh"), clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
        osd->DrawEllipse(2*tab+8, osdheight -fh + 8, 2*tab + 20 , osdheight -fh  +21, clrStatusYellow, 0);
        osd->DrawText( 2*tab +28, osdheight -fh, tr("Radar"), clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
        osd->DrawEllipse(3*tab+8, osdheight -fh + 8, 3*tab + 20 , osdheight -fh  +21, clrStatusBlue, 0);
        osd->DrawText( 3*tab +28, osdheight -fh, tr("More.."), clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
      break;
    case 1:
        osd->DrawEllipse(14     , osdheight -fh + 8,  26 , osdheight -fh +21, clrStatusRed, 0);
        osd->DrawText(       34 , osdheight -fh, wetterSetup.w_id1_name, clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
        osd->DrawEllipse(tab+8  , osdheight -fh + 8, tab + 20 , osdheight -fh  +21, clrStatusGreen, 0);
        osd->DrawText(  tab + 28, osdheight -fh, wetterSetup.w_id2_name, clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
        osd->DrawEllipse(2*tab+8, osdheight -fh + 8, 2*tab + 20 , osdheight -fh  +21, clrStatusYellow, 0);
        osd->DrawText( 2*tab +28, osdheight -fh, wetterSetup.w_id3_name, clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
        osd->DrawEllipse(3*tab+8, osdheight -fh + 8, 3*tab + 20 , osdheight -fh  +21, clrStatusBlue, 0);
        osd->DrawText( 3*tab +28, osdheight -fh, tr("Parent.."), clrStatusFG, clrStatusBG, font, 16*fw, fh, taLeft);
      break;
  }         
    
}


/* Colors */
void cWetterOsd::SetVars(void)
{
  fw = 6;
  fh = 26;

  cw = ( Setup.OSDWidth -   (fh +8 +196 +8));
  cw = cw & ~0x07;

  osdleft   = Setup.OSDLeft + wetterSetup.w_osdoffset_x;
  osdtop    = Setup.OSDTop  + wetterSetup.w_osdoffset_y;
  osdwidth  = Setup.OSDWidth;
  osdheight = Setup.OSDHeight;

  Radar_left   = wetterSetup.w_left;
  Radar_top    = wetterSetup.w_top;
  Radar_width  = wetterSetup.w_width;
  Radar_height = wetterSetup.w_height;



  clrTopBG        = WeatherSkin.clrTopBG;
  clrTopFG        = WeatherSkin.clrTopFG;
  clrBG1          = WeatherSkin.clrBG1;
  clrBG2          = WeatherSkin.clrBG2;
  clrHighFG       = WeatherSkin.clrHighFG;
  clrIconFG       = WeatherSkin.clrIconFG;
  clrTextFG       = WeatherSkin.clrTextFG;
  clrDateFG       = WeatherSkin.clrDateFG;
  clrStatusBG     = WeatherSkin.clrStatusBG;
  clrStatusFG     = WeatherSkin.clrStatusFG;
  clrStatusRed    = WeatherSkin.clrStatusRed;
  clrStatusGreen  = WeatherSkin.clrStatusGreen;
  clrStatusYellow = WeatherSkin.clrStatusYellow;
  clrStatusBlue   = WeatherSkin.clrStatusBlue;
}


/* Display */
void cWetterOsd::Show(void)
{

  SetVars();

  IsSatelite=false;
  dsyslog("DEBUG : weatherng-sd: Part: %i\n",part);


  delete osd;
  osd = cOsdProvider::NewOsd(osdleft ,osdtop);
  if (osd) {
    sat_file = DataDir;
    sat_file = sat_file + data_file;


    if(wetterSetup.w_hiquality) {
      colordepth  = 200;
      areadepth   =   8;

      tArea Area[] = {  {0, 0, osdwidth -1, osdheight -1, areadepth} };


      eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
      if (result == oeOk) {
        osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
	}
      else {
        const char *errormsg = NULL;
	switch (result) {
	    case oeTooManyAreas:
		errormsg = "Too many OSD areas!"; break;
	    case oeTooManyColors:
		errormsg = "Too many colors!"; break;
	    case oeBppNotSupported:
		errormsg = "Depth not suppoerted!"; break;
	    case oeAreasOverlap:
		errormsg = "Areas are overlapped!"; break;
	    case oeWrongAlignment:
		errormsg = "Areas not correctly aligned!"; break;
	    case oeOutOfMemory:
		errormsg = "OSD memory overflow!"; break;
	    case oeUnknown:
		errormsg = "Unknown OSD error!"; break;
    	    default:
		break;
	}	
        esyslog("weatherng-sd: ERROR : %s: OSD open failed! Can't handle areas (%d)\n",PLUGIN_NAME_I18N, result);
        if(osd) { delete osd; osd=0; }
	
	return;
      }
    }
    else {
      colordepth  =  14;
      areadepth   =   4;

      tArea Area[] = {
  		{0, 0, osdwidth -1, 2*fh, 2},
		{0, 2*fh +1, cw -1, 2*fh +8 +196 +8, 2},
		{cw, 2*fh +1, osdwidth -1, 2*fh +8 +196 +8, areadepth},
		{0, 2*fh +8 +196 +9, osdwidth -1, 2*fh +8 +128 +9 +3*fh +4*fh +18 -1, 2},
		{0, 2*fh +8 +128 +9 +3*fh +4*fh +18, osdwidth -1, osdheight -fh -1, 2},
		{0, osdheight -fh, osdwidth -1, osdheight -1, 4},
		};

      eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
      if (result == oeOk) {
        osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
	}
      else {
        const char *errormsg = NULL;
	switch (result) {
	    case oeTooManyAreas:
		errormsg = "Too many OSD areas!"; break;
	    case oeTooManyColors:
		errormsg = "Too many colors!"; break;
	    case oeBppNotSupported:
		errormsg = "Depth not suppoerted!"; break;
	    case oeAreasOverlap:
		errormsg = "Areas are overlapped!"; break;
	    case oeWrongAlignment:
		errormsg = "Areas not correctly aligned!"; break;
	    case oeOutOfMemory:
		errormsg = "OSD memory overflow!"; break;
	    case oeUnknown:
		errormsg = "Unknown OSD error!"; break;
    	    default:
		break;
	}	
        esyslog("weatherng-sd: ERROR : %s: OSD open failed! Can't handle areas (%d)\n",PLUGIN_NAME_I18N, result);
        if(osd) { delete osd; osd=0; }
	
	return;
      }              
    }


// top field ok
    osd->DrawRectangle( 0                , 0     , osdwidth -1 , fh -3, clrTopBG);
    osd->DrawEllipse(   0                , 0     , fh/2        , fh/2 , clrTransparent, -2);
    osd->DrawEllipse(   osdwidth -1 -fh/2, 0     , osdwidth -1 , fh/2 , clrTransparent, -1);
    osd->DrawRectangle( 0                , fh -2 , osdwidth -1 , 2*fh , clrBG1);
    
// infofield ok
    osd->DrawRectangle( 0          , 2*fh +1            , cw -1 , 2*fh +8 +196 +8, clrBG1 );
    osd->DrawRectangle( fh         , 2*fh +1            , cw -1 , 2*fh +8 +196 +8, clrBG2 );
    osd->DrawEllipse(   fh         , 2*fh +1            , fh +10, 2*fh +1 +10    , clrBG1, -2);
    osd->DrawEllipse(   (cw -1) -10, 2*fh +1            , cw -1 , 2*fh +1 +10    , clrBG1, -1);
    osd->DrawEllipse(   fh         , 2*fh +8 +196 +8 -10, fh +10, 2*fh +8 +196 +8, clrBG1, -3);
    osd->DrawEllipse(   (cw -1) -10, 2*fh +8 +196 +8 -10, cw -1 , 2*fh +8 +196 +8, clrBG1, -4);
    
// iconfield ok
    osd->DrawRectangle( cw                   , 2*fh +1            , osdwidth -1      , 2*fh +8 +196 +8, clrBG1);
    osd->DrawRectangle( cw +10               , 2*fh +1            , (osdwidth -1) -fh, 2*fh +8 +196 +8, clrBG2);
    osd->DrawEllipse(   cw +10               , 2*fh +1            , cw +20           , 2*fh +1 +10    , clrBG1, -2);
    osd->DrawEllipse(   (osdwidth -1) -fh -10, 2*fh +1            , (osdwidth -1) -fh, 2*fh +1 +10    , clrBG1, -1);
    osd->DrawEllipse(   cw +10               , 2*fh +8 +196 +8 -10, cw +20           , 2*fh +8 +196 +8, clrBG1, -3);
    osd->DrawEllipse(   (osdwidth -1) -fh -10, 2*fh +8 +196 +8 -10, (osdwidth -1) -fh, 2*fh +8 +196 +8, clrBG1, -4);

// textfield ok
    osd->DrawRectangle( 0,  2*fh +8 +196 +9    , osdwidth -1     , 2*fh +8 +128 +9 +3*fh +4*fh +18 -1, clrBG1 );
    osd->DrawRectangle( fh, 2*fh +8 +196 +9 +10, (osdwidth -1)-fh, 2*fh +8 +128 +9 +3*fh +4*fh +18 -8 -1, clrBG2 );
    osd->DrawEllipse(   fh, 2*fh +8 +196 +9 +10, fh +10          , 2*fh +8 +196 +9 +20      , clrBG1, -2);
    osd->DrawEllipse(   (osdwidth -1) -fh -10, 2*fh +8 +196 +9 +10          , (osdwidth -1) -fh, 2*fh +8 +196 +9 +20      , clrBG1, -1);
    osd->DrawEllipse(   fh                   , 2*fh +8 +128 +9 +3*fh +4*fh +18 -8 -1 -10, fh +10           , 2*fh +8 +128 +9 +3*fh +4*fh +18 -8 -1, clrBG1, -3);
    osd->DrawEllipse(   (osdwidth -1) -fh -10, 2*fh +8 +128 +9 +3*fh +4*fh +18 -8 -1 -10, (osdwidth -1) -fh, 2*fh +8 +128 +9 +3*fh +4*fh +18 -8 -1, clrBG1, -4);

// datefield ok
    osd->DrawRectangle( 0                    , 2*fh +8 +128 +9 +3*fh +4*fh +18           , osdwidth -1        , osdheight -fh -1     , clrBG1);
    osd->DrawRectangle( fh                   , 2*fh +8 +128 +9 +3*fh +4*fh +18           , (osdwidth -1) -fh  , 2*fh +8 +128 +9 +3*fh +4*fh +18 +64 -1 -fh , clrBG2);
    osd->DrawEllipse(   fh                   , 2*fh +8 +128 +9 +3*fh +4*fh +18           , fh + 10            , 2*fh +8 +128 +9 +3*fh +4*fh +28            , clrBG1 , -2);
    osd->DrawEllipse(   (osdwidth -1) -fh -10, 2*fh +8 +128 +9 +3*fh +4*fh +18           , (osdwidth -1) -fh  , 2*fh +8 +128 +9 +3*fh +4*fh +28            , clrBG1 , -1);
    osd->DrawEllipse(   fh                   , 2*fh +8 +128 +9 +3*fh +4*fh +8 +64 -1 -fh , fh +10             , 2*fh +8 +128 +9 +3*fh +4*fh +18 +64 -1 -fh , clrBG1 , -3);
    osd->DrawEllipse(   (osdwidth -1) -fh -10, 2*fh +8 +128 +9 +3*fh +4*fh +8 +64 -1 -fh , (osdwidth -1) -fh  , 2*fh +8 +128 +9 +3*fh +4*fh +18 +64 -1 -fh , clrBG1 , -4);

//bottom ok
    osd->DrawRectangle( 0                 , osdheight -fh , osdwidth -1, osdheight -1, clrStatusBG);
    osd->DrawEllipse(   0                 , osdheight -fh , fh/2       , osdheight -1, clrTransparent, -3);
    osd->DrawEllipse(   osdwidth -1 -fh/2 , osdheight -fh , osdwidth -1, osdheight -1, clrTransparent, -4);


// show the buttons
    ShowStatusButtons(0);


    BOOL ex = if_exist_file(sat_file.c_str());
    if (ex) {
      dsyslog("DEBUG : weatherng-sd: Parse %s !\n", sat_file.c_str());
      cxmlParse parser;
      dsyslog("DEBUG : weatherng-sd: day: %i\n",day);	
      parser.xmlParse((int) day, DataDir, data_file, part);

      if(!show_wait) {
        file = ImageDir;

        if(wetterSetup.w_hiquality)
          file = file  + "/hqimages/";
        else
          file = file  + "/images/";

        file = file + parser.icon.c_str();
        file = file + ".png";

        cWeatherBitmap *bmp;
        if((bmp = cWeatherBitmap::Load(file.c_str(), wetterSetup.w_alpha, 196, 196, colordepth )) != NULL)
          osd->DrawBitmap(  cw +14 , 2*fh +1 +5, bmp->Get(),clrTransparent,clrTransparent);
        }
      else {
	file = ImageDir;

        if(wetterSetup.w_hiquality)
          file = file  + "/hqimages/wait.png";
        else
          file = file  + "/images/wait.png";
	  
        cWeatherBitmap *bmp;
        if((bmp = cWeatherBitmap::Load(file.c_str(), wetterSetup.w_alpha, 196, 196, colordepth )) != NULL)
          osd->DrawBitmap(  cw +14 , 2*fh +1 +5, bmp->Get(),clrTransparent,clrTransparent);
      }


//text
//    osd->DrawRectangle( fh, 2*fh +8 +196 +9 +10, (osdwidth -1)-fh, 2*fh +8 +128 +9 +3*fh +4*fh +18 -8 -1, clrBG2 );

      osd->DrawText(fh +10, 2*fh +1 +5, tr("High temperature: "), clrHighFG,clrBG2,font);
      ausgabe = parser.hi.c_str();
      ausgabe = ausgabe + " ";
      ausgabe = ausgabe + tr(parser.celsius.c_str());
      osd->DrawText(cw -1 -5 -CELLWIDTH, 2*fh +1 +5, ausgabe.c_str(), clrHighFG,clrBG2,font,CELLWIDTH,20,taRight);

      osd->DrawText(fh +10, 2*fh +1 +5 +fh, tr("Low temperature: "), clrDateFG,clrBG2,font);
      ausgabe = parser.low.c_str();
      ausgabe = ausgabe + " ";
      ausgabe = ausgabe + tr(parser.celsius.c_str());
      osd->DrawText(cw -1 -5 -CELLWIDTH, 2*fh +1 +5 +fh, ausgabe.c_str(), clrDateFG,clrBG2,font,CELLWIDTH,20,taRight);

      osd->DrawText(fh +10, 2*fh +1 +5 +3*fh, tr("Wind comes from: "), clrDateFG,clrBG2,font);
      ausgabe = parser.winddir.c_str();
      osd->DrawText(cw -1 -5 -CELLWIDTH , 2*fh +1 +5 +3*fh, ausgabe.c_str(), clrDateFG,clrBG2,font,CELLWIDTH,20,taRight);

      osd->DrawText(fh +10, 2*fh +1 +5 +4*fh, tr("Wind speed: "), clrDateFG,clrBG2,font);
      ausgabe = parser.windspeed.c_str();
      ausgabe = ausgabe + " ";
      ausgabe = ausgabe + parser.speed.c_str();
      osd->DrawText(cw -1 -5 -CELLWIDTH , 2*fh +1 +5 +4*fh, ausgabe.c_str(), clrDateFG,clrBG2,font,CELLWIDTH,20,taRight);

      osd->DrawText(fh +10, 2*fh +1 +5 +5*fh, tr("Humitidy: "), clrDateFG,clrBG2,font);
      ausgabe = parser.humidity.c_str();
      ausgabe = ausgabe + tr(" %");
      osd->DrawText(cw -1 -5 -CELLWIDTH , 2*fh +1 +5 +5*fh, ausgabe.c_str(), clrDateFG,clrBG2,font,CELLWIDTH,20,taRight);

      osd->DrawText(fh +10, 2*fh +1 +5 +6*fh, tr("Precipitation: "), clrDateFG,clrBG2,font);
      ausgabe = parser.raindown.c_str();
      ausgabe = ausgabe + tr(" %");
      osd->DrawText(cw -1 -5 -CELLWIDTH , 2*fh +1 +5 +6*fh, ausgabe.c_str(), clrDateFG,clrBG2,font,CELLWIDTH,20,taRight);


      if (part==0) {
        osd->DrawText( 10, 0, tr("DAY"), clrIconFG, clrTopBG,font,osdwidth -20,20,taCenter);

        ausgabe = tr("On  ");
        ausgabe = ausgabe + tr(parser.dayname.c_str()) + " " + parser.date + "  ";
        ausgabe = ausgabe + tr("in ");
        ausgabe = ausgabe + parser.ort.c_str() + tr(" dayover");

        osd->DrawText( fh +10, 2*fh +8 +196 +9 +16, ausgabe.c_str(), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);

        ausgabe = parser.wetter;
        osd->DrawText( fh +10, 3*fh +8 +196 +9 +16, tr(ausgabe.c_str()), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);

        ausgabe = tr("The temperature varies between  ");
        ausgabe = ausgabe + parser.hi.c_str() + tr("  and  ");
        ausgabe = ausgabe + parser.low.c_str() + tr("  Grad");
        osd->DrawText( fh +10, 4*fh +8 +196 +9 +16, ausgabe.c_str(), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);

        ausgabe = tr("The sun comes up at  ");
        ausgabe = ausgabe + parser.sunrise.c_str() + tr(" and goes at  ");
        ausgabe = ausgabe + parser.sunset.c_str() + tr(" down");
        osd->DrawText( fh +10, 5*fh +8 +196 +9 +16, ausgabe.c_str(), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);
      }

      if (part==1) {
//        osd->DrawText( cw +10 +20, 2*fh +1, tr("NIGHT"), clrIconFG, clrBG2,font,170,20,taCenter);
        osd->DrawText( 10, 0, tr("NIGHT"), clrIconFG, clrTopBG,font,osdwidth -20,20,taCenter);

        ausgabe = tr("On  ");
        ausgabe = ausgabe + tr(parser.dayname.c_str()) + " " + parser.date + "  ";
        ausgabe = ausgabe + tr("in ");
        ausgabe = ausgabe + parser.ort.c_str() + tr(" nightover");
        osd->DrawText( fh +10, 2*fh +8 +196 +9 +16, ausgabe.c_str(), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);

        ausgabe = parser.wetter;
        osd->DrawText( fh +10, 3*fh +8 +196 +9 +16, tr(ausgabe.c_str()), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);

        ausgabe = tr("The temperature varies between  ");
        ausgabe = ausgabe + parser.hi.c_str() + tr("  and  ");
        ausgabe = ausgabe + parser.low.c_str() + tr("  Grad");
        osd->DrawText( fh +10, 4*fh +8 +196 +9 +16, ausgabe.c_str(), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);

        ausgabe = tr("The sun comes up at  ");
        ausgabe = ausgabe + parser.sunrise.c_str() + tr(" and goes at  ");
        ausgabe = ausgabe + parser.sunset.c_str() + tr(" down");
        osd->DrawText( fh +10, 5*fh +8 +196 +9 +16, ausgabe.c_str(), clrTextFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taLeft);
      }


      dsyslog("DEBUG : weatherng-sd: Information about: %s\n",parser.ort.c_str());
      // left Side
      ausgabe = tr("Weather for: ");
      ausgabe = ausgabe + parser.ort.c_str() + "  /  ";
      ausgabe = ausgabe + tr(parser.dayname.c_str());
      ausgabe = ausgabe + "  (" + parser.date + ")";
      osd->DrawText( fh +10, 2*fh +8 +128 +9 +3*fh +4*fh +18 +8, ausgabe.c_str(), clrDateFG, clrBG2,font, (osdwidth -2*fh -15) ,20,taCenter);

    }

  ShowStatusButtons(0);
  osd->Flush();

  }

  // debug
  dsyslog("DEBUG : weatherng-sd: SHOW\n");
#ifdef USE_MAGICK
  dsyslog("DEBUG : weatherng-sd: USE IMAGEMAGICK\n");;
#else
  dsyslog("DEBUG : weatherng-sd: USE IMLIB2\n");;
#endif
  dsyslog("DEBUG : weatherng-sd: AREADEPTH   = %i\n", areadepth);
  dsyslog("DEBUG : weatherng-sd: COLORDEPTH = %i\n", colordepth);
  dsyslog("DEBUG : weatherng-sd: RADAR_LEFT = %i\n", Radar_left);
  dsyslog("DEBUG : weatherng-sd: RADAR_TOP  = %i\n", Radar_top);
  dsyslog("DEBUG : weatherng-sd: RADAR_WIDTH  = %i\n", Radar_width);
  dsyslog("DEBUG : weatherng-sd: RADAR_HEIGHT  = %i\n", Radar_height);
}



void cWetterOsd::ChangeRadar(int Radar)
{
  radar = Radar;
  
  switch (radar) {
	case 0:
	    Radarmap="/pic0";
	    cWetterOsd::Satelite();
	    break;

	case 1:
	    Radarmap="/pic1";
	    cWetterOsd::Satelite();
	    break;

	case 2:
	    Radarmap="/pic2";
	    cWetterOsd::Satelite();
	    break;

	case 3:
	    Radarmap="/pic3";
	    cWetterOsd::Satelite();
	    break;

	case 4:
	    Radarmap="/pic4";
	    cWetterOsd::Satelite();
	    break;

	case 5:
	    Radarmap="/pic5";
	    cWetterOsd::Satelite();
	    break;
		
	case 6:
	    Radarmap="/pic6";
	    cWetterOsd::Satelite();
	    break;

	case 7:
	    Radarmap="/pic7";
	    cWetterOsd::Satelite();
	    break;

	case 8:
	    Radarmap="/pic8";
	    cWetterOsd::Satelite();
	    break;

	case 9:
	    Radarmap="/pic9";
	    cWetterOsd::Satelite();
	    break;		
  }
}


eOSState cWetterOsd::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
//     switch (Key & ~k_Repeat) {
     switch (Key) {
       case kLeft: 
                        if(IsSatelite) {
			    if(radar==0) {
			      ChangeRadar(9);
			      }
			    else if(radar==1) {
			      ChangeRadar(0);
			      }  
			    else if(radar==2) {
			      ChangeRadar(1);
			      }  
			    else if(radar==3) {
			      ChangeRadar(2);
			      }  
			    else if(radar==4) {
			      ChangeRadar(3);
			      }  
			    else if(radar==5) {
			      ChangeRadar(4);
			      }				
			    else if(radar==6) {
			      ChangeRadar(5);
			      }  
			    else if(radar==7) {
			      ChangeRadar(6);
			      }  
			    else if(radar==8) {
			      ChangeRadar(7);
			      }  
			    else if(radar==9) {
			      ChangeRadar(8);				  
			      }  
			  }
			else {  
			    day--;
			    if (day<1){
			      day=1;
			    }
			    cWetterOsd::Show();  
			}    
		        break;
       case kRight: 
                        if(IsSatelite) {
			    if(radar==0) {
			      ChangeRadar(1);
			      }
			    else if(radar==1) {
			      ChangeRadar(2);
			      }  
			    else if(radar==2) {
			      ChangeRadar(3);
			      }  
			    else if(radar==3) {
			      ChangeRadar(4);
			      }  
			    else if(radar==4) {
			      ChangeRadar(5);
			      }  
			    else if(radar==5) {
			      ChangeRadar(6);
			      }
			    else if(radar==6) {
			      ChangeRadar(7);
			      }  
			    else if(radar==7) {
			      ChangeRadar(8);
			      }  
			    else if(radar==8) {
			      ChangeRadar(9);
			      }  
			    else if(radar==9) {				
			      ChangeRadar(0);
			      }  
			  }
                        else {
                	    day++;
              		    if (day>9){
                	      day=9;
                	    }
			    cWetterOsd::Show();
			}    
		        break;
       case kBack:
		        if(IsSatelite==true){
                            ShowStatusButtons(0);
			    cWetterOsd::Show();
		            }
		        else{
		            return osEnd;
		        }
		        break;

	case kRed:
                        if(showbuttons==0 && !IsSatelite) {
                            if(part==0) {
                	        part++;
                	        if (part>1){
                	          part=1;
                	        }
                              }
                            else if(part==1) {
			        part--;
                	        if (part<0){
                	          part=0;
                	        }
                            }
                	    cWetterOsd::Show();
                          }
                        else if(showbuttons==1) {
		            if(IsSatelite==true){ IsSatelite = false; }
			    data_file  = "/data1.xml";
			    cWetterOsd::Show();
			}
		        break;
	case kGreen:
                        if(showbuttons==0 && !IsSatelite) {
		            if(IsSatelite==true) {
        		      IsSatelite = false; 
			      cWetterOsd::Show();
		              }
		            else {
                              show_wait = true;
			      cWetterOsd::Show();

			      cWetterOsd::GetData();
                              show_wait = false;
			      cWetterOsd::Show();
		            }
                          }
                        else if(showbuttons==1) {
		            if(IsSatelite==true){IsSatelite = false; }
			    data_file  = "/data2.xml";
			    cWetterOsd::Show();
			  }
		        break;
	case kYellow:
                        if(showbuttons==0 && !IsSatelite) {
			    IsSatelite=true;
		            ChangeRadar(0);
                          }
                        else if(showbuttons==1) {
		            if(IsSatelite==true){ IsSatelite =false; }
			    data_file  = "/data3.xml";
			    cWetterOsd::Show();
			  }
		        break;
	case kBlue:
                        if(showbuttons==0 && !IsSatelite) {
			    ShowStatusButtons(1);
                            osd->Flush();
                          }
			else if(showbuttons==1) {
			    ShowStatusButtons(0);
                            osd->Flush();
			}    
 		        break;
	default: return state;
       }
     state = osContinue;
     }
  return state;
}
