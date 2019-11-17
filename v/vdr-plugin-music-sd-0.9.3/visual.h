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

#ifndef ___MP3VisLoader_H
#define ___MP3VisLoader_H

#include <string> 

class cMP3VisLoader {
private:
  std::string Visualization;
  std::string Channel1Left;
  std::string Channel2Left;
  std::string BarWidth;
  std::string ChannelsSA;
  std::string BandsSA;
  std::string Background;


public:
  cMP3VisLoader(void);
  virtual ~cMP3VisLoader();
  int LoadVis(const char *VisName);
  void SetVis(void);
  int StoreVis(const char *Vis);

  inline const char *VisLoaderBackground(void) const { return Background.c_str(); }
};

extern cMP3VisLoader MP3VisLoader;

#endif //___MP3VisLoader_H
