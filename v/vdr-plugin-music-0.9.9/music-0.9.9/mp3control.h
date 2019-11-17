#ifndef __MP3CONTROL_H
#define __MP3CONTROL_H

#include <string>
#include <vdr/menuitems.h>
#include <vdr/status.h>
#include <vdr/skins.h>

#include <vdr/osd.h>
#include <vdr/plugin.h>

#include "player-mp3.h"
#include "commands.h"
#include "rating.h"
#include "mp3id3.h"
#include "tracklist.h"


class cMP3Control : public cControl, public cStatus {
private:
  cOsd *osd;
  std::string visimagefile;
  std::string _message;
 
  int fh, sfh, rows;
  //
  cMP3Player   *player;

  cPixmap *BgrdPixmap;  //OK
  cPixmap *PlaystatusPixmap;  //OK
  cPixmap *CoverPixmap; //OK
  cPixmap *VisPixmap; // OK
  cPixmap *RatingPixmap; // OK
  cPixmap *AlbumPixmap; // OK
  cPixmap *GenrePixmap; // OK
  cPixmap *YearPixmap; // OK
  cPixmap *ModePixmap; // OK
  cPixmap *NumberPixmap; // OK
  cPixmap *VolumeBGPixmap; // OK
  cPixmap *VolumeFGPixmap; // OK
  cPixmap *VolumePixmap; // OK
  cPixmap *IconsPixmap; // OK
  cPixmap *TracksPixmap;
  cPixmap *TracksRatingPixmap;
  cPixmap *ArtistPixmap;
  cPixmap *TitlePixmap;
  cPixmap *ProgressBGPixmap;
  cPixmap *ProgressFGPixmap;
  cPixmap *ProgressPixmap;
  cPixmap *ButtonPixmap;
  cPixmap *CoverOnlyPixmap;

  cMP3Commands      *cmdMenu;
  cMP3SearchID3Info *id3infoMenu;
  cMP3Rating        *rateMenu;
  cTrackList        *trackList;

  bool copyfile, visible, refresh, flush, skiprew, skipfwd;
  int th, ch, cw, ih, lh, sh, coversize, lastkeytime, num, number, playstatus, laststatus, isstream;
  bool selecting, artistfirst, muted, showmessage, gtft_exist;
  //
  cMP3PlayInfo *lastMode;
  time_t fliptime, listtime;
  int flip, flipint, osdwidth, osdtop, osdleft, osdheight, showbuttons;
  int coverh,coverw,coverx,covery;
  int lastIndex, lastTotal;
  int framesPerSecond;
  int vol;
  
  char lastfile[256];
  char oldcoverpicture[256];
  
  char *rating;
  //
  bool jumpactive, jumpsecs, ShowSA, force;
  int jumpmm, channelsSA, bandsSA, visualization, channel1left, channel2left, barcolor, barcolor2, barwidth, barheight, barpeakheight, barfalloff;

  //
  int clrTextBG;
  int clrArtist;
  int clrRating;
  int clrRatingBG;
  int clrPlayStatus;
  int clrInfo;
  int clrProgressbarBG;
  int clrProgressbarFG;
  int clrListTitle;
  int clrList;
  int clrListRating;
  int clrListRatingBG;
  int clrSymbol;
  int clrSymbolActive;
  int clrRecordingActive;
  int clrVolumeActive;
  int clrVolumeProgressbarBG;
  int clrVolumeProgressbarFG; 
  int clrStatusRed;
  int clrStatusGreen;
  int clrStatusYellow;
  int clrStatusBlue;
  int ImgAlpha;
  int osdalpha;
  //
  // ####################################################  
  // bitmap  
//  int newheight;
//  int newwidth;

//  bool DrawCover(const char *Filename, int width, int height, bool ratio); 
//  bool DrawCoverOnly(const char *Filename, int width, int height, bool ratio, bool bgrd); 
//  bool Load(const std::string &Filename);
  
  // ###################################################  
  const cFont *pFontStatus;
  const cFont *pFontRating;
  const cFont *pFontGenre;
  const cFont *pFontInfo;
  const cFont *pFontSymbol;
  const cFont *pFontList;
  const cFont *pFontListRating;
  const cFont *pFontInterpret;
  const cFont *pFontTitle;
  const cFont *pFontButton;

  //
  void ShowSpectrumAnalyzer(void);
  void ShowCoverOnly(bool open=false);
  void ShowHelpButtons(int ShowButtons);
  void ConvertRatingToChar_Tracks(int Posx, int Posy, int value, int bgClr, int fgClr, int imagewidth, int imageheight);
  void ConvertRatingToChar_Rating(int Posx, int Posy, int value, int bgClr, int fgClr, int imagewidth, int imageheight);
  void SetVars(void);
  void SetFonts(void);
  void SetVol(void);
  void ShowTracks(void);
  bool BuildAreas(void);
  void BuildDefault(void);
  void DrawIcon(int Icon);
  void DrawProgressBar(int Index, int Total);
  void ShowProgress(bool open=false);
  void DisplayInfo(const char *s);
  void JumpDisplay(void);
  void JumpProcess(eKeys Key);
  void Jump(void);
  void ShowMessage(int Message, bool open);  
  void Stop(void);
  void LoadCoverOnly(void);
  void LoadCover(void);
  void LoadBackground(void);
  void LoadVisCover(void);
  void CopyTrack(void);
  int Tupper(char *q);
  inline void Flush(void);
  void ShutDown(void);
  eOSState MessageProcess(eKeys Key);

  // GraphTFT stuff
  void sendPlaylistItem(const char* item, int index, int count);
  void sendHelpButtons(const char* red, const char* green, const char* yellow, const char* blue);

  char *CheckImage(char *fileName);
  void DefaultImage(void);
  void LoadImage(const char *fullname, const char *artist, bool coveronly);


//  bool RenderImage(const char *Filename, int width, int height, bool ratio, int startx, isProgressbar);
//  int newwidth;
//  int newheight;

public:
  cMP3Control(void);
  virtual ~cMP3Control();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Hide(void);
  bool Visible(void) { return visible; }
  static bool SetPlayList(cPlayList *plist);
};

#endif // __MP3CONTROL_H
