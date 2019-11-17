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
  int clrTopBG;
  int clrTopFG;
  int clrBG1;
  int clrBG2;
  int clrHighFG;
  int clrIconFG;
  int clrTextFG;
  int clrDateFG;
  int clrStatusBG;
  int clrStatusFG;
  int clrStatusRed;
  int clrStatusGreen;
  int clrStatusYellow;
  int clrStatusBlue;
  int imgalpha;
};

extern cWeatherSkin WeatherSkin;

#endif //___SKIN_H
