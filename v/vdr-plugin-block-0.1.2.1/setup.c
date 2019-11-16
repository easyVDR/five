/**
 * based on setup.c,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include "setup.h"

#include <vdr/interface.h>

cMenuSetupBlock::cMenuSetupBlock():
    cMenuSetupPage()
{
  mEventsData = EventsBlock; // need to use copy constructor
  mSetupData  = SetupBlock;
  Set();
}

void cMenuSetupBlock::Set(void) {
  int current = Current();
  cOsdItem *item;

  Clear();

  Add(new cMenuEditBoolItem(tr("Hide Mainmenu Entry"), &mSetupData.HideMenuEntry));
  Add(new cMenuEditIntItem(tr("Message Timeout [s]"),  &mSetupData.MessageTimeout, 0, 10));

  DetectionMethods[0] = tr("On Switch");
  DetectionMethods[1] = tr("Channel EPG");
  
  FuzzyFallsback[0] = tr("black");
  FuzzyFallsback[1] = tr("white");

  cMenuEditStraItem *methoditem = new cMenuEditStraItem(tr("Detection Method"), &mSetupData.DetectionMethod, 2, DetectionMethods);
  cMenuEditBoolItem *okitem = new cMenuEditBoolItem(tr("Ok deblocks temporarily"), &mSetupData.OkAllowed);
  cMenuEditStraItem *whitelistitem = new cMenuEditStraItem(tr("Fuzzy fallback"), &mSetupData.FuzzyFallback, 2, FuzzyFallsback);
  cMenuEditIntItem *dvbratingitem = new cMenuEditIntItem(tr("DVB rating"), &mSetupData.DVBRating,0,21);
//  cMenuEditIntItem *ratingitem = new cMenuEditIntItem(tr("Rating"), &mSetupData.Rating, 0, 21));
  
  if(cSetupBlock::ParentalGuidance==1)
  {
    methoditem->SetSelectable(false);
    okitem->SetSelectable(false);
    whitelistitem->SetSelectable(false);
    dvbratingitem->SetSelectable(false);
  }
  Add(methoditem);
  Add(okitem);
  Add(whitelistitem);
  if (VDRVERSNUM > 10711)  Add(dvbratingitem);
//TODO translation, setup menü einrichten ->define mit if versehen für ein/ausblenden von options? variablen deklarieren  
  item = new cOsdItem("");
  item->SetSelectable(false);
  Add(item);

  item = new cOsdItem(tr("--- Keywords -------------------------------------------------------------------"));
  item->SetSelectable(false);
  Add(item);

#if VDRVERSNUM<10711
#define NONKEYWORDITEMS 7
#else
#define NONKEYWORDITEMS 8
#endif

  int index = 0;
  cEventBlock *event = mEventsData.First();
  char* entry=NULL;
  char listtype=NULL;
  while (event != NULL) 
  {
   if (event->isWhitelisted()) listtype='+';
   else listtype='-';
   asprintf(&entry,"%c:%s",listtype,event->Pattern());
   Add(new cOsdItem(entry));
   event = mEventsData.Next(event);
   ++index;
  }

  SetCurrent(Get(current));
  SetHelpKeys();
  Display();
}

void cMenuSetupBlock::SetHelpKeys(void)
{
  const char *red    = NULL;
  const char *yellow = NULL;
  const char *blue = NULL;

  printf("sethelpkeys, current = %d\n", Current());

  if (Current() >= NONKEYWORDITEMS) 
  {
   red    = trVDR("Button$Edit");
   if (cSetupBlock::ParentalGuidance!=1)
   {
    yellow = trVDR("Button$Delete");
    blue = trVDR("Button$+/-");
   }
  }
  if(cSetupBlock::ParentalGuidance!=1)
  {
   SetHelp(red, trVDR("Button$New"), yellow, blue);
  }
  else
   SetHelp(red,trVDR("Button$New"));
}

void cMenuSetupBlock::Store(void) 
{
  EventsBlock = mEventsData;
  EventsBlock.Sort();
  EventsBlock.Save();

  SetupBlock  = mSetupData;
  SetupStore("HideMenuEntry",  SetupBlock.HideMenuEntry);
  SetupStore("MessageTimeout", SetupBlock.MessageTimeout);
  SetupStore("DetectionMethod", SetupBlock.DetectionMethod);
  SetupStore("OkAllowed", SetupBlock.OkAllowed);
  SetupStore("FuzzyFallback", SetupBlock.FuzzyFallback);
  SetupStore("DVBRating", SetupBlock.DVBRating);
  cEventBlock::LastTitle=(char*)"force recheck from setup"; 
}

eOSState cMenuSetupBlock::Edit(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;


  cEventBlock *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL)
  {
   if (cSetupBlock::ParentalGuidance==1 && strcmp(event->Pattern(),tr("New Entry"))!=0)
   {
    Skins.Message(mtError, tr("Permission denied!"));
    dsyslog("plugin-block: Parental guidance detected attempt to edit another than a new entry. Permission denied!");
    return osContinue;
   }
   eOSState retval= AddSubMenu(new cMenuSetupEditBlock(event));
   EventsBlock = mEventsData;
   EventsBlock.Sort();
   EventsBlock.Save();        
   return retval;
  }   
  return osContinue;
}

eOSState cMenuSetupBlock::New(void)
{
  if (HasSubMenu())
    return osContinue;
      
  cEventBlock* dummy1=new cEventBlock();
  if(mEventsData.hasStringMatch(dummy1->Pattern(),true)!=NULL)
  {
   Set();
   SetCurrent(Get (NONKEYWORDITEMS + mEventsData.getIndexOf(dummy1)));
   delete(dummy1);
   return osContinue;
  }
  mEventsData.Add(dummy1);
  Set();
  SetCurrent(Get(NONKEYWORDITEMS + mEventsData.getIndexOf(dummy1)));
  return osContinue;
}

eOSState cMenuSetupBlock::SetListProperty(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;
  
  cEventBlock *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL)
  {
   event->setWhitelisted(!(event->isWhitelisted()));
   EventsBlock = mEventsData;
   EventsBlock.Save();        
  }   
  return osContinue;
}

  
eOSState cMenuSetupBlock::Delete(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;
        
  cEventBlock *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL) {
	if (Interface->Confirm(tr("Delete keyword?")))
	  {
	   mEventsData.Del(event,true);
           EventsBlock = mEventsData;
           EventsBlock.Save();
	   
	  }
  }
  Set();
  return osContinue;
}

eOSState cMenuSetupBlock::ProcessKey(eKeys Key) 
{
 bool hadSubMenu = HasSubMenu();
 eOSState state = cMenuSetupPage::ProcessKey(Key);

 if (hadSubMenu && !HasSubMenu()) 
 {
  Set();
  return state;
 }

  switch (state) 
  {
   case osUnknown: // normal key handling
    switch (Key) 
    {
     case kRed:    return Edit();
                   break;
     case kGreen:  return New();
                   break;
     case kYellow: if (mSetupData.ParentalGuidance!=1) return Delete();
                   break;
     case kBlue:   if (mSetupData.ParentalGuidance!=1) return SetListProperty();

     default:    
                   break;
    }

    break;

   default:
                  break;
  }
	
  if (!HasSubMenu())
      Set();

  return state;
}

static const char *ALLOWEDCHARS = "$ abcdefghijklmnopqrstuvwxyz0123456789-_.#~/[]{}()+*^$";

cMenuSetupEditBlock::cMenuSetupEditBlock(cEventBlock *Event):
    cOsdMenu("", 33),
    mEvent(Event),
    mData(*Event)
{
 char buf[80];
 snprintf(buf, sizeof(buf), "%s - %s '%s'", trVDR("Setup"), trVDR("Plugin"), "block");
 SetTitle(buf);
 Add(new cMenuEditStrItem(tr("Pattern"), mData.mPattern, sizeof(mData.mPattern), tr(ALLOWEDCHARS)));
 Add(new cMenuEditBoolItem(tr("Regular Expression"), &mData.mRegularExp));
 if (cSetupBlock::ParentalGuidance!=1)
 {
  Add(new cMenuEditBoolItem(tr("Ignore Case"), &mData.mIgnoreCase));
  Add(new cMenuEditBoolItem(tr("Whitelist"), &mData.whitelisted));
 }
}

eOSState cMenuSetupEditBlock::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
    case kOk:
			if (!mData.Compile()) 
			{
			 Skins.Message(mtError, tr("Malformed regular expression!"));
			 state = osContinue;
			} 
			else 
			{
			 if(EventsBlock.hasStringMatch(mData.Pattern(),true)!=NULL && (strcmp(mEvent->Pattern(),mData.Pattern())!=0))
			 {
			  Skins.Message(mtError, tr("Please edit duplicate instead"));
			  state=osBack;
			 }
			 else
			 {
			  *mEvent = mData;
			  state = osBack;

			 }
			}
                        break;

    default:
      break;
    }
  }
  return state;
}
