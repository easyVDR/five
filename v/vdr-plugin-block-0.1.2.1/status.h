/**
 * based on status.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_STATUS_H
#define VDR_BLOCK_STATUS_H

#include <vdr/status.h>

class cStatusBlock : public cStatus {
private:

protected:
#if VDRVERSNUM < 10726
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
#else
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber, bool LiveView);
#endif  

  virtual void Replaying(const cControl *Control,
                         const char *Name,
                         const char *FileName, bool On);
  virtual void OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle);  
  
public:
  cStatusBlock(void);
};

#endif // VDR_BLOCK_STATUS_H
