#include "action.h"
#include "loop.h"

uint32_t cPulseAction::_next_index = 0;

cPulseAction::cPulseAction(cPulseLoop &loop, const char *name)
 :cPulseObject(++_next_index, name)
 ,_loop(loop)
 ,_mainloop(NULL)
 ,_ready(false)
{
  loop.AddAction(this);
}

cPulseAction::~cPulseAction(void)
{
}

void cPulseAction::SignalReady()
{
  dsyslog("pulsecontrol: action %s is ready", Name());
  _ready = true;
  pa_threaded_mainloop_signal(_mainloop, 1);
}

int cPulseAction::Run(pa_threaded_mainloop *mainloop, pa_context *context)
{
  dsyslog("pulsecontrol: running action %s", Name());
  int ret = 0;
  pa_threaded_mainloop_lock(mainloop);
  _mainloop = mainloop;
  _ready = false;
  pa_operation *o = Action(context);
  pa_context_state_t context_state;
  while (!_ready) {
        context_state = pa_context_get_state(context);
        if (!PA_CONTEXT_IS_GOOD(context_state)) {
          esyslog("pulsecontrol: action %s, context state is not good anymore (%d), bailing out", Name(), context_state);
          ret = -1;
          break;
          }
        pa_threaded_mainloop_wait(mainloop);
        }
  if (o)
     pa_operation_unref(o);
  pa_threaded_mainloop_accept(mainloop);
  pa_threaded_mainloop_unlock(mainloop);
  return ret;
}
