/*
 * sectionfilter.h: IPTV plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __IPTV_SECTIONFILTER_H
#define __IPTV_SECTIONFILTER_H

#ifdef __FreeBSD__
#include <sys/socket.h>
#endif // __FreeBSD__
#include <vdr/device.h>

#include "common.h"
#include "statistics.h"

class cIptvSectionFilter : public cIptvSectionStatistics {
private:
  enum {
    eDmxMaxFilterSize      = 18,
    eDmxMaxSectionCount    = 64,
    eDmxMaxSectionSize     = 4096,
    eDmxMaxSectionFeedSize = (eDmxMaxSectionSize + TS_SIZE)
  };

  int pusiSeenM;
  int feedCcM;
  int doneqM;

  uint8_t *secBufM;
  uint8_t secBufBaseM[eDmxMaxSectionFeedSize];
  uint16_t secBufpM;
  uint16_t secLenM;
  uint16_t tsFeedpM;
  uint16_t pidM;

  cRingBufferFrame *ringBufferM;
  int deviceIndexM;
  int socketM[2];

  uint8_t filterValueM[eDmxMaxFilterSize];
  uint8_t filterMaskM[eDmxMaxFilterSize];
  uint8_t filterModeM[eDmxMaxFilterSize];

  uint8_t maskAndModeM[eDmxMaxFilterSize];
  uint8_t maskAndNotModeM[eDmxMaxFilterSize];

  inline uint16_t GetLength(const uint8_t *dataP);
  void New(void);
  int Filter(void);
  inline int Feed(void);
  int CopyDump(const uint8_t *bufP, uint8_t lenP);

public:
  // constructor & destructor
  cIptvSectionFilter(int deviceIndexP, uint16_t pidP, uint8_t tidP, uint8_t maskP);
  virtual ~cIptvSectionFilter();
  void Process(const uint8_t* dataP);
  bool Send(void);
  int GetFd(void) { return socketM[0]; }
  uint16_t GetPid(void) const { return pidM; }
};

class cIptvSectionFilterHandler : public cThread {
private:
  enum {
    eMaxSecFilterCount = 32
  };
  cRingBufferLinear *ringBufferM;
  cMutex mutexM;
  int deviceIndexM;
  cIptvSectionFilter *filtersM[eMaxSecFilterCount];

  bool Delete(unsigned int indexP);
  bool IsBlackListed(u_short pidP, u_char tidP, u_char maskP) const;

protected:
  virtual void Action(void);

public:
  cIptvSectionFilterHandler(int deviceIndexP, unsigned int bufferLenP);
  virtual ~cIptvSectionFilterHandler();
  cString GetInformation(void);
  int Open(u_short pidP, u_char tidP, u_char maskP);
  void Close(int handleP);
  int GetPid(int handleP);
  void Write(u_char *bufferP, int lengthP);
};

#endif // __IPTV_SECTIONFILTER_H