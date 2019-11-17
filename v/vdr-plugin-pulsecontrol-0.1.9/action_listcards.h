#ifndef _action_listcards_h
#define _action_listcards_h

#include "action.h"
#include "card.h"

class cPulseListCardsAction : public cPulseAction {
private:
  cList<cPulseCard> _cards;

  static void get_card_info_list_callback(pa_context *c, const pa_card_info* info, int eol, void *userdata)
  {
    cPulseListCardsAction *action = (cPulseListCardsAction*)userdata;
    if (eol) {
       action->_cards.Sort();
       action->SignalReady();
       }
    else  {
       cString alsa_name = pa_proplist_gets(info->proplist, "alsa.card_name");
       cString description = pa_proplist_gets(info->proplist, PA_PROP_DEVICE_DESCRIPTION);
       cPulseCard *card = new cPulseCard(info->index, info->name, *alsa_name, *description);
       const char *ap = "";
       if (info->active_profile)
          ap = info->active_profile->name;
       for (uint32_t p = 0; p < info->n_profiles; p++)
           card->AddProfile(info->profiles[p].name, strcmp(info->profiles[p].name, ap) == 0);
       action->_cards.Add(card);
       }
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_context_get_card_info_list(context, get_card_info_list_callback, this);
  }

public:
  cPulseListCardsAction(cPulseLoop &loop)
   :cPulseAction(loop, "ListCards")
  {
  }

  virtual ~cPulseListCardsAction(void)
  {
  }

  const cList<cPulseCard> &Cards(void) const
  {
    return _cards;
  }
  
  cList<cPulseCard> &Cards(void)
  {
    return _cards;
  }
  
  cString Info(void) const
  {
    if (Cards().Count() == 0)
       return "no cards found";
    cString ret = "";
    for (const cPulseCard *c = Cards().First(); c; c = Cards().Next(c)) {
        ret = cString::sprintf("%scard %d: %s\r\n", *ret, c->Index(), c->Name());
	if (c->AlsaCardName())
           ret = cString::sprintf("%s name: %s\r\n", *ret, c->AlsaCardName());
	if (c->Description())
           ret = cString::sprintf("%s desc: %s\r\n", *ret, c->Description());
        const char *ap = "";
        if (c->ActiveProfile())
           ap = c->ActiveProfile()->Name(); 
        for (const cPulseProfile *p = c->Profiles().First(); p; p = c->Profiles().Next(p))
            ret = cString::sprintf("%s %sprofile: %s\r\n", *ret, (strcmp(p->Name(), ap) == 0) ? "*" : " ", p->Name());
        }
    return ret;
  }
  };

#endif
