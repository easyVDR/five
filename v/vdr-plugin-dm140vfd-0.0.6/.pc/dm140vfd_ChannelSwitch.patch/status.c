/*
 * dm140vfd plugin for VDR (C++)
 *
 * (C) 2010 Andreas Brachold <vdr07 AT deltab de>
 *
 * This dm140vfd plugin is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 3 of the License.
 *
 * See the files README and COPYING for details.
 *
 */

#include <stdint.h>
#include <time.h>
#include <vdr/eitscan.h>

#include "watch.h"
#include "status.h"

//#define MOREDEBUGMSG

// ---
cVFDStatusMonitor::cVFDStatusMonitor(cVFDWatch*    pDev)
: m_pDev(pDev)
{

}

void cVFDStatusMonitor::ChannelSwitch(const cDevice *pDevice, int nChannelNumber)
{
    if (nChannelNumber > 0 
        && pDevice->IsPrimaryDevice() 
        && !EITScanner.UsesDevice(pDevice)
        && (nChannelNumber == cDevice::CurrentChannel()))
    {
#ifdef MOREDEBUGMSG
        dsyslog("DM140VFD: channel switched to %d on DVB %d", nChannelNumber, pDevice->CardIndex());
#endif
        m_pDev->Channel(nChannelNumber);
    }
}

void cVFDStatusMonitor::SetVolume(int Volume, bool Absolute)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: SetVolume  %d %d", Volume, Absolute);
#endif
  m_pDev->Volume(Volume,Absolute);
}

void cVFDStatusMonitor::Recording(const cDevice *pDevice, const char *szName, const char *szFileName, bool bOn)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: Recording  %d %s", pDevice->CardIndex(), szName);
#endif
  m_pDev->Recording(pDevice,szName,szFileName,bOn);
}

void cVFDStatusMonitor::Replaying(const cControl *pControl, const char *szName, const char *szFileName, bool bOn)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: Replaying  %s", szName);
#endif
  m_pDev->Replaying(pControl,szName,szFileName,bOn);
}

void cVFDStatusMonitor::OsdClear(void)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: OsdClear");
#endif
  m_pDev->OsdClear();
}

void cVFDStatusMonitor::OsdTitle(const char *Title)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: OsdTitle '%s'", Title);
#endif
  m_pDev->OsdTitle(Title);
}

void cVFDStatusMonitor::OsdStatusMessage(const char *szMessage)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: OsdStatusMessage '%s'", szMessage ? szMessage : "NULL");
#endif
  m_pDev->OsdStatusMessage(szMessage);
}

void cVFDStatusMonitor::OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
#ifdef unusedMOREDEBUGMSG
  dsyslog("DM140VFD: OsdHelpKeys %s - %s - %s - %s", Red, Green, Yellow, Blue);
#endif
}

void cVFDStatusMonitor::OsdCurrentItem(const char *szText)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: OsdCurrentItem %s", szText);
#endif
  m_pDev->OsdCurrentItem(szText);
}

void cVFDStatusMonitor::OsdTextItem(const char *Text, bool Scroll)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: OsdTextItem %s %d", Text, Scroll);
#endif
}

void cVFDStatusMonitor::OsdChannel(const char *Text)
{
#ifdef MOREDEBUGMSG
  dsyslog("DM140VFD: OsdChannel %s", Text);
#endif
  if (0 == strncmp(Text,"->",2)) 
    m_pDev->OsdStatusMessage(Text);
}

void cVFDStatusMonitor::OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle)
{
#ifdef unusedMOREDEBUGMSG
  char buffer[25];
  struct tm tm_r;
  dsyslog("DM140VFD: OsdProgramme");
  strftime(buffer, sizeof(buffer), "%R", localtime_r(&PresentTime, &tm_r));
  dsyslog("%5s %s", buffer, PresentTitle);
  dsyslog("%5s %s", "", PresentSubtitle);
  strftime(buffer, sizeof(buffer), "%R", localtime_r(&FollowingTime, &tm_r));
  dsyslog("%5s %s", buffer, FollowingTitle);
  dsyslog("%5s %s", "", FollowingSubtitle);
#endif
}

