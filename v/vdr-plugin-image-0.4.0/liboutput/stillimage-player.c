/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *  Created: Thu Aug  5 2004
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#include "stillimage-player.h"
#include "stillimage.h"

cStillImagePlayer::cStillImagePlayer(ePlayMode PlayMode,bool bUseDeviceStillPicture)
 : cPlayer(PlayMode)
 , m_StillImage(this,bUseDeviceStillPicture) 
{
}

cStillImagePlayer::~cStillImagePlayer()
{
   Detach();
}

void cStillImagePlayer::Activate(bool bOn) {
   if (bOn) {
      m_StillImage.Start();
   } else {
      m_StillImage.Stop();
   }
}

void cStillImagePlayer::Play(const uchar *Data, int Length) {
   PlayPes(Data, Length,true);
}

bool cStillImagePlayer::Wait() {
   cPoller Poller;
   return DevicePoll(Poller, 100);
}
