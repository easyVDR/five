/*
 * vdrstatus.h: Keeping track of several VDR status settings
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINELCHI_VDRSTATUS_H_
#define __SKINELCHI_VDRSTATUS_H_

#include <vdr/status.h>

class cRecordingEntry : public cListObject
{
   //friend class cRecordingList;
private:
   const cDevice *device;
   char *name;
   char *filename;
public:
   cRecordingEntry(const cDevice *Device, const char *Name, const char *FileName);
   ~cRecordingEntry();
   virtual int Compare(const cListObject & listObj) const {
      cRecordingEntry *entry = (cRecordingEntry *) & listObj;
      return strcmp (entry->filename, filename);
   }
   const char *GetFilename() { return filename; }
   const char *GetName() { return name; }
   const cDevice *GetDevice() { return device; }
};


class cSkinElchiStatus : public cStatus
{
private:
   const cDevice *ChannelDevice[MAXDEVICES];
   int ChannelNumber[MAXDEVICES];
   int RecordingChange;
   char *AudioTrack;
   const char *AudioChannel;
   const char * const *Tracks;
   int AudioTrackIndex;
   int AudioChange;
   int Volume;
   int VolumeChange;
protected:
#if VDRVERSNUM < 010726
   virtual void ChannelSwitch(const cDevice *device, int channelNumber);
#else
   virtual void ChannelSwitch(const cDevice *device, int channelNumber, bool LiveView);
#endif
   virtual void Recording(const cDevice *Device, const char *Name, const char *FileName, bool On);
   virtual void Replaying(const cControl *Control, const char *Name, const char *FileName, bool On);
   virtual void SetVolume(int Volume, bool Absolute);
   virtual void SetAudioTrack(int Index, const char * const *Tracks);
   virtual void SetAudioChannel(int AudioChannel);
   //virtual void OsdClear();
   //virtual void OsdTitle(const char *Title);
   //virtual void OsdStatusMessage(const char *Message);
   //virtual void OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue);
   //virtual void OsdItem(const char *Text, int Index); //to slow in channels-menu, not all channels are schow, but it is call for all channels
   //virtual void OsdCurrentItem(const char *Text);
   //virtual void OsdTextItem(const char *Text, bool Scroll);
   //virtual void OsdChannel(const char *Text);
   //virtual void OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle);

public:
   cSkinElchiStatus();
   ~cSkinElchiStatus();
   cList<cRecordingEntry> recordinglist;
   int GetVolumeChange(void) { return VolumeChange; };
   int GetVolume(void) { return Volume; };
   const char *GetAudioTrack(void) { return AudioTrack; };
   int GetAudioTrackIndex(void) { return AudioTrackIndex; };
   const char *GetAudioChannel(void) { return AudioChannel; };
   int GetAudioChange(void) { return AudioChange; };
   int GetRecordingChange(void) { return RecordingChange; };
};

#endif
