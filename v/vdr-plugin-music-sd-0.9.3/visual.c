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

#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vdr/plugin.h>

#include "i18n.h"
#include "visual.h"
#include "setup-mp3.h"
#include "skin.h"
#include "vars.h"

cMP3VisLoader MP3VisLoader;

// --- cMP3VisLoader ---------------------------------------------------------------

cMP3VisLoader::cMP3VisLoader(void)
{

  Visualization = "4";
  ChannelsSA = "2";
  BandsSA = "30";
  Background ="";
};


cMP3VisLoader::~cMP3VisLoader()
{
}

int cMP3VisLoader::LoadVis(const char *VisName)
{
  using namespace std;
  ifstream filestr;
  std::string line;
  std::string Value;
  std::string datei;
  std::string data;
  std::string visname;

  bool result=false;
  
  visname = VisName;

  Background = "";

  datei = config;
  data  = datei;
  data  = data  + "/visual/data/";
  datei = datei + "/visual/themes/";
  datei = datei + visname;

  filestr.open (datei.c_str());
  if(filestr) {

    if (MP3Skin.isHDTheme) {
      dsyslog("music-sd: Load HD visfile '%s'\n", datei.c_str());
      while (getline(filestr, line, '\n')) {
        int len = line.length();
        string::size_type pos = line.find ("<value>",0);

        if(pos != string::npos) {
          if (strstr(line.c_str(),".HD_ID.")) {
            Value   = line.substr(len -1, len);
            Visualization  = Value;
          }
          else if (strstr(line.c_str(),".HD_CHANNEL.")) {
            Value   = line.substr(len -1, len);
            ChannelsSA  = Value;
	  }
          else if (strstr(line.c_str(),".HD_BARS.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      BandsSA = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".HD_CHANNEL1LEFT.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      Channel1Left = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".HD_CHANNEL2LEFT.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      Channel2Left = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".HD_BARWIDTH.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      BarWidth = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".HD_BACKGROUND.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      data    = data + Value;
	      Background = data;
	    }     
          }    
        }
      }
    }
    else {
      dsyslog("music-sd: Load visfile '%s'\n", datei.c_str());
      while (getline(filestr, line, '\n')) {
        int len = line.length();
        string::size_type pos = line.find ("<value>",0);

        if(pos != string::npos) {
          if (strstr(line.c_str(),".ID.")) {
            Value   = line.substr(len -1, len);
            Visualization  = Value;
          }
          else if (strstr(line.c_str(),".CHANNEL.")) {
            Value   = line.substr(len -1, len);
            ChannelsSA  = Value;
	  }
          else if (strstr(line.c_str(),".BARS.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      BandsSA = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".CHANNEL1LEFT.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      Channel1Left = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".CHANNEL2LEFT.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      Channel2Left = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".BARWIDTH.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      BarWidth = Value;
	    }     
          }    
          else if (strstr(line.c_str(),".BACKGROUND.")) {
            pos = line.rfind ("=",len);
            if(pos != string::npos) {
	      Value   = line.substr(pos +1,len);
	      data    = data + Value;
	      Background = data;
	    }     
          }    
        }
      }
    }
    filestr.close();
    result = true;
  }

  return result;
}

void cMP3VisLoader::SetVis(void)
{

  printf("Visualization =%s\n", Visualization.c_str());
  printf("Channels      =%s\n", ChannelsSA.c_str());
  printf("Bands         =%s\n", BandsSA.c_str());
  printf("Channel1Left  =%s\n", Channel1Left.c_str());
  printf("Channel2Left  =%s\n", Channel2Left.c_str());
  printf("BarWidth      =%s\n", BarWidth.c_str());


  MP3Setup.Visualization  = atoi(Visualization.c_str());
  MP3Setup.ChannelsSA     = atoi(ChannelsSA.c_str());
  MP3Setup.BandsSA        = atoi(BandsSA.c_str());
  MP3Setup.Channel1Left   = atoi(Channel1Left.c_str());
  MP3Setup.Channel2Left   = atoi(Channel2Left.c_str());
  MP3Setup.BarWidth       = atoi(BarWidth.c_str());
}

int cMP3VisLoader::StoreVis(const char *Vis)
{
  using namespace std;
  ifstream filestr;
  std::string line;
  std::string datei;
  std::string dateiout;
  std::string vis;
 
  bool res=false;
  
  vis = Vis;
  
  datei = config;
  datei = datei + "/visual/themes/";
  datei = datei + vis;
  
  dateiout = config;
  dateiout = dateiout + "/visual/themes/current.vis";
  
  if( FILE *f = fopen(dateiout.c_str(), "w")) {
    filestr.open(datei.c_str());
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
