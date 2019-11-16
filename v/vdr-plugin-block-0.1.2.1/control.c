/**
 * based on control.c,v 1.4 2007/06/18 17:58:52 lordjaxom
 *
 * version by Midas
 *
 */

#include "control.h"
#include "config.h"
#include "common.h"
#include <vdr/remote.h>

inline uint64_t BlockTimeout() { return SetupBlock.MessageTimeout * 1000; }

bool cControlBlock::mRequested = false;

cControlBlock::cControlBlock(const cChannel *Channel, const cEvent *Present, const cEvent *Following):
    cControl(new cPlayer),
		mChannel(Channel),
		mPresent(Present),
		mFollowing(Following),
		mStart(0),
		mSwitch(true),
		mOsd(NULL),
		direction(0)
{
#if APIVERSNUM >= 10500
	SetNeedsFastResponse(true);
#else
	needsFastResponse = true;
#endif

    cRemote::Put(kBack,true); //Hide OSD new version
#ifdef LOGGING
    dsyslog("plugin-block: constructor cControl for %i",cDevice::CurrentChannel());   
#endif    
}

cControlBlock::~cControlBlock()
{ 
#ifdef LOGGING
 dsyslog("plugin-block: destructor cControl for %i",cDevice::CurrentChannel());
#endif
	if (mOsd != NULL)
		delete mOsd;
        if (mRequested)
        {
#ifdef LOGGING
          dsyslog("plugin-block: userint user requested to watch blocked channel");
#endif  

          if (mChannel != NULL)
          {
            cDevice::PrimaryDevice()->SwitchChannel(mChannel, true);
            cSetupBlock::LastAcceptableChannel=mChannel->Number();
          }
          else
          {
             dsyslog("plugin-block: userint Cannot switch - channel unknown!");
          }
          mRequested=false;
        }

	if (mSwitch) 
	{
	        int lastchannel=cSetupBlock::LastAcceptableChannel;
		// possibly first or last available channel, fall back to old channel

		if (direction == 0)
		{
		 if (cSetupBlock::user_direction!=0)
		 {
		  direction=cSetupBlock::user_direction;
                 }
                 else
                 {
                  direction = 1; //TODO could there be another way to switch somewhere if directions are 0?
                 }
                }
                
                bool switch_success=cDevice::SwitchChannel(direction);
//                dsyslog("plugin-block-DEV: direction %i, user_direction %i, lastchannel %i, cSetupBlock::lastchannel %i, lastcchannel %s", direction, cSetupBlock::user_direction,lastchannel,cSetupBlock::LastAcceptableChannel, cSetupBlock::LastcChannel->Name());
		if (!switch_success)
		{
		 if (cSetupBlock::ParentalGuidance==1)
		 {
		  direction=-direction;
		  cSetupBlock::user_direction=direction;
		  switch_success=cDevice::SwitchChannel(direction);
                 }
                 else
                 {
		  if (lastchannel != 0)
		  {
		   dsyslog("plugin-block: ERROR: Don't know where to switch (1) - switching to last channel!");
         	   switch_success=Channels.SwitchTo(lastchannel);
         	  }
                 }
                 if (!switch_success) 
                 {
                  dsyslog("plugin-block: ERROR: Don't know where to switch (2) - changing direction!");
                  direction=-direction;
                  cSetupBlock::user_direction=direction;
                  switch_success=cDevice::SwitchChannel(direction);
                  
                 }
                 if (!switch_success)
                 {
                  dsyslog("plugin-block: ERROR: Dont' know where to switch (3) - final fallback to channel 1/direction up!");
                  cSetupBlock::user_direction=1;
                  switch_success=Channels.SwitchTo(1);
                 }
                 if (!switch_success) dsyslog("plugin-block: Don't know where to switch (4) - Giving up! Please contact the author.");
                }
	}
}

void cControlBlock::Show(void)
{
	if (mOsd == NULL)
		mOsd = Skins.Current()->DisplayChannel(true);

	mOsd->SetChannel(mChannel, 0);
	mOsd->SetEvents(mPresent, mFollowing);
	mOsd->SetMessage(mtError, tr("Current show blocked!"));
	mOsd->Flush();

	mStart = time_ms();
}

eOSState cControlBlock::ProcessKey(eKeys Key)
{

  switch (Key) {

	case kOk:
#ifdef LOGGING
		dsyslog("plugin-block: userint Processing 'Ok' event");
#endif		
		if (cSetupBlock::OkAllowed==1)
		{
		  mRequested = true;
		  mSwitch = false;
		  return osEnd;
                }
                else
                {
                  mRequested = false;
                  mSwitch = true;
                  if (mOsd != NULL)
                  {
                    mOsd->SetMessage(mtError, tr("Permission denied!"));
                    mOsd->Flush();
                  }
                }
                return osContinue;
                
	case kNone:
		if (mStart == 0)
		{
			Show();
                }
		else if (time_ms() - mStart > BlockTimeout()) 
		{
                 #ifdef LOGGING
                 dsyslog("plugin-block: userint Processing kNone (no user interaction)");
                 #endif
         	 if (cSetupBlock::user_direction ==0)direction = cDevice::CurrentChannel() -cSetupBlock::LastAcceptableChannel;
		 else direction =0; //TODO cleanup this direction chaos (doubleset and doublechecked here and there...)
                 mSwitch = true;
		 return osEnd;
		}
	  return osContinue;

	case kChanUp:
                //workaround cause 'normal' code wont work here; vdr does not react on code/destructor here o.O
                //this is because in case of chan +/- keys have a chance to get eaten by vdr itself
                cRemote::Put(kUp,true);
                return osContinue;
                break;
        
	case kUp:                
#ifdef LOGGING
dsyslog("plugin-block: userint Processing up event (userrequest)");
#endif
		if (mStart == 0)
			Show();
		else 
		{
		  mRequested=false;//TODO:necessary? as above
		 cSetupBlock::user_direction = 1;
		  mSwitch = true;
		  return osEnd;
		}
	        break;

	case kChanDn:
                //workaround cause 'normal' code wont work here; vdr does not react on code/destructor here o.O
                //this is because in case of chan +/- keys have a chance to get eaten by vdr itself
                cRemote::Put(kDown,true);
                return osContinue;
                break;
        
	case kDown:
#ifdef LOGGING
dsyslog("plugin-block: userint Processing down event (userrequest)");
#endif
		if (mStart == 0)
			Show();
		else 
		{
		  mRequested=false;//TODO:necessary? as above
		  cSetupBlock::user_direction = -1;
		  mSwitch = true;
		  return osEnd;
		}
	        break;

        default:
                break;
  }
  return osContinue;
}

