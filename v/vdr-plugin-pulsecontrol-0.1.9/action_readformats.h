#ifndef _action_readformats_h
#define _action_readformats_h

#include "action.h"
#include "format.h"

#include <pulse/ext-device-restore.h>

class cPulseReadFormatsAction : public cPulseAction {
private:
  uint32_t _index;
  pa_device_type_t _type;
  cList<cPulseFormat> _formats;

  static void ext_device_restore_read_formats_callback(pa_context *c, const pa_ext_device_restore_info* info, int eol, void *userdata)
  {
    cPulseReadFormatsAction *action = (cPulseReadFormatsAction*)userdata;
    if (eol) {
       if (eol < 0)
          esyslog("pulsecontrol: failed to initialize device-restore extension: %s", pa_strerror(pa_context_errno(c)));
       action->SignalReady();
       }
    else {
       action->_formats.Clear();
       for (int i = 0; i < info->n_formats; i++)
           action->_formats.Add(new cPulseFormat(info->formats[i]->encoding));
       }
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_ext_device_restore_read_formats(context, _type, _index, ext_device_restore_read_formats_callback, this);
  }

public:
  cPulseReadFormatsAction(cPulseLoop &loop, uint32_t index, pa_device_type_t type)
   :cPulseAction(loop, "ReadFormats")
   ,_index(index)
   ,_type(type)
  {
  }

  virtual ~cPulseReadFormatsAction(void)
  {
  }

  const cList<cPulseFormat> &Formats(void) const
  {
    return _formats;
  }
  
  cString Info(void) const
  {
    if (Formats().Count() == 0)
       return "no formats found";
    cString ret = "";
    for (const cPulseFormat *f = Formats().First(); f; f = Formats().Next(f))
        ret = cString::sprintf("%sformat: %s\r\n", *ret, f->Name());
    return ret;
  }
  };

#endif
