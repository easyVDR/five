/*
 * vdrstatus.c: Keeping track of several VDR status settings
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "vdrstatus.h"
//#ifdef VIDEOSTATUS
#include "videostatus.h"
//#endif
#include "setup.h"
#include "common.h"
#include <vdr/tools.h>


cRecordingEntry::cRecordingEntry(const cDevice *Device, const char *Name, const char *FileName)
{
   device = Device;
   name = strdup(Name);
   filename = strdup(FileName);
}


cRecordingEntry::~cRecordingEntry()
{
   free (name);
   free (filename);
}


cSkinElchiStatus::cSkinElchiStatus()
{
   //isyslog("skinelchi cSkinElchiStatus()");
   AudioTrack = NULL;
   AudioChannel = NULL;
   Tracks = NULL;
}


cSkinElchiStatus::~cSkinElchiStatus()
{
   //isyslog("skinelchi ~cSkinElchiStatus()");
}

#if VDRVERSNUM < 010726
void cSkinElchiStatus::ChannelSwitch(const cDevice * device, int channelNumber)
#else
void cSkinElchiStatus::ChannelSwitch(const cDevice * device, int channelNumber, bool LiveView)
#endif
{  // Indicates a channel switch on the given DVB device.
   // If ChannelNumber is 0, this is before the channel is being switched,
   // otherwise ChannelNumber is the number of the channel that has been switched to.
   // LiveView tells whether this channel switch is for live viewing.
   int i = device->CardIndex();
   ChannelDevice[i] = device;
   ChannelNumber[i] = channelNumber;
   if (!channelNumber && cDevice::ActualDevice()->CardIndex() == i) {
       //isyslog("skinelchi-Status: ChannelSwitch delete AudioTrack/Channel");
      if (AudioTrack) {
         free(AudioTrack);
         AudioTrack = NULL;
      }
      if (AudioChannel) {
         AudioChannel = NULL;
      }
   }
}


void cSkinElchiStatus::Recording(const cDevice *Device, const char *Name, const char *FileName, bool On)
{  // The given DVB device has started (On = true) or stopped (On = false) recording Name.
   // Name is the name of the recording, without any directory path. The full file name
   // of the recording is given in FileName, which may be NULL in case there is no
   // actual file involved. If On is false, Name may be NULL.

   DSYSLOG("skinelchi statusrec: %d %d %s %s", Device->CardIndex(), On, Name, FileName)
   // Name may be null if On=false, but FileName is always set
   if (Name) {
      cRecordingEntry *newentry = new cRecordingEntry(Device, Name, FileName);

      cRecordingEntry *entry = recordinglist.First();
      while (entry && (Device->CardIndex() >= entry->GetDevice()->CardIndex()))
      {
         DSYSLOG2("skinelchi vdrstatus recording: entry1: %d-%d:%s", Device->CardIndex(), entry->GetDevice()->CardIndex(), entry->GetFilename());
         entry = recordinglist.Next(entry);
      }

      DSYSLOG("skinelchi vdrstatus recording: insert %d,%d:%s", Device->CardIndex(), entry?entry->GetDevice()->CardIndex():-1, entry?entry->GetFilename():"NULL");
      if (entry && (Device->CardIndex() < entry->GetDevice()->CardIndex()))
         recordinglist.Ins(newentry, entry);
      else
         recordinglist.Add(newentry, entry);
   }
   else {
      cRecordingEntry *entry = recordinglist.First();
      bool found;
      for (entry = recordinglist.First(); !(found = !strcmp(entry->GetFilename(), FileName)); entry = recordinglist.Next(entry)) {
      }

      if (found) {
         DSYSLOG("skinelchi vdrstatus: deleting %s", entry ? entry->GetFilename() : "NULL");
         recordinglist.Del(entry, true);
      }
   }
   RecordingChange++;
}


void cSkinElchiStatus::Replaying(const cControl *Control, const char *Name, const char *FileName, bool On)
{  // The given player control has started (On = true) or stopped (On = false) replaying Name.
   // Name is the name of the recording, without any directory path. In case of a player that can't provide
   // a name, Name can be a string that identifies the player type (like, e.g., "DVD").
   // The full file name of the recording is given in FileName, which may be NULL in case there is no
   // actual file involved. If On is false, Name may be NULL.
}


void cSkinElchiStatus::SetVolume(int volume, bool absolute)
{  // The volume has been set to the given value, either
   // absolutely or relative to the current volume.

   VolumeChange++;
#if APIVERSNUM < 10402
   Volume = volume;//vdr 1.3.22 -> get volume always absolute
#else
   Volume = absolute ? volume : Volume + volume;
#endif
}


void cSkinElchiStatus::SetAudioTrack(int Index, const char * const *tracks)
{  // The audio track has been set to the one given by Index, which
   // points into the Tracks array of strings. Tracks is NULL terminated.
   //isyslog("skinelchi-Status: SetAudioTrack (%d, %s)", Index, tracks?(const char *)tracks[Index]:"NULL");
   if (tracks) {
      AudioTrackIndex = Index + 1;
      Tracks = tracks;
      AudioTrack = strdup(tracks[Index]);
   }
   else
      if (AudioTrack) {
         AudioTrackIndex = 0;
         Tracks = NULL;
         free(AudioTrack);
         AudioTrack = NULL;
      }
}


void cSkinElchiStatus::SetAudioChannel(int audioChannel)
{  // The audio channel has been set to the given value.
   // 0=stereo, 1=left, 2=right, -1=no information available.
   //isyslog("skinelchi-Status: SetAudioChannel (%d)", audioChannel);
   switch (audioChannel) {
     case -1: AudioChannel = tr("Digital"); break;
     case 0:  AudioChannel = tr("Stereo"); break;
     case 1:  AudioChannel = tr("Left channel"); break;
     case 2:  AudioChannel = tr("Right channel"); break;
     default: AudioChannel = NULL;
   }
}
