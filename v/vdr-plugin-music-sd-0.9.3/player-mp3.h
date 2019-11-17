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

#ifndef ___DVB_MP3_H
#define ___DVB_MP3_H

#include <vdr/thread.h>
#include <vdr/player.h>

#include <mad.h>

#include "config.h"
#include "service.h"

/*
 * Macros.
 */ 
#define PCM_FRAME_SIZE 2048

#define SPAN_PROVIDER_CHECK_ID  "Span-ProviderCheck-v1.0"
#define SPAN_CLIENT_CHECK_ID    "Span-ClientCheck-v1.0"
#define SPAN_SET_PCM_DATA_ID    "Span-SetPcmData-v1.1"
#define SPAN_SET_PLAYINDEX_ID   "Span-SetPlayindex-v1.0"
#define SPAN_GET_BAR_HEIGHTS_ID "Span-GetBarHeights-v1.0"

#define GRAPHTFT_COVERNAME_ID   "GraphTftCovername-v1.0"
#define GRAPHTFT_PLAYLIST_ID    "GraphTftPlaylist-v1.0"
#define GRAPHTFT_STATUS_ID      "GraphTftStatus-v1.0"
#define GRAPHTFT_HELPBUTTONS_ID "GraphTftHelpButtons-v1.0"
#define GRAPHTFT_INFO_ID "GraphTftInfo-v1.0"


//Span requests to collect possible providers / clients
struct Span_Provider_Check_1_0 {
    bool *isActive;
    bool *isRunning;
};
            
struct Span_Client_Check_1_0 {
	bool *isActive;
	bool *isRunning;
};
                        
// SpanData
struct Span_SetPcmData_1_0 {
	unsigned int length;            // the length of the PCM-data
	const unsigned char *data;      // the PCM-Data
	int index;                      // the timestamp (ms) of the frame(s) to be visualized
	unsigned int bufferSize;        // for span-internal bookkeeping of the data to be visualized
	bool bigEndian;                 // are the pcm16-data coded bigEndian?
};

struct Span_SetPlayindex_1_0 {
	int index;                      // the timestamp (ms) of the frame(s) being currently played
};
                                                                        
struct Span_GetBarHeights_v1_0 {
	unsigned int bands;                     // number of bands to compute
	unsigned int *barHeights;               // the heights of the bars of the two channels combined
	unsigned int *barHeightsLeftChannel;    // the heights of the bars of the left channel
	unsigned int *barHeightsRightChannel;   // the heights of the bars of the right channel
	unsigned int *volumeLeftChannel;        // the volume of the left channels
	unsigned int *volumeRightChannel;       // the volume of the right channels
	unsigned int *volumeBothChannels;       // the combined volume of the two channels
	const char *name;                       // name of the plugin that wants to get the data
                                                // (must be unique for each client!)
	unsigned int falloff;                   // bar falloff value
	unsigned int *barPeaksBothChannels;     //bar peaks of the two channels combined
	unsigned int *barPeaksLeftChannel;      // bar peaks of the left channel
	unsigned int *barPeaksRightChannel;     // bar peaks of the right channel   
};

// -------------------------------------------------------------------

class cRingBufferFrame;
class cFrame;
class cPlayList;
class cSong;
class cSongInfo;
class cBackgroundScan;
class cDecoder;
class cOutput;
class cOutputDvb;
class cShuffle;

// -------------------------------------------------------------------

class cMP3PlayInfo {
public:
  char Title[128], Artist[128], Album[128], Genre[32], Comment[128], SMode[32], Filename[256];
  int Year, SampleFreq, Bitrate, MaxBitrate, Rating;
  int Num, MaxNum;
  // not in hash
  bool Loop, Shuffle;
  int Hash;
  };

// -------------------------------------------------------------------

class cPlayManager : public cThread {
private:
  cMutex listMutex;
  cCondVar fgCond, bgCond;
//  cList<cSong> list;
//  cSong *curr;
//  int currIndex, maxIndex;
  //
  cSong *play;
  bool playNew, eol;
  //
  cShuffle *shuffle;
  bool shuffleMode, loopMode;
  //
  cSong *scan;
  bool stopscan, throttle, pass2, release, scanning;
  //
  bool tracksadded;
  //
  virtual void Action(void);
  void NoScan(cSong *nono);
  void ThrottleWait(void);
  void NoPlay(cSong *nono);

  int length;
public:
  cPlayManager(void);
  ~cPlayManager();
  // Control interface (to be called from frontend thread only!)
  int currIndex, maxIndex;
  int currindex;
  bool playnow;
  cSong *curr;
  cList<cSong> list;
  bool ShuffleMode(void) { return shuffleMode; }
  bool LoopMode(void) { return loopMode; }
  bool Eol(void) { return eol; }
  bool TracksAdded(void);
  void Flush(void);
  bool SaveList(const char *pl_filename, bool full);
  void DeleteRecord(int Sidx);
  void EmptyPlaylist(void);
  void Add(cPlayList *pl);
  bool Next(void);
  bool Prev(void);
  void Goto(int num);
  void ToggleShuffle(void);
  void ToggleLoop(void);
  bool Info(int num, cMP3PlayInfo *info);
  int InfoCount() { return list.Count(); }
  void Halt(void);
  // Player interface (to be called from player thread only!)
  cSong *Current(void);
  cSong *Track(int index);
  bool NewCurrent(void);
  bool NextCurrent(void);
  void RateSong(int sz_rating, cSong *r_Song);
  void UpdateSong(const char *sz_artist, const char *sz_album, const char *sz_title, const char *sz_genre, int sz_year, int sz_rating, cSong *r_Song);
  int GetListLength(void);
  void Release(void);
  void Throttle(bool thr);
  bool Scanning(void);
  
  };

extern cPlayManager *mgr;

// -------------------------------------------------------------------

class cMP3Player : public cPlayer, cThread {
friend class cOutputDvb;
private:
  std::string file;
  std::string mpegfile;
  std::string oldmpegfile;
  bool active, started;
  int statusMode;
  int m_size;
  int m_length;
  uchar *m_buffer;
  cRingBufferFrame *ringBuffer;
  cMutex playModeMutex;
  cCondVar playModeCond;
 //
  int total;
  cDecoder *decoder;
  cOutput *output;
  cFrame *rframe, *pframe;
  enum ePlayMode { pmPlay, pmStopped, pmPaused, pmStartup };
  ePlayMode playMode;
  enum eState { msStart, msStop, msDecode, msNormalize, msResample, msOutput, msError, msEof, msWait, msRestart};
  eState state;
  bool levelgood, isStream;
  unsigned int dvbSampleRate;
  
  void Empty(void);
  void StopPlay(void);
  void SetPlayMode(ePlayMode mode);
  void WaitPlayMode(ePlayMode mode, bool inv);
  void DefaultImage(void);
protected:
  virtual void Activate(bool On);
  virtual void Action(void);
public:

  cMP3Player(void);
  virtual ~cMP3Player();
  int playindex;
  void Pause(void);
  void Play(void);
  bool PrevCheck(void);
  void SkipSeconds(int secs);
  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame=false);
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
  bool Active(void) { return active; }
  bool IsStream(void) { return isStream; }
  bool ExistsLyrics(const char *songpath);
  char *CheckImage(char *fileName);
  void LoadImage(const char *fullname, const char *artist, bool coveronly);
  void CheckMpeg(void);
  void ShowMpeg(const char *FileName);
  void send_pes_packet(unsigned char *data, int len, int timestamp);
  int StatusMode(void) {return statusMode;}
  void DeleteTrack(bool All, int idx);
  bool Muted(void);
  int CurrentVolume(void);
  };

#endif //___DVB_MP3_H
