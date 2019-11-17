#ifndef __RIPIT_H
#define __RIPIT_H

#include <vdr/plugin.h>
#include <string>
#include <vdr/osd.h>
#include "ripitosd.h"
#include <sstream>

static const char *VERSION = "0.0.2";
static const char *DESCRIPTION = trNOOP("AudioCD Ripper");
static const char *MAINMENUENTRY = trNOOP("RipIt");


class cPluginRipit : public cPlugin {
 // Add any member variables or functions you may need here.
private:
  cRipitOsd *ripitosd;
public:
  cPluginRipit(void);
  virtual ~cPluginRipit();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual cString Active(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
#if APIVERSNUM >= 10331
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
#endif  
  };



class cMenuRipitSetup : public cMenuSetupPage {
private:
  const char *excludespecialchars[3], *lowbitrate[10], *maxbitrate[10], *preset[6];
virtual void Setup(void);
  cVector<const char*> help;
  std::stringstream about;
  int oldCurrent;
protected:
  virtual void Store(void);
  virtual eOSState ProcessKey(eKeys Key);
public:
  cMenuRipitSetup(void);
};

#endif // __WEATHERNG_H
