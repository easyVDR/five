#ifndef _action_listsinks_h
#define _action_listsinks_h

#include "action.h"
#include "sink.h"

class cPulseListSinksAction : public cPulseAction {
private:
  cList<cPulseSink> _sinks;

  static void get_sink_info_list_callback(pa_context *c, const pa_sink_info* info, int eol, void *userdata)
  {
    cPulseListSinksAction *action = (cPulseListSinksAction*)userdata;
    if (eol) {
       action->_sinks.Sort();
       action->SignalReady();
       }
    else  {
       cPulseSink *sink = new cPulseSink(info->index, info->name);
       action->_sinks.Add(sink);
       }
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_context_get_sink_info_list(context, get_sink_info_list_callback, this);
  }

public:
  cPulseListSinksAction(cPulseLoop &loop)
   :cPulseAction(loop, "ListSinks")
  {
  }

  virtual ~cPulseListSinksAction(void)
  {
  }

  const cList<cPulseSink> &Sinks(void) const
  {
    return _sinks;
  }
  
  cString Info(void) const
  {
    if (Sinks().Count() == 0)
       return "no sinks found";
    cString ret = "";
    for (const cPulseSink *s = Sinks().First(); s; s = Sinks().Next(s)) {
        ret = cString::sprintf("%ssink %d: %s\r\n", *ret, s->Index(), s->Name());
        }
    return ret;
  }
  };

#endif
