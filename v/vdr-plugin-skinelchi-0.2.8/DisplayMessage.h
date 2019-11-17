/*
 * DisplayMessage.h: Message display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DISPLAYMESSAGE_H
#define __DISPLAYMESSAGE_H

#include <vdr/skins.h>
#include "scroll.h"
#include "config.h"


class cSkinElchiDisplayMessage : public cSkinDisplayMessage
{
private:
   cOsd *osd;
   int lh, lhh;
   bool changed;
public:
   cSkinElchiDisplayMessage(void);
   virtual ~cSkinElchiDisplayMessage();
   virtual void SetMessage(eMessageType Type, const char *Text);
   virtual void Flush(void);
   cCharScroll *MessageScroll;
};

#endif //__DISPLAYMESSAGE_H
