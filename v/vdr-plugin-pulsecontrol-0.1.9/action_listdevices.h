#ifndef _action_listdevices_h
#define _action_listdevices_h

#include "action.h"
#include "device.h"

#include <pulse/ext-device-manager.h>
#include <pulse/ext-device-restore.h>

class cPulseListDevicesAction : public cPulseAction {
private:
  pa_device_type_t _type;
  cList<cPulseDevice> _devices;

  static void ext_device_manager_read_callback(pa_context *c, const pa_ext_device_manager_info* info, int eol, void *userdata)
  {
    cPulseListDevicesAction *action = (cPulseListDevicesAction*)userdata;
    if (eol) {
       if (eol < 0)
          esyslog("pulsecontrol: failed to initialize device-manager extension: %s", pa_strerror(pa_context_errno(c)));
       action->_devices.Sort();
       action->SignalReady();
       }
    else if (info->index != PA_INVALID_INDEX) {
       cPulseDevice *device = cListHelper<cPulseDevice>::Find(action->_devices, info->index);
       if (device) {
          device->SetName(info->name);
          device->SetDescription(info->description);
          }
       else if (action->_type == PA_INVALID_INDEX) {
          device = new cPulseDevice(info->index, info->name, (pa_device_type_t)PA_INVALID_INDEX, info->description);
          action->_devices.Add(device);
          }
       }
  }

  static void ext_device_restore_read_formats_callback(pa_context *c, const pa_ext_device_restore_info* info, int eol, void *userdata)
  {
    cPulseListDevicesAction *action = (cPulseListDevicesAction*)userdata;
    if (eol) {
       if (eol < 0)
          esyslog("pulsecontrol: failed to initialize device-restore extension: %s", pa_strerror(pa_context_errno(c)));
       pa_operation *o = pa_ext_device_manager_read(c, ext_device_manager_read_callback, userdata);
       if (o)
          pa_operation_unref(o);
       }
    else if ((info->index != PA_INVALID_INDEX) && ((action->_type == PA_INVALID_INDEX) || (info->type == action->_type))) {
       cPulseDevice *device = new cPulseDevice(info->index, "", info->type, NULL);
       for (int i = 0; i < info->n_formats; i++)
           device->AddFormat(new cPulseFormat(info->formats[i]->encoding));
       action->_devices.Add(device);
       }
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_ext_device_restore_read_formats_all(context, ext_device_restore_read_formats_callback, this);
  }

public:
  cPulseListDevicesAction(cPulseLoop &loop, pa_device_type_t type)
   :cPulseAction(loop, "ListDevices")
   ,_type(type)
  {
  }

  virtual ~cPulseListDevicesAction(void)
  {
  }

  const cList<cPulseDevice> &Devices(void) const
  {
    return _devices;
  }
  
  cList<cPulseDevice> &Devices(void)
  {
    return _devices;
  }
  
  cString Info(void) const
  {
    if (Devices().Count() == 0)
       return "no devices found";
    cString ret = "";
    for (const cPulseDevice *d = Devices().First(); d; d = Devices().Next(d)) {
        ret = cString::sprintf("%sdevice %d: %s\r\n", *ret, d->Index(), d->Name());
        if (d->Description())
           ret = cString::sprintf("%s desc: %s\r\n", *ret, d->Description());
        if (d->Type() != PA_INVALID_INDEX)
           ret = cString::sprintf("%s type: %s\r\n", *ret, d->TypeName());
        for (const cPulseFormat *f = d->Formats().First(); f; f = d->Formats().Next(f))
            ret = cString::sprintf("%s format: %s\r\n", *ret, f->Name());
        }
    return ret;
  }
  };

#endif
