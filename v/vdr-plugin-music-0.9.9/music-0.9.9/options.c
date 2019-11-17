#include <string.h>
#include <fstream>
#include <stdio.h>

#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include <vdr/plugin.h>

#include "options.h"
#include "setup-mp3.h"
#include "player-mp3.h"
#include "i18n.h"
#include "common.h"
#include "vars.h"
#include "decoder.h"

//#define MODE 0755


//---------------- cMP3TimerShutdown --------------------------------------------------------------------------------------------------------------------------
cMP3TimerShutdown::cMP3TimerShutdown(void)
:cOsdMenu(tr("Music: Timer Shutdown"),40)
{
  timertime = MP3Setup.TimerShutdownTime;
  ShowDisplay();
}

cMP3TimerShutdown::~cMP3TimerShutdown()
{
}

void cMP3TimerShutdown::ShowDisplay(void)
{
  Clear();
  Add(new cMenuEditIntItem(tr("Minutes before shutdown"), &timertime, 1, 90));
  SetHelp();
  Display();
}

void cMP3TimerShutdown::SetHelp()
{
  if(MP3Setup.TimerEnabled)
    cOsdMenu::SetHelp(tr("Parent"), NULL, NULL, tr("Disable"));
  else	
    cOsdMenu::SetHelp(tr("Parent"), NULL, NULL, tr("Enable"));
}

eOSState cMP3TimerShutdown::Execute()
{
  if(MP3Setup.TimerEnabled) {
    MP3Setup.TimerEnabled = false;
    MP3Setup.TimerShutdownTime = 0;
	MP3Setup.TimerActiveTime = 0;
	isyslog("music: options: timer disabled");
    }
   else {
    MP3Setup.TimerEnabled = true;
    MP3Setup.TimerShutdownTime    = timertime;
	MP3Setup.TimerActiveTime  = time_ms();
	isyslog("music: options: timer enabled : %i min", timertime);
  }
  return osBack;
}


eOSState cMP3TimerShutdown::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kOk:
        case kBack:
	         state = osBack; 
          break;
        case kRed:
        case kGreen:
	    case kYellow:
		  break;
        case kBlue:
	        state = Execute();
	      break;

        default: state = osContinue;
    }
  }
  return state;
}


//---------------- cMP3Skiptime --------------------------------------------------------------------------------------------------------------------------
cMP3Skiptime::cMP3Skiptime(void)
:cOsdMenu(tr("Music: Change time to skip"),20)
{
  skiptime = MP3Setup.Jumptime;
  ShowDisplay();
}

cMP3Skiptime::~cMP3Skiptime()
{
}

void cMP3Skiptime::ShowDisplay(void)
{
  Clear();
  Add(new cMenuEditIntItem(tr("Seconds to skip"), &skiptime, 1, 120));
  SetHelp();
  Display();
}

void cMP3Skiptime::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"), NULL, NULL, NULL);
}

eOSState cMP3Skiptime::Execute()
{
  MP3Setup.Jumptime = skiptime;

  return osBack;
}


eOSState cMP3Skiptime::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kOk:
	  state = Execute();
	  break;
        case kBack:
	  state = osBack; 
          break;
        case kRed:
        case kGreen:
	case kYellow:
        case kBlue:
	  break;

        default: state = osContinue;
    }
  }
  return state;
}



//---------------- cMP3AddCopyDir --------------------------------------------------------------------------------------------------------------------------
cMP3AddCopyDir::cMP3AddCopyDir(void)
:cOsdMenu(tr("Music: Add new target directory"),20)
{
  strncpy(AddDir, "/tmp", sizeof(AddDir));
  ShowDisplay();
}

cMP3AddCopyDir::~cMP3AddCopyDir()
{
}

void cMP3AddCopyDir::ShowDisplay(void)
{
  Clear();
  Add(new cMenuEditStrItem(tr("Directory to add"), AddDir, sizeof(AddDir) -1));
  SetHelp();
  Display();
}

void cMP3AddCopyDir::SetHelp()
{
  cOsdMenu::SetHelp(NULL);
}

void cMP3AddCopyDir::Save()
{
  static struct stat FileInfo;
  std::string datei;
  
  datei = config;
  datei = datei + langdir;
  datei = datei + "/data/copydir.dat";

#ifdef DEBUG
  isyslog("music: options: loading copydir.dat from '%s'",datei.c_str());
#endif

  if(AddDir) {
    std::ofstream filestr(datei.c_str(), std::ios::app);
    if(filestr) {
      filestr <<AddDir<<std::endl;
      if(mkdir(AddDir, FileInfo.st_mode) == -1) {
        if(errno == EEXIST) esyslog("music: options: '%s' already exist", AddDir);
      }    
    }
    else
      esyslog("music: options: could not open '%s'", datei.c_str());	  
  }
}


eOSState cMP3AddCopyDir::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
        case kOk:
	  Save();
	  state = osBack;
	  break;
        case kBack:
	  state = osBack; 
          break;
        case kRed:
        case kGreen:
	case kYellow:
        case kBlue:
	  break;

        default: state = osContinue;
    }
  }
  return state;
}


//---------------- cMP3CopyDir --------------------------------------------------------------------------------------------------------------------------
cMP3CopyDir::cMP3CopyDir(void)
:cOsdMenu(tr("Music: Goal listing"))
{

  GetCopyDirs();
}

cMP3CopyDir::~cMP3CopyDir()
{
}


void cMP3CopyDir::SetHelp()
{
  cOsdMenu::SetHelp(tr("Parent"),tr("Add"),tr("Delete"),tr("Execute"));
}


cOsdItem *cMP3CopyDir::Item(const char *dir, bool selectable)
{
  std::string m_item("");
  m_item = dir;
  cOsdItem *item = new cOsdItem(m_item.c_str(), osUnknown, selectable);

//  char *buf=0;
//  asprintf(&buf, "%s", dir?dir:"");
//  cOsdItem *item = new cOsdItem(buf, osUnknown, selectable);
//  free(buf);

  Add(item); return item;
}

void cMP3CopyDir::GetCopyDirs()
{
  std::ifstream filestr;
  std::string datei;
  std::string line;
  std::string dirs;
  std::string buff;

  datei = config;
  datei = datei + langdir;
  datei = datei +"/data/copydir.dat";

#ifdef DEBUG
  isyslog("music: options: loading copydir.dat from '%s'",datei.c_str());
#endif

  filestr.open(datei.c_str());
  
  if(filestr) {
    while ((getline(filestr,line,'\n'))) {
      Item(line.c_str(), true);
    }  
    filestr.close();

    Item("", false);
     
    buff = tr("Current goal listing: ");
    buff = buff + MP3Setup.CopyDir;
    Item( buff.c_str(), false);

  }
  else
    esyslog("music: options: ERROR: no directories defined! check '%s'", datei.c_str());

  SetHelp();
  Display();
}


int cMP3CopyDir::DeleteCopyDir()
{

  cOsdItem *item =(cOsdItem*)Get(Current());
  bool res = false;
  
  if(item){
    std::ifstream filestr;
    std::ofstream tfilestr;
    std::string datei;
    std::string tdatei;
    std::string line;
    std::string dline;

    dline = strdup(item->Text());
    datei = config;
    datei = datei + langdir;
    datei = datei + "/data/copydir.dat";

#ifdef DEBUG
  isyslog("music: options: loading copydir.dat from '%s'",datei.c_str());
#endif


    tdatei = config; 
    tdatei = tdatei + langdir;
    tdatei = tdatei + "/data/tmp.dat";

#ifdef DEBUG
  isyslog("music: options: loading tmp.dat from '%s'",tdatei.c_str());
#endif

    res = true;

    if(Interface->Confirm(tr("Remove ?"))) {
      filestr.open(datei.c_str(), std::ios::in);
      tfilestr.open(tdatei.c_str(), std::ios::out);

      if(filestr && tfilestr) {
        while(!filestr.eof()) {
          getline(filestr, line);
          if(!(line == dline) && (line > "")) 
            tfilestr << line << std::endl;
        }

        filestr.close();
        tfilestr.close();

        remove(datei.c_str());
        rename(tdatei.c_str(),datei.c_str());

        Clear();
        GetCopyDirs();
      }
      else res = false;
    }
  }
  return res;

}


eOSState cMP3CopyDir::Execute()
{
  cOsdItem *item = (cOsdItem*)Get(Current());
  
  if(item) {
    if(Interface->Confirm(tr("New goal listing ?"))) {
      std::string newdir;
      newdir  = strdup(item->Text());
      strncpy(MP3Setup.CopyDir, newdir.c_str(), sizeof(MP3Setup.CopyDir));

      return osBack;
    }
  }    

  return osContinue;
}


eOSState cMP3CopyDir::ProcessKey(eKeys Key)
{
  bool hadSubmenu = HasSubMenu();
  
  eOSState state = cOsdMenu::ProcessKey(Key);
  
//  if(hadSubmenu && !HasSubMenu()) return osBack; 
  if(hadSubmenu && !HasSubMenu()) { Clear(); GetCopyDirs(); } 
  

  if (state == osUnknown) {
    switch (Key) {
        case kBack:
        case kRed:
	  state = osBack; 
          break;
        case kOk:
	case kBlue:
	  return Execute();
	  break;
	case kYellow:
	  if(!DeleteCopyDir())
            return AddSubMenu(new cMenuText(tr("Error:"), tr("ERROR: Could not remove entry !"), fontFix));
	  break;  
        case kGreen:
          return AddSubMenu(new cMP3AddCopyDir());

        default: state = osContinue;
       
    }
  }
  return state;
}



//---------------- cMP3Options --------------------------------------------------------------------------------------------------------------------------

cMP3Options::cMP3Options(void)
:cOsdMenu(tr("Music: Quick settings"))
{
  SetHelp();

// 0
  if(MP3Setup.EnableShutDown)
    Item(tr("Disable system shutdown after player stopped"));
  else    
    Item(tr("Enable system shutdown after player stopped"));
// 1
  Add(new cOsdItem(hk(tr("Timer Shutdown"))));

// 2
  Item("--------------------------------------------------------------------------------");

// 3
  if(!mgr->ShuffleMode())
    Add(new cOsdItem(hk(tr("Activate Shufflemode"))));
  else
    Add(new cOsdItem(hk(tr("Deactivate Shufflemode"))));
// 4
  if(!mgr->LoopMode())
    Add(new cOsdItem(hk(tr("Activate Loopmode"))));
  else
    Add(new cOsdItem(hk(tr("Deactivate Loopmode"))));
// 5
  Item("--------------------------------------------------------------------------------");

// 6
    Add(new cOsdItem(hk(tr("Change directory where to copy tracks"))));
// 7
  Item("--------------------------------------------------------------------------------");
// 8
  Add(new cOsdItem(hk(tr("Change time to skip"))));
//9
  Item("--------------------------------------------------------------------------------");
// 10
  Add(new cOsdItem(hk(tr("Empty ID3 Cache"))));



  Display();
}

cMP3Options::~cMP3Options()
{
}


cOsdItem *cMP3Options::Item(const char *text)
{
  std::string m_item("");
  m_item = text;
  cOsdItem *item = new cOsdItem(m_item.c_str(), osUnknown, true);

//  char *buf=0;
//  asprintf(&buf, "%s", text?text:"");
//  cOsdItem *item = new cOsdItem(buf, osUnknown, true);
//  free(buf);
  Add(item, osUnknown);
  if(strstr(item->Text(), "----------"))
    item->SetSelectable(false);
  else
    item->SetSelectable(true);    
  
  return item;
}

void cMP3Options::SetHelp() {
  cOsdMenu::SetHelp(tr("Parent"),NULL,NULL,NULL);
}


eOSState cMP3Options::Execute() {
  int current = Current();

// 0 // Enable AutoShutdown;;
  if (current==0) { 
    if(MP3Setup.EnableShutDown) {
      if(Interface->Confirm(tr("Disable automatic shutdown ?"))) {
        MP3Setup.EnableShutDown = 0;
        return AddSubMenu(new cMenuText(tr("Music: Automatic shutdown"), tr("Automatic shutdown disabled !"), fontFix));
      }
    }
    else {
      if(Interface->Confirm(tr("Enable automatic shutdown ?"))) {
        MP3Setup.EnableShutDown = 1;  
        return AddSubMenu(new cMenuText(tr("Music: Automatic shutdown"), tr("VDR will shutdown after player stopped !"), fontFix));
      }
    }
  }


// 1 // Timer;;
  if (current == 1) {
    return AddSubMenu(new cMP3TimerShutdown());
  }  

// 2 !!! Separator  
  
//  SetupStore("InitLoopMode",     MP3Setup.InitLoopMode   );
//  SetupStore("InitShuffleMode",  MP3Setup.InitShuffleMode);  
  
// 3 // Shufflemode;;
  if (current == 3) {
    if(mgr->maxIndex <0) 
      return AddSubMenu(new cMenuText(tr("Error:"), tr("ERROR: No track(s) loaded !"), fontFix));

    if(!mgr->ShuffleMode()) {
      if(Interface->Confirm(tr("Always start in shuffle mode ?")) ) {
        MP3Setup.InitShuffleMode = true;
        cPlugin* Plugin = cPluginManager::GetPlugin(PLUGIN_NAME_I18N);
        Plugin->SetupStore("InitShuffleMode", 1);
      }
    }
    else {  
      if(Interface->Confirm(tr("Disable shuffle mode only for this session ?")) ) {
        mgr->ToggleShuffle();
        cPlugin* Plugin = cPluginManager::GetPlugin(PLUGIN_NAME_I18N);
        Plugin->SetupStore("InitShuffleMode", 1);
 #ifdef DEBUG
        printf("music: options: ShuffleMode = %d\n", mgr->ShuffleMode());
        printf("music: options: InitShuffleMode = %d\n", MP3Setup.InitShuffleMode);
#endif
        return osBack;
        }
      else {  
        MP3Setup.InitShuffleMode = false;
        cPlugin* Plugin = cPluginManager::GetPlugin(PLUGIN_NAME_I18N);
        Plugin->SetupStore("InitShuffleMode", 0);
      }
    }

    mgr->ToggleShuffle();
#ifdef DEBUG
    printf("music: options: ShuffleMode = %d\n", mgr->ShuffleMode());
    printf("music: options: InitShuffleMode = %d\n", MP3Setup.InitShuffleMode);
#endif
    return osBack;
  }  


// 4 // Loopmode;;
  if (current == 4) {
    if(mgr->maxIndex <0) 
      return AddSubMenu(new cMenuText(tr("Error:"), tr("ERROR: No track(s) loaded !"), fontFix));

    if(!mgr->LoopMode()) {  
      if(Interface->Confirm(tr("Always start in loop mode ?"))) {
        MP3Setup.InitLoopMode = true;
        cPlugin* Plugin = cPluginManager::GetPlugin(PLUGIN_NAME_I18N);
        Plugin->SetupStore("InitLoopMode", 1);
      }
    }    
    else {
      if(Interface->Confirm(tr("Disable loop mode only for this session ?"))) {
        mgr->ToggleLoop();
        cPlugin* Plugin = cPluginManager::GetPlugin(PLUGIN_NAME_I18N);
        Plugin->SetupStore("InitLoopMode", 1);
#ifdef DEBUG
        printf("music: options: LoopMode = %d\n", mgr->LoopMode());
        printf("music: options: InitLoopMode = %d\n", MP3Setup.InitLoopMode);
#endif
        return osBack;
      }
      else {
        MP3Setup.InitLoopMode = false;
        cPlugin* Plugin = cPluginManager::GetPlugin(PLUGIN_NAME_I18N);
        Plugin->SetupStore("InitLoopMode", 0);
      }
    }
    
    mgr->ToggleLoop();
#ifdef DEBUG
    printf("music: options: LoopMode = %d\n", mgr->LoopMode());
    printf("music: options: InitLoopMode = %d\n", MP3Setup.InitLoopMode);
#endif
    return osBack;
  }  

// 5 !!! Separator  
  
// 6 // CopyDir;;
  if (current == 6) {
    return AddSubMenu(new cMP3CopyDir());
  }  

// 7 !!! Separator  
  
// 8 // SkipTime;;
  if (current == 8) {
    return AddSubMenu(new cMP3Skiptime());
  }  

// 9 !!! Separator

// 10 // Empty ID3 cache
  if (current == 10) {
    if(Interface->Confirm(tr("Empty ID3 cache ?"))) {
      InfoCache.Empty();
      return osBack;
    }
  }  

  
  
  return osContinue;
}
	 

eOSState cMP3Options::ProcessKey(eKeys Key)
{
  bool hadSubmenu = HasSubMenu();
  
  eOSState state = cOsdMenu::ProcessKey(Key);
  
  if(hadSubmenu && !HasSubMenu()) return osBack; 
  
  if (state == osUnknown) {
    switch (Key) {
        case kBack:
        case kRed:
	  state = osBack; 
          break;
        case kOk:
	  return Execute();
	  break;
        default: state = osContinue;
    }
  }
  return state;
}


