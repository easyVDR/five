#ifndef __STATUS_H
#define __STATUS_H

#include <vdr/status.h>


class cMessages : public cStatus
{
private:
  char *msg;
public:
  void cMessages()
  virtual ~cMessages();
  virtual void OsdStatusMessage(const char *Message) { if(Message) strcpy(msg, Message); }
  inline const char *MessageText(void) const { return msg; }
};

#endif //__STATUS_H
