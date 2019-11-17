/*
 * weather-plugin for VDR (C++)
 *
 * (C) 2008 Morone
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

class cWeatherSkin {
private:

public:
  cWeatherSkin(void);
  virtual ~cWeatherSkin();
  int ParseSkin(const char *SkinName);
  int StoreSkin(const char *ThemeName);
  int ReloadFonts;
// COLORS
  int clrStatusFG;
  int clrTextBG;
  int clrTextFG;
  int clrHighTempFG;
  int clrLowTempFG;
  int clrFeelTempFG;
  int clrWindFG;
  int clrUVFG;
  int clrRainFG;
  int clrThunderFG;
  int clrDateFG;
  int clrStatusRedFG;
  int clrStatusGreenFG;
  int clrStatusYellowFG;
  int clrStatusBlueFG;

// GENERAL -------------------------------------
  std::string Theme;
  std::string BigIconTheme;
  std::string SmallIconTheme;
  int OsdWidth;
  int OsdHeight;
  int OsdTop;
  int OsdLeft;
  int OsdAlpha;
  std::string OsdBgrd;
  int FontHeight;
  int SmallFontHeight;

// BgrdPixmap ----------------------------------
  int Bgrd_Layer;
  int Bgrd_Left;
  int Bgrd_Top;
  int Bgrd_Width;
  int Bgrd_Height;

// RadarPixmap ----------------------------------
  int Radar_Layer;
  int Radar_Left;
  int Radar_Top;
  int Radar_Width;
  int Radar_Height;
  
// Icon1Pixmap ---------------------------------
  int Icon1_Layer;
  int Icon1_Left;
  int Icon1_Top;
  int Icon1_Width;
  int Icon1_Height;  
  
// Icon2Pixmap ---------------------------------
  int Icon2_Layer;
  int Icon2_Left;
  int Icon2_Top;
  int Icon2_Width;
  int Icon2_Height;  

// Icon3Pixmap ---------------------------------
  int Icon3_Layer;
  int Icon3_Left;
  int Icon3_Top;
  int Icon3_Width;
  int Icon3_Height;  

// StatusPixmap ----------------------------------
  int Status_Layer;
  int Status_Left;
  int Status_Top;
  int Status_Width;
  int Status_Height;
  int Status_Align;

// HighTempPixmap ----------------------------------
  int HighTemp_Layer;
  int HighTemp_Left;
  int HighTemp_Top;
  int HighTemp_Width;
  int HighTemp_Height;
  int HighTemp_Align;

// LowTempPixmap ----------------------------------
  int LowTemp_Layer;
  int LowTemp_Left;
  int LowTemp_Top;
  int LowTemp_Width;
  int LowTemp_Height;
  int LowTemp_Align;

// FeelTempPixmap ----------------------------------
  int FeelTemp_Layer;
  int FeelTemp_Left;
  int FeelTemp_Top;
  int FeelTemp_Width;
  int FeelTemp_Height;
  int FeelTemp_Align;

// WindPixmap ----------------------------------
  int Wind_Layer;
  int Wind_Left;
  int Wind_Top;
  int Wind_Width;
  int Wind_Height;
  int Wind_Align;

// UVPixmap ----------------------------------
  int UV_Layer;
  int UV_Left;
  int UV_Top;
  int UV_Width;
  int UV_Height;
  int UV_Align;

// RainPixmap ----------------------------------
  int Rain_Layer;
  int Rain_Left;
  int Rain_Top;
  int Rain_Width;
  int Rain_Height;
  int Rain_Align;

// ThunderPixmap ----------------------------------
  int Thunder_Layer;
  int Thunder_Left;
  int Thunder_Top;
  int Thunder_Width;
  int Thunder_Height;
  int Thunder_Align;

// DatePixmap ----------------------------------
  int Date_Layer;
  int Date_Left;
  int Date_Top;
  int Date_Width;
  int Date_Height;
  int Date_Align;

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



// SECONDDAY
  int SecondDay_Layer;
  int SecondDay_Left;
  int SecondDay_Top;
  int SecondDay_Width;
  int SecondDay_Height;
  int SecondDay_Align;
  int SecondDay_HighTemp_Top;
  int SecondDay_LowTemp_Top;
  int SecondDay_FeelTemp_Top;
  int SecondDay_Wind_Top;
  int SecondDay_UV_Top;
  int SecondDay_Rain_Top;
  int SecondDay_Thunder_Top;
  int SecondDay_Date_Top;
  int SecondDay_FontHeight;


// THIRDDAY
  int ThirdDay_Layer;
  int ThirdDay_Left;
  int ThirdDay_Top;
  int ThirdDay_Width;
  int ThirdDay_Height;
  int ThirdDay_Align;
  int ThirdDay_HighTemp_Top;
  int ThirdDay_LowTemp_Top;
  int ThirdDay_FeelTemp_Top;
  int ThirdDay_Wind_Top;
  int ThirdDay_UV_Top;
  int ThirdDay_Rain_Top;
  int ThirdDay_Thunder_Top;
  int ThirdDay_Date_Top;
  int ThirdDay_FontHeight;





// FONTS ---------------------------------------
 std::string FONTSTATUSNAME;
 int FONTSTATUSSIZE;

 std::string FONTHIGHTEMPNAME;
 int FONTHIGHTEMPSIZE;

 std::string FONTLOWTEMPNAME;
 int FONTLOWTEMPSIZE;

 std::string FONTFEELTEMPNAME;
 int FONTFEELTEMPSIZE;

 std::string FONTWINDNAME;
 int FONTWINDSIZE;

 std::string FONTUVNAME;
 int FONTUVSIZE;

 std::string FONTRAINNAME;
 int FONTRAINSIZE;

 std::string FONTTHUNDERNAME;
 int FONTTHUNDERSIZE;

 std::string FONTDATENAME;
 int FONTDATESIZE;

 std::string FONTTEXTNAME;
 int FONTTEXTSIZE;

 std::string FONTBUTTONNAME;
 int FONTBUTTONSIZE;

// SECONDDAY
 std::string FONTSECONDDAYNAME;
 int FONTSECONDDAYSIZE;

// THIRDDAY
 std::string FONTTHIRDDAYNAME;
 int FONTTHIRDDAYSIZE;

 
};

extern cWeatherSkin WeatherSkin;

#endif //___SKIN_H
