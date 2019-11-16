/**
 * based on status.c,v 1.2 2007/05/08 10:16:23 thomas
 *
 * version by Midas
 *
 */

#include "status.h"
#include "control.h"
#include "event.h"
#include "config.h"

cStatusBlock::cStatusBlock(void):
    cStatus()
{
}
#if VDRVERSNUM < 10726
void cStatusBlock::ChannelSwitch(const cDevice *Device, int ChannelNumber)
#else
void cStatusBlock::ChannelSwitch(const cDevice *Device, int ChannelNumber, bool LiveView)
#endif
{
  int current_channel=cDevice::CurrentChannel();
#ifdef LOGGING
  int device_number=Device->DeviceNumber();
#endif  
  
  if (cSetupBlock::LastAcceptableChannel==0)
  {
#ifdef LOGGING
    dsyslog("plugin-block: ChannelSwitch returned because LastAcceptableChannel=0");
#endif    
    cSetupBlock::LastAcceptableChannel=current_channel;
    return;
  }
  if (ChannelNumber==0)
  {
#ifdef LOGGING
    dsyslog("plugin-block: ChannelSwitch returned because ChannelNumber=0 (switch in progress).");
#endif    
    return; //Switch in progress;
  }

#ifdef LOGGING
  dsyslog("plugin-block: cStatusBlock noticed channel switch at device %d, channel no %d",device_number,ChannelNumber);
  dsyslog("plugin-block: cDevice::CurrentChannel %d",current_channel);
  dsyslog("plugin-block: Device %d, ActualDevice %d, primary Device %d",device_number,cDevice::ActualDevice()->DeviceNumber(),cDevice::PrimaryDevice()->DeviceNumber());
  for (int ii=0;ii<cDevice::NumDevices();ii++)
  {
    cDevice* tmpdev=cDevice::GetDevice(ii);
    dsyslog("plugin-block: cDevice %d replaying: %d, transferring %d, hasprogramme %d",ii,tmpdev->Replaying(),tmpdev->Transferring(),tmpdev->HasProgramme());
  }
#endif

#if VDRVERSNUM < 10726
  if (Device->DeviceNumber()!=cDevice::PrimaryDevice()->DeviceNumber())
#else
  if (!LiveView)
#endif
  {
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing cause ChannelSwitch not on active livedevice.");
#endif
    return;
  }


  if (ChannelNumber!=current_channel)
  {
#ifdef LOGGING
   dsyslog("plugin-block: Did nothing because ChannelNumber!=CurrentChannel (switch still in progress)");
#endif

   return;
  }

  if (cSetupBlock::DetectionMethod!=0) 
  {
#ifdef LOGGING
    dsyslog("plugin-block: ChannelSwitch returned because other detection method active");
#endif
    return;
  }


  
  if (Device->Replaying()) 
  {
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing because Device is Replaying");
#endif
    return; 
  }

  const cChannel *channel = Channels.GetByNumber(ChannelNumber);
  if (channel != NULL && !channel->GroupSep()) 
  {
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
    if (scheds == NULL) 
    {
#ifdef LOGGING
      dsyslog("plugin-block: ChannelSwitch: scheds=null caused return");
#endif
      return;
    }

    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL)
    {
#ifdef LOGGING
      dsyslog("plugin-block: ChannelSwitch: sched=null caused return");
#endif      
      return;
    }
    const cEvent *present = sched->GetPresentEvent();
    const cEvent *follow  = sched->GetFollowingEvent();
    if (present == NULL)
    {
#ifdef LOGGING
      dsyslog("plugin-block: present=null return");
#endif
      return;
    }
    if (!cControlBlock::IsRequested() && !EventsBlock.Acceptable(present->Title())) 
    {
      dsyslog("plugin-block: channel %d blocked", ChannelNumber);
      cControl::Launch(new cControlBlock(channel, present, follow));
    }
    else
    {
     cSetupBlock::LastAcceptableChannel=cDevice::CurrentChannel();
     cSetupBlock::user_direction=0;
    }
  }
  
}
  
void cStatusBlock::Replaying(const cControl *Control,
                                      const char *Name,
                                      const char *FileName, bool On)
{
 char *replaystate;
#ifdef LOGGING
 if (On) replaystate=(char*)"started";
 else replaystate=(char*)"stopped";
#endif
 cEventBlock::ReplayingRecording=(bool*)On;
 if (Name!=NULL) strncpy(cSetupBlock::ReplayingName,Name,sizeof(cSetupBlock::ReplayingName));
#ifdef LOGGING
 dsyslog("plugin-block: cStatusBlock: Replay: '%s' from '%s' %s",Name,FileName,replaystate);
#endif
}

void cStatusBlock::OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle)
{
/*  char buffer[25];
  struct tm tm_r;
  dsyslog("status: cStatusBlock::OsdProgramme");
  strftime(buffer, sizeof(buffer), "%R", localtime_r(&PresentTime, &tm_r));
  dsyslog("%5s %s", buffer, PresentTitle);
  dsyslog("%5s %s", "", PresentSubtitle);
  strftime(buffer, sizeof(buffer), "%R", localtime_r(&FollowingTime, &tm_r));
  dsyslog("%5s %s", buffer, FollowingTitle);
  dsyslog("%5s %s", "", FollowingSubtitle);
*/
}