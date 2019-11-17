#ifndef _profile_h
#define _profile_h

#include "object.h"

class cPulseProfile : public cPulseObject {
public:
  cPulseProfile(const char *name)
   :cPulseObject(0, name)
  {
  }

  cPulseProfile(const cPulseProfile &profile)
   :cPulseObject(profile)
  {
  }

  virtual ~cPulseProfile(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    return Name();
  }  
  };

#endif
