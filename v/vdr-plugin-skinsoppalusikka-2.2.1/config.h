/*
 * config.h: 'Soppalusikka' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINSOPPALUSIKKA_CONFIG_H
#define __SKINSOPPALUSIKKA_CONFIG_H

class cSoppalusikkaConfig {
private:
  cString logoDirM;
  unsigned int traceModeM;
  int showAuxInfoM;
  int showLogoM;
  int showVpsM;
  int showDurationM; // 0 = remaining; 1 = total
  int showSymbolsM;
  int showProgressBarM;
  int cacheSizeM;

public:
  enum eTraceMode {
    eTraceModeNormal  = 0x0000,
    eTraceModeDebug1  = 0x0001,
    eTraceModeDebug2  = 0x0002,
    eTraceModeDebug3  = 0x0004,
    eTraceModeDebug4  = 0x0008,
    eTraceModeDebug5  = 0x0010,
    eTraceModeDebug6  = 0x0020,
    eTraceModeDebug7  = 0x0040,
    eTraceModeDebug8  = 0x0080,
    eTraceModeDebug9  = 0x0100,
    eTraceModeDebug10 = 0x0200,
    eTraceModeDebug11 = 0x0400,
    eTraceModeDebug12 = 0x0800,
    eTraceModeDebug13 = 0x1000,
    eTraceModeDebug14 = 0x2000,
    eTraceModeDebug15 = 0x4000,
    eTraceModeDebug16 = 0x8000,
    eTraceModeMask    = 0xFFFF
  };
  cSoppalusikkaConfig();
  const char *GetLogoDir(void) { return logoDirM; }
  unsigned int GetTraceMode(void) const { return traceModeM; }
  bool IsTraceMode(eTraceMode modeP) const { return (traceModeM & modeP); }
  int GetShowAuxInfo(void) { return showAuxInfoM; }
  int GetShowLogo(void) { return showLogoM; }
  int GetShowVps(void) { return showVpsM; }
  int GetShowDuration(void) { return showDurationM; }
  int GetShowSymbols(void) { return showSymbolsM; }
  int GetShowProgressBar(void) { return showProgressBarM; }
  int GetCacheSize(void) const { return cacheSizeM; }

  void SetLogoDir(const char *logoDirP);
  void SetTraceMode(unsigned int modeP) { traceModeM = (modeP & eTraceModeMask); }
  void SetShowAuxInfo(int onoffP) { showAuxInfoM = onoffP; }
  void SetShowLogo(int onoffP) { showLogoM = onoffP; }
  void SetShowVps(int onoffP) { showVpsM = onoffP; }
  void SetShowDuration(int onoffP) { showDurationM = onoffP; }
  void SetShowSymbols(int onoffP) { showSymbolsM = onoffP; }
  void SetShowProgressBar(int onoffP) { showProgressBarM = onoffP; }
  void SetCacheSize(int cacheSizeP) { cacheSizeM = cacheSizeP; }
};

extern cSoppalusikkaConfig SoppalusikkaConfig;

#endif // __SKINSOPPALUSIKKA_CONFIG_H
