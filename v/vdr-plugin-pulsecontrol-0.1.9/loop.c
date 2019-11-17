#include "loop.h"
#include <vdr/tools.h>
#include <pulse/pulseaudio.h>

cPulseLoop::cPulseLoop(void)
{
}

cPulseLoop::~cPulseLoop(void)
{
  // clean up actions without deleting them
  while (NextAction());
}

static void context_state_callback(pa_context *c, void *userdata)
{
  if (!c)
     dsyslog("pulsecontrol: context state is now %d", pa_context_get_state(c));
  pa_threaded_mainloop *ml = static_cast<pa_threaded_mainloop*>(userdata);
  pa_threaded_mainloop_signal(ml, 0); 
}

static void context_drain_complete(pa_context *c, void *userdata)
{
  dsyslog("pulsecontrol: drain complete, disconnecting");
  pa_context_disconnect(c);
  pa_threaded_mainloop *ml = static_cast<pa_threaded_mainloop*>(userdata);
  pa_threaded_mainloop_signal(ml, 0); 
}

int cPulseLoop::Run(void)
{
  int ret = 0;
  pa_threaded_mainloop *ml = NULL;
  pa_mainloop_api *api = NULL;
  pa_context *context = NULL;
  if (!(ml = pa_threaded_mainloop_new())) {
      esyslog("pulsecontrol: pa_threaded_mainloop_new() failed");
      return -1;
     }

  if (pa_threaded_mainloop_start(ml) < 0) {
     esyslog("pulsecontrol: pa_threaded_mainloop_start() failed");
     ret = -2;
     }
  else {
     dsyslog("pulsecontrol: mainloop started");
     pa_threaded_mainloop_lock(ml);
     api = pa_threaded_mainloop_get_api(ml);
     if (!(context = pa_context_new(api, "vdr-pulsecontrol"))) {
        pa_threaded_mainloop_unlock(ml);
        esyslog("pulsecontrol: pa_context_new() failed");
        ret = -3;
        }
     else {
        pa_context_set_state_callback(context, context_state_callback, ml);
        if (pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL) < 0) {
           pa_threaded_mainloop_unlock(ml);
           esyslog("pulsecontrol: pa_context_connect() failed: %s", pa_strerror(pa_context_errno(context)));
           ret = -4;
           }
        else {
           bool ready = false;
           while (true) {
                 pa_context_state_t context_state = pa_context_get_state(context);
                 if (!PA_CONTEXT_IS_GOOD(context_state)) {
                    esyslog("pulsecontrol: context state is not good (%d), quitting", context_state);
                    ret = -5;
                    break;
                    }
                 if (context_state == PA_CONTEXT_READY) {
                    ready = true;
                    break;
                    }
                 pa_threaded_mainloop_wait(ml);
                 }
           pa_threaded_mainloop_unlock(ml);

           if (ready) {
              dsyslog("pulsecontrol: executing actions");
              cPulseAction *a;
              while ((a = NextAction()) != NULL) {
                 if (a->Run(ml, context) < 0)
                    break;
                 }

              pa_threaded_mainloop_lock(ml);
              dsyslog("pulsecontrol: disconnecting");
              pa_context_set_state_callback(context, NULL, NULL);
              pa_operation *o;
              if (!(o = pa_context_drain(context, context_drain_complete, ml)))
                  pa_context_disconnect(context);
              else {
                  while (pa_operation_get_state(o) == PA_OPERATION_RUNNING) {
                        pa_threaded_mainloop_wait(ml);
		        }
                  pa_operation_unref(o);
                  }
              dsyslog("pulsecontrol: disconnected");
              pa_threaded_mainloop_unlock(ml);
              }
           }
        }
     pa_threaded_mainloop_stop(ml);
     dsyslog("pulsecontrol: mainloop stopped");
     }

  if (context)    
     pa_context_unref(context);
  pa_threaded_mainloop_free(ml);
  return ret;
}

void cPulseLoop::AddAction(cPulseAction *action)
{
  if (!action)
     return;
  _actionMutex.Lock();
  _actions.Add(action);
  _actionMutex.Unlock();
}

cPulseAction *cPulseLoop::NextAction(void)
{
  _actionMutex.Lock();
  cPulseAction *a = _actions.First();
  if (a)
     _actions.Del(a, false);
  _actionMutex.Unlock();
  return a;
}
