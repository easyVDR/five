#include <vector>
//#include <string>
#include <vdr/plugin.h>
#include <vdr/osd.h>
#include <vdr/config.h>
#include "OsdWeather.h"
//#include "parsing.h"
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
  osd            = NULL;
  part           = 0;

  show_wait      = false;

  day            = 1;
  IsSatelite     = false;
  data_file      = "/data1.xml";
  DataDir        = "";
  Theme          = "";
  BigIconTheme   = "";
  SmallIconTheme = "";

  showbuttons    = 0;
  radar          = 0;

  BgrdPixmap = NULL;
  PlaystatusPixmap = NULL;
  Icon1Pixmap = NULL;
  Icon2Pixmap = NULL;
  Icon3Pixmap = NULL;
  StatusPixmap = NULL;
  HighTempPixmap = NULL;
  LowTempPixmap = NULL;
  FeelTempPixmap = NULL;
  WindPixmap = NULL;
  UVPixmap = NULL;
  RainPixmap = NULL;
  ThunderPixmap = NULL;
  DatePixmap = NULL;
  ButtonPixmap = NULL;
  SecondDayPixmap = NULL;
  ThirdDayPixmap = NULL;
  RadarPixmap = NULL;
  
  SetFonts();


}

cWetterOsd::~cWetterOsd(void)
{
  BgrdPixmap = NULL;
  PlaystatusPixmap = NULL;
  Icon1Pixmap = NULL;
  Icon2Pixmap = NULL;
  Icon3Pixmap = NULL;
  StatusPixmap = NULL;
  HighTempPixmap = NULL;
  LowTempPixmap = NULL;
  FeelTempPixmap = NULL;
  WindPixmap = NULL;
  UVPixmap = NULL;
  RainPixmap = NULL;
  ThunderPixmap = NULL;
  DatePixmap = NULL;
  ButtonPixmap = NULL;
  SecondDayPixmap = NULL;
  ThirdDayPixmap = NULL;
  RadarPixmap = NULL;


//  cImageloadBitmap::FlushCache();
  if (osd)
     delete osd;
}

bool cWetterOsd::BuildAreas(void)
{
#ifdef BUGHUNT
  printf("build areas\n");
#endif

tArea Area[] = { { 0, 0, osdwidth -1,osdheight -1 , 32} };
  
  
// tArea Area[] = { { 0, 0, 10,76, 16} };
 
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
            errormsg = "weather: Too many OSD areas"; break;
          case oeTooManyColors:
            errormsg = "weather: Too many colors"; break;
          case oeBppNotSupported:
            errormsg = "weather: Depth not supported"; break;	
          case oeAreasOverlap:
            errormsg = "weather: Areas are overlapped"; break;
          case oeWrongAlignment:
            errormsg = "weather: Areas not correctly aligned"; break;
          case oeOutOfMemory:
            errormsg = "weather: OSD memory overflow"; break;
          case oeUnknown:
            errormsg = "weather: Unknown OSD error"; break;
          default:
            break;
        }
      esyslog("weather: ERROR! osd open failed! can't handle areas (%d)-%s\n", result, errormsg);
      if(osd){ delete osd; osd=0;}

      return false;
  }
}

void cWetterOsd::LoadBackground(void)
{

  std::string background;

  background = config_path;
  background = background + "/moronsuite/weather/themes/" + Theme + "/images/" + WeatherSkin.OsdBgrd;
//  background = background + Theme;
//  background = background + "/images/";
//  background = background + WeatherSkin.OsdBgrd;

//  printf("LOADING BACKGROUND\n");
  cImageloadBitmap osdbitmap;
  if((osdbitmap.Load(background.c_str()))) {
    osdbitmap.LoadImage(background.c_str(), osdwidth , osdheight , false);
    cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
    osdbitmap.Render(img);
    BgrdPixmap->DrawImage(cPoint(0,0), img);
  }
//  printf("BACKGROUND LOADED\n");
}


void cWetterOsd::BuildDefault(void)
{
#ifdef BUGHUNT
  printf("Build default\n");
#endif

  BgrdPixmap = NULL;
  PlaystatusPixmap = NULL;
  Icon1Pixmap = NULL;
  Icon2Pixmap = NULL;
  Icon3Pixmap = NULL;
  StatusPixmap = NULL;
  HighTempPixmap = NULL;
  LowTempPixmap = NULL;
  FeelTempPixmap = NULL;
  WindPixmap = NULL;
  UVPixmap = NULL;
  RainPixmap = NULL;
  ThunderPixmap = NULL;
  DatePixmap = NULL;
  ButtonPixmap = NULL;
  SecondDayPixmap = NULL;
  ThirdDayPixmap = NULL;
  RadarPixmap = NULL;


  // load the backgroundpicture , Pixmap will be created there
  if(!BgrdPixmap) {
#ifdef BUGHUNT
    printf("BgrdPixmap not exists: Creating now\n");
#endif
    BgrdPixmap = osd->CreatePixmap(WeatherSkin.Bgrd_Layer, cRect(WeatherSkin.Bgrd_Left, WeatherSkin.Bgrd_Top, WeatherSkin.Bgrd_Width, WeatherSkin.Bgrd_Height) );
  }
  
#ifdef BUGHUNT
  printf("Load Background\n");
#endif
  LoadBackground();

  // Create Pixmap for Icon1 
  if(!Icon1Pixmap) {
#ifdef BUGHUNT
    printf("Icon1Pixmap not exists: Creating now\n");
#endif
    Icon1Pixmap = osd->CreatePixmap(WeatherSkin.Icon1_Layer, cRect(WeatherSkin.Icon1_Left, WeatherSkin.Icon1_Top, WeatherSkin.Icon1_Width, WeatherSkin.Icon1_Height) );
    Icon1Pixmap->Clear();
  }

  // Create Pixmap for Icon2 
  if(!Icon2Pixmap) {
#ifdef BUGHUNT
    printf("Icon2Pixmap not exists: Creating now\n");
#endif
    Icon2Pixmap = osd->CreatePixmap(WeatherSkin.Icon2_Layer, cRect(WeatherSkin.Icon2_Left, WeatherSkin.Icon2_Top, WeatherSkin.Icon2_Width, WeatherSkin.Icon2_Height) );
    Icon2Pixmap->Clear();
  }

  // Create Pixmap for Icon3 
  if(!Icon3Pixmap) {
#ifdef BUGHUNT
    printf("Icon3Pixmap not exists: Creating now\n");
#endif
    Icon3Pixmap = osd->CreatePixmap(WeatherSkin.Icon3_Layer, cRect(WeatherSkin.Icon3_Left, WeatherSkin.Icon3_Top, WeatherSkin.Icon3_Width, WeatherSkin.Icon3_Height) );
    Icon3Pixmap->Clear();
  }

  // Create Pixmap for Status
  if(!StatusPixmap) {
#ifdef BUGHUNT
    printf("StatusPixmap not exists: Creating now\n");
#endif
    StatusPixmap = osd->CreatePixmap(WeatherSkin.Status_Layer, cRect(WeatherSkin.Status_Left, WeatherSkin.Status_Top, WeatherSkin.Status_Width, WeatherSkin.Status_Height) );
//    StatusPixmap->Clear();
  }

  // Create Pixmap for HighTemp
  if(!HighTempPixmap) {
#ifdef BUGHUNT
    printf("HighTempPixmap not exists: Creating now\n");
#endif
    HighTempPixmap = osd->CreatePixmap(WeatherSkin.HighTemp_Layer, cRect(WeatherSkin.HighTemp_Left, WeatherSkin.HighTemp_Top, WeatherSkin.HighTemp_Width, WeatherSkin.HighTemp_Height) );
//    HighTempPixmap->Clear();
  }

  // Create Pixmap for LowTemp
  if(!LowTempPixmap) {
#ifdef BUGHUNT
    printf("LowTempPixmap not exists: Creating now\n");
#endif
    LowTempPixmap = osd->CreatePixmap(WeatherSkin.LowTemp_Layer, cRect(WeatherSkin.LowTemp_Left, WeatherSkin.LowTemp_Top, WeatherSkin.LowTemp_Width, WeatherSkin.LowTemp_Height) );
//    LowTempPixmap->Clear();
  }

  // Create Pixmap for FeelTemp
  if(!FeelTempPixmap) {
#ifdef BUGHUNT
    printf("FeelTempPixmap not exists: Creating now\n");
#endif
    FeelTempPixmap = osd->CreatePixmap(WeatherSkin.FeelTemp_Layer, cRect(WeatherSkin.FeelTemp_Left, WeatherSkin.FeelTemp_Top, WeatherSkin.FeelTemp_Width, WeatherSkin.FeelTemp_Height) );
//    FeelTempPixmap->Clear();
  }

  // Create Pixmap for Wind
  if(!WindPixmap) {
#ifdef BUGHUNT
    printf("WindPixmap not exists: Creating now\n");
#endif
    WindPixmap = osd->CreatePixmap(WeatherSkin.Wind_Layer, cRect(WeatherSkin.Wind_Left, WeatherSkin.Wind_Top, WeatherSkin.Wind_Width, WeatherSkin.Wind_Height) );
//    WindPixmap->Clear();
  }

  // Create Pixmap for UV
  if(!UVPixmap) {
#ifdef BUGHUNT
    printf("UVPixmap not exists: Creating now\n");
#endif
    UVPixmap = osd->CreatePixmap(WeatherSkin.UV_Layer, cRect(WeatherSkin.UV_Left, WeatherSkin.UV_Top, WeatherSkin.UV_Width, WeatherSkin.UV_Height) );
//    UVPixmap->Clear();
  }

  // Create Pixmap for Rain
  if(!RainPixmap) {
#ifdef BUGHUNT
    printf("RainPixmap not exists: Creating now\n");
#endif
    RainPixmap = osd->CreatePixmap(WeatherSkin.Rain_Layer, cRect(WeatherSkin.Rain_Left, WeatherSkin.Rain_Top, WeatherSkin.Rain_Width, WeatherSkin.Rain_Height) );
//    RainPixmap->Clear();
  }

  // Create Pixmap for Thunder
  if(!ThunderPixmap) {
#ifdef BUGHUNT
    printf("ThunderPixmap not exists: Creating now\n");
#endif
    ThunderPixmap = osd->CreatePixmap(WeatherSkin.Thunder_Layer, cRect(WeatherSkin.Thunder_Left, WeatherSkin.Thunder_Top, WeatherSkin.Thunder_Width, WeatherSkin.Thunder_Height) );
//    ThunderPixmap->Clear();
  }

  // Create Pixmap for Date
  if(!DatePixmap) {
#ifdef BUGHUNT
    printf("DatePixmap not exists: Creating now\n");
#endif
    DatePixmap = osd->CreatePixmap(WeatherSkin.Date_Layer, cRect(WeatherSkin.Date_Left, WeatherSkin.Date_Top, WeatherSkin.Date_Width, WeatherSkin.Date_Height) );
//    DatePixmap->Clear();
  }

  // Create Pixmap for Button
  if(!ButtonPixmap) {
#ifdef BUGHUNT
    printf("ButtonPixmap not exists: Creating now\n");
#endif
    ButtonPixmap = osd->CreatePixmap(WeatherSkin.Button_Layer, cRect(WeatherSkin.Button_Left, WeatherSkin.Button_Top, WeatherSkin.Button_Width, WeatherSkin.Button_Height) );
//    ButtonPixmap->Clear();
  }

  // Create Pixmap for SecondDay
  if(!SecondDayPixmap) {
#ifdef BUGHUNT
    printf("SecondDayPixmap not exists: Creating now\n");
#endif
    SecondDayPixmap = osd->CreatePixmap(WeatherSkin.SecondDay_Layer, cRect(WeatherSkin.SecondDay_Left, WeatherSkin.SecondDay_Top, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_Height) );
//    SecondDayPixmap->Clear();
  }

  // Create Pixmap for ThirdDay
  if(!ThirdDayPixmap) {
#ifdef BUGHUNT
    printf("ThirdDayPixmap not exists: Creating now\n");
#endif
    ThirdDayPixmap = osd->CreatePixmap(WeatherSkin.ThirdDay_Layer, cRect(WeatherSkin.ThirdDay_Left, WeatherSkin.ThirdDay_Top, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_Height) );
//    ThirdDayPixmap->Clear();
  }

  // Create Pixmap for Radar
  if(!RadarPixmap) {
#ifdef BUGHUNT
    printf("RadarPixmap not exists: Creating now\n");
#endif
    RadarPixmap = osd->CreatePixmap(WeatherSkin.Radar_Layer, cRect(WeatherSkin.Radar_Left, WeatherSkin.Radar_Top, WeatherSkin.Radar_Width, WeatherSkin.Radar_Height) );
//    RadarPixmap->Clear();
  }



#ifdef BUGHUNT
  printf("Default built\n");
#endif
}


void cWetterOsd::GetData(void)
{

 char *buffer;

 std::string PluginPath;
 
 PluginPath = config_path;
 PluginPath = PluginPath + "/moronsuite/weather";


 asprintf(&buffer, "%s%s '%s' '%s' '%s' '%s'",config_path,"/moronsuite/weather/scripts/weather.sh", PluginPath.c_str(), wetterSetup.w_id1, wetterSetup.w_id2, wetterSetup.w_id3);
 
 system(buffer);
 dsyslog("DEBUG : weather: Executing GetData :'%s'", buffer);

 free(buffer);
}


void cWetterOsd::Satelite(void)
{
//  delete osd;
//  osd = cOsdProvider::NewOsd(osdleft ,osdtop );
  if (!osd){
    esyslog("weather: ERROR : (satelite): No OSD avaiable");
    return;
  }

//  BuildAreas();

//  RadarPixmap = NULL;
  
  // Create Pixmap for Radar
//  if(!RadarPixmap) {
//#ifdef BUGHUNT
//    printf("RadarPixmap not exists: Creating now\n");
//#endif
//    RadarPixmap = osd->CreatePixmap(WeatherSkin.Radar_Layer, cRect(WeatherSkin.Radar_Left, WeatherSkin.Radar_Top, WeatherSkin.Radar_Width, WeatherSkin.Radar_Height) );
//    RadarPixmap->Clear();
//  }

  sat_dir = config_path;
  sat_dir = sat_dir + "/moronsuite/weather/downloads";
  sat_dir = sat_dir  + Radarmap;
  BOOL ex = if_exist_file(sat_dir.c_str());

  if ( ex ) {
    cImageloadBitmap osdbitmap;
    if( osdbitmap.Load(sat_dir.c_str()) ) {
      osdbitmap.LoadImage(sat_dir.c_str(), WeatherSkin.Radar_Width , WeatherSkin.Radar_Height , false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      RadarPixmap->DrawImage(cPoint(0,0), img);
      }
    }
  else {
    perror(sat_dir.c_str());
    sat_dir = config_path;
    sat_dir = sat_dir + "/moronsuite/weather/themes/";
    sat_dir = sat_dir + Theme;
    sat_dir = sat_dir + "/images/noradar.png";
    cImageloadBitmap osdbitmap;
    if( osdbitmap.Load(sat_dir.c_str()) ) {
      osdbitmap.LoadImage(sat_dir.c_str(), WeatherSkin.Radar_Width , WeatherSkin.Radar_Height , false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      RadarPixmap->DrawImage(cPoint(0,0), img);
    }
  }

  osd->Flush();
}

/* buttons */
void cWetterOsd::ShowStatusButtons(int ShowButtons)
{
  showbuttons = ShowButtons;

  if (ButtonPixmap) ButtonPixmap->Clear();

  switch(showbuttons) {
    case 0:
        if(part==0)
          ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText1posx , WeatherSkin.ButtonText1posy) , tr("Night"), clrStatusRedFG, clrTextBG, pFontButton, WeatherSkin.Button1w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
      	else  
          ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText1posx , WeatherSkin.ButtonText1posy) , tr("Day"), clrStatusRedFG, clrTextBG, pFontButton, WeatherSkin.Button1w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);

          ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText2posx , WeatherSkin.ButtonText2posy) , tr("Refresh"), clrStatusGreenFG,  clrTextBG, pFontButton, WeatherSkin.Button2w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
          ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText3posx , WeatherSkin.ButtonText3posy) , tr("Radar"),   clrStatusYellowFG, clrTextBG, pFontButton, WeatherSkin.Button3w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
          ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText4posx , WeatherSkin.ButtonText4posy) , tr("More.."),  clrStatusBlueFG,  clrTextBG, pFontButton, WeatherSkin.Button4w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
        break;
    case 1:

        ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText1posx , WeatherSkin.ButtonText1posy) , wetterSetup.w_id1_name,  clrStatusRedFG, clrTextBG, pFontButton, WeatherSkin.Button1w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
        ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText2posx , WeatherSkin.ButtonText2posy) , wetterSetup.w_id2_name,  clrStatusGreenFG,  clrTextBG, pFontButton, WeatherSkin.Button2w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
        ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText3posx , WeatherSkin.ButtonText3posy) , wetterSetup.w_id3_name,  clrStatusYellowFG, clrTextBG, pFontButton, WeatherSkin.Button3w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
        ButtonPixmap->DrawText( cPoint(WeatherSkin.ButtonText4posx , WeatherSkin.ButtonText4posy) , tr("Parent.."),          clrStatusBlueFG,  clrTextBG, pFontButton, WeatherSkin.Button4w, WeatherSkin.Button_Height, WeatherSkin.Button_Align);
      break;
  }         
    
}


/* Colors */
void cWetterOsd::SetVars(void)
{
  fh  = WeatherSkin.FontHeight;
  sfh = WeatherSkin.SmallFontHeight;
  
  osdheight          = WeatherSkin.OsdHeight;
  osdwidth           = WeatherSkin.OsdWidth;
  osdtop             = WeatherSkin.OsdTop;
  osdleft            = WeatherSkin.OsdLeft;

  Theme              = WeatherSkin.Theme;
  BigIconTheme       = WeatherSkin.BigIconTheme;
  SmallIconTheme     = WeatherSkin.SmallIconTheme;

  clrStatusFG        = WeatherSkin.clrStatusFG;
  clrTextBG          = WeatherSkin.clrTextBG;
  clrTextFG          = WeatherSkin.clrTextFG;
  clrHighTempFG      = WeatherSkin.clrHighTempFG;
  clrLowTempFG       = WeatherSkin.clrLowTempFG;
  clrFeelTempFG      = WeatherSkin.clrFeelTempFG;
  clrWindFG          = WeatherSkin.clrWindFG;
  clrUVFG            = WeatherSkin.clrUVFG;
  clrRainFG          = WeatherSkin.clrRainFG;
  clrThunderFG       = WeatherSkin.clrThunderFG;
  clrDateFG          = WeatherSkin.clrDateFG;
  clrStatusRedFG     = WeatherSkin.clrStatusRedFG;
  clrStatusGreenFG   = WeatherSkin.clrStatusGreenFG;
  clrStatusYellowFG  = WeatherSkin.clrStatusYellowFG;
  clrStatusBlueFG    = WeatherSkin.clrStatusBlueFG;
}

void cWetterOsd::SetFonts(void)
{
  if (WeatherSkin.ReloadFonts) {
    if(pFontStatus)        delete pFontStatus;
    if(pFontHighTemp)      delete pFontHighTemp;
    if(pFontLowTemp)       delete pFontLowTemp;
    if(pFontFeelTemp)      delete pFontFeelTemp;
    if(pFontWind)          delete pFontWind;
    if(pFontUV)            delete pFontUV;
    if(pFontRain)          delete pFontRain;
    if(pFontThunder)       delete pFontThunder;
    if(pFontDate)          delete pFontDate;
    if(pFontButton)        delete pFontButton;
    if(pFontSecondDay)     delete pFontSecondDay;
    if(pFontThirdDay)      delete pFontThirdDay;

    WeatherSkin.ReloadFonts=0;
  }
  pFontStatus     = cFont::CreateFont(WeatherSkin.FONTSTATUSNAME.c_str()   , WeatherSkin.FONTSTATUSSIZE   , 0);
  pFontHighTemp   = cFont::CreateFont(WeatherSkin.FONTHIGHTEMPNAME.c_str() , WeatherSkin.FONTHIGHTEMPSIZE , 0);
  pFontLowTemp    = cFont::CreateFont(WeatherSkin.FONTLOWTEMPNAME.c_str()  , WeatherSkin.FONTLOWTEMPSIZE  , 0);
  pFontFeelTemp   = cFont::CreateFont(WeatherSkin.FONTFEELTEMPNAME.c_str() , WeatherSkin.FONTFEELTEMPSIZE , 0);
  pFontWind       = cFont::CreateFont(WeatherSkin.FONTWINDNAME.c_str()     , WeatherSkin.FONTWINDSIZE     , 0);
  pFontUV         = cFont::CreateFont(WeatherSkin.FONTUVNAME.c_str()       , WeatherSkin.FONTUVSIZE       , 0);
  pFontRain       = cFont::CreateFont(WeatherSkin.FONTRAINNAME.c_str()     , WeatherSkin.FONTRAINSIZE     , 0);
  pFontThunder    = cFont::CreateFont(WeatherSkin.FONTTHUNDERNAME.c_str()  , WeatherSkin.FONTTHUNDERSIZE  , 0);
  pFontDate       = cFont::CreateFont(WeatherSkin.FONTDATENAME.c_str()     , WeatherSkin.FONTDATESIZE     , 0);
  pFontText       = cFont::CreateFont(WeatherSkin.FONTTEXTNAME.c_str()     , WeatherSkin.FONTTEXTSIZE     , 0);
  pFontButton     = cFont::CreateFont(WeatherSkin.FONTBUTTONNAME.c_str()   , WeatherSkin.FONTBUTTONSIZE   , 0);
  pFontSecondDay  = cFont::CreateFont(WeatherSkin.FONTSECONDDAYNAME.c_str(), WeatherSkin.FONTSECONDDAYSIZE, 0);
  pFontThirdDay   = cFont::CreateFont(WeatherSkin.FONTTHIRDDAYNAME.c_str() , WeatherSkin.FONTTHIRDDAYSIZE , 0);

}


void cWetterOsd::ClearPixmaps(void)
{
  if(RadarPixmap)     RadarPixmap->Clear();
  if(StatusPixmap)    StatusPixmap->Clear();
  if(HighTempPixmap)  HighTempPixmap->Clear();
  if(LowTempPixmap)   LowTempPixmap->Clear();
  if(FeelTempPixmap)  FeelTempPixmap->Clear();
  if(WindPixmap)      WindPixmap->Clear();
  if(UVPixmap)        UVPixmap->Clear();
  if(RainPixmap)      RainPixmap->Clear();
  if(ThunderPixmap)   ThunderPixmap->Clear();
  if(DatePixmap)      DatePixmap->Clear();
//  if(ButtonPixmap)    ButtonPixmap->Clear();
  if(SecondDayPixmap) SecondDayPixmap->Clear();
  if(ThirdDayPixmap)  ThirdDayPixmap->Clear();
}



/* Display */
void cWetterOsd::Show(void)
{
#ifdef BUGHUNT
  printf("show Show\n");
#endif
  SetVars();

  IsSatelite=false;
//  day = 1;

  if (!osd) {
    osd = cOsdProvider::NewOsd(osdleft ,osdtop);
    if (!BuildAreas()) return;
    BuildDefault();
//    ShowStatusButtons(0);
  }


  if (osd) {

    sat_file = config_path;
    sat_file = sat_file + "/moronsuite/weather/downloads";
    sat_file = sat_file + data_file;

    ClearPixmaps();

    if(WeatherSkin.ReloadFonts) SetFonts();


    BOOL ex = if_exist_file(sat_file.c_str());
    if (ex) {
#ifdef DEBUG_WEATHER_SYSTEM
      printf("DEBUG : weather: Parse %s !\n", sat_file.c_str());
#endif
//      cxmlParse parser;
#ifdef DEBUG_WEATHER_SYSTEM
      printf("DEBUG : weather: day : %i\n",day);	
      printf("DEBUG : weather: part: %i\n",part);	
     
#endif

      DataDir = config_path;
      DataDir = DataDir + "/moronsuite/weather/downloads"; 
      parser.xmlParse((int) day, DataDir, data_file, part);

      if(!show_wait) {
        file = config_path;
        file = file + "/moronsuite/weather/themes/icons/" + BigIconTheme + parser.icon + ".png";

        cImageloadBitmap osdbitmap;
        if( osdbitmap.Load(file.c_str()) ) {
          osdbitmap.LoadImage(file.c_str(), WeatherSkin.Icon1_Width , WeatherSkin.Icon1_Height , false);
          cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
          osdbitmap.Render(img);
          Icon1Pixmap->DrawImage(cPoint(0,0), img);
          }
        }
      else {
        file = config_path;
        file = file + "/moronsuite/weather/themes/";
        file = file + Theme;
        file = file + "/images/wait_big.png";
        cImageloadBitmap osdbitmap;
        if( osdbitmap.Load(file.c_str()) ) {
          osdbitmap.LoadImage(file.c_str(), WeatherSkin.Icon1_Width , WeatherSkin.Icon1_Height , false);
          cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
          osdbitmap.Render(img);
          Icon1Pixmap->DrawImage(cPoint(0,0), img);
        }
      }


//text

      ausgabe = tr("High temperature:");
      ausgabe = ausgabe + " " + parser.hi.c_str();
      ausgabe = ausgabe + " ";
      ausgabe = ausgabe + tr(parser.celsius.c_str());
      HighTempPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrHighTempFG, clrTextBG, pFontHighTemp, WeatherSkin.HighTemp_Width, WeatherSkin.HighTemp_Height, WeatherSkin.HighTemp_Align);


      ausgabe = tr("Low temperature:");
      ausgabe = ausgabe + " " + parser.low.c_str();
      ausgabe = ausgabe + " ";
      ausgabe = ausgabe + tr(parser.celsius.c_str());
      LowTempPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrLowTempFG, clrTextBG, pFontLowTemp, WeatherSkin.LowTemp_Width, WeatherSkin.LowTemp_Height, WeatherSkin.LowTemp_Align);


      ausgabe = tr("Wind-Chill-Index:");
      ausgabe = ausgabe + " " + parser.feellow.c_str();
      ausgabe = ausgabe + " - ";
      ausgabe = ausgabe + parser.feelhi.c_str();
      ausgabe = ausgabe + " ";
      ausgabe = ausgabe + tr(parser.celsius.c_str());
      FeelTempPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrFeelTempFG, clrTextBG, pFontFeelTemp, WeatherSkin.FeelTemp_Width, WeatherSkin.FeelTemp_Height, WeatherSkin.FeelTemp_Align);


      ausgabe = tr("Wind:");
      ausgabe = ausgabe + " " + tr("from ");
      ausgabe = ausgabe + parser.winddir.c_str();
      ausgabe = ausgabe + tr(" with ");
      ausgabe = ausgabe + parser.windspeed.c_str();
      ausgabe = ausgabe + " km/h";
      WindPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrWindFG, clrTextBG, pFontWind, WeatherSkin.Wind_Width, WeatherSkin.Wind_Height, WeatherSkin.Wind_Align);

      ausgabe = tr("Max. UV Index:");
      ausgabe = ausgabe + " " + parser.maxuv.c_str();
      UVPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrUVFG, clrTextBG, pFontUV, WeatherSkin.UV_Width, WeatherSkin.UV_Height, WeatherSkin.UV_Align);



      ausgabe = tr("Amount of Precipitation:");
      ausgabe = ausgabe + " " + parser.precipamount.c_str();
      ausgabe = ausgabe + tr(" mm/qm");
      RainPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrRainFG, clrTextBG, pFontRain, WeatherSkin.Rain_Width, WeatherSkin.Rain_Height, WeatherSkin.Rain_Align);

      
      ausgabe = tr("Tstorm probability:");
      ausgabe = ausgabe + " " + parser.tstormprob.c_str();
      ausgabe = ausgabe + " %";
      ThunderPixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrThunderFG, clrTextBG, pFontThunder, WeatherSkin.Thunder_Width, WeatherSkin.Thunder_Height, WeatherSkin.Thunder_Align);

/*      
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
*/

/*
      if (part==1) {
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
*/

      dsyslog("DEBUG : weather: Information about: %s\n",parser.ort.c_str());
      // left Side
//      ausgabe = tr("Weather for: ");
      ausgabe = parser.ort.c_str();
      ausgabe = ausgabe + "  -  ";
      ausgabe = ausgabe + tr(parser.dayname.c_str());
      ausgabe = ausgabe + "  (" + parser.date + ")";
      DatePixmap->DrawText(cPoint(0,0), ausgabe.c_str(), clrDateFG, clrTextBG, pFontDate, WeatherSkin.Date_Width, WeatherSkin.Date_Height, WeatherSkin.Date_Align);


      ShowSecondDay(day +1);
      ShowThirdDay(day +2);

    }



  ShowStatusButtons(0);
  osd->Flush();

  }

  // debug
#ifdef DEBUG_WEATHER_OSD  
  dsyslog("DEBUG : weather: SHOW\n");
//  dsyslog("DEBUG : weather: RADAR_LEFT = %i\n", Radar_left);
//  dsyslog("DEBUG : weather: RADAR_TOP  = %i\n", Radar_top);
//  dsyslog("DEBUG : weather: RADAR_WIDTH  = %i\n", Radar_width);
//  dsyslog("DEBUG : weather: RADAR_HEIGHT  = %i\n", Radar_height);
#endif

}




/* Display 2. day */
void cWetterOsd::ShowSecondDay(int day2)
{
#ifdef BUGHUNT
  printf("show ShowSecondDay\n");
#endif

#ifdef DEBUG_WEATHER_SYSTEM
  printf("DEBUG : weather: day : %i\n",day2);
  printf("DEBUG : weather: part: %i\n",part);
#endif

  parser.xmlParse((int) day2, DataDir, data_file, part);

  if(!show_wait) {
    file = config_path;
    file = file + "/moronsuite/weather/themes/icons/";
    file = file + SmallIconTheme;
    file = file + parser.icon.c_str();
    file = file + ".png";
    cImageloadBitmap osdbitmap;
    if( osdbitmap.Load(file.c_str()) ) {
      osdbitmap.LoadImage(file.c_str(), WeatherSkin.Icon2_Width , WeatherSkin.Icon2_Height , false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      Icon2Pixmap->DrawImage(cPoint(0,0), img);
      }
    }
  else {
    file = config_path;
    file = file + "/moronsuite/weather/themes/";
    file = file + Theme;
    file = file + "/images/wait_small.png";

    cImageloadBitmap osdbitmap;
    if( osdbitmap.Load(file.c_str()) ) {
      osdbitmap.LoadImage(file.c_str(), WeatherSkin.Icon2_Width , WeatherSkin.Icon2_Height , false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      Icon2Pixmap->DrawImage(cPoint(0,0), img);
    }
  }

//text

  ausgabe = tr("High temperature:");
  ausgabe = ausgabe + " " + parser.hi.c_str();
  ausgabe = ausgabe + " ";
  ausgabe = ausgabe + tr(parser.celsius.c_str());
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_HighTemp_Top), ausgabe.c_str(), clrHighTempFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  ausgabe = tr("Low temperature:");
  ausgabe = ausgabe + " " + parser.low.c_str();
  ausgabe = ausgabe + " ";
  ausgabe = ausgabe + tr(parser.celsius.c_str());
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_LowTemp_Top), ausgabe.c_str(), clrLowTempFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  ausgabe = tr("Wind-Chill-Index:");
  ausgabe = ausgabe + " " + parser.feellow.c_str();
  ausgabe = ausgabe + " - ";
  ausgabe = ausgabe + parser.feelhi.c_str();
  ausgabe = ausgabe + " ";
  ausgabe = ausgabe + tr(parser.celsius.c_str());
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_FeelTemp_Top), ausgabe.c_str(), clrFeelTempFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  ausgabe = tr("Wind:");
  ausgabe = ausgabe + " " + tr("from ");
  ausgabe = ausgabe + parser.winddir.c_str();
  ausgabe = ausgabe + tr(" with ");
  ausgabe = ausgabe + parser.windspeed.c_str();
  ausgabe = ausgabe + " km/h";
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_Wind_Top), ausgabe.c_str(), clrWindFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  ausgabe = tr("Max. UV Index:");
  ausgabe = ausgabe + " " + parser.maxuv.c_str();
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_UV_Top), ausgabe.c_str(), clrUVFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  ausgabe = tr("Amount of Precipitation:");
  ausgabe = ausgabe + " " + parser.precipamount.c_str();
  ausgabe = ausgabe + tr(" mm/qm");
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_Rain_Top), ausgabe.c_str(), clrRainFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  ausgabe = tr("Tstorm probability:");
  ausgabe = ausgabe + " " + parser.tstormprob.c_str();
  ausgabe = ausgabe + " %";
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_Thunder_Top), ausgabe.c_str(), clrThunderFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

  dsyslog("DEBUG : weather: Information about: %s\n",parser.ort.c_str());
  ausgabe = parser.ort.c_str();
  ausgabe = ausgabe + "  -  ";
  ausgabe = ausgabe + tr(parser.dayname.c_str());
  ausgabe = ausgabe + "  (" + parser.date + ")";
  SecondDayPixmap->DrawText(cPoint(0,WeatherSkin.SecondDay_Date_Top), ausgabe.c_str(), clrDateFG, clrTextBG, pFontSecondDay, WeatherSkin.SecondDay_Width, WeatherSkin.SecondDay_FontHeight, WeatherSkin.SecondDay_Align);

}





/* Display 3. day */
void cWetterOsd::ShowThirdDay(int day3)
{
#ifdef BUGHUNT
  printf("show ShowThirdDay\n");
#endif

#ifdef DEBUG_WEATHER_SYSTEM
  printf("DEBUG : weather: day : %i\n",day3);
  printf("DEBUG : weather: part: %i\n",part);
#endif

  parser.xmlParse((int) day3, DataDir, data_file, part);

  if(!show_wait) {
    file = config_path;
    file = file + "/moronsuite/weather/themes/icons/";
    file = file + SmallIconTheme;
    file = file + parser.icon.c_str();
    file = file + ".png";

    cImageloadBitmap osdbitmap;
    if( osdbitmap.Load(file.c_str()) ) {
      osdbitmap.LoadImage(file.c_str(), WeatherSkin.Icon3_Width , WeatherSkin.Icon3_Height , false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      Icon3Pixmap->DrawImage(cPoint(0,0), img);
      }
    }
  else {
    file = config_path;
    file = file + "/moronsuite/weather/themes/";
    file = file + Theme;
    file = file + "/images/wait_small.png";
    cImageloadBitmap osdbitmap;
    if( osdbitmap.Load(file.c_str()) ) {
      osdbitmap.LoadImage(file.c_str(), WeatherSkin.Icon3_Width , WeatherSkin.Icon3_Height , false);
      cImage img( cSize( osdbitmap.newWidth(), osdbitmap.newHeight() ) );
      osdbitmap.Render(img);
      Icon3Pixmap->DrawImage(cPoint(0,0), img);
    }
  }

//text

  ausgabe = tr("High temperature:");
  ausgabe = ausgabe + " " + parser.hi.c_str();
  ausgabe = ausgabe + " ";
  ausgabe = ausgabe + tr(parser.celsius.c_str());
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_HighTemp_Top), ausgabe.c_str(), clrHighTempFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  ausgabe = tr("Low temperature:");
  ausgabe = ausgabe + " " + parser.low.c_str();
  ausgabe = ausgabe + " ";
  ausgabe = ausgabe + tr(parser.celsius.c_str());
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_LowTemp_Top), ausgabe.c_str(), clrLowTempFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  ausgabe = tr("Wind-Chill-Index:");
  ausgabe = ausgabe + " " + parser.feellow.c_str();
  ausgabe = ausgabe + " - ";
  ausgabe = ausgabe + parser.feelhi.c_str();
  ausgabe = ausgabe + " ";
  ausgabe = ausgabe + tr(parser.celsius.c_str());
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_FeelTemp_Top), ausgabe.c_str(), clrFeelTempFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  ausgabe = tr("Wind:");
  ausgabe = ausgabe + " " + tr("from ");
  ausgabe = ausgabe + parser.winddir.c_str();
  ausgabe = ausgabe + tr(" with ");
  ausgabe = ausgabe + parser.windspeed.c_str();
  ausgabe = ausgabe + " km/h";
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_Wind_Top), ausgabe.c_str(), clrWindFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  ausgabe = tr("Max. UV Index:");
  ausgabe = ausgabe + " " + parser.maxuv.c_str();
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_UV_Top), ausgabe.c_str(), clrUVFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  ausgabe = tr("Amount of Precipitation:");
  ausgabe = ausgabe + " " + parser.precipamount.c_str();
  ausgabe = ausgabe + tr(" mm/qm");
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_Rain_Top), ausgabe.c_str(), clrRainFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  ausgabe = tr("Tstorm probability:");
  ausgabe = ausgabe + " " + parser.tstormprob.c_str();
  ausgabe = ausgabe + " %";
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_Thunder_Top), ausgabe.c_str(), clrThunderFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

  dsyslog("DEBUG : weather: Information about: %s\n",parser.ort.c_str());
  ausgabe = parser.ort.c_str();
  ausgabe = ausgabe + "  -  ";
  ausgabe = ausgabe + tr(parser.dayname.c_str());
  ausgabe = ausgabe + "  (" + parser.date + ")";
  ThirdDayPixmap->DrawText(cPoint(0,WeatherSkin.ThirdDay_Date_Top), ausgabe.c_str(), clrDateFG, clrTextBG, pFontThirdDay, WeatherSkin.ThirdDay_Width, WeatherSkin.ThirdDay_FontHeight, WeatherSkin.ThirdDay_Align);

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

	case 10:
	    Radarmap="/pic10";
	    cWetterOsd::Satelite();
	    break;

	case 11:
	    Radarmap="/pic11";
	    cWetterOsd::Satelite();
	    break;

	case 12:
	    Radarmap="/pic12";
	    cWetterOsd::Satelite();
	    break;

	case 13:
	    Radarmap="/pic13";
	    cWetterOsd::Satelite();
	    break;

	case 14:
	    Radarmap="/pic14";
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
			      ChangeRadar(14);
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
			    else if(radar==10) {
			      ChangeRadar(9);
			      }  
			    else if(radar==11) {
			      ChangeRadar(10);
			      }  
			    else if(radar==12) {
			      ChangeRadar(11);
			      }  
			    else if(radar==13) {
			      ChangeRadar(12);
			      }  
			    else if(radar==14) {
			      ChangeRadar(13);
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
			      ChangeRadar(10);
			      }  
			    else if(radar==10) {
			      ChangeRadar(11);
			      }  
			    else if(radar==11) {
			      ChangeRadar(12);
			      }  
			    else if(radar==12) {
			      ChangeRadar(13);
			      }  
			    else if(radar==13) {
			      ChangeRadar(14);
			      }  
			    else if(radar==14) {
			      ChangeRadar(0);
			      }  
			  }
                        else {
                	    day++;
              		    if (day>3){
                	      day=3;
                	    }
			    cWetterOsd::Show();
			}    
		        break;
       case kBack:
		        if(IsSatelite==true){
//                            ShowStatusButtons(0);
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
/*
			//Debug/Refresh
			WeatherSkin.ParseSkin("current.skin");
			cWetterOsd::Show();
                        WeatherSkin.ReloadFonts=1;
                        SetFonts();
*/

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
