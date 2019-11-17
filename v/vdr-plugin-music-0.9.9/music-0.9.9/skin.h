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
//
public:
  cMP3Skin(void);
  virtual ~cMP3Skin();
  int ParseSkin(const char *SkinName);
  int StoreSkin(const char *ThemeName);
  int ReloadFonts;
// COLORS
  int clrTextBG;
  int clrArtist;
  int clrRating;
  int clrRatingBG;
  int clrPlayStatus;
  int clrInfo;
  int clrProgressbarBG;
  int clrProgressbarFG;
  int clrListTitle;
  int clrList;
  int clrListRating;
  int clrListRatingBG;
  int clrSymbol;
  int clrSymbolActive;
  int clrRecordingActive;
  int clrVolumeActive;
  int clrVolumeProgressbarBG;
  int clrVolumeProgressbarFG;
  int clrStatusRed;
  int clrStatusGreen;
  int clrStatusYellow;
  int clrStatusBlue;

// GENERAL -------------------------------------
  std::string Description;
  std::string SkinPath;
  int OsdWidth;
  int OsdHeight;
  int OsdTop;
  int OsdLeft;
  int OsdAlpha;
  std::string OsdBgrd;
  std::string LocalCover;
  std::string StreamCover;
  int TrackCount;
  int FontHeight;
  int SmallFontHeight;

// TRACKLIST_TAB -------------------------------
  int Tracklist_tab1;
  int Tracklist_tab2;
  int Tracklist_tab3;
  int Tracklist_tab4;

// SEARCHLIST_TAB ------------------------------
  int Searchlist_tab1;
  int Searchlist_tab2;
  int Searchlist_tab3;
  int Searchlist_tab4;

// OBJECTS && PIXMAPS --------------------------
  int CoverOnly_Layer;
  int CoverOnly_Left;
  int CoverOnly_Top;
  int CoverOnly_Width;
  int CoverOnly_Height;

// BgrdPixmap ----------------------------------
  int Bgrd_Layer;
  int Bgrd_Left;
  int Bgrd_Top;
  int Bgrd_Width;
  int Bgrd_Height;

// PlaystatusPixmap ----------------------------
  int Playstatus_Layer;
  int Playstatus_Left;
  int Playstatus_Top;
  int Playstatus_Width;
  int Playstatus_Height;

// CoverPixmap ---------------------------------
  int Cover_Layer;
  int Cover_Left;
  int Cover_Top;
  int Cover_Width;
  int Cover_Height;

// RatingPixmap --------------------------------
  int Rating_Layer;
  int Rating_Left;
  int Rating_Top;
  int Rating_Width;
  int Rating_Height;
  int Rating_ImageWidth;
  int Rating_ImageHeight;

// AlbumPixmap ---------------------------------
  int Album_Layer;
  int Album_Left;
  int Album_Top;
  int Album_Width;
  int Album_Height;

// GenrePixmap ---------------------------------
  int Genre_Layer;
  int Genre_Left;
  int Genre_Top;
  int Genre_Width;
  int Genre_Height;

// YearPixmap ----------------------------------
  int Year_Layer;
  int Year_Left;
  int Year_Top;
  int Year_Width;
  int Year_Height;

// ModePixmap ----------------------------------
  int Mode_Layer;
  int Mode_Left;
  int Mode_Top;
  int Mode_Width;
  int Mode_Height;
  int Mode_Align;

// NumberPixmap --------------------------------
  int Number_Layer;
  int Number_Left;
  int Number_Top;
  int Number_Width;
  int Number_Height;
  int Number_Align;

// VolumeBGPixmap --------------------------------
  int VolumeBG_Layer;
  int VolumeBG_Left;
  int VolumeBG_Top;
  int VolumeBG_Width;
  int VolumeBG_Height;

// VolumeFGPixmap --------------------------------
  int VolumeFG_Layer;
  int VolumeFG_Left;
  int VolumeFG_Top;
  int VolumeFG_Width;
  int VolumeFG_Height;
  int VolumeFG_Slider;
  int VolumeFG_Scale;
  
// VolumePixmap --------------------------------
  int Volume_Layer;
  int Volume_Left;
  int Volume_Top;
  int Volume_Width;
  int Volume_Height;

// IconsPixmap ---------------------------------
  int Symbol_Layer;
  int Symbol_Left;
  int Symbol_Top;
  int Symbol_Width;
  int Symbol_Height;

  int symbol_width;
  int symbol_height;
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

// TracksPixmap --------------------------------
  int Tracks_Layer;
  int Tracks_Left;
  int Tracks_Top;
  int Tracks_Width;
  int Tracks_Height;
  int Tracks_TextHeight;
  int Tracks_TextWidth;
  int Tracks_TextPuffer;


// TextRatingPixmap --------------------------------
  int TracksRating_Layer;
  int TracksRating_Left;
  int TracksRating_Top;
  int TracksRating_Width;
  int TracksRating_Height;
  int TracksRating_ImageWidth;
  int TracksRating_ImageHeight;
  int TracksRating_TextPuffer;

// ArtistPixmap --------------------------------
  int Artist_Layer;
  int Artist_Left;
  int Artist_Top;
  int Artist_Width;
  int Artist_Height;

// TitlePixmap ---------------------------------
  int Title_Layer;
  int Title_Left;
  int Title_Top;
  int Title_Width;
  int Title_Height;
  int Title_Align;

// ProgressBGPixmap --------------------------------
  int ProgressBG_Layer;
  int ProgressBG_Left;
  int ProgressBG_Top;
  int ProgressBG_Width;
  int ProgressBG_Height;
  
// ProgressFGPixmap --------------------------------
  int ProgressFG_Layer;
  int ProgressFG_Left;
  int ProgressFG_Top;
  int ProgressFG_Width;
  int ProgressFG_Height;
  int ProgressFG_Slider;
  int ProgressFG_Scale;
  
// ProgressPixmap ------------------------------
  int Progress_Layer;
  int Progress_Left;
  int Progress_Top;
  int Progress_Width;
  int Progress_Height;
  int Timex;
  int Timey;
  int Timew;
  int Totalx;
  int Totaly;
  int Totalw;

// ButtonPixmap --------------------------------
  int Button_Layer;
  int Button_Left;
  int Button_Top;
  int Button_Width;
  int Button_Height;
  int Button_Align;
  int ButtonText1posx;
  int ButtonText2posx;
  int ButtonText3posx;
  int ButtonText4posx;
  int ButtonText1posy;
  int ButtonText2posy;
  int ButtonText3posy;
  int ButtonText4posy;
  int Button1w;
  int Button2w;
  int Button3w;
  int Button4w;


// FONTS ---------------------------------------
 std::string FONTSTATUSNAME;
 int FONTSTATUSSIZE;

 std::string FONTRATINGNAME;
 int FONTRATINGSIZE;

 std::string FONTGENRENAME;
 int FONTGENRESIZE;

 std::string FONTINFONAME;
 int FONTINFOSIZE;

 std::string FONTSYMBOLNAME;
 int FONTSYMBOLSIZE;

 std::string FONTLISTNAME;
 int FONTLISTSIZE;

 std::string FONTLISTRATINGNAME;
 int FONTLISTRATINGSIZE;

 std::string FONTINTERPRETNAME;
 int FONTINTERPRETSIZE;

 std::string FONTTITLENAME;
 int FONTTITLESIZE;

 std::string FONTBUTTONNAME;
 int FONTBUTTONSIZE;
};

extern cMP3Skin MP3Skin;

#endif //___SKIN_H
