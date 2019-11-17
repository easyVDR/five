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
  std::string BarHeight;
  std::string BarPeakHeight;
  std::string BarFallOff;
  std::string ChannelsSA;
  std::string BandsSA;
  std::string BarColor;
  std::string BarColor2;
  std::string Background;


public:
  cMP3VisLoader(void);
  virtual ~cMP3VisLoader();
  int LoadVis(const char *VisName);
  void SetVis(void);
  int StoreVis(const char *Vis);

  inline const char *VisLoaderBackground(void)    const { return Background.c_str(); }
  inline const char *VisLoaderVisualization(void) const { return Visualization.c_str(); }
  inline const char *VisLoaderChannel1Left(void)  const { return Channel1Left.c_str(); }
  inline const char *VisLoaderChannel2Left(void)  const { return Channel2Left.c_str(); }
  inline const char *VisLoaderBarWidth(void)      const { return BarWidth.c_str(); }
  inline const char *VisLoaderBarHeight(void)     const { return BarHeight.c_str(); }
  inline const char *VisLoaderBarPeakHeight(void) const { return BarPeakHeight.c_str(); }
  inline const char *VisLoaderBarFallOff(void)    const { return BarFallOff.c_str(); }
  inline const char *VisLoaderChannelsSA(void)    const { return ChannelsSA.c_str(); }
  inline const char *VisLoaderBandsSA(void)       const { return BandsSA.c_str(); }
  inline const char *VisLoaderBarColor(void)      const { return BarColor.c_str(); }
  inline const char *VisLoaderBarColor2(void)     const { return BarColor2.c_str(); }

};

extern cMP3VisLoader MP3VisLoader;

#endif //___MP3VisLoader_H
