/**
 * block.c: A plugin for the Video Disk Recorder
 *
 * based on taste.c v 1.1.1.1 2006/02/26 14:11:02 by lordjaxom
 *
 * See the README file for copyright and contact information.
 *
 * version by Midas
 *
 */

#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <libsi/descriptor.h>

#include "status.h"
#include "setup.h"
#include "control.h"

#include <fstream>
using namespace std;

static const char *VERSION        = "0.1.2~201304020120dev";
static const char *DESCRIPTION    = trNOOP("Block unwanted shows by EPG title");
static const char *MAINMENUENTRY  = trNOOP("(De)Block broadcast");

static int channelnumber=0;
#ifdef LOGGING
  char *channel_groupsep_string=(char*)"";  
  char *has_been_checked_string=(char*)"";
  bool temp_replaying_recording=false;
#endif


class cPluginBlock : public cPlugin {
private:
  cStatusBlock *mStatus;

public:
  cPluginBlock(void);
  virtual ~cPluginBlock();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual const char *MainMenuEntry(void) { return SetupBlock.HideMenuEntry ? NULL : tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual void MainThreadHook(void);
  //virtual bool ProcessArgs(int argc, char *argv[]);
  };

cPluginBlock::cPluginBlock(void):
    cPlugin(),
    mStatus(NULL)
{
  cEventBlock::LastTitle=(char*)"vdr plugin block started";
}

cPluginBlock::~cPluginBlock()
{
  delete mStatus;
}

bool cPluginBlock::Initialize(void)
{
  char* blacklist_path=NULL;
  int io_result=-1;
  asprintf(&blacklist_path,"%s/%s",cPlugin::ConfigDirectory(),"block");
  io_result=mkdir (blacklist_path, S_IRWXU);
  
  if (errno==EEXIST) dsyslog("plugin-block: path '%s' seems to exist...ok.",blacklist_path);
  else if (errno==EPERM) dsyslog("plugin-block: ERROR: Permission denied accessing '%s'",blacklist_path);
       else if (io_result==0) dsyslog("plugin-block: path '%s' was created...ok.",blacklist_path);
            else dsyslog("plugin-block: ERROR: Accessing '%s'! Doesn't seem to exist though permissions seem to be ok.",blacklist_path);
  
  char* blacklist_file=NULL;

  asprintf(&blacklist_file,"%s/block.conf",blacklist_path); //block.conf in new place (config/block/block.conf) ?
  io_result=access(blacklist_file,F_OK);

  if (io_result==-1) //not there lets check the old places
  {
   dsyslog("plugin-block: '%s' doesn't exist. Checking old paths.",blacklist_file);
   char* source_file=NULL;
   asprintf(&source_file,"%s/block.conf",cPlugin::ConfigDirectory());
   io_result=access(source_file,F_OK);
   if (io_result==-1) //also no file config/block.conf (old path)
   {
    dsyslog("plugin-block: '%s' doesn't exist. Looking for taste.conf.",source_file);
    source_file=NULL;
    asprintf(&source_file,"%s/taste.conf",cPlugin::ConfigDirectory());
    io_result=access(source_file,F_OK);
    if (io_result==-1) // config/taste.conf also does not exist (very old path)
    {
     dsyslog("plugin-block: '%s' doesn't exist. Giving up.",source_file);
     dsyslog("plugin-block: WARNING: No block.conf or taste.conf found!");
     source_file=NULL;
    }
   }
   if (source_file!=NULL)
   {
    dsyslog("plugin-block: source %s, dest %s",source_file,blacklist_file);
    ifstream infile(source_file);
    ofstream outfile(blacklist_file);
    outfile<<infile.rdbuf();
    infile.close();
    outfile.close();
    free((void *) source_file);
   }
  }

  char* backup_file=NULL;
  asprintf(&backup_file,"%s.safe-pre0.1.0",blacklist_file);

  if (access(backup_file,F_OK)==-1) //backup not found
  {
   if (access(blacklist_file,F_OK)==0) //ok, now that we have a block.conf file in the right path do the backup
   {
    ifstream infile(blacklist_file);
    ofstream outfile(backup_file);
    outfile<<infile.rdbuf();
    infile.close();
    outfile.close();
    free((void *)blacklist_path);
    free((void *)blacklist_file); 
    free((void *) backup_file);

   }
  }
  
  bool b = EventsBlock.Load(AddDirectory(cPlugin::ConfigDirectory(), "block/block.conf"), true, false);
  EventsBlock.Sort();
  cEventBlock* listptr=EventsBlock.First();
  while (listptr!=NULL)
  {
   if (listptr->Next()==NULL) break;
   const char* src=cEventBlock::duptolower(listptr->Pattern());
   const char* cmp=cEventBlock::duptolower(((cEventBlock*)listptr->Next())->Pattern());
#ifdef LOGGING
   dsyslog("plugin-block: Checking for duplicates (case insensitive): '%s' and '%s'",src,cmp);
#endif   
   if (strcmp(src,cmp)==0)
   {
    dsyslog("plugin-block: WARNING - import taste|block.conf: deleting duplicate entry %s",cmp);
    EventsBlock.Del(listptr->Next(),true);
    continue;
   }
   free((void *)src);
   free((void *)cmp);
   listptr=EventsBlock.Next(listptr);
  }

  cSetupBlock::LastcChannel=Channels.GetByNumber(cDevice::CurrentChannel());
  return b;
}

bool cPluginBlock::Start(void)
{
#if VDRVERSNUM < 10507 
  RegisterI18n(Phrases);
#endif 
  mStatus = new cStatusBlock();
  return true;
}


cOsdObject *cPluginBlock::MainMenuAction(void)
{
  const cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  const char* current_title=NULL;
  if (channel != NULL && !channel->GroupSep()) 
  { 
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
    if (scheds != NULL)
    {
     const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
     if (sched != NULL)
     {
      const cEvent *present = sched->GetPresentEvent();
      if (present != NULL)
       current_title=present->Title();
     }
    }
   }
   if (current_title==NULL) current_title="";
    
   bool acceptable=EventsBlock.Acceptable(current_title);



    if (!acceptable && cSetupBlock::ParentalGuidance)
    {
     dsyslog("plugin-block: Parental Guidance: Attempt to deblock '%s' from main menu! Permission denied!",current_title);
     cSkinDisplayChannel* mOsd = Skins.Current()->DisplayChannel(true);
     mOsd->SetMessage(mtError, tr("Permission denied!"));
     mOsd->Flush();
     return NULL;
    }
    cEventBlock* stringmatch=NULL;
    stringmatch=EventsBlock.hasStringMatch(current_title,true);
    if (stringmatch!=NULL)
    {
     bool case_insensitive_match=strcmp(stringmatch->Pattern(),current_title);
     if (case_insensitive_match)
     {
      stringmatch->setIgnoreCase(true);
     }
     stringmatch->setWhitelisted(!acceptable);
     EventsBlock.Save();
     cEventBlock::LastTitle="force recheck from main menu";
    }
    else //create a new enrty
    {
     cEventBlock *new_entry=new cEventBlock(current_title);
     new_entry->setWhitelisted(!acceptable);
     EventsBlock.Add(new_entry);
     EventsBlock.Sort();
     EventsBlock.Save();
     cEventBlock::LastTitle="force recheck from main menu";
     
    }
  
  return NULL;
} 
     

cMenuSetupPage *cPluginBlock::SetupMenu(void)
{
  return new cMenuSetupBlock();
}


bool cPluginBlock::SetupParse(const char *Name, const char *Value)
{
  return SetupBlock.Parse(Name, Value);
}


void cPluginBlock::MainThreadHook()
{
  if (cSetupBlock::DetectionMethod!=1) 
  {
//
// commented out cause spams the syslog:
//
//   #ifdef LOGGING
//   dsyslog("plugin-block: MainThreadHook returned because other detection method active in setup");
//   #endif   
   return;//other detection method active in setup
  }
  
  if (cEventBlock::ReplayingRecording) //no block events on the underlying channel processed - user watches recording
  {
   #ifdef LOGGING
   if (!temp_replaying_recording)
   {
    temp_replaying_recording=true;
   #endif
    if (cSetupBlock::ParentalGuidance==1)
    {
     while (cSetupBlock::ReplayingName==NULL);
     if (!EventsBlock.Acceptable(cSetupBlock::ReplayingName))
     {
      dsyslog("plugin-block: Parental Guidance detected attempt to replay recording with blocked title '%s'! Permission denied!",cSetupBlock::ReplayingName);
      int destination=cSetupBlock::LastAcceptableChannel;
      if (destination==0) destination=cDevice::CurrentChannel();
      cDevice::PrimaryDevice()->SwitchChannel(Channels.GetByNumber(destination),true);
      Skins.Message(mtError, tr("Permission denied!"));
      cEventBlock::LastTitle=(char*)"parental guidance event";
      return;
     }
    }
    #ifdef LOGGING
    dsyslog("plugin-block: doing nothing because user watches recording");
   }
    #endif
   return;
  }
  #ifdef LOGGING
  else
  {
   if (temp_replaying_recording)
   {
    temp_replaying_recording=false;
    dsyslog("plugin-block: replay of recording ended. Resuming detection mode.");
   }
  }
  #endif
  channelnumber=cDevice::CurrentChannel();
  if (channelnumber==0) //TODO last check revealed this possibly would never be true because ChannelSwitch handles it?
  {
  #ifdef LOGGING
   dsyslog("plugin-block: Channel number is 0 => Channel switch on primary device");
  #endif
   return; //switch in progress
  }
  
  const cChannel *channel=Channels.GetByNumber(channelnumber);
  
  if (cSetupBlock::LastcChannel!=channel)//necessary cause otherwise switching from one channel to another with both having
                                         //the same blocked EPG title would result in the second channel not being
                                         //blocked (more precise: until a different EPG title is analysed)
  {
   cEventBlock::LastTitle=(char*)"assuming Channelswitch";
   #ifdef LOGGING
   if (cSetupBlock::LastcChannel!=NULL && channel!=NULL) dsyslog("plugin-block: current channel changed (%i->%i)- switching? using dummy for LastTitle: %s",cSetupBlock::LastcChannel->Number(), channel->Number(),cEventBlock::LastTitle);
   #endif     
  }
  const char* title=NULL;
  const cEvent *present=NULL;
  const cEvent *follow=NULL;
  const char* description=NULL;
  if (channel != NULL && !channel->GroupSep())
  {
  /*//only for debugging purposes:
   #ifdef LOGGING
   char *temp_string;
   asprintf(&temp_string,"channel: %d channel->GroupSep(): %d", channel->Number(), channel->GroupSep());
   if (strcmp(temp_string,channel_groupsep_string)!=0)
   {
    dsyslog("plugin-block: %s",temp_string);
    channel_groupsep_string=temp_string;
   }
   #endif
  */




   cSchedulesLock schedLock;
   const cSchedules *scheds = cSchedules::Schedules(schedLock);
    
   if (scheds != NULL)
   {
    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched != NULL)
    {
     present = sched->GetPresentEvent();
     follow  = sched->GetFollowingEvent();
     if (present!=NULL)
      title=present->Title();
    }
   }
  }
  if (title==NULL) title="";//TODO this means even if title is NULL the whole procedure of verifying is being done
                            //absolutely unnecessary! if NULL && PR just block! otherwise do not!
                            //except if PR && rating check rating and then decide
                            //the following check still must stay included as it detects new titles !!!!!

    if (strcmp(title,cEventBlock::LastTitle)==0) 
    {
    #ifdef LOGGING
     char *temp_string;
     asprintf(&temp_string, "'%s' has already been checked",title);
     if (strcmp(temp_string,has_been_checked_string)!=0)
     {
      dsyslog("plugin-block: %s",temp_string); //outputs: 'bla' has already been checked
      has_been_checked_string=temp_string; //TODO a bool may increase performance
     }
     #endif     
     return; //current show has already been checked
    }
    
    
    cSetupBlock::LastcChannel=(cChannel*)channel;
    
    
        
    #ifdef LOGGING
    dsyslog("plugin-block: new EPG title detected: '%s' - comparing with '%s'",title, cEventBlock::LastTitle);
    #endif
    
    int rating=-1;//in case VDRVERSNUM is < 10711 this being <0 makes sure we do not run into complications see below
    if (present==NULL) description="present event is null. No info from EPG.";
    else 
    {
     description=present->Description();
#if VDRVERSNUM>10711
     rating=present->ParentalRating();
     dsyslog ("plugin-block: detected Rating: %i",rating);
#endif
    }
    
    cEventBlock::LastTitle=(char*)title;
    if (rating<1) rating=100; // Unfortunately most of the channels do not broadcast valuable rating information
                               // eg Sesamy Street would of course be ok to have a rating of 0
                               // but lots of adult shows and so forth also are broadcasted with rating 0 :/
                               // so we only explicitly check ratings > 0
                               // if ParentalGuidance is active
                               // this gives us another few shows which are deblocked automatically
                               // hopefully the ratings > 0 are broadcasted correctly btw 
                               // two rating states are checked here:
                               // 0  means sent by broadcaster
                               // -1 the initial value; may still be set if there is no epg etc
    if ((VDRVERSNUM > 10710 && cSetupBlock::ParentalGuidance > 0 && cSetupBlock::DVBRating < rating ) || !EventsBlock.Acceptable(title))
    {
     dsyslog("plugin-block: channel %i blocked", channelnumber);

     if (cOsd::IsOpen() && !cDisplayChannel::IsOpen())
     {
#ifdef LOGGING
      dsyslog("plugin-block: OSD is open (%i)",channelnumber);
#endif      
      int lastchannel=cSetupBlock::LastAcceptableChannel;
      int direction = cDevice::CurrentChannel() - lastchannel;
      if (direction == 0)
         direction = 1;
      int switch_result=cDevice::SwitchChannel(direction);
      if (switch_result)
      {
       cEventBlock::LastTitle="force recheck from OSD workaround";
       //in case the EPG title changes while the block message is displayed
       //AND the the former and the current title are equal
       //(AND maybe other conditions like the plugin is already coming from a
       //blocked channel etc.) this code block is reached and only in this
       //case we have to force a recheck! Maybe this is an ugly workaround
       //but it seems to work...
      }
      else
      {
       if (lastchannel != 0)
       {
        dsyslog("plugin-block: main thread switching to last channel %i (fallback)",lastchannel);
        Channels.SwitchTo(lastchannel);
       }
      }
     }
     else
     {
#ifdef LOGGING
      dsyslog("plugin-block: launching cControl for %i",channelnumber);
#endif      
      cControl::Launch(new cControlBlock(channel, present, follow));
     }
     
    }
    else
    {
     cSetupBlock::LastAcceptableChannel=cDevice::CurrentChannel();
     cSetupBlock::user_direction=0;
     cSetupBlock::LastcChannel=Channels.GetByNumber(cDevice::CurrentChannel());
    }

  }
  
                                                                                                                                                



/*
bool cPluginBlock::ProcessArgs (int argc, char *argv[])
{
  static const char short_options[] = "p:";
  
  static const struct option long_options[] =
  {
    { "ParentalGuidance", required_argument, NULL, 'p' },
    {NULL}
  };
  
  int c;
  for (c=0;c<argc;c++)
  {
    dsyslog("plugin-block-DEV: argv[%d]=%s",c,argv[c]);
  }
  c=0;
  while ((c = getopt_long(argc,argv,short_options,long_options,NULL))!=-1)
  {
    switch (c)
    {
      case 'p': cSetupBlock::ParentalGuidance=atoi(optarg);
                //dsyslog("plugin-block-DEV: ParentalGuidance set to %s by cl argument.",optarg);
                break;
      default:  //dsyslog("plugin-block-DEV: unrecognized command line option %s",optarg );
                break;//usually return false, but parental guidance could also be set in setup.conf or not at all!
    }
  }
  return true;
}
*/
                                                                                                                                                                                                                                                                                                
VDRPLUGINCREATOR(cPluginBlock); // Don't touch this!
