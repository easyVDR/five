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
 
#ifndef STILLIMAGE_H
#define STILLIMAGE_H

#include <vdr/thread.h>
#include "encode.h"

class cStillImagePlayer;


class cStillImage 
: public cThread 
, public cEncode {

  cStillImagePlayer *player;
  volatile bool m_bEncodeRequired;
  bool m_bUseDeviceStillPicture;
protected:
  virtual void Action(void);
  void BuildPesPacket(const unsigned char *data, int len, int timestamp);

public:
  cStillImage(cStillImagePlayer *,bool bUseDeviceStillPicture);
  virtual ~cStillImage();
  
  void Stop();
  bool EncodeRequired() const {return m_bEncodeRequired;}
  void EncodeRequired(bool b) {m_bEncodeRequired = b;}
};

#endif
