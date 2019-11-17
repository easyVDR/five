#ifndef __MP3CONTROL_H
#define __MP3CONTROL_H

#include <string>
#include <vdr/menuitems.h>
#include <vdr/status.h>
#include <vdr/skins.h>

#include "player-mp3.h"
#include "commands.h"
#include "rating.h"
#include "tracklist.h"


class cMP3Control : public cControl, public cStatus {
private:
  cOsd *osd;
  std::string visimagefile;
  std::string _message;
 
  int fh, sfh, rows;
  //
  cMP3Player   *player;

  cMP3Commands *cmdMenu;
  cMP3Rating   *rateMenu;
  cTrackList   *trackList;

  bool copyfile, visible, refresh, flush, skiprew, skipfwd;
  int th, ch, cw, ih, lh, sh, coversize, sw, lastkeytime, num, number, playstatus, laststatus;
  bool selecting, artistfirst, muted, showmessage, gtft_exist;
  //
  cMP3PlayInfo *lastMode;
  time_t fliptime, listtime;
  int flip, flipint, osdwidth, osdtop, osdleft, osdheight, showbuttons;
  int coverh,coverw,coverx,covery,covercolor;
  int lastIndex, lastTotal;
  int framesPerSecond;
  int vol;
  
  char lastfile[256];
  char oldcoverpicture[256];
  
  char *rating;
  //
  bool jumpactive, jumpsecs, ShowSA, force;
  int jumpmm, channelsSA, bandsSA, visualization, channel1left, channel2left, barwidth;

  //
  int clrBG;
  int clrTopBG;
  int clrTopFG;
  int clrCoverBG;
  int clrCoverBar;
  int clrArtistBG;
  int clrArtistFG;
  int clrRatingFG;
  int clrPlayStatusBG;
  int clrPlayStatusFG;
  int clrInfoBG;
  int clrInfoFG;
  int clrProgressbarBG;
  int clrProgressbarFG;
  int clrListTitle;
  int clrListBG;
  int clrListFG;
  int clrListRating;
  int clrSymbolBG;
  int clrSymbolFG;
  int clrSymbolActive;
  int clrRecordingActive;
  int clrVolumeActive;
  int clrVolumeProgressbarBG;
  int clrVolumeProgressbarFG; 
  int clrStatusBG;
  int clrStatusFG1;
  int clrStatusFG2;
  int clrStatusFG3;
  int clrStatusFG4;
  int clrStatusRed;
  int clrStatusGreen;
  int clrStatusYellow;
  int clrStatusBlue;
  int imgalpha;
  int osdalpha;
  //
  const cFont *pFontTop;
  const cFont *pFontInterpret;
  const cFont *pFontGenre;
  const cFont *pFontStatus;
  const cFont *pFontInfo;
  const cFont *pFontTitle;
  const cFont *pFontList;
  const cFont *pFontButton;
  const cFont *pFontSymbol;
  //
  void ShowSpectrumAnalyzer(void);
  void ShowCoverOnly(bool open=false);
  void ShowHelpButtons(int ShowButtons);
  void ConvertRatingToChar(int Posx, int Posy, int value, int bgClr, int fgClr, int fontheight, int fontwidth);
  void SetVars(void);
  void SetFonts(void);
  void SetVol(void);
  void ShowTracks(void);
  bool BuildAreas(void);
  void BuildDefault(void);
  void ShowProgress(bool open=false);
  void DisplayInfo(const char *s=0);
  void JumpDisplay(void);
  void JumpProcess(eKeys Key);
  void Jump(void);
  void ShowMessage(int Message, bool open);  
  void Stop(void);
  void LoadCover(void);
  void LoadBackground(void);
  void LoadVisCover(void);
  void CopyTrack(void);
  inline void Flush(void);
  void ShutDown(void);
  eOSState MessageProcess(eKeys Key);

  // GraphTFT stuff
  void sendPlaylistItem(const char* item, int index, int count);
  void sendHelpButtons(const char* red, const char* green, const char* yellow, const char* blue);
  
public:
  cMP3Control(void);
  virtual ~cMP3Control();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Hide(void);
  bool Visible(void) { return visible; }
  static bool SetPlayList(cPlayList *plist);
};

#endif // __MP3CONTROL_H
