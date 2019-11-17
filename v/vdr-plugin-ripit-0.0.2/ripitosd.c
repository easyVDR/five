#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <vdr/osdbase.h>
#include <vdr/config.h>
#include "ripitosd.h"
#include <vector>
#include <sstream>

const cFont *font;


cRipitOsd::cRipitOsd() :cOsdMenu("")
{
  font = cFont::GetFont(fontSml);
  lastlogsize=-1;
  logbuffer.str("");
}

cRipitOsd::~cRipitOsd()
{
  DisplayMenu()->SetTitle(tr("AudioCD Ripper"));
  // keep
}

bool cRipitOsd::Rip_On(void)
{
  return access(LOCK_FILE,F_OK) == 0;
}


eOSState cRipitOsd::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osBack) return osEnd;

  if (state == osUnknown) {
    switch (Key) {

      case kNone:
        Display();
      break;

      case kRed:
      case kOk:
        return osEnd;
      break;

      case kBlue:
        if (!Rip_On()) {
          Start_Encode();
          Display();
        }
        else {
          Abort_Encode(0);
          Display();
        }
      break;

      default: break;
    }
  } 
  return osContinue;
}


void cRipitOsd::Start_Encode(void)
{
  char *buffer;
  char encoptions[256];

  static int bitrate[10] = { 32, 64, 96, 112, 128, 160, 192, 224, 256, 320 };
  static char fileschars[3][5] = { "", "NTFS", "HFS" };

  if      (RipitSetup.Ripit_encoding_type ==  1) strcpy(encoptions, RipitSetup.Ripit_encopts1);
  else if (RipitSetup.Ripit_encoding_type ==  2) strcpy(encoptions, RipitSetup.Ripit_encopts2);
  else if (RipitSetup.Ripit_encoding_type ==  3) strcpy(encoptions, RipitSetup.Ripit_encopts3);
  else if (RipitSetup.Ripit_encoding_type ==  4) strcpy(encoptions, RipitSetup.Ripit_encopts4);
  else if (RipitSetup.Ripit_encoding_type ==  5) strcpy(encoptions, RipitSetup.Ripit_encopts5);
  else strcpy(encoptions, "");

  asprintf(&buffer,"touch \"%s\"; echo \"vdr-ripit -M 'start' -n '%i' -v '%i' -d '%s' -o '%i' -l '%i' -m '%i' -e '%s' -t '%i' -c '%i' -f '%i' -x '%s' -u '%i' -p '%i' -r '%s' -X '%s' -L '%s' -A '%s'\" | at now",
					LOCK_FILE,
					RipitSetup.Ripit_nice,
					RipitSetup.Ripit_verbosity,
					RipitSetup.Ripit_dir,
					RipitSetup.Ripit_oggquality,
					bitrate[RipitSetup.Ripit_lowbitrate],
					bitrate[RipitSetup.Ripit_maxbitrate],
					encoptions,
					RipitSetup.Ripit_encoding_type,
					RipitSetup.Ripit_crc,
					RipitSetup.Ripit_fastrip,
					fileschars[RipitSetup.Ripit_excludespecialchars],
					RipitSetup.Ripit_underscores,
					RipitSetup.Ripit_playlist,
					RipitSetup.Ripit_ripopts,
					LOCK_FILE,
					LOG_FILE,
					ABORT_FILE);

  dsyslog("Ripit: Executing '%s'",buffer);
  SystemExec(buffer);
  free(buffer);

}

void cRipitOsd::Abort_Encode(const int external)
{
  char *buffer;
  asprintf(&buffer, "echo \"vdr-ripit -M 'kill' -X '%s' -L '%s' -A '%s'\" | at now",
					LOCK_FILE,
					LOG_FILE,
					ABORT_FILE);
  
  esyslog("Ripit: Executing '%s' (Process manually aborted ...!!!)\n",buffer);
  SystemExec(buffer);
  free(buffer);

  if ( external == 0 ) Display();
//keep
}

void cRipitOsd::Display(void)
{

  cOsdMenu::Display();

  using namespace std;

  struct stat filestatus;
  int filestatusok;
  if (stat( LOG_FILE, &filestatus ) == 0 ) {
    filestatusok=1;
  }
  else {
    filestatusok=0;
    lastlogsize=-1;
    logbuffer.str("");
  }

  // check for running process  
  if (Rip_On()) {
    DisplayMenu()->SetTitle(tr("Process started..."));
    SetHelp(tr("Hide"), NULL, NULL, tr("Abort"));
    DisplayMenu()->SetText(tr("Ready To Rumble"), true);
  }
  else {
    DisplayMenu()->SetTitle(tr("No process started"));
    SetHelp(tr("Hide"),  NULL, NULL, tr("Start"));
    DisplayMenu()->SetText(tr("No process started"), true);
  }

  if ( (filestatusok == 1) && (lastlogsize != filestatus.st_size)) { //logfile changed
    lastlogsize=filestatus.st_size;
    ifstream logfile(LOG_FILE);
    if (logfile.is_open()){
      //esyslog("update log");
      vector<string> values;
      for(string line; getline(logfile, line); ){
        values.push_back(line);
      }
      int start;
      if (values.size() < (unsigned)RipitSetup.Ripit_lastlog){
        start=0;
      }
      else{
        start=values.size()-RipitSetup.Ripit_lastlog;
      }
      logbuffer.str("");
      for(unsigned i=start; i<values.size(); i++ ){
        string r;
        r=values.at(i);
        while ( r.find( "\t") != std::string::npos){
          r.replace( r.find( "\t"), strlen("\t"), "        ");
        }
        logbuffer << r << endl;
      }
      DisplayMenu()->SetText(logbuffer.str().c_str(), true);
    }
  }
  else {
    if (lastlogsize > -1)
      DisplayMenu()->SetText(logbuffer.str().c_str(), true);
  }

}
