#ifndef __OSDWEATHER_H
#define __OSDWEATHER_H

#include <string>
#include <vdr/plugin.h>
#include <vdr/status.h>
#include "setup.h"
#include "parsing.h"


class cWetterOsd : public cOsdObject {
private:
  cOsd *osd;
  cxmlParse parser;

  cPixmap  *BgrdPixmap;
  cPixmap  *RadarPixmap;
  cPixmap  *PlaystatusPixmap;
  cPixmap  *Icon1Pixmap;
  cPixmap  *Icon2Pixmap;
  cPixmap  *Icon3Pixmap;
  cPixmap  *StatusPixmap;
  cPixmap  *HighTempPixmap;
  cPixmap  *LowTempPixmap;
  cPixmap  *FeelTempPixmap;
  cPixmap  *WindPixmap;
  cPixmap  *UVPixmap;
  cPixmap  *RainPixmap;
  cPixmap  *ThunderPixmap;
  cPixmap  *DatePixmap;
  cPixmap  *ButtonPixmap;
  cPixmap  *SecondDayPixmap;
  cPixmap  *ThirdDayPixmap;

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

  const cFont *pFontStatus;
  const cFont *pFontHighTemp;
  const cFont *pFontLowTemp;
  const cFont *pFontFeelTemp;
  const cFont *pFontWind;
  const cFont *pFontUV;
  const cFont *pFontRain;
  const cFont *pFontThunder;
  const cFont *pFontDate;
  const cFont *pFontText;
  const cFont *pFontButton;
  const cFont *pFontSecondDay;
  const cFont *pFontThirdDay;

  std::string file;
  std::string sat_dir;
  std::string sat_file;
  std::string data_file;
  std::string ausgabe;
  std::string DataDir;
  std::string Radarmap;

  std::string Theme;
  std::string BigIconTheme;
  std::string SmallIconTheme;

  int osdleft, osdtop, osdwidth, osdheight, fh, sfh;
  int part, showbuttons, radar;
  bool IsSatelite, show_wait;

  int  day;

  void ShowStatusButtons(int ShowButtons);
  void ChangeRadar(int Radar);

  void SetFonts(void);
  void LoadBackground(void);
  bool BuildAreas(void);
  void BuildDefault(void);  
  void ClearPixmaps(void);
  void ShowSecondDay(int day2);
  void ShowThirdDay(int day3);
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
