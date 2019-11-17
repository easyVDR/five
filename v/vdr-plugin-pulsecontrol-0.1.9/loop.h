#ifndef _loop_h
#define _loop_h

#include <vdr/thread.h>
#include <pulse/pulseaudio.h>

#include "action.h"

class cPulseLoop {
private:
  cMutex _actionMutex;
  cList<cPulseAction> _actions;

  cPulseAction *NextAction(void);

public:
  cPulseLoop(void);
  virtual ~cPulseLoop(void);

  int Run(void);

  void AddAction(cPulseAction *action);
  };

#endif
