
/*
 * config.c: 'Soppalusikka' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stdlib.h>
#include "common.h"
#include "log.h"
#include "config.h"

cSoppalusikkaConfig SoppalusikkaConfig;

cSoppalusikkaConfig::cSoppalusikkaConfig()
: logoDirM(""),
  traceModeM(eTraceModeNormal),
  showAuxInfoM(1),
  showLogoM(0),
  showVpsM(1),
  showDurationM(1),
  showSymbolsM(0),
  showProgressBarM(1),
  cacheSizeM(100)
{
}

void cSoppalusikkaConfig::SetLogoDir(const char *logoDirP)
{
  debug1("%s (%s)", __PRETTY_FUNCTION__, logoDirP);
  logoDirM = cString(logoDirP);
}
