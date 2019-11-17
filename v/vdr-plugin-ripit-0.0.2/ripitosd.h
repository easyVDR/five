#ifndef __RIPITOSD_H
#define __RIPITOSD_H

#include <string>
#include <vdr/plugin.h>
#include <vdr/status.h>
#include "setup.h"
#include <sstream>

class cRipitOsd : public cOsdMenu {
private:
public:
  cRipitOsd();
  ~cRipitOsd();
  void Start_Encode(void);
  void Abort_Encode(int external);
  void External_Start(void);
  void External_Abort(void);
  virtual void Display(void);
//  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  bool Rip_On(void);
  int lastlogsize;
  std::stringstream logbuffer;
  };

extern cRipitOsd *ripitosd;

#endif //__RIPITOSD_H
