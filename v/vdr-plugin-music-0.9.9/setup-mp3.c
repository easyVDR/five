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

#include <string.h>

#include "common.h"
#include "setup-mp3.h"

cMP3Setup MP3Setup;

// --- cMP3Setup ---------------------------------------------------------------

cMP3Setup::cMP3Setup(void)
{
  InitLoopMode = 0;
  InitShuffleMode = 0;
  AudioMode = 1;
  BgrScan = 1;
  EditorMode = 1;
  UseDeviceStillPicture = 1;
  MenuMode = 2;
  TargetLevel = DEFAULT_TARGET_LEVEL;
  LimiterLevel = DEFAULT_LIMITER_LEVEL;
  Only48kHz = 0;
  ConnectTimeout = 5;
  UseProxy = 0;
  strncpy(ProxyHost,"localhost",sizeof(ProxyHost));
  ProxyPort = 8080;
  UseCddb = 1;
  strncpy(CddbHost,"freedb.freedb.org",sizeof(CddbHost));
  CddbPort = 8880;
  AudioOutMode = 0;
  AbortAtEOL = 0;
  HideMainMenu = 0;
  InstantPlay = 0;
  KeepSelect = 0;
  ExitClose = 1;
  BgrMode = 1;
  strncpy(CopyDir,"/tmp",sizeof(CopyDir));
  strncpy(RecordDir,"/media/easyvdr01/mp3/stream-ripper",sizeof(RecordDir));
  strncpy(RecordOpts,"-r 127.0.0.1:8080",sizeof(RecordOpts));
  strncpy(CoverDir,"/tmp",sizeof(CoverDir));
  strncpy(ReplaceString,"none",sizeof(ReplaceString));
  ShowMessage = 0;
  AdminMode = 0;
  EnableRating = 1;
  strncpy(RateCompat,"no@email",sizeof(RateCompat));
  RateInFile = 0;
  RatingFirst = 0;
  Jumptime = 10;
  OSDoffsetx = 0;
  OSDoffsety = 0;
  ArtistFirst = 1;
  Coversize = 3;
//  ImgAlpha = 255;
  ImgDither = 0;
  EnableVis = 0;
  FalloffSA = 14;
//  IsHDTheme = 1;
//  UseOneArea = 0;

#ifdef HAVE_MAGICK
  MaxCacheFill = 10;
  ImgTreeDepth = 4;
#endif

  TimerEnabled = 0;
  TimerShutdownTime = 0;
  TimerActiveTime = 0;

  EnableShutDown = 0;
//  AutoPurge = 1;
  showcoveronly = 0;
  isWebstream = 0;
  RecordStream = 0;
  DeleteTracks = 0;
  DeleteTrack = 0;
  user_is_admin = 0;  
  ShowStatus = 0;
  ShowRatingInTracklist = 1;
  First_Time_Run = 1;
  PlayerActive =0;
  
  ShoutcastLimit = 10;
  ShoutcastBitrate = 6;
  strncpy(WebstreamDat,"/var/lib/vdr/plugins/moronsuite/music/language/german/data",sizeof(WebstreamDat));
  
  VizRunning = 0;
};
