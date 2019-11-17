#ifndef _card_h
#define _card_h

#include "profile.h"

class cPulseCard : public cPulseObject {
private:
  cString _alsa_name;
  cString _description;
  cList<cPulseProfile> _profiles;
  const cPulseProfile *_active_profile;

public:
  cPulseCard(uint32_t index, const char *name, const char *alsa_name, const char *description)
   :cPulseObject(index, name)
   ,_alsa_name(alsa_name)
   ,_description(description)
   ,_active_profile(NULL)
  {
  }

  cPulseCard(const cPulseCard &card)
   :cPulseObject(card)
   ,_alsa_name(card._alsa_name)
   ,_description(card._description)
   ,_active_profile(NULL)
  {
    cListHelper<cPulseProfile>::Copy(card.Profiles(), _profiles);
    if (card.ActiveProfile())
       _active_profile = cListHelper<cPulseProfile>::Find(_profiles, card.ActiveProfile()->Name()); 
  }

  virtual ~cPulseCard(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    if (*_description && **_description)
       return cString::sprintf("%d - %s", Index(), Description());
    if (*_alsa_name && **_alsa_name)
       return cString::sprintf("%d - %s", Index(), AlsaCardName());
    return cString::sprintf("%d - %s", Index(), Name());
  }

  void AddProfile(const char *name, bool isactive)
  {
    cPulseProfile *p = new cPulseProfile(name);
    if (isactive)
       _active_profile = p; 
    _profiles.Add(p);
  }

  const char *AlsaCardName(void) const
  {
    return *_alsa_name;
  }
  
  const char *Description(void) const
  {
    return *_description;
  }
  
  const cPulseProfile *ActiveProfile(void) const
  {
    return _active_profile;
  }

  const cList<cPulseProfile> &Profiles(void) const
  {
    return _profiles;
  }
  };

#endif
