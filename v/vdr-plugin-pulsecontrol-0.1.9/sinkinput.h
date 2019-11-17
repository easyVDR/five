#ifndef _sinkinput_h
#define _sinkinput_h

#include "object.h"

class cPulseSinkInput : public cPulseObject {
private:
  uint32_t _sink;

public:
  cPulseSinkInput(uint32_t index, const char *name, uint32_t sink)
   :cPulseObject(index, name)
   ,_sink(sink)
  {
  }

  cPulseSinkInput(const cPulseSinkInput &input)
   :cPulseObject(input)
   ,_sink(input._sink)
  {
  }

 virtual ~cPulseSinkInput(void)
  {
  }
  
  uint32_t Sink(void) const
  {
    return _sink;
  }
  };

#endif
