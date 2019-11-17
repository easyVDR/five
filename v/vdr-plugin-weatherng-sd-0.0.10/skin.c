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

clrTopBG            =0xFF1C2128;
clrTopFG            =0xFFA2F828;
clrBG1              =0xFF26303A;
clrBG2              =0xFF0C0C0C;
clrHighFG           =0xFFF2A00C;
clrIconFG           =0xFFF2A00C;
clrTextFG           =0xFF9BBAD7;
clrDateFG           =0xFF9BBAD7;
clrStatusBG         =0xFF1C2128;
clrStatusFG         =0xFFCE7B00;
clrStatusRed        =0xFFC00000;
clrStatusGreen      =0xFF00FF00;
clrStatusYellow     =0xFFE0E222;
clrStatusBlue       =0xFF3B96FD;
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
  datei = datei + "/themes/";
  datei = datei + skinname;

  dsyslog("weatherng-sd: Load themefile '%s'\n", datei.c_str());

  filestr.open (datei.c_str());
  if(filestr) {
    while (getline(filestr, line, '\n')) {
      int len = line.length();
      string::size_type pos = line.find ("<value>",0);

      if(pos != string::npos) {
        Value = line.substr(len -10, len);

        if      (strstr(line.c_str(),"clrTopBG"))           clrTopBG             = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrTopFG"))           clrTopFG             = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),"clrBG1"))             clrBG1               = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrBG2"))             clrBG2               = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),"clrHighFG"))          clrHighFG            = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrIconFG"))          clrIconFG            = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrTextFG"))          clrTextFG            = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrDateFG"))          clrDateFG            = strtoul(Value.c_str(), NULL,16);

        else if (strstr(line.c_str(),"clrStatusBG"))        clrStatusBG          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusFG"))        clrStatusFG          = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusRed"))       clrStatusRed         = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusGreen"))     clrStatusGreen       = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusYellow"))    clrStatusYellow      = strtoul(Value.c_str(), NULL,16);
        else if (strstr(line.c_str(),"clrStatusBlue"))      clrStatusBlue        = strtoul(Value.c_str(), NULL,16);

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
  datei = datei + "/themes/";
  datei = datei + ThemeName;

  dateiout = config_path;
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
