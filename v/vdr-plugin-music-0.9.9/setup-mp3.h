/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#ifndef ___SETUP_MP3_H
#define ___SETUP_MP3_H

extern const char *cddbpath;
extern const char *netscript;
extern const char *instantplay;
extern const char *dspdevice;
extern const char *alsadevice;
// ----------------------------------------------------------------

#define DEFAULT_TARGET_LEVEL  25
#define MAX_TARGET_LEVEL      50
#define DEFAULT_LIMITER_LEVEL 70
#define MIN_LIMITER_LEVEL     25

#define MAX_HOSTNAME 128
#define MAX_FILEPATH 256
#define AUDIOOUTMODES 3
#define AUDIOOUTMODE_DVB  0
#define AUDIOOUTMODE_OSS  1
#define AUDIOOUTMODE_ALSA 2

class cMP3Setup {
public:
  cMP3Setup(void);
  int InitLoopMode;
  int InitShuffleMode;
  int AudioMode;
  int BgrScan;
  int EditorMode;
  int UseDeviceStillPicture;
  int MenuMode;
  int TargetLevel;
  int LimiterLevel;
  int Only48kHz;
  int ConnectTimeout;
  int UseProxy;
  char ProxyHost[MAX_HOSTNAME];
  int ProxyPort;
  int UseCddb;
  char CddbHost[MAX_HOSTNAME];
  int CddbPort;
  int AudioOutMode;
  int AbortAtEOL;
  int HideMainMenu;
  int InstantPlay;
  int KeepSelect;
  int ExitClose;
  int BgrMode;
  char CopyDir[MAX_FILEPATH];
  char RecordDir[MAX_FILEPATH];
  char RecordOpts[MAX_FILEPATH];
  char CoverDir[MAX_FILEPATH];
  char ReplaceString[MAX_FILEPATH];
  int Jumptime;
  int AdminMode;
  int ShowMessage;
  int EnableRating;
  char RateCompat[MAX_FILEPATH];
  int RateInFile;
  int RatingFirst;
  int OSDoffsetx;
  int OSDoffsety;
  int ArtistFirst;
  int Coversize;
//  int ImgAlpha;
  int ImgDither;
  int EnableVis;
  int FalloffSA;
//  int IsHDTheme;
//  int UseOneArea;

#ifdef HAVE_MAGICK
  int MaxCacheFill;
  int ImgTreeDepth;
#endif

  int TimerEnabled;
  int TimerShutdownTime;
  int TimerActiveTime;

  int EnableShutDown;
//  int AutoPurge;
  int showcoveronly;
  int isWebstream;
  int RecordStream;
  int DeleteTracks;
  int DeleteTrack;
  int user_is_admin;
  int ShowStatus;
  int ShowRatingInTracklist;
  
  int First_Time_Run;
  int PlayerActive;

  int ShoutcastLimit;
  int ShoutcastBitrate;
  char WebstreamDat[MAX_FILEPATH];


  int VizRunning;


  };

extern cMP3Setup MP3Setup;

#endif //___SETUP_MP3_H
