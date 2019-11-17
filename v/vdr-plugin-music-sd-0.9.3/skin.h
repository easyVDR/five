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

#ifndef ___SKIN_H
#define ___SKIN_H

#include <string> 

class cMP3Skin {
private:
//  int isMpeg;
//  char *localbackground, *streambackground;
public:
  cMP3Skin(void);
  virtual ~cMP3Skin();
  int ParseSkin(const char *SkinName, bool ReloadMpeg);
  int StoreSkin(const char *ThemeName);
  int ReloadFonts;
  int clrBG;
  int clrTopBG;
  int clrTopFG;
  int clrCoverBG;
  int clrCoverBar;
  int clrArtistBG;
  int clrArtistFG;
  int clrRatingFG;
  int clrPlayStatusBG;
  int clrPlayStatusFG;
  int clrInfoBG;
  int clrInfoFG;
  int clrProgressbarBG;
  int clrProgressbarFG;
  int clrListTitle;
  int clrListBG;
  int clrListFG;
  int clrListRating;
  int clrSymbolBG;
  int clrSymbolFG;
  int clrSymbolActive;
  int clrRecordingActive;
  int clrVolumeActive;
  int clrVolumeProgressbarBG;
  int clrVolumeProgressbarFG;
  int clrStatusBG;
  int clrStatusFG1;
  int clrStatusFG2;
  int clrStatusFG3;
  int clrStatusFG4;
  int clrStatusRed;
  int clrStatusGreen;
  int clrStatusYellow;
  int clrStatusBlue;
  int imgalpha;
// GENERAL
  int isOSDTheme;
  int isHDTheme;
  int OSDAlpha;
  int roundedCorner;
  int drawBars;
  int drawButtons;
  int isMpeg;
  int symrect;
  std::string localbackground;
  std::string streambackground;
  std::string localcover;
  std::string streamcover;
  std::string osdbackground;
  int osdbackgroundcolor;
  int symbolxy;
  int trackcount;
  int osdheight;
  int osdwidth;
  int osdtop;
  int osdleft;
  int fontheight;
  int smallfontheight;
 
  bool reloadmpeg;


// AREAS
  int UseOneArea;

// AREAS colordepth
  int OneAreaDepth;
  int topdepth;
  int coverdepth;
  int trackinfodepth;
  int statusdepth;
  int tracksdepth;
  int symboldepth;
  int buttondepth;

// AREAS size
  int area_toph;
  int area_coverw;
  int area_coverh;
  int area_infoh;
  int area_listh;
  int area_symbolh;

 // P O S I T I O N S
 //:cover (small)------------------------  
  int coverbgx;
  int coverbgy;
  int coverbgx2;
  int coverbgy2;
  int coverelipser;

  int coverx;
  int covery;
  int coverw;
  int coverh;
  int covercolor;
//:coveronly (big)-----------------------  
  int bigcoverx;
  int bigcovery;
  int bigcoverw;
  int bigcoverh;
  int bigcovercolor;
  int bigcoverdepth;
//::artistfield -----------------------------------
  int artistx;
  int artisty;
  int artistw;
  int albumx;
  int albumy;
  int albumw;
  int genrex;
  int genrey;
  int genrew;
  int yearx;
  int yeary;
  int yearw;
  int ratingx;
  int ratingy;
  int ratingw;
//::status -----------------------------------
  int statusx;
  int statusy;
  int statusw;
//::infofield -----------------------------------
  int modex;
  int modey;
  int modew;
  int numberx;
  int numbery;
  int numberw;
  int timex;
  int timey;
  int timew;
  int progressx;
  int progressy;
  int progressw;
  int progressh;
  int totalx;
  int totaly;
  int totalw;
//::tracklist -----------------------------------
  int titlex;
  int titley;
  int titlew;
  int tracksx;
  int tracksy;
  int tracksw;
  int ratesmx;
  int ratesmy;
  int ratesmw;
// ::symbols -----------------------------------
  int shufflex;
  int shuffley;
  int loopx;
  int loopy;
  int recordx;
  int recordy;
  int lyricsx;
  int lyricsy;
  int copyx;
  int copyy;
  int shutdownx;
  int shutdowny;
  int timerx;
  int timery;
  int mutex;
  int mutey;
  int volbarx;
  int volbary;
  int volbarw;
  int volbarh;  
// buttons -----------------------------------
  int buttonalign;
  int buttontext1posx;
  int buttontext2posx;
  int buttontext3posx;
  int buttontext4posx;
  int buttontext1posy;
  int buttontext2posy;
  int buttontext3posy;
  int buttontext4posy;
  int button1w;
  int button2w;
  int button3w;
  int button4w;
// elipse --------------------------------------
  int elipseredx;
  int elipseredy;
  int elipseyellowx;
  int elipseyellowy;
  int elipsegreenx;
  int elipsegreeny;
  int elipsebluex;
  int elipsebluey;
  int elipseh;
// tracklist_tabs ------------------------------
  int Tracklist_tab1;
  int Tracklist_tab2;
  int Tracklist_tab3;
  int Tracklist_tab4;
// tracklist_tabs ------------------------------
  int Searchlist_tab1;
  int Searchlist_tab2;
  int Searchlist_tab3;
  int Searchlist_tab4;
// FONTS ---------------------------------------
 std::string FONTBUTTONNAME;
 int FONTBUTTONSIZE;
 std::string FONTGENRENAME;
 int FONTGENRESIZE;
 std::string FONTINFONAME;
 int FONTINFOSIZE;
 std::string FONTINTERPRETNAME;
 int FONTINTERPRETSIZE;
 std::string FONTLISTNAME;
 int FONTLISTSIZE;
 std::string FONTSTATUSNAME;
 int FONTSTATUSSIZE;
 std::string FONTSYMBOLNAME;
 int FONTSYMBOLSIZE;
 std::string FONTTITLENAME;
 int FONTTITLESIZE;
 std::string FONTTOPNAME;
 int FONTTOPSIZE;
  
};

extern cMP3Skin MP3Skin;

#endif //___SKIN_H
