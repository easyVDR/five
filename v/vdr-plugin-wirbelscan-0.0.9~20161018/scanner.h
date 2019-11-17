/*
 * scanner.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __WIRBELSCAN_SCANNER_H_
#define __WIRBELSCAN_SCANNER_H_

#include <linux/dvb/frontend.h>
#include <vdr/thread.h>
#include <linux/types.h>
#include <vdr/dvbdevice.h>

#include "dvb_wrapper.h"
#include "scanfilter.h"
#include "statemachine.h"
#include "common.h"

class cScanner : public cThread {
private:
  bool       shouldstop;
  bool       single;
  uint32_t   user[3];
  int        status;
  int        initialTransponders;
  scantype_t type;
  cDevice  * dev;
  cChannel * aChannel;
  cDvbTransponderParameters * params;
  cStateMachine * StateMachine;
protected:
  virtual void Action(void);
public:
  cScanner(const char * Description, scantype_t Type);
  virtual      ~cScanner(void);
  virtual void SetShouldstop(bool On) { shouldstop = On; if (StateMachine) StateMachine->DoStop();};
  virtual bool ActionAllowed(void)    { return (Running() && !shouldstop); };
          int  Status()               { return status; };
          int  DvbType()              { return type; };
          int  InitialTransponders()  { return initialTransponders; };
  };

#endif
