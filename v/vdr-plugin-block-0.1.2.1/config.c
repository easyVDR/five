/**
 * based on config.c,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include "config.h"


cSetupBlock SetupBlock;

int cSetupBlock::LastAcceptableChannel=1;
int cSetupBlock::DetectionMethod=0;
int cSetupBlock::OkAllowed=1;
int cSetupBlock::ParentalGuidance=-1; //negative value to check later if already set by commandline argument
int cSetupBlock::FuzzyFallback=0;
int cSetupBlock::DVBRating=0;
int cSetupBlock::MessageTimeout=2;
char cSetupBlock::ReplayingName[256]={' '}; //256 corresponds to the initial EVMAXLINELENGTH in event.h; may have a more reliable implementation
cChannel* cSetupBlock::LastcChannel=NULL;
int cSetupBlock::user_direction=0;

cSetupBlock::cSetupBlock(void):
	HideMenuEntry(0)
{
}


bool cSetupBlock::Parse(const char *Name, const char *Value)
{
  if (strcmp(Name, "HideMenuEntry")  == 0) HideMenuEntry  = atoi(Value);
    else if (strcmp(Name, "MessageTimeout") == 0) MessageTimeout = atoi(Value);
      else if (strcmp(Name, "DetectionMethod") == 0) DetectionMethod = atoi(Value);
        else if (strcmp(Name, "OkAllowed")==0) OkAllowed = atoi(Value);
          else if (strcmp(Name, "ParentalGuidance") == 0) ParentalGuidance=atoi(Value); //if cl is parsed a check has to be implemented here, to let cl value override setup.conf
           else if (strcmp(Name, "FuzzyFallback")==0) FuzzyFallback=atoi(Value);
             else if (strcmp(Name, "DVBRating")==0) DVBRating=atoi(Value);
               else return false;
  if (ParentalGuidance!=-1 && ParentalGuidance!=0) //-1 if not set by cl oder setup.conf - 0 if set to disabled - other value = enabled
  {
    cSetupBlock::DetectionMethod=1;
    cSetupBlock::OkAllowed=0;
    cSetupBlock::FuzzyFallback=0;
  }
  return true;
}

