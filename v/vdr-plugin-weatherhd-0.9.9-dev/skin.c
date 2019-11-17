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

//#include <string>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vdr/plugin.h>

#include "skin.h"
#include "vars.h"

cWeatherSkin WeatherSkin;

// --- cMP3Skin ---------------------------------------------------------------

cWeatherSkin::cWeatherSkin(void)
{
  ReloadFonts = 0;
};


cWeatherSkin::~cWeatherSkin()
{
}

int cWeatherSkin::ParseSkin(const char *SkinName)
{
  
  using namespace std;
  ifstream filestr;
  std::string line;
  std::string Value;
  std::string datei;
  std::string skinname;

  bool result=false;

  skinname = SkinName;

  datei = config_path;
  datei = datei + "/moronsuite/weather/themes/";
  datei = datei + skinname;

  dsyslog("weather: Load themefile '%s'\n", datei.c_str());

  filestr.open (datei.c_str());
  if(filestr) {
    while (getline(filestr, line, '\n')) {
      int len = line.length();
      string::size_type pos = line.find ("<value>",0);

      if(pos != string::npos) {
        Value = line.substr(len -10, len);

// COLORS        
        if      (strstr(line.c_str(),"clrStatusFG"))           clrStatusFG            = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrTextBG"))             clrTextBG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrTextFG"))             clrTextFG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrHighTempFG"))         clrHighTempFG          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrLowTempFG"))          clrLowTempFG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrFeelTempFG"))         clrFeelTempFG          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrWindFG"))             clrWindFG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrUVFG"))               clrUVFG                = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrRainFG"))             clrRainFG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrThunderFG"))          clrThunderFG           = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrDateFG"))             clrDateFG              = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusRedFG"))        clrStatusRedFG         = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusGreenFG"))      clrStatusGreenFG       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusYellowFG"))     clrStatusYellowFG      = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusBlueFG"))       clrStatusBlueFG        = strtoul(Value.c_str(), NULL,16);

// GENERAL
        else if (strstr(line.c_str(),".Theme.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Theme = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".BigIconTheme.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          BigIconTheme = Value.c_str();
	          }     
          }    
        else if (strstr(line.c_str(),".SmallIconTheme.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SmallIconTheme = Value.c_str();
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
// RadarPixmap
        else if (strstr(line.c_str(),".Radar_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Radar_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Radar_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Radar_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Radar_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Radar_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Radar_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Radar_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Radar_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Radar_Height = atoi(Value.c_str());
            }
          }  
// Icon1Pixmap
        else if (strstr(line.c_str(),".Icon1_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon1_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon1_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon1_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon1_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon1_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon1_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon1_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon1_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon1_Height = atoi(Value.c_str());
            }
          }  
// Icon2Pixmap
        else if (strstr(line.c_str(),".Icon2_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon2_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon2_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon2_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon2_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon2_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon2_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon2_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon2_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon2_Height = atoi(Value.c_str());
            }
          }  
// Icon3Pixmap
        else if (strstr(line.c_str(),".Icon3_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon3_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon3_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon3_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon3_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon3_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon3_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon3_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Icon3_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Icon3_Height = atoi(Value.c_str());
            }
          }  
// StatusPixmap
        else if (strstr(line.c_str(),".Status_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Status_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Status_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Status_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Status_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Status_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Status_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Status_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Status_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Status_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Status_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Status_Align = atoi(Value.c_str());
            }
          }  

// HighTempPixmap
        else if (strstr(line.c_str(),".HighTemp_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          HighTemp_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".HighTemp_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          HighTemp_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".HighTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          HighTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".HighTemp_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          HighTemp_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".HighTemp_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          HighTemp_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".HighTemp_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          HighTemp_Align = atoi(Value.c_str());
            }
          }  

// LowTempPixmap
        else if (strstr(line.c_str(),".LowTemp_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LowTemp_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".LowTemp_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LowTemp_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".LowTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LowTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".LowTemp_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LowTemp_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".LowTemp_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LowTemp_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".LowTemp_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          LowTemp_Align = atoi(Value.c_str());
            }
          }  

// FeelTempPixmap
        else if (strstr(line.c_str(),".FeelTemp_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FeelTemp_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".FeelTemp_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FeelTemp_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".FeelTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FeelTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".FeelTemp_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FeelTemp_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".FeelTemp_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FeelTemp_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".FeelTemp_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FeelTemp_Align = atoi(Value.c_str());
            }
          }  

// WindPixmap
        else if (strstr(line.c_str(),".Wind_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Wind_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Wind_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Wind_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Wind_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Wind_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Wind_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Wind_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Wind_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Wind_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Wind_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Wind_Align = atoi(Value.c_str());
            }
          }  

// UVPixmap
        else if (strstr(line.c_str(),".UV_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UV_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".UV_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UV_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".UV_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UV_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".UV_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UV_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".UV_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UV_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".UV_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          UV_Align = atoi(Value.c_str());
            }
          }  
// RainPixmap
        else if (strstr(line.c_str(),".Rain_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rain_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rain_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rain_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rain_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rain_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rain_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rain_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rain_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rain_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Rain_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Rain_Align = atoi(Value.c_str());
            }
          }  
// ThunderPixmap
        else if (strstr(line.c_str(),".Thunder_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Thunder_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Thunder_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Thunder_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Thunder_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Thunder_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Thunder_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Thunder_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Thunder_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Thunder_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Thunder_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Thunder_Align = atoi(Value.c_str());
            }
          }  
// DatePixmap
        else if (strstr(line.c_str(),".Date_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Date_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Date_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Date_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Date_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Date_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Date_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Date_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Date_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Date_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".Date_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          Date_Align = atoi(Value.c_str());
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
// SECONDDAY
        else if (strstr(line.c_str(),".SecondDay_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Align = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_HighTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_HighTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_LowTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_LowTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_FeelTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_FeelTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Wind_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Wind_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_UV_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_UV_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Rain_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Rain_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Thunder_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Thunder_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_Date_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_Date_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".SecondDay_FontHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          SecondDay_FontHeight = atoi(Value.c_str());
            }
          }  
// THIRDDAY
        else if (strstr(line.c_str(),".ThirdDay_Layer.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Layer = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Left.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Left = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Width.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Width = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Height.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Height = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Align.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Align = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_HighTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_HighTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_LowTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_LowTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_FeelTemp_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_FeelTemp_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Wind_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Wind_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_UV_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_UV_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Rain_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Rain_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Thunder_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Thunder_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_Date_Top.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_Date_Top = atoi(Value.c_str());
            }
          }  
        else if (strstr(line.c_str(),".ThirdDay_FontHeight.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          ThirdDay_FontHeight = atoi(Value.c_str());
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
        else if (strstr(line.c_str(),".FONTSTATUSSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSTATUSSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTHIGHTEMPNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTHIGHTEMPNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTHIGHTEMPSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTHIGHTEMPSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTLOWTEMPNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLOWTEMPNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTLOWTEMPSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTLOWTEMPSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTFEELTEMPNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTFEELTEMPNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTFEELTEMPSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTFEELTEMPSIZE = atoi(Value.c_str());
            }
          }
       else if (strstr(line.c_str(),".FONTWINDNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTWINDNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTWINDSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTWINDSIZE = atoi(Value.c_str());
            }
          }
       else if (strstr(line.c_str(),".FONTUVNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTUVNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTUVSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTUVSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTRAINNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTRAINNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTRAINSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTRAINSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTTHUNDERNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTHUNDERNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTTHUNDERSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTHUNDERSIZE = atoi(Value.c_str());
            }
          }
       else if (strstr(line.c_str(),".FONTDATENAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTDATENAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTDATESIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTDATESIZE = atoi(Value.c_str());
            }
          }
       else if (strstr(line.c_str(),".FONTTEXTNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTEXTNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTTEXTSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTEXTSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTBUTTONNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTBUTTONNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTBUTTONSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTBUTTONSIZE = atoi(Value.c_str());
            }
          }
       else if (strstr(line.c_str(),".FONTSECONDDAYNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSECONDDAYNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTSECONDDAYSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTSECONDDAYSIZE = atoi(Value.c_str());
            }
          }

       else if (strstr(line.c_str(),".FONTTHIRDDAYNAME.")) {
          pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTHIRDDAYNAME = Value.c_str();
	          }      
          }    
        else if (strstr(line.c_str(),".FONTTHIRDDAYSIZE.")) {
	        pos = line.rfind ("=",len);
          if(pos != string::npos) {
	          Value   = line.substr(pos +1,len);
	          FONTTHIRDDAYSIZE = atoi(Value.c_str());
            }
          }
      }
    }

    filestr.close();
    result = true;
  }

  return result;
}


int cWeatherSkin::StoreSkin(const char *ThemeName)
{
  using namespace std;
  ifstream filestr;
  std::string line;
  std::string datei;
  std::string dateiout;
  std::string themename;

  bool res=false;
  
  themename = ThemeName;

  datei = config_path;
  datei = datei + "/moronsuite/weather/themes/";
  datei = datei + ThemeName;

  dateiout = config_path;
  dateiout = dateiout + "/moronsuite/weather/themes/current.skin";

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
