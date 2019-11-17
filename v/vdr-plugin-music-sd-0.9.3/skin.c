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

ReloadFonts         = 0;

clrBG               =0xEF2D435A;

clrTopBG            =0xCFF2A00C;
clrTopFG            =0xFFBABBC0;

clrCoverBG          =0xDF303F52;
clrCoverBar         =0xFF000000;

clrArtistBG         =0xDF303F52;
clrArtistFG         =0xFFBABBC0;
clrRatingFG         =0xFFCC0C0C;

clrPlayStatusBG     =0xDF303F52;
clrPlayStatusFG     =0xFFBABBC0;
clrInfoBG           =0xDF303F52;
clrInfoFG           =0xFFBABBC0;
clrProgressbarBG    =0xFFBABBC0; 
clrProgressbarFG    =0xEFA00404;

clrListTitle        =0xFFBABBC0;
clrListBG           =0xDF303F52;
clrListFG           =0xFFBABBC0;
clrListRating       =0xFFBABBC0;

clrSymbolBG         =0xDF303F52;
clrSymbolFG         =0xEF2D435A;
clrSymbolActive     =0xFFBABBC0;
clrRecordingActive  =0xFFBABBC0;
clrVolumeActive        =0xFFBABBC0;
clrVolumeProgressbarBG =0xEF2D435A;
clrVolumeProgressbarFG =0xFFBABBC0;

clrStatusBG         =0xCFF2A00C;
clrStatusFG1        =0xFF000000;
clrStatusFG2        =0xFF000000;
clrStatusFG3        =0xFF000000;
clrStatusFG4        =0xFF000000;
clrStatusRed        =0xFFC00000;
clrStatusGreen      =0xFF00FF00;
clrStatusYellow     =0xFFE0E222;
clrStatusBlue       =0xFF3B96FD;

// GENERAL
isOSDTheme          = 1;
isHDTheme           = 0;
OSDAlpha            = 255;
roundedCorner       = 1;
drawBars            = 1;
drawButtons         = 1;
isMpeg              = 0;
symrect             = 0;
localbackground     = "";
streambackground    = "";
localcover          = "/music-default-cover.png";
streamcover         = "/music-default-stream.png";
osdbackground       = "";
osdbackgroundcolor  = 2;
symbolxy            = 30;
trackcount          = 3;
osdheight           = 503;
osdwidth            = 616;
osdtop              = 48;
osdleft             = 54;
fontheight          = 26;
smallfontheight     = 22;
reloadmpeg          = true;

// AREAS
UseOneArea          = 0;

// AREAS COLORDEPTH
OneAreaDepth        = 8;
topdepth            = 2;
coverdepth          = 4;
trackinfodepth      = 2;
statusdepth         = 2;
tracksdepth         = 2;
symboldepth         = 2;
buttondepth         = 4;

// AREAS SIZE
area_toph           = 52;
area_coverw         = 170;
area_coverh         = 196;
area_infoh          = 284;
area_listh          = 407;
area_symbolh        = 476;

// POSTIONS
//::cover
coverbgx			= 26;
coverbgy			= 53;
coverbgx2			= 167;
coverbgy2    		= 196;
coverelipser        = 11;


coverx				= 33;
covery				= 60;
coverw				= 128;
coverh				= 128;
covercolor          = 14;

//::coveronly
bigcoverx			= 33;
bigcovery			= 60;
bigcoverw			= 120;
bigcoverh			= 120;
bigcovercolor       = 252;
bigcoverdepth       = 8;

//::artistfield
artistx				= 193;
artisty				= 59;
artistw				= 371;
albumx				= 193;
albumy				= 85;
albumw				= 371;
genrex				= 193;
genrey				= 111;
genrew				= 371;
yearx				= 193;
yeary				= 137;
yearw				= 371;
ratingx				= 193;
ratingy				= 167;
ratingw				= 371;

//::status
statusx				= 31;
statusy				= 232;
statusw				= 136;

//::infofield
modex				= 193;
modey				= 212;
modew				= 375;
numberx				= 193;
numbery				= 232;
numberw				= 375;
timex				= 193;
timey				= 254;
timew				= 80;
progressx			= 273;
progressy			= 263;
progressh           = 6;
progressw			= 210;
totalx				= 482;
totaly				= 254;
totalw				= 80;

//::tracklist
titlex				= 36;
titley				= 308;
titlew				= 528;
tracksx				= 36;
tracksy				= 334;
tracksw				= 450;
ratesmx				= 496;
ratesmy				= 341;
ratesmw				= 80;

// ::symbols
shufflex			= 36;
shuffley			= 425;
loopx				= 72;
loopy				= 425;
recordx				= 108;
recordy				= 425;
lyricsx				= 144;
lyricsy				= 425;
copyx				= 180;
copyy				= 425;
shutdownx			= 216;
shutdowny			= 425;
timerx				= 252;
timery				= 425;
mutex				= 430;
mutey				= 425;
volbarx				= 464;
volbary				= 435;
volbarw             = 100;
volbarh             = 6;
 
// buttons
buttonalign         = 1;
buttontext1posx     = 50;
buttontext2posx     = 200;
buttontext3posx     = 350;
buttontext4posx     = 500;
buttontext1posy     = 477;
buttontext2posy     = 477;
buttontext3posy     = 477;
buttontext4posy     = 477;
button1w            = 110;
button2w            = 110;
button3w            = 110;
button4w            = 70;

//elipse
elipseredx          = 30;
elipseredy          = 485;
elipsegreenx		= 180;
elipsegreeny		= 485;
elipseyellowx		= 330;
elipseyellowy		= 485;
elipsebluex			= 480;
elipsebluey			= 485;
elipseh				= 12;

#ifdef HAVE_HD_OSD
//Tracklist_tab
Tracklist_tab1 = 6;
Tracklist_tab2 = 9;
Tracklist_tab3 = 1;
Tracklist_tab4 = 29;

//Searchlist_tab
Searchlist_tab1 = 3;
Searchlist_tab2 = 7;
Searchlist_tab3 = 8;
Searchlist_tab4 = 26;
#else
//Tracklist_tab
Tracklist_tab1 = 6;
Tracklist_tab2 = 8;
Tracklist_tab3 = 1;
Tracklist_tab4 = 29;

//Searchlist_tab
Searchlist_tab1 = 3;
Searchlist_tab2 = 7;
Searchlist_tab3 = 8;
Searchlist_tab4 = 26;
#endif

};


cMP3Skin::~cMP3Skin()
{
}

int cMP3Skin::ParseSkin(const char *SkinName, bool ReloadMpeg)
{
  
  using namespace std;
  ifstream filestr;
  std::string line;
  std::string Value;
  std::string datei;
  std::string skinname;

  bool result=false;

  reloadmpeg = ReloadMpeg;
  
  skinname = SkinName;

  isMpeg = 0;
  localbackground = "";
  streambackground = "";
  osdbackground = "";

  datei = config;
  datei = datei + "/themes/";
  datei = datei + skinname;

  dsyslog("music-sd: Load themefile '%s'\n", datei.c_str());

  filestr.open (datei.c_str());
  if(filestr) {
    while (getline(filestr, line, '\n')) {
      int len = line.length();
      string::size_type pos = line.find ("<value>",0);

      if(pos != string::npos) {
        Value = line.substr(len -10, len);

        if      (strstr(line.c_str(),".clrBG."))              clrBG                 = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),".clrTopBG."))           clrTopBG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrTopFG."))           clrTopFG              = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),".clrCoverBG."))         clrCoverBG            = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrCoverBar."))        clrCoverBar           = strtoul(Value.c_str(), NULL,16);
         
        else if (strstr(line.c_str(),".clrArtistBG."))        clrArtistBG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrArtistFG."))        clrArtistFG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrRatingFG."))        clrRatingFG           = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),".clrPlayStatusBG."))    clrPlayStatusBG       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrPlayStatusFG."))    clrPlayStatusFG       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrInfoBG."))          clrInfoBG             = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrInfoFG."))          clrInfoFG             = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrProgressbarBG."))   clrProgressbarBG      = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrProgressbarFG."))   clrProgressbarFG      = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),".clrListTitle."))       clrListTitle          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrListBG."))          clrListBG             = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrListFG."))          clrListFG             = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrListRating."))      clrListRating         = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),".clrSymbolBG."))        clrSymbolBG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrSymbolFG."))        clrSymbolFG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrSymbolActive."))    clrSymbolActive       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrRecordingActive.")) clrRecordingActive    = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrVolumeActive.")) clrVolumeActive                  = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrVolumeProgressbarBG.")) clrVolumeProgressbarBG    = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrVolumeProgressbarFG.")) clrVolumeProgressbarFG    = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),".clrStatusBG."))        clrStatusBG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusFG1."))       clrStatusFG1          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusFG2."))       clrStatusFG2          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusFG3."))       clrStatusFG3          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusFG4."))       clrStatusFG4          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusRed."))       clrStatusRed          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusGreen."))     clrStatusGreen        = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusYellow."))    clrStatusYellow       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),".clrStatusBlue."))      clrStatusBlue         = strtoul(Value.c_str(), NULL,16);
//isOSDtheme
        else if (strstr(line.c_str(),".isOSDTheme.")) {
          Value   = line.substr(len -1, len);
          isOSDTheme  = atoi(Value.c_str());
	      }
        else if (strstr(line.c_str(),".isHDTheme.")) {
          Value   = line.substr(len -1, len);
          isHDTheme  = atoi(Value.c_str());
	      }
        else if (strstr(line.c_str(),".roundedCorner.")) {
          Value   = line.substr(len -1, len);
          roundedCorner  = atoi(Value.c_str());
	      }
        else if (strstr(line.c_str(),".drawBars.")) {
          Value   = line.substr(len -1, len);
          drawBars  = atoi(Value.c_str());
	      }
        else if (strstr(line.c_str(),".drawButtons.")) {
          Value   = line.substr(len -1, len);
          drawButtons  = atoi(Value.c_str());
	      }
//ismpeg		
        else if (strstr(line.c_str(),".isMpeg.")) {
          Value   = line.substr(len -1, len);
          isMpeg  = atoi(Value.c_str());
	      }
        else if (strstr(line.c_str(),".symrect.")) {
          Value   = line.substr(len -1, len);
          symrect  = atoi(Value.c_str());
	      }
        else if (strstr(line.c_str(),".localbackground.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          localbackground = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".streambackground.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          streambackground = Value.c_str();
            }
        }  
        else if (strstr(line.c_str(),".localcover.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          localcover = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".streamcover.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          streamcover = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".osdbackground.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          osdbackground = Value.c_str();
            }
        }  
        else if (strstr(line.c_str(),".OSDAlpha.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OSDAlpha = atoi(Value.c_str());
            }
        }  
        else if (strstr(line.c_str(),".osdbackgroundcolor.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          osdbackgroundcolor = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".symbolxy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          symbolxy = atoi(Value.c_str());
            }
          }  
	    else if (strstr(line.c_str(),".trackcount.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          trackcount = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".osdheight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          osdheight = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".osdwidth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          osdwidth = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".osdtop.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          osdtop = atoi(Value.c_str());
            }
	        }  
        else if (strstr(line.c_str(),".osdleft.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          osdleft = atoi(Value.c_str());
            }
	        }  		
        else if (strstr(line.c_str(),".fontheight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          fontheight = atoi(Value.c_str());
            }
	        }  		
        else if (strstr(line.c_str(),".smallfontheight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          smallfontheight = atoi(Value.c_str());
            }
	        }  		
//
// =========
// AREAS
// =========
        else if (strstr(line.c_str(),".UseOneArea.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UseOneArea = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".OneAreaDepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          OneAreaDepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".topdepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          topdepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverdepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverdepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".trackinfodepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          trackinfodepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".statusdepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          statusdepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".tracksdepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          tracksdepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".symboldepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          symboldepth = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttondepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttondepth = atoi(Value.c_str());
            }
          }  
// ==========
// AREAS SIZE
// ==========
//
        else if (strstr(line.c_str(),".area_toph.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          area_toph = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".area_coverw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          area_coverw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".area_coverh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          area_coverh = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".area_infoh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          area_infoh = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".area_listh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          area_listh = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".area_symbolh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          area_symbolh = atoi(Value.c_str());
            }
          }  
// =========
// POSITIONS
// =========
//
//cover or visbackground in playerview

        else if (strstr(line.c_str(),".coverbgx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverbgx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverbgy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverbgy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverbgx2.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverbgx2 = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverbgy2.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverbgy2 = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverelipser.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverelipser = atoi(Value.c_str());
            }
          }  
		else if (strstr(line.c_str(),".coverx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".covery.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          covery = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".coverh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          coverh = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".covercolor.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          covercolor = atoi(Value.c_str());
            }
          }  
//coveronly
        else if (strstr(line.c_str(),".bigcoverx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          bigcoverx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".bigcovery.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          bigcovery = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".bigcoverw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          bigcoverw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".bigcoverh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          bigcoverh = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".bigcovercolor.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          bigcovercolor = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".bigcoverdepth.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          bigcoverdepth = atoi(Value.c_str());
            }
          }  
//artistfield
        else if (strstr(line.c_str(),".artistx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          artistx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".artisty.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          artisty = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".artistw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          artistw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".albumx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          albumx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".albumy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          albumy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".albumw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          albumw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".genrex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          genrex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".genrey.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          genrey = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".genrew.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          genrew = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".yearx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          yearx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".yeary.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          yeary = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".yearw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          yearw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ratingx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ratingx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ratingy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ratingy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ratingw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ratingw = atoi(Value.c_str());
            }
          }  
//status
        else if (strstr(line.c_str(),".statusx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          statusx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".statusy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          statusy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".statusw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          statusw = atoi(Value.c_str());
            }
          }  
//infofield
        else if (strstr(line.c_str(),".modex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          modex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".modey.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          modey = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".modew.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          modew = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".numberx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          numberx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".numbery.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          numbery = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".numberw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          numberw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".timex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          timex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".timey.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          timey = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".timew.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          timew = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".progressx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          progressx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".progressy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          progressy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".progressh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          progressh = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".progressw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          progressw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".totalx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          totalx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".totaly.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          totaly = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".totalw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          totalw = atoi(Value.c_str());
            }
          }  
//tracklist
        else if (strstr(line.c_str(),".titlex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          titlex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".titley.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          titley = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".titlew.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          titlew = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".tracksx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          tracksx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".tracksy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          tracksy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".tracksw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          tracksw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ratesmx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ratesmx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ratesmy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ratesmy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ratesmw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ratesmw = atoi(Value.c_str());
            }
          }  
//symbols
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
        else if (strstr(line.c_str(),".mutex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          mutex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".mutey.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          mutey = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".volbarx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          volbarx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".volbary.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          volbary = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".volbarw.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          volbarw = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".volbarh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          volbarh = atoi(Value.c_str());
            }
          }  
//buttons
        else if (strstr(line.c_str(),".buttonalign.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttonalign = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext1posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext1posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext2posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext2posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext3posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext3posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext4posx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext4posx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext1posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext1posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext2posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext2posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext3posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext3posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".buttontext4posy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          buttontext4posy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".button1w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          button1w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".button2w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          button2w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".button3w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          button3w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".button4w.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          button4w = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipseredx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipseredx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipseredy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipseredy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipsegreenx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipsegreenx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipsegreeny.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipsegreeny = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipseyellowx.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipseyellowx = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipseyellowy.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipseyellowy = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipsebluex.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipsebluex = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipsebluey.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipsebluey = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".elipseh.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          elipseh = atoi(Value.c_str());
            }
          }
// Tracklist_tab
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
// Searchlist_tab
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
// FONTS
        else if (strstr(line.c_str(),".FONTBUTTONNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTBUTTONNAME = Value.c_str();
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
        else if (strstr(line.c_str(),".FONTINTERPRETNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTINTERPRETNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTLISTNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLISTNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTSTATUSNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSTATUSNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTSYMBOLNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSYMBOLNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTTITLENAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTITLENAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTTOPNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTOPNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTBUTTONSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTBUTTONSIZE = atoi(Value.c_str());
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
        else if (strstr(line.c_str(),".FONTINTERPRETSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTINTERPRETSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTLISTSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLISTSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTSTATUSSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSTATUSSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTSYMBOLSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSYMBOLSIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTTITLESIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTITLESIZE = atoi(Value.c_str());
            }
          }
        else if (strstr(line.c_str(),".FONTTOPSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTOPSIZE = atoi(Value.c_str());
            }
          }
       }
    }

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

  if( FILE *f = fopen(dateiout.c_str(), "w")) {
    filestr.open (datei.c_str());
    if(filestr) {
      while (getline(filestr, line, '\n')) {
        line = line + "\n";
        fprintf(f, line.c_str());
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
