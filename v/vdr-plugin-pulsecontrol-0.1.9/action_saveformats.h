#ifndef _action_saveformats_h
#define _action_saveformats_h

#include "action.h"
#include "device.h"

#include <pulse/ext-device-restore.h>

class cPulseSaveFormatsAction : public cPulseAction {
private:
  uint32_t _index;
  cList<cPulseFormat> _formats;
  bool _success;
  cString _error;

  static void ext_device_restore_save_formats_callback(pa_context *c, int success, void *userdata)
  {
    cPulseSaveFormatsAction *action = (cPulseSaveFormatsAction*)userdata;
    action->_success = (success != 0);
    action->_error = pa_strerror(pa_context_errno(c));
    action->SignalReady();
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    uint8_t n = 0;
    pa_format_info **formats = NULL; 
    if (_formats.Count() > 0) {
       formats = new pa_format_info*[_formats.Count()];
       for (const cPulseFormat *f = _formats.First(); f; f = _formats.Next(f)) {
           formats[n] = f->ToFormatInfo();
           n++;
           } 
       }
    pa_operation *o = pa_ext_device_restore_save_formats(context, PA_DEVICE_TYPE_SINK, _index, n, formats, ext_device_restore_save_formats_callback, this);
    delete [] formats;
    return o;
  }

public:
  cPulseSaveFormatsAction(cPulseLoop &loop, uint32_t index, const cList<cPulseFormat> &formats)
   :cPulseAction(loop, "SaveFormats")
   ,_index(index)
   ,_success(false)
   ,_error("")
  {
    cListHelper<cPulseFormat>::Copy(formats, _formats);
  }

  virtual ~cPulseSaveFormatsAction(void)
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
