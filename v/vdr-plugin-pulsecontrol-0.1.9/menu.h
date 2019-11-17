#ifndef _menu_h
#define _menu_h

#include <vdr/menuitems.h>

#include "object.h"

class cPulseCard;
class cPulseFormat;
class cPulseProfile;
class cPulseScriptName;
class cPulseSink;
class cPulseSinkInput;
class cPulsecontrolMenuItem;

class cPulsecontrolMainMenu : public cOsdMenu
{
private:
  cPulsecontrolMenuItem *_item_movesinkinput;
  cPulsecontrolMenuItem *_item_setcardprofile;
  
  cList<cPulseCard> _cards;
  cList<cPulseFormat> _formats;
  cPulseCard *_card;
  cPulseProfile *_profile;
  cPulseSink *_sink;
  cPulseSinkInput *_input;
  cPulseScriptName *_script;

  void Reset(void);

public:
  cPulsecontrolMainMenu(void);
  virtual ~cPulsecontrolMainMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
  
  eOSState SelectCard(void);
  eOSState SelectDevice(void);
  eOSState SelectFormats(void);
  eOSState SelectProfile(void);
  eOSState SelectScript(void);
  eOSState SelectSink(void);
  eOSState SelectSinkInput(void);
};

#endif

