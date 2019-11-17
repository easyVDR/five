#ifndef _action_listsinkinputs_h
#define _action_listsinkinputs_h

#include "action.h"
#include "sinkinput.h"

class cPulseListSinkInputsAction : public cPulseAction {
private:
  cList<cPulseSinkInput> _inputs;

  static void get_sink_input_info_list_callback(pa_context *c, const pa_sink_input_info* info, int eol, void *userdata)
  {
    cPulseListSinkInputsAction *action = (cPulseListSinkInputsAction*)userdata;
    if (eol) {
       action->_inputs.Sort();
       action->SignalReady();
       }
    else  {
       cPulseSinkInput *input = new cPulseSinkInput(info->index, info->name, info->sink);
       action->_inputs.Add(input);
       }
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_context_get_sink_input_info_list(context, get_sink_input_info_list_callback, this);
  }

public:
  cPulseListSinkInputsAction(cPulseLoop &loop)
   :cPulseAction(loop, "ListSinkInputs")
  {
  }

  virtual ~cPulseListSinkInputsAction(void)
  {
  }

  const cList<cPulseSinkInput> &SinkInputs(void) const
  {
    return _inputs;
  }
  
  cString Info(void) const
  {
    if (SinkInputs().Count() == 0)
       return "no sink inputs found";
    cString ret = "";
    for (const cPulseSinkInput *i = SinkInputs().First(); i; i = SinkInputs().Next(i)) {
        ret = cString::sprintf("%ssink-input %d: %s\r\n", *ret, i->Index(), i->Name());
        }
    return ret;
  }
  };

#endif
