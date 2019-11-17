#ifndef __OSDWEATHER_H
#define __OSDWEATHER_H

#include <string>
#include <vdr/plugin.h>
#include <vdr/status.h>
#include "setup.h"


class cWetterOsd : public cOsdObject {
private:
  cOsd *osd;
  
  const cFont *font;
  
  std::string file;
  std::string sat_dir;
  std::string sat_file;
  std::string data_file;
  std::string ausgabe;
  std::string Radarmap;

  int osdleft, osdtop, osdwidth, osdheight, fw, fh;
  int part, colordepth, areadepth, cw, showbuttons, radar;
  bool IsSatelite, show_wait;

  int  day;
  int  Radar_left;
  int  Radar_top;
  int  Radar_width;
  int  Radar_height;

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

  void ShowStatusButtons(int ShowButtons);
  void ChangeRadar(int Radar);
  
public:
  cWetterOsd(void);
  ~cWetterOsd();
  void Satelite(void);
  void GetData(void);
  void SetVars(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__OSDWEATHER_H
