#ifndef _action_setdefaultsink_h
#define _action_setdefaultsink_h

#include "action.h"

class cPulseSetDefaultSinkAction : public cPulseAction {
private:
  cString _sink;
  bool _success;

  static void set_default_sink_callback(pa_context *c, int success, void *userdata)
  {
    cPulseSetDefaultSinkAction *action = (cPulseSetDefaultSinkAction*)userdata;
    action->_success = (success != 0);
    action->SignalReady();
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    _success = false;
    return pa_context_set_default_sink(context, *_sink, set_default_sink_callback, this);
  }

public:
  cPulseSetDefaultSinkAction(cPulseLoop &loop, const char *sink)
   :cPulseAction(loop, "SetDefaultSink")
   ,_sink(sink)
   ,_success(false)
  {
  }

  virtual ~cPulseSetDefaultSinkAction(void)
  {
  }

  bool Success(void) const
  {
    return _success;
  }
  };

#endif
