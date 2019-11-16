/**
 * based on config.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include <vdr/channels.h>
#ifndef VDR_BLOCK_CONFIG_H
#define VDR_BLOCK_CONFIG_H

class cSetupBlock {
public:
  int  HideMenuEntry;
  static int MessageTimeout;
  static int DetectionMethod;
  static int LastAcceptableChannel;
  static int ParentalGuidance;
  static int OkAllowed;
  static int FuzzyFallback;  
  static int DVBRating;
  static char ReplayingName[256];
  static cChannel *LastcChannel;
  static int user_direction;
  
  cSetupBlock(void);
  bool Parse(const char *Name, const char *Value);
    
};

extern cSetupBlock SetupBlock;

#endif // VDR_BLOCK_CONFIG_H
