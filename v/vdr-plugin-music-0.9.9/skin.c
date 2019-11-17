/*
 * Music plugin to VDR (C++)
 *
 * (C) 2006 Morone
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

//#include <string>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vdr/plugin.h>

#include "i18n.h"
#include "skin.h"
#include "vars.h"

cMP3Skin MP3Skin;

// --- cMP3Skin ---------------------------------------------------------------

cMP3Skin::cMP3Skin(void)
{
  ReloadFonts				=0;

// COLORS
  clrTextBG				=0x00000000;
  clrArtist				=0xFFffffff;
  clrRating				=0xFFffab69;
  clrRatingBG				=0xFF505050;
 
  clrPlayStatus				=0xFF707070;
  clrInfo				=0xFF1a5aaa;
  clrProgressbarBG			=0xFF16223b;
  clrProgressbarFG			=0xFF133f79;
 
  clrListTitle				=0xFFf2a00c;
  clrList				=0xFF9f9f9f;
  clrListRating				=0xFFf2a00c;
  clrListRatingBG			=0xFF505050;
 
  clrSymbol				=0xFF707070;
  clrSymbolActive			=0xFFff4100;
  clrRecordingActive			=0xFFb90808;
  clrVolumeActive			=0xFF707070;
  clrVolumeProgressbarBG		=0xFF303030;
  clrVolumeProgressbarFG		=0xFF5a6163;

  clrStatusRed				=0xFF992900;
  clrStatusGreen			=0xFF6cb752;
  clrStatusYellow			=0xFFce7b00;
  clrStatusBlue				=0xFF52b3b7;

// GENERAL -------------------------------------
  SkinPath				="";
  Description				="DEFAULT";
  OsdWidth				=1920;
  OsdHeight				=1080;
  OsdTop				=0;
  OsdLeft				=0;
  OsdAlpha				=175;
  OsdBgrd				="default-background.png";
  LocalCover				="default-local.png";
  StreamCover				="default-stream.png";
  TrackCount				=14;
  FontHeight				=50;
  SmallFontHeight			=42;

// TRACKLIST_TAB -------------------------------
  Tracklist_tab1			=6;
  Tracklist_tab2			=9;
  Tracklist_tab3			=1;
  Tracklist_tab4			=29;

// SEARCHLIST_TAB ------------------------------
  Searchlist_tab1			=3;
  Searchlist_tab2			=7;
  Searchlist_tab3			=8;
  Searchlist_tab4			=28;

// OBJECTS && PIXMAPS --------------------------
  CoverOnly_Layer			=7;
  CoverOnly_Left			=0;
  CoverOnly_Top				=0;
  CoverOnly_Width			=1920;
  CoverOnly_Height			=1080;

// BgrdPixmap ----------------------------------
  Bgrd_Layer				=0;
  Bgrd_Left				=0;
  Bgrd_Top				=0;
  Bgrd_Width				=1920;
  Bgrd_Height				=1080;

// PlaystatusPixmap ----------------------------
  Playstatus_Layer			=1;
  Playstatus_Left			=35;
  Playstatus_Top			=80;
  Playstatus_Width			=520;
  Playstatus_Height			=32;

// CoverPixmap ---------------------------------
  Cover_Layer				=2;
  Cover_Left				=93;
  Cover_Top				=132;
  Cover_Width				=400;
  Cover_Height				=400;

// RatingPixmap --------------------------------
  Rating_Layer				=3;
  Rating_Left				=200;
  Rating_Top				=550;
  Rating_Width				=210;
  Rating_Height				=42;
  Rating_ImageWidth			=197;
  Rating_ImageHeight			=37;

// AlbumPixmap ---------------------------------
  Album_Layer				=3;
  Album_Left				=35;
  Album_Top				=620;
  Album_Width				=520;
  Album_Height				=32;

// GenrePixmap ---------------------------------
  Genre_Layer				=3;
  Genre_Left				=35;
  Genre_Top				=684;
  Genre_Width				=520;
  Genre_Height				=32;

// YearPixmap ----------------------------------
  Year_Layer				=3;
  Year_Left				=35;
  Year_Top				=738;
  Year_Width				=520;
  Year_Height				=32;

// ModePixmap ----------------------------------
  Mode_Layer				=3;
  Mode_Left				=35;
  Mode_Top				=800;
  Mode_Width				=520;
  Mode_Height				=32;
  Mode_Align				=1;

// NumberPixmap --------------------------------
  Number_Layer				=3;
  Number_Left				=35;
  Number_Top				=850;
  Number_Width				=520;
  Number_Height				=32;
  Number_Align				=1;

// VolumeBGPixmap --------------------------------
  VolumeBG_Layer			=1;
  VolumeBG_Left				=86;
  VolumeBG_Top				=1033;
  VolumeBG_Width			=300;
  VolumeBG_Height			=24;

// VolumePixmap --------------------------------
  VolumeFG_Layer			=3;
  VolumeFG_Left				=74;
  VolumeFG_Top				=1033;
  VolumeFG_Width			=324;
  VolumeFG_Height			=24;
  VolumeFG_Slider			=1;
  VolumeFG_Scale			=300;
  
// VolumePixmap --------------------------------
  Volume_Layer				=6;
  Volume_Left				=35;
  Volume_Top				=1030;
  Volume_Width				=29;
  Volume_Height				=29;

// IconsPixmap ---------------------------------
  Symbol_Layer				=3;
  Symbol_Left				=35;
  Symbol_Top				=948;
  Symbol_Width				=378;
  Symbol_Height				=54;
  symbol_width				=54;
  symbol_height				=54;
  shufflex				=0;
  shuffley				=0;
  loopx					=54;
  loopy					=0;
  recordx				=108;
  recordy				=0;
  lyricsx				=162;
  lyricsy				=0;
  copyx					=216;
  copyy					=0;
  shutdownx				=270;
  shutdowny				=0;
  timerx				=324;
  timery				=0;

// TracksPixmap --------------------------------
  Tracks_Layer				=4;
  Tracks_Left				=642;
  Tracks_Top				=148;
  Tracks_Height				=715;
  Tracks_Width				=1070;
  Tracks_TextWidth			=1060;
  Tracks_TextHeight			=40;
  Tracks_TextPuffer			=2;

// TracksPixmap --------------------------------
  TracksRating_Layer			=4;
  TracksRating_Left			=1756;
  TracksRating_Top			=156;
  TracksRating_Height			=715;
  TracksRating_Width			=120;
  TracksRating_ImageWidth		=115;
  TracksRating_ImageHeight		=22;
  TracksRating_TextPuffer		=20;

// ArtistPixmap --------------------------------
  Artist_Layer				=4;
  Artist_Left				=620;
  Artist_Top				=815;
  Artist_Width				=1260;
  Artist_Height				=54;

// TitlePixmap ---------------------------------
  Title_Layer				=5;
  Title_Left				=620;
  Title_Top				=880;
  Title_Width				=1260;
  Title_Height				=54;
  Title_Align				=1;

// ProgressBGPixmap ------------------------------
  ProgressBG_Layer			=1;
  ProgressBG_Left			=482;
  ProgressBG_Top			=1033;
  ProgressBG_Width			=300;
  ProgressBG_Height			=24;

// ProgressFGPixmap ------------------------------
  ProgressFG_Layer			=3;
  ProgressFG_Left			=470;
  ProgressFG_Top			=1033;
  ProgressFG_Width			=324;
  ProgressFG_Height			=24;  
  ProgressFG_Slider			=1;  
  ProgressFG_Scale			=300;  
  
// ProgressPixmap ------------------------------
  Progress_Layer			=6;
  Progress_Left				=35;
  Progress_Top				=900;
  Progress_Width			=300;
  Progress_Height			=32;
  Timex					=0;
  Timey					=0;
  Timew					=300;
  Totalx				=0;
  Totaly				=0;
  Totalw				=0;

// ButtonPixmap --------------------------------
  Button_Layer				=6;
  Button_Left				=936;
  Button_Top				=1004;
  Button_Width				=917;
  Button_Height				=40;
  Button_Align				=0;
  ButtonText1posx			=0;
  ButtonText2posx			=244;
  ButtonText3posx			=484;
  ButtonText4posx			=740;
  ButtonText1posy			=0;
  ButtonText2posy			=0;
  ButtonText3posy			=0;
  ButtonText4posy			=0;
  Button1w				=194;
  Button2w				=194;
  Button3w				=194;
  Button4w				=194;


// FONTS ---------------------------------------
 FONTSTATUSNAME				="VDRSymbols Sans:Book";
 FONTSTATUSSIZE				=32;

 FONTRATINGNAME				="VDRSymbols Sans:Book";
 FONTRATINGSIZE				=44;

 FONTGENRENAME				="VDRSymbols Sans:Book";
 FONTGENRESIZE				=28;

 FONTINFONAME				="VDRSymbols Sans:Book";
 FONTINFOSIZE				=22;

 FONTSYMBOLNAME				="VDRSymbols Sans:Book";
 FONTSYMBOLSIZE				=44;

 FONTLISTNAME				="VDRSymbols Sans:Book";
 FONTLISTSIZE				=22;

 FONTLISTRATINGNAME			="VDRSymbols Sans:Book";
 FONTLISTRATINGSIZE			=22;

 FONTINTERPRETNAME			="VDRSymbols Sans:Book";
 FONTINTERPRETSIZE			=44;

 FONTTITLENAME				="VDRSymbols Sans:Book";
 FONTTITLESIZE				=40;

 FONTBUTTONNAME				="VDRSymbols Sans:Book";
 FONTBUTTONSIZE				=22;

};


cMP3Skin::~cMP3Skin()
{
}

int cMP3Skin::ParseSkin(const char *SkinName)
{

  using namespace std;
  ifstream filestr;
  std::string line;
  std::string Value;
  std::string datei;
  std::string skinname;

  bool result=false;

  skinname = SkinName;

  datei = config;
  datei = datei + "/themes/";
  datei = datei + skinname;

#ifdef DEBUG
  isyslog("music: skin: load themefile from '%s'", datei.c_str());
#endif

  filestr.open (datei.c_str());
  if(filestr) {

    OsdBgrd          = "";
    LocalCover       = "";
    StreamCover      = "";

    while (getline(filestr, line, '\n')) {
      int len = line.length();
      string::size_type pos = line.find ("<value>",0);

      if(pos != string::npos) {
        Value = line.substr(len -10, len);


// COLORS
        if      (strstr(line.c_str(),".clrTextBG."))              clrTextBG                     = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrArtist."))              clrArtist                     = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrRating."))              clrRating                     = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrRatingBG."))            clrRatingBG                   = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrPlayStatus."))          clrPlayStatus                 = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrInfo."))                clrInfo                       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrProgressbarBG."))       clrProgressbarBG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrProgressbarFG."))       clrProgressbarFG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrListTitle."))           clrListTitle                  = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrList."))                clrList                       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrListRating."))          clrListRating                 = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrListRatingBG."))        clrListRatingBG               = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrSymbol."))              clrSymbol                     = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrSymbolActive."))        clrSymbolActive               = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrRecordingActive."))     clrRecordingActive            = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrVolumeActive."))        clrVolumeActive               = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrVolumeProgressbarBG.")) clrVolumeProgressbarBG        = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrVolumeProgressbarFG.")) clrVolumeProgressbarFG        = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusRed."))           clrStatusRed                  = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusGreen."))         clrStatusGreen                = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusYellow."))        clrStatusYellow               = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusBlue."))          clrStatusBlue                 = strtoul(Value.c_str(), NULL,16);

// GENERAL


        else if (strstr(line.c_str(),".Description.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Description = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".OsdWidth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OsdWidth = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".OsdHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OsdHeight = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".OsdTop.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OsdTop = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".OsdLeft.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OsdLeft = atoi(Value.c_str());
            }
	        }  		
        else if (strstr(line.c_str(),".OsdAlpha.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OsdAlpha = atoi(Value.c_str());
            }
        }  
        else if (strstr(line.c_str(),".OsdBgrd.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OsdBgrd = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".LocalCover.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LocalCover = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".StreamCover.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          StreamCover = Value.c_str();
	          }     
          }    
	    else if (strstr(line.c_str(),".TrackCount.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TrackCount = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".FontHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FontHeight = atoi(Value.c_str());
            }
	        }  		
        else if (strstr(line.c_str(),".SmallFontHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SmallFontHeight = atoi(Value.c_str());
            }
	        }  		

// TRACKLIST_TAB
        else if (strstr(line.c_str(),".Tracklist_tab1.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracklist_tab1 = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".Tracklist_tab2.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracklist_tab2 = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".Tracklist_tab3.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracklist_tab3 = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".Tracklist_tab4.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracklist_tab4 = atoi(Value.c_str());
            }
          }
// SEARCHLIST_TAB
        else if (strstr(line.c_str(),".Searchlist_tab1.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Searchlist_tab1 = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".Searchlist_tab2.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Searchlist_tab2 = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".Searchlist_tab3.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Searchlist_tab3 = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".Searchlist_tab4.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Searchlist_tab4 = atoi(Value.c_str());
            }
          }

// OBJECTS && PIXMAPS
        else if (strstr(line.c_str(),".CoverOnly_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          CoverOnly_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".CoverOnly_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          CoverOnly_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".CoverOnly_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          CoverOnly_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".CoverOnly_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          CoverOnly_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".CoverOnly_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          CoverOnly_Height = atoi(Value.c_str());
            }
          }  

// BgrdPixmap
        else if (strstr(line.c_str(),".Bgrd_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Bgrd_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Bgrd_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Bgrd_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Bgrd_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Bgrd_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Bgrd_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Bgrd_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Bgrd_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Bgrd_Height = atoi(Value.c_str());
            }
          }  
// Playstatus
        else if (strstr(line.c_str(),".Playstatus_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Playstatus_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Playstatus_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Playstatus_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Playstatus_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Playstatus_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Playstatus_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Playstatus_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Playstatus_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Playstatus_Height = atoi(Value.c_str());
            }
          }  

// CoverPixmap
        else if (strstr(line.c_str(),".Cover_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Cover_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Cover_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Cover_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Cover_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Cover_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Cover_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Cover_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Cover_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Cover_Height = atoi(Value.c_str());
            }
          }  

// RatingPixmap
        else if (strstr(line.c_str(),".Rating_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rating_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rating_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rating_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rating_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rating_ImageWidth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_ImageWidth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rating_ImageHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rating_ImageHeight = atoi(Value.c_str());
            }
          }  

// AlbumPixmap
        else if (strstr(line.c_str(),".Album_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Album_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Album_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Album_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Album_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Album_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Album_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Album_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Album_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Album_Height = atoi(Value.c_str());
            }
          }  


// GenrePixmap
        else if (strstr(line.c_str(),".Genre_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Genre_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Genre_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Genre_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Genre_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Genre_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Genre_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Genre_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Genre_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Genre_Height = atoi(Value.c_str());
            }
          }  

// YearPixmap
        else if (strstr(line.c_str(),".Year_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Year_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Year_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Year_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Year_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Year_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Year_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Year_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Year_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Year_Height = atoi(Value.c_str());
            }
          }  

// ModePixmap
        else if (strstr(line.c_str(),".Mode_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Mode_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Mode_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Mode_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Mode_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Mode_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Mode_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Mode_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Mode_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Mode_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Mode_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Mode_Align = atoi(Value.c_str());
            }
          }  

// NumberPixmap
        else if (strstr(line.c_str(),".Number_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Number_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Number_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Number_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Number_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Number_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Number_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Number_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Number_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Number_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Number_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Number_Align = atoi(Value.c_str());
            }
          }  

// VolumeBGPixmap
        else if (strstr(line.c_str(),".VolumeBG_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeBG_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeBG_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeBG_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeBG_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeBG_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeBG_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeBG_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeBG_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeBG_Height = atoi(Value.c_str());
            }
          }  

// VolumeFGPixmap
        else if (strstr(line.c_str(),".VolumeFG_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeFG_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeFG_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeFG_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeFG_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeFG_Slider.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Slider = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".VolumeFG_Scale.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          VolumeFG_Scale = atoi(Value.c_str());
            }
          }  

// VolumePixmap (Button)
        else if (strstr(line.c_str(),".Volume_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Volume_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Volume_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Volume_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Volume_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Volume_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Volume_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Volume_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Volume_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Volume_Height = atoi(Value.c_str());
            }
          }  

// IconsPixmap
        else if (strstr(line.c_str(),".Symbol_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Symbol_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Symbol_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Symbol_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Symbol_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Symbol_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Symbol_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Symbol_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Symbol_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Symbol_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".symbol_width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          symbol_width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".symbol_height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          symbol_height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".shufflex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          shufflex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".shuffley.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          shuffley = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".loopx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          loopx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".loopy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          loopy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".recordx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          recordx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".recordy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          recordy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".lyricsx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          lyricsx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".lyricsy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          lyricsy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".copyx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          copyx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".copyy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          copyy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".shutdownx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          shutdownx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".shutdowny.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          shutdowny = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".timerx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          timerx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".timery.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          timery = atoi(Value.c_str());
            }
          }  

// TracksPixmap
        else if (strstr(line.c_str(),".Tracks_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_TextWidth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_TextWidth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_TextHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_TextHeight = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Tracks_TextPuffer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Tracks_TextPuffer = atoi(Value.c_str());
            }
          }

// TracksRatingPixmap
        else if (strstr(line.c_str(),".TracksRating_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_ImageWidth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_ImageWidth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_ImageHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_ImageHeight = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".TracksRating_TextPuffer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          TracksRating_TextPuffer = atoi(Value.c_str());
            }
          }

// ArtistPixmap
        else if (strstr(line.c_str(),".Artist_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Artist_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Artist_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Artist_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Artist_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Artist_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Artist_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Artist_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Artist_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Artist_Height = atoi(Value.c_str());
            }
          }  

// TitlePixmap
        else if (strstr(line.c_str(),".Title_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Title_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Title_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Title_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Title_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Title_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Title_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Title_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Title_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Title_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Title_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Title_Align = atoi(Value.c_str());
            }
          }  

// ProgressBGPixmap
        else if (strstr(line.c_str(),".ProgressBG_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressBG_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressBG_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressBG_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressBG_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressBG_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressBG_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressBG_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressBG_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressBG_Height = atoi(Value.c_str());
            }
          }  		  

// ProgressFGPixmap
        else if (strstr(line.c_str(),".ProgressFG_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressFG_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressFG_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressFG_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressFG_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressFG_Slider.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Slider = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ProgressFG_Scale.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ProgressFG_Scale = atoi(Value.c_str());
            }
          }

// ProgressPixmap
        else if (strstr(line.c_str(),".Progress_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Progress_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Progress_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Progress_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Progress_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Progress_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Progress_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Progress_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Progress_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Progress_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Timex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Timex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Timey.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Timey = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Timew.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Timew = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Totalx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Totalx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Totaly.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Totaly = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Totalw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Totalw = atoi(Value.c_str());
            }
          }  

// ButtonPixmap
        else if (strstr(line.c_str(),".Button_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button_Align = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText1posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText1posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText2posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText2posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText3posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText3posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText4posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText4posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText1posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText1posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText2posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText2posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText3posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText3posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ButtonText4posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ButtonText4posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button1w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button1w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button2w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button2w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button3w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button3w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Button4w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Button4w = atoi(Value.c_str());
            }
          }  

// FONTS
        else if (strstr(line.c_str(),".FONTSTATUSNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSTATUSNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTRATINGNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTRATINGNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTGENRENAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTGENRENAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTINFONAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTINFONAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTSYMBOLNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSYMBOLNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTLISTNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLISTNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTLISTRATINGNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLISTRATINGNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTINTERPRETNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTINTERPRETNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTTITLENAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTITLENAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTBUTTONNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTBUTTONNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTSTATUSSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSTATUSSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTRATINGSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTRATINGSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTGENRESIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTGENRESIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTINFOSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTINFOSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTSYMBOLSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSYMBOLSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTLISTSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLISTSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTLISTRATINGSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLISTRATINGSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTINTERPRETSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTINTERPRETSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTTITLESIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTITLESIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTBUTTONSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTBUTTONSIZE = atoi(Value.c_str());
            }
          }
       }
    }



    SkinPath = config;
    SkinPath = SkinPath + "/themes/skins/" + Description + "/";
#ifdef DEBUG
    printf("music: skin.c : SkinPath = '%s'\n", SkinPath.c_str());
#endif

    filestr.close();
    result = true;
  }

  return result;
}


int cMP3Skin::StoreSkin(const char *ThemeName)
{
  using namespace std;
  ifstream filestr;
  std::string line;
  std::string datei;
  std::string dateiout;
  std::string themename;

  bool res=false;
  
  themename = ThemeName;

  datei = config;
  datei = datei + "/themes/";
  datei = datei + ThemeName;

  dateiout = config;
  dateiout = dateiout + "/themes/current.colors";

#ifdef DEBUG
  isyslog("music: skin: store themefile '%s' to '%s'\n", datei.c_str(), dateiout.c_str());
#endif

  FILE *f;
  f = fopen(dateiout.c_str(), "w");
  if (f != NULL ) {
    filestr.open (datei.c_str());
    if(filestr) {
      while (getline(filestr, line, '\n')) {
        line = line + "\n";
        fprintf(f, "%s",line.c_str());
      }
      filestr.close();
    }
    res = true;
    fclose(f);
  }
  else
    res = false;

  return res;
}

