#ifndef _action_movesinkinput_h
#define _action_movesinkinput_h

#include "action.h"

class cPulseMoveSinkInputAction : public cPulseAction {
private:
  uint32_t _input;
  uint32_t _sink_index;
  cString _sink;
  bool _success;
  cString _error;

  static void move_sink_input_callback(pa_context *c, int success, void *userdata)
  {
    cPulseMoveSinkInputAction *action = (cPulseMoveSinkInputAction*)userdata;
    action->_success = (success != 0);
    action->_error = pa_strerror(pa_context_errno(c));
    action->SignalReady();
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    _success = false;
    uint32_t index = _sink_index;
    if ((*_sink) && !isnumber(*_sink))
       return pa_context_move_sink_input_by_name(context, _input, *_sink, move_sink_input_callback, this);
    else if (*_sink)
       index = (uint32_t)strtol(*_sink, NULL, 10);
    return pa_context_move_sink_input_by_index(context, _input, index, move_sink_input_callback, this);
  }

public:
  cPulseMoveSinkInputAction(cPulseLoop &loop, uint32_t input, const char *sink)
   :cPulseAction(loop, "MoveSinkInput")
   ,_input(input)
   ,_sink_index(PA_INVALID_INDEX)
   ,_sink(sink)
   ,_success(false)
   ,_error("")
  {
  }

  cPulseMoveSinkInputAction(cPulseLoop &loop, uint32_t input, uint32_t sink)
   :cPulseAction(loop, "MoveSinkInput")
   ,_input(input)
   ,_sink_index(sink)
   ,_sink(NULL)
   ,_success(false)
   ,_error("")
  {
  }

  virtual ~cPulseMoveSinkInputAction(void)
  {
  }

  bool Success(void) const
  {
    return _success;
  }
  
  const char *Error(void) const
  {
    return *_error;
  }
  };

#endif
