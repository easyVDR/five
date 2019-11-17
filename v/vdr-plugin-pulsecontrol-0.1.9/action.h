#ifndef _action_h
#define _action_h

#include "object.h"
#include <pulse/pulseaudio.h>

class cPulseLoop;

class cPulseAction : public cPulseObject {
private:
  friend class cPulseLoop;

  static uint32_t _next_index;

  cPulseLoop &_loop;

  pa_threaded_mainloop *_mainloop;
  bool _ready;

  int Run(pa_threaded_mainloop *mainloop, pa_context *context);

protected:
  void SignalReady();
  virtual pa_operation *Action(pa_context *context) = 0;

public:
  cPulseAction(cPulseLoop &loop, const char *name);
  virtual ~cPulseAction(void);
  };

#endif
