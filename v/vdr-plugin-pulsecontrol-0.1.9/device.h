#ifndef _device_h
#define _device_h

#include "format.h"
#include <pulse/pulseaudio.h>

class cPulseDevice : public cPulseObject {
private:
  pa_device_type_t _type;
  cString _description;
  cList<cPulseFormat> _formats;

public:
  cPulseDevice(uint32_t index, const char *name, pa_device_type_t type, const char *description)
   :cPulseObject(index, name)
   ,_type(type)
   ,_description(description)
  {
  }

  cPulseDevice(const cPulseDevice &device)
   :cPulseObject(device)
   ,_type(device._type)
   ,_description(device._description)
  {
  }

  virtual ~cPulseDevice(void)
  {
  }

  void AddFormat(cPulseFormat *format)
  {
    _formats.Add(format);
  }

  virtual cString MenuItemText(void) const
  {
    if (*_description && **_description)
       return cString::sprintf("%d - %s", Index(), Description());
    return cString::sprintf("%d - %s", Index(), Name());
  }

  void SetDescription(const char *description)
  {
    _description = description;
  }

  const char *Description(void) const
  {
    return *_description;
  }

  pa_device_type_t Type(void) const
  {
    return _type;
  }

  const char *TypeName(void) const
  {
    switch (_type) {
      case PA_DEVICE_TYPE_SINK:
        return "sink";
      case PA_DEVICE_TYPE_SOURCE:
        return "source";
      }
    return "";
  }

  const cList<cPulseFormat> &Formats(void) const
  {
    return _formats;
  }
  };

#endif
