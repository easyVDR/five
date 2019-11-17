/*
 * sleeptimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/device.h>
#include <vdr/status.h>

#if VDRVERSNUM < 10507
 #include "i18n.h"
#endif 

#ifndef COMMANDLENGTH
 #define COMMANDLENGTH 512
#endif

static const char *VERSION        = "0.8.3-201301261156dev";
static const char *DESCRIPTION    = "Sleeptimer for VDR";
static const char *MAINMENUENTRY  = tr("Sleeptimer");

int method = 0;
int default_timespan=15;
int shutdown_time = 0;
int shutdown_minutes = 2;
bool start_w_default=false;
bool do_confirm=false;
char command[COMMANDLENGTH] = "/sbin/poweroff";
char cli_command[COMMANDLENGTH]="";
bool process_red=false;

#define ARRAYLENGTH 9
int autoswitch_vals[] = { 0,15,30,45,60,90,120,180,-1 }; //-1 is a placeholder for the default timespan


// Tools
int i2s(int val) //prepare a special int for vdrs OSD
{
 time_t tme = time(NULL);
 struct tm *TM;
 int dif;
 int h;
 int min;

 TM = localtime(&tme);
 (*TM).tm_sec = 0;
 (*TM).tm_min = 0;
 (*TM).tm_hour = 0;
 dif = (val - mktime(TM));
 h = dif / 3600;
 min = ((dif - h * 3600) / 60);
 if (h > 23) h-=24;
 return h * 100 + min; //Std * 100 because vdr interprets the value in a special way (eg 10:46h in vdr is 1046)
}

time_t s2i(int &min, int &h, int value) //convert vdrs representation of time (see i2s) to time_t and check
{                                         //if the shutdown time is after midnight
 time_t tme = time(NULL);
 struct tm *TM;
 TM = localtime(&tme);
 h = int(value / 100);
 min = ((value - h * 100) % 60);

 if (h * 60 + min < (*TM).tm_hour * 60 + (*TM).tm_min)
  (*TM).tm_mday++;

 (*TM).tm_sec = 0;
 (*TM).tm_min = 0;
 (*TM).tm_hour = 0;
 return mktime(TM) + h * 3600 + min * 60;
}

void InsertDefaultTimespan()
{
 int reinit_array[] = { 0, 15, 30, 45, 60, 90, 120, 180, -1};
 int i=0;
 bool already_in=false;
 for (i=0;i<ARRAYLENGTH;i++) 
 {
  autoswitch_vals[i]=reinit_array[i];
  if (default_timespan==reinit_array[i]) already_in=true;
 }
 if (already_in) return;
 
 if (default_timespan>autoswitch_vals[ARRAYLENGTH-2])
 {
  autoswitch_vals[ARRAYLENGTH-1]=default_timespan;
  return;
 }
 else
 {
  for (i=ARRAYLENGTH-2;autoswitch_vals[i]>default_timespan;i--);
  int j=ARRAYLENGTH-1;
  for (;j>i+1;j--) autoswitch_vals[j]=autoswitch_vals[j-1];
  autoswitch_vals[i+1]=default_timespan;
 }
}

void ResetAutoswitchValues()
{
 int reinit_array[] = { 0, 15, 30, 45, 60, 90, 120, 180, -1};
 int i=0;
 for (i=0;i<ARRAYLENGTH;i++) 
 {
  autoswitch_vals[i]=reinit_array[i];
 }
}
 
// cPluginSleeptimer
class cPluginSleeptimer : public cPlugin {


private:
  static time_t sleepat;
  time_t lastaction;
  void HotkeyAction();
  
public:
  cPluginSleeptimer(void);
  virtual ~cPluginSleeptimer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);

  static time_t getSleeptimer() { return cPluginSleeptimer::sleepat; }
  static void setSleeptimer(time_t tt) { cPluginSleeptimer::sleepat=tt; }
  static eKeys ShowMessage(cString msg);
};

cPluginSleeptimer::cPluginSleeptimer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  lastaction=0;
}

//Initialization of static members
time_t cPluginSleeptimer::sleepat=0;


cPluginSleeptimer::~cPluginSleeptimer()
{
  // Clean up after yourself!
}

const char *cPluginSleeptimer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
	return "  -e command   shutdown command (default: /sbin/poweroff)\n";
}

bool cPluginSleeptimer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
	int c;
	if (!strcmp(*argv, "sleeptimer"))
		++argv, --argc;
	while((c = getopt(argc, argv, "e:")) != -1 ) {
		switch(c) {
			case 'e': 
				strncpy(cli_command, optarg, sizeof(cli_command) - 1);
				break;
			default: return false;
		}
	}
  return true;
}

bool cPluginSleeptimer::Start(void)
{
#if VDRVERSNUM < 10507 
 RegisterI18n(Phrases);
#endif
 return true;
}

void cPluginSleeptimer::Housekeeping(void)
{
}

// cBackgroundSleeptimer 
class cBackgroundSleeptimer : public cThread {
private:
	virtual void Action(void);
	static cBackgroundSleeptimer *inst;
	cBackgroundSleeptimer(void);
	
public:
	static cBackgroundSleeptimer *getInstance(void);
	~cBackgroundSleeptimer();
};

class MainMenu : public cOsdMenu {
private: 
        int minpos;
        int timepos;
        int actionpos;

public:
	MainMenu(void);
	eOSState ProcessKey(eKeys k);
        ~MainMenu();
};


MainMenu::MainMenu(void) : cOsdMenu("Sleeptimer", 20) 
{
 if (process_red) return;
 char buf[80];
 SetHelp(tr("Autoswitch"),tr("Disable"),NULL,NULL);
 if(!cPluginSleeptimer::getSleeptimer()) 
 {
  snprintf(buf, sizeof(buf), "%s", tr("Not active"));
  shutdown_time = i2s((int)(time(NULL))+120);
  shutdown_minutes = default_timespan;
  Add(new cMenuEditIntItem(tr("Shutdown [min]"), &shutdown_minutes, 2,1440));
  Add(new cMenuEditTimeItem(tr("Shutdown (time)"), &shutdown_time));
  Add(new cOsdItem(hk(buf)));
  minpos=0;
  timepos=1;
  actionpos=2;
 }
 else
 {
  int dif=(int)(difftime(cPluginSleeptimer::getSleeptimer(),time(NULL)));

  shutdown_minutes = dif/60;

  //make sure shutdown_time (in the OSD) is always at least 2 minutes in the future
  int shutd_tmp=0;
  if (shutdown_minutes==0) shutd_tmp=2;
  if (shutdown_minutes==1) shutd_tmp=1;
  shutdown_time = i2s((int)cPluginSleeptimer::getSleeptimer()+shutd_tmp);

  snprintf(buf, sizeof(buf), tr("Disable sleeptimer in %d minutes"), shutdown_minutes);
  Add(new cOsdItem(hk(buf)));
  Add(new cMenuEditIntItem(tr("Shutdown [min]"), &shutdown_minutes,2,1440));
  Add(new cMenuEditTimeItem(tr("Shutdown (time)"), &shutdown_time));
  minpos=1;
  timepos=2;
  actionpos=0;
 }
}


cOsdObject *cPluginSleeptimer::MainMenuAction(void)
{
 if (Interface->GetKey(false)==kNone && !process_red) return new MainMenu();
 
 if (process_red) process_red=false;
 
 HotkeyAction();
 
 return NULL;

}

class cMenuSetupSleeptimer : public cMenuSetupPage {
private:
	int new_method;
	int timespan;
	int setup_start_w_default;
	int setup_do_confirm;
	char setup_command[COMMANDLENGTH];
	const char* methods[3];
public:
	cMenuSetupSleeptimer(void);
	virtual void Store(void);
};

cMenuSetupPage *cPluginSleeptimer::SetupMenu(void)
{
  return new cMenuSetupSleeptimer;
}

bool cPluginSleeptimer::SetupParse(const char *Name, const char *Value)
{
 if(!strcasecmp(Name, "Method"))
  method = atoi(Value);
 else if (!strcasecmp(Name,"DefaultTimespan"))
      {
       default_timespan=atoi(Value);
       if (start_w_default)
        InsertDefaultTimespan();
       else 
        ResetAutoswitchValues();
      }
      else if (!strcasecmp(Name,"StartWithDefault"))
           {
            start_w_default=atoi(Value);
            if (start_w_default)                 
             InsertDefaultTimespan();
            else 
             ResetAutoswitchValues();
           }
           else if (!strcasecmp(Name,"Confirmation"))
            do_confirm=atoi(Value);
           else if (!strcasecmp(Name,"Command"))
	strncpy(command, Value, sizeof(command) - 1);
 else return false;
 return true;
}

 
// cMenuSetupSleeptimer
cMenuSetupSleeptimer::cMenuSetupSleeptimer(void) 
{
 new_method = method;
 timespan = default_timespan;
 setup_start_w_default=start_w_default;
 strncpy(setup_command, command, sizeof(setup_command) - 1);
 setup_do_confirm=do_confirm;
 
 methods[0]=tr("Shutdown");
 methods[1]=tr("Mute");
 methods[2]=tr("Execute command");
 
 char* allowed_chars=(char*)" abcdefghijklmnopqrstuvwxyz0123456789-.,#~\\^$[]|()*+?{}/:%@&";
 
 Add(new cMenuEditIntItem(tr("Default Timespan [min]"),  &timespan, 2));
 Add(new cMenuEditStraItem(tr("Action"), &new_method, 3, methods)); 
 
 cMenuEditStrItem *commanditem;
 if (cli_command[0]=='\0')
 {
  commanditem = new cMenuEditStrItem(tr("Command"), setup_command, COMMANDLENGTH, allowed_chars);
 }
 else
 {
  commanditem = new cMenuEditStrItem(tr("Command"), cli_command, COMMANDLENGTH, allowed_chars);
  commanditem->SetSelectable(false);
 }
 Add(commanditem);
  
 Add(new cMenuEditBoolItem(tr("Use default for autoswitch"), &setup_start_w_default, tr("No"), tr("Yes")));
 Add(new cMenuEditBoolItem(tr("Confirm"), &setup_do_confirm, tr("No"), tr("Yes")));
}

void cMenuSetupSleeptimer::Store(void) 
{
 SetupStore("Method", method = new_method);
 SetupStore("DefaultTimespan", default_timespan=timespan);
 strncpy(command, setup_command, sizeof(command) - 1);
 SetupStore("Command", command);
 SetupStore("StartWithDefault", start_w_default=setup_start_w_default);
 SetupStore("Confirmation", do_confirm=setup_do_confirm);
 if (start_w_default) 
  InsertDefaultTimespan();
 else 
  ResetAutoswitchValues();
}


MainMenu::~MainMenu()
{
}

cBackgroundSleeptimer::cBackgroundSleeptimer(void) 
{
 Start();
}

cBackgroundSleeptimer* cBackgroundSleeptimer::inst=NULL;

cBackgroundSleeptimer* cBackgroundSleeptimer::getInstance()
{
 if (inst==NULL)
 {
  inst=new cBackgroundSleeptimer();
 }
 return inst;
}
 

cBackgroundSleeptimer::~cBackgroundSleeptimer() 
{
 if (inst) delete inst;
 cPluginSleeptimer::setSleeptimer(0);
}

void cBackgroundSleeptimer::Action(void) 
{
 isyslog("plugin-sleeptimer: thread started (pid=%d)", getpid());
 while(cPluginSleeptimer::getSleeptimer())
 {
  if(cPluginSleeptimer::getSleeptimer() <= time(NULL)) 
  {
   isyslog("plugin-sleeptimer: timeout");
   cPluginSleeptimer::setSleeptimer(0);

   if(method == 0) 
   {
    isyslog("plugin-sleeptimer: pressing key Power internally");
    if(!(cRemote::Put(kPower)))
     isyslog("plugin-sleeptimer: ERROR: internal keypress did not succeed \"%s\"!", command);
   }

   if(method == 1) 
   {
    isyslog("plugin-sleeptimer: muting audio");
    if(!cDevice::PrimaryDevice()->IsMute())
     cDevice::PrimaryDevice()->ToggleMute();
   }

   if(method == 2) 
   {
    dsyslog("plugin-sleeptimer: executing \"%s\"", command);
    if(SystemExec(command) == -1)
     isyslog("plugin-sleeptimer: ERROR while executing \"%s\"!", command);
   }

  } 
  else //timeout not reached yet
  {
   if((cPluginSleeptimer::getSleeptimer() - 60) <= time(NULL)) 
   {
    Skins.QueueMessage(mtInfo,tr("Going to sleep in about one minute"),0,-1);
    isyslog("plugin-sleeptimer: going to sleep in about one minute");
   }
  }
  int i=0;
  while (cPluginSleeptimer::getSleeptimer() && i<10)
  {
   sleep(1);
   i++;
  }
 }
 isyslog("plugin-sleeptimer: thread end (pid=%d)", getpid());
}


eOSState MainMenu::ProcessKey(eKeys k) 
{
 int current;

 eOSState state = cOsdMenu::ProcessKey(k); 
 switch(state) 
 {
  case osUnknown:
   switch(k) 
   {
    case kRed:
     process_red=true;
     cRemote::CallPlugin("sleeptimer");
     return osEnd;
     break;
    
    case kGreen:
     cPluginSleeptimer::setSleeptimer(0);
     return osEnd;
     break;

    case kOk:
     current = Current();
     if(current == actionpos) 
     {
      if(Interface->Confirm(tr("Disable sleeptimer?"))) 
      {
       cPluginSleeptimer::setSleeptimer(0);
       return(osEnd);
      }
     } 
     else if(current == timepos) 
     {
      char buf[80];
      int Std, Min;
      time_t tmp;
      tmp = s2i(Min, Std, shutdown_time);
                       
      snprintf(buf, sizeof(buf), tr("Activate sleeptimer at %i:%.2i?"), Std, Min);
           
      if(Interface->Confirm(buf)) 
      {
       if (tmp<time(NULL)+61)
       {
        Skins.Message(mtError,tr("Timespan shorter than 2 minutes")); return
        osContinue;
       }
             
       cPluginSleeptimer::setSleeptimer(tmp);

       cBackgroundSleeptimer::getInstance()->Start();
       return(osEnd);
      }
     } 
     else if(current == minpos) 
     {
      char buf[80];
      snprintf(buf, sizeof(buf), tr("Activate sleeptimer in %i minutes?"), shutdown_minutes);
      if(Interface->Confirm(buf)) 
      {
       cPluginSleeptimer::setSleeptimer(time(NULL) + shutdown_minutes * 60);
       cBackgroundSleeptimer::getInstance()->Start();
       return(osEnd);
      }
     }
     return(osPlugin);
     break;
     
    default: break;
   }
   default: break;
  }
  return state;
}


//   ************************
//   ***** HotkeyAction *****
//   ************************

void cPluginSleeptimer::HotkeyAction()
{
 eKeys user_key=(eKeys)NULL;
 cString msg="";
 bool first_action=false;
 time_t now=time(NULL);
 int next_sleepmin_index=-1;

 while (true) //OSD message / user interaction loop
 {
  now=time(NULL);
  if (difftime(now,lastaction)>5) first_action=true;

  lastaction=now;

  if (first_action)
  {
   next_sleepmin_index=-1;
   if (sleepat)
   {
    int sleepint=(int)(difftime(sleepat,now)/60);
    if (sleepint==1)
    {
     msg=msg.sprintf(tr("Sleeptimer in %i minute"),sleepint);
    }
    else
    {
     msg=msg.sprintf(tr("Sleeptimer in %i minutes"),sleepint);
    }
    
   }
   else
   {
    msg=msg.sprintf(tr("Sleeptimer disabled"));
   }
  }
  else
  {
   if (next_sleepmin_index > 0)
   {
    if (do_confirm)
    {
     msg=msg.sprintf(tr("Enable sleeptimer in %i minutes?"), autoswitch_vals[next_sleepmin_index]);
    }
    else
    {
     msg=msg.sprintf(tr("Sleeptimer in %i minutes"), autoswitch_vals[next_sleepmin_index]);
    }
   } 
   else
   {
    if (do_confirm)
    {
     msg=msg.sprintf(tr("Disable sleeptimer?"));
    }
    else
    {
     msg=msg.sprintf(tr("Sleeptimer disabled"));
    }
   } 
  }
  eKeys k=cPluginSleeptimer::ShowMessage(msg);


// ***********************
// ***** KEYHANDLING *****
// ***********************

  if (first_action)
  {
   if ((k>=kUser1 && k<=kUser9) || k==kRed)
    user_key=k;
  }

  if (k!=kRed && k!=kGreen && k!=kBack && k!=kOk && k!= kNone && k!=user_key) 
  {
   cRemote::Put(k);
   break;
  }

  if (k==kBack) 
  {
   break;
  }
 
  if (k==kRed || k==user_key)
  {
   if (!first_action)
   {
    if (!do_confirm)
    {
     if (next_sleepmin_index > 0)
     {
      sleepat=now + autoswitch_vals[next_sleepmin_index] * 60;
      cBackgroundSleeptimer::getInstance()->Start();
     }
     else
     {
      sleepat=0;
     }
    }
   }
  }


  if (k==kOk || (k==kNone && !do_confirm))
  {
   if (first_action) break;
   if (next_sleepmin_index > 0)
   {
    sleepat=now + autoswitch_vals[next_sleepmin_index] * 60;
    cBackgroundSleeptimer::getInstance()->Start();
   }
   else
   {
    sleepat=0;
   }
   break;
  }

  if (k==kGreen)
  {
   sleepat=0;
   msg=msg.sprintf(tr("Sleeptimer disabled"));
   break;
  }
  
  if (k==kNone) break;
  
  if (first_action)
  {
    first_action=false;
 
   if (!sleepat)
   {
    if (start_w_default)
    {
     int i=0;
     for (i=0;i<ARRAYLENGTH-1;i++)
     if (autoswitch_vals[i]==default_timespan) next_sleepmin_index=i-1;
    }
    else
    {
     next_sleepmin_index=0;//bit confusing; final value should be 1 but 1 will be added at the end of the method
    }
   }
   else //there is an active sleeptimer so starting from this we have to find the next higher value
   {
    next_sleepmin_index=0;
    int last_field=ARRAYLENGTH-1;
    if (autoswitch_vals[last_field]<0) last_field=ARRAYLENGTH-2;
 
    if (sleepat > autoswitch_vals[last_field]*60 + now) continue;
    
    int dest_index=-1;
    int i=0;
    for (;i<=last_field;i++)
    {
     if (sleepat >= autoswitch_vals[i]*60 + now) continue;
 
     dest_index=i;
     break;
    }
    if (dest_index==-1)
     continue;
    else
    {
     next_sleepmin_index=dest_index;
     continue;
    }
   }
  }  
  next_sleepmin_index+=1;

  if (next_sleepmin_index>=ARRAYLENGTH || autoswitch_vals[next_sleepmin_index]<0) next_sleepmin_index=0;
 }
 Interface->GetKey(false);
 lastaction=0;
}


eKeys cPluginSleeptimer::ShowMessage(cString msg)
{
 cSkinDisplayMessage *csdmsg=Skins.Current()->DisplayMessage();
 cSkinDisplay::Current()->SetMessage(mtInfo,*msg);
 cSkinDisplay::Current()->Flush();
   
 Interface->GetKey(false);
 eKeys retval= Interface->Wait(5);
     
 delete csdmsg;
       
 return retval;
}

VDRPLUGINCREATOR(cPluginSleeptimer); // Don't touch this!
